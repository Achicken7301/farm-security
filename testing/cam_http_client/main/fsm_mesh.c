#include "fsm_mesh.h"

#if USE_STA
MeshState mState = MESH_STA_INTERFACE;
#elif USE_MESH
MeshState mState = MESH_INIT;
#endif

const uint8_t MESH_ID[6] = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
mesh_addr_t id;
uint16_t mesh_layer;
mesh_addr_t mesh_parent_addr;
int last_layer = -1;

/**
 * @brief Set Mesh State
 *
 * @param MESH_STA_INTERFACE,
 * @param MESH_SOCKET_INIT,
 * @param MESH_SOCKET_SEND,
 * @param MESH_SOCKET_RECEIVE,
 * @param MESH_SOCKET_CLOSE,
 * @param MESH_LEAF_ROOT,
 * @param MESH_INIT,
 * @param MESH_DEINIT,
 * @param MESH_START,
 * @param MESH_DO_NOTHING,
 */
void set_mState(MeshState state)
{
    mState = state;
    printf("MESH_TAG: Current State: %s\n", get_mState());
}

const char *get_mState()
{
    switch (mState)
    {
    case MESH_STA_INTERFACE:
        return "MESH_STA_INTERFACE";
    case MESH_SOCKET_INIT:
        return "MESH_SOCKET_INIT";
    case MESH_SOCKET_SEND:
        return "MESH_SOCKET_SEND";
    case MESH_SOCKET_CLOSE:
        return "MESH_SOCKET_CLOSE";
    case MESH_LEAF_ROOT:
        return "MESH_LEAF_ROOT";
    case MESH_INIT:
        return "MESH_INIT";
    case MESH_DEINIT:
        return "MESH_DEINIT";
    case MESH_START:
        return "MESH_START";
    case MESH_DO_NOTHING:
        return "MESH_DO_NOTHING";
    default:
        return "UNKNOWN_MESH_STATE";
    }
}

void fsm_mesh()
{
    switch (mState)
    {
    case MESH_INIT:
    {
#if USE_HTTP_SERVER || USE_TCP_SERVER
        if (!is_router_config())
        {
            break;
        }
#endif
#if !USE_STA
        /* Mesh need this to scan for leaves, BUT when init station mode, we
         * already use this. SO that use #if #endif here for */
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
        ESP_ERROR_CHECK(esp_wifi_start());
#endif
        /*  mesh initialization */
        ESP_ERROR_CHECK(esp_mesh_init());
        /*  register mesh events handler */
        ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID,
                                                   &mesh_event_handler, NULL));

        /* Enable the Mesh IE encryption by default */
        mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
        /* mesh ID */
        memcpy(&cfg.mesh_id, MESH_ID, 6);
        /* router */
        cfg.router.ssid_len = strlen(ROUTER_SSID);
        memcpy(&cfg.router.ssid, ROUTER_SSID, cfg.router.ssid_len);
        memcpy(&cfg.router.password, ROUTER_PASS, strlen(ROUTER_PASS));

        /* mesh softAP */
        cfg.mesh_ap.max_connection = MESH_AP_CONNECTION;
        memcpy(&cfg.mesh_ap.password, ROUTER_PASS, strlen(ROUTER_PASS));
        ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
        /* mesh start */
        ESP_ERROR_CHECK(esp_mesh_start());

        set_mState(MESH_DO_NOTHING);
        // set_mState(MESH_DO_NOTHING);
    }
    break;
    case MESH_DEINIT:
    {
        esp_mesh_stop();
    }
    break;
    case MESH_START:
    {
        esp_mesh_start();
    }
    break;
    case MESH_DO_NOTHING:
    {
    }
    break;
    default:
        break;
    }
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
    }
    break;
    case MESH_EVENT_PARENT_CONNECTED:
    {
        mesh_event_connected_t *connected =
            (mesh_event_connected_t *)event_data;
        esp_mesh_get_id(&id);
        mesh_layer = connected->self_layer;
        strcpy((char *)mesh_parent_addr.addr,
               (char *)connected->connected.bssid);
        ESP_LOGI(MESH_TAG,
                 "<MESH_EVENT_PARENT_CONNECTED>layer:%d-->%d, parent:" MACSTR
                 "%s, ID:" MACSTR ", duty:%d",
                 last_layer, mesh_layer, MAC2STR(mesh_parent_addr.addr),
                 esp_mesh_is_root()  ? "<ROOT>"
                 : (mesh_layer == 2) ? "<layer2>"
                                     : "",
                 MAC2STR(id.addr), connected->duty);
        last_layer = mesh_layer;
    }
    break;
    case MESH_EVENT_PARENT_DISCONNECTED:
    {
        mesh_event_disconnected_t *disconnected =
            (mesh_event_disconnected_t *)event_data;
        ESP_LOGI(MESH_TAG, "<MESH_EVENT_PARENT_DISCONNECTED>reason:%d",
                 disconnected->reason);
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
        // printf("NEW mesh_event_handler event_id: %d\n", event_id);
        ESP_LOGI(MESH_TAG, "mesh_event_handler event_id: %d", event_id);
        break;
    }
}
