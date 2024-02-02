/**
 * @file fsm_espmesh.c
 * @author Khang (buiankhang130301@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "fsm_espmesh.h"

const char *MESH_TAG = "ESP_MESH";
static EspMeshState espState;
static const uint8_t MESH_ID[6] = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
static mesh_addr_t id;
static uint16_t mesh_layer;
static mesh_addr_t mesh_parent_addr;
static int last_layer = -1;
static bool is_mesh_connected = false;

const char *get_EspMeshState()
{
    switch (espState)
    {
    case MESH_PREREQUISITE:
        return "MESH_PREREQUISITE";
    case MESH_INIT:
        return "MESH_INIT";
    case MESH_CONFIGURATION:
        return "MESH_CONFIGURATION";
    case MESH_START:
        return "MESH_START";
    case MESH_SEND:
        return "MESH_SEND";
    case MESH_RELAX:
        return "MESH_RELAX";
    default:
        return "UNKNOWN_STATE";
    }
}

void set_EspMeshState(EspMeshState state)
{
    espState = state;
    printf("%s current state is: %s\n", MESH_TAG, get_EspMeshState());
}

void fsm_espmesh()
{
    switch (espState)
    {
    case MESH_INIT:
    {
        nvs_flash_init();
        /*  tcpip initialization */
        ESP_ERROR_CHECK(esp_netif_init());
        /*  event initialization */
        ESP_ERROR_CHECK(esp_event_loop_create_default());

        /*  Wi-Fi initialization */
        wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&config));
        /*  register IP events handler */
        ESP_ERROR_CHECK(esp_event_handler_register(
            IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
        ESP_ERROR_CHECK(esp_wifi_start());

        set_EspMeshState(MESH_CONFIGURATION);
    }
    break;
    case MESH_CONFIGURATION:
    {

        /*  mesh initialization */
        ESP_ERROR_CHECK(esp_mesh_init());
        /*  register mesh events handler */
        ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID,
                                                   &mesh_event_handler, NULL));

        /* Enable the Mesh IE encryption by default */
        mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
        /* mesh ID */
        memcpy((uint8_t *)&cfg.mesh_id, MESH_ID, 6);
        /* channel (must match the router's channel) */
        cfg.channel = CONFIG_MESH_CHANNEL;
        /* router */
        cfg.router.ssid_len = strlen(CONFIG_MESH_ROUTER_SSID);
        memcpy((uint8_t *)&cfg.router.ssid, CONFIG_MESH_ROUTER_SSID,
               cfg.router.ssid_len);
        memcpy((uint8_t *)&cfg.router.password, CONFIG_MESH_ROUTER_PASSWD,
               strlen(CONFIG_MESH_ROUTER_PASSWD));
        /* mesh softAP */
        cfg.mesh_ap.max_connection = CONFIG_MESH_AP_CONNECTIONS;
        memcpy((uint8_t *)&cfg.mesh_ap.password, CONFIG_MESH_AP_PASSWD,
               strlen(CONFIG_MESH_AP_PASSWD));
        ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));

        /* HTTPD CONFIGURATION */

        set_EspMeshState(MESH_START);
    }
    break;
    case MESH_START:
    {

        /* mesh start */
        ESP_ERROR_CHECK(esp_mesh_start());

        set_EspMeshState(MESH_RELAX);
    }
    break;
    case MESH_SEND:
    {
    }
    break;
    case MESH_RELAX:
    {
    }
    break;

    default:
        break;
    }
}

void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                      void *event_data)
{
    printf("ip_event_handler event_id: %d\n", event_id);
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(MESH_TAG, "<IP_EVENT_STA_GOT_IP>IP:" IPSTR,
             IP2STR(&event->ip_info.ip));
}

void mesh_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case MESH_EVENT_STARTED:
    {

        esp_mesh_get_id(&id);
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_MESH_STARTED>ID:" MACSTR "",
                 MAC2STR(id.addr));
        is_mesh_connected = false;
        mesh_layer = esp_mesh_get_layer();
    }
    break;
    case MESH_EVENT_ROOT_ADDRESS:
    {
        mesh_event_root_address_t *root_addr =
            (mesh_event_root_address_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_ADDRESS>root address:" MACSTR "",
                 MAC2STR(root_addr->addr));
    }
    break;
    case MESH_EVENT_TODS_STATE:
    {
        mesh_event_toDS_state_t *toDs_state =
            (mesh_event_toDS_state_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d", *toDs_state);
    }
    break;

    case MESH_EVENT_ROUTING_TABLE_ADD:
    {
        mesh_event_routing_table_change_t *routing_table =
            (mesh_event_routing_table_change_t *)event_data;
        ESP_LOGW(MESH_TAG,
                 "<MESH_EVENT_ROUTING_TABLE_ADD>add %d, new:%d, layer:%d",
                 routing_table->rt_size_change, routing_table->rt_size_new,
                 mesh_layer);
    }
    break;
    case MESH_EVENT_CHILD_CONNECTED:
    {
        mesh_event_child_connected_t *child_connected =
            (mesh_event_child_connected_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_CONNECTED>aid:%d, " MACSTR "",
                 child_connected->aid, MAC2STR(child_connected->mac));

        /*         mesh_addr_t add;
                mesh_data_t data;
                int flag;

                esp_mesh_recv(&add.addr, &data, 2000, &flag, NULL, 1);
                printf("data flag: %d\n", flag);
                printf("ROOT recv data from:\t");
                for (int i = 0; i < 6; i++)
                {
                    printf("%x:", add.addr[i]);
                }
                printf("\n");
                printf("ROOT recv data:\t");
                for (int i = 0; i < data.size; i++)
                {
                    printf("%c", data.data[i]);
                }
                printf("\n"); */
    }
    break;
    case MESH_EVENT_PARENT_CONNECTED:
    {
        mesh_event_connected_t *connected =
            (mesh_event_connected_t *)event_data;
        esp_mesh_get_id(&id);
        mesh_layer = connected->self_layer;
        strcpy(&mesh_parent_addr.addr, (char *)connected->connected.bssid);
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_CONNECTED>layer:%d-->%d, parent:" MACSTR
                 "%s, ID:" MACSTR ", duty:%d",
                 last_layer, mesh_layer, MAC2STR(mesh_parent_addr.addr),
                 esp_mesh_is_root()  ? "<ROOT>"
                 : (mesh_layer == 2) ? "<layer2>"
                                     : "",
                 MAC2STR(id.addr), connected->duty);
        last_layer = mesh_layer;
        is_mesh_connected = true;

        /* if i parent is connected, send hello to it */
        /* mesh_data_t data = {
            .data = (uint8_t *)"HELLO ROOT",
            .proto = MESH_PROTO_BIN,
            .size = sizeof("HELLO ROOT!"),
            .tos = MESH_TOS_P2P,
        };
        // strcpy(data.data, "HELLO ROOT!");
        printf("Data sent is %s\n", data.data);
        esp_err_t err = esp_mesh_send(NULL, &data, 0, NULL, 1);
        printf("Success sent to root\n"); */
    }
    break;
    case MESH_EVENT_PARENT_DISCONNECTED:
    {
        mesh_event_disconnected_t *disconnected =
            (mesh_event_disconnected_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_PARENT_DISCONNECTED>reason:%d",
                 disconnected->reason);
        is_mesh_connected = false;
        mesh_layer = esp_mesh_get_layer();
    }
    break;
    case MESH_EVENT_FIND_NETWORK:
    {
        mesh_event_find_network_t *find_network =
            (mesh_event_find_network_t *)event_data;
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_FIND_NETWORK>new channel:%d, router BSSID:" MACSTR
                 "",
                 find_network->channel, MAC2STR(find_network->router_bssid));
    }
    break;
    case MESH_EVENT_PS_PARENT_DUTY:
    {
        mesh_event_ps_duty_t *ps_duty = (mesh_event_ps_duty_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_PS_PARENT_DUTY>duty:%d", ps_duty->duty);
    }
    break;
    case MESH_EVENT_NETWORK_STATE:
    {
        mesh_event_network_state_t *network_state =
            (mesh_event_network_state_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_NETWORK_STATE>is_rootless:%d",
                 network_state->is_rootless);
    }
    break;

    case MESH_EVENT_PS_CHILD_DUTY:
    {
        mesh_event_ps_duty_t *ps_duty = (mesh_event_ps_duty_t *)event_data;
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PS_CHILD_DUTY>cidx:%d, " MACSTR ", duty:%d",
                 ps_duty->child_connected.aid - 1,
                 MAC2STR(ps_duty->child_connected.mac), ps_duty->duty);
    }
    break;
    default:
        printf("NEW mesh_event_handler event_id: %d\n", event_id);

        break;
    }
}