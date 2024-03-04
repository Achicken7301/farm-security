/**
 * @file fsm_sta.c
 * @author your name (you@domain.com)
 * @brief This file is used for root connect to router and access through the
 * internet by using tcp socket API.
 * @version 0.1
 * @date 2024-02-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "fsm_sta.h"

/* Create socket, send to server */
char rx_buffer[128];
const char host_ip[] = HOST_IP_ADDR;
int addr_family = 0;
struct sockaddr_in dest_addr;
int ip_protocol = 0;
int err;
int sock = -1;
const char *MESH_TAG = "MESH_TAG";
MeshState mState = MESH_STA_INTERFACE;

char *payload = "";

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

void set_mState(MeshState state)
{
    mState = state;
    printf("MESH_TAG: Current State: %s\n", get_mState());
}

void fsm_sta_init()
{
    switch (mState)
    {
    case MESH_STA_INTERFACE:
    {

#if USE_HTTP_SERVER || USE_TCP_SERVER
        if (!is_router_config())
        {
            break;
        }
#endif

        /* Start config Station Interface */
        ESP_LOGI(MESH_TAG, "Start TCP/IP configuration");
        // esp_netif_create_default_wifi = esp_netif_create_default_wifi_sta();
        ESP_ERROR_CHECK(esp_event_handler_instance_register(
            IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL, NULL));

        esp_netif_create_default_wifi_sta();
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

        wifi_config_t sta_config;
        strcpy((char *)sta_config.sta.ssid, (char *)ROUTER_SSID);
        strcpy((char *)sta_config.sta.password, (char *)ROUTER_PASS);

        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));

        set_mState(MESH_INIT);
    }
    break;

    case MESH_SOCKET_INIT:
    {
        if (sock != -1)
        {
            ESP_LOGI(MESH_TAG, "Socket is all ready init %d", sock);
            set_mState(MESH_SOCKET_SEND);
            break;
        }

        dest_addr.sin_addr.s_addr = inet_addr(host_ip);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(MESH_TAG, "Unable to create socket: errno %d", errno);
            break;
        }

        ESP_LOGI(MESH_TAG, "Socket created, connecting to %s:%d", host_ip,
                 PORT);

        err = connect(sock, (struct sockaddr *)&dest_addr,
                      sizeof(struct sockaddr_in6));

        if (err != 0)
        {
            ESP_LOGE(MESH_TAG, "Socket unable to connect: errno %d", errno);
            set_mState(MESH_SOCKET_CLOSE);
            break;
        }
        ESP_LOGI(MESH_TAG, "Successfully connected");

        set_mState(MESH_SOCKET_SEND);
    }
    break;
    case MESH_SOCKET_SEND:
    {

        err = send(sock, payload, strlen(payload), 0);
        if (err < 0)
        {
            ESP_LOGE(MESH_TAG, "Error occurred during sending: errno %d",
                     errno);
            ESP_LOGE(MESH_TAG, "Try after %d seconds", RE_SEND_MESS_TIME);
            set_mState(MESH_SOCKET_CLOSE);
            break;
        }
        /* Check if server reponse */
        set_mState(MESH_SOCKET_RECEIVE);
    }
    break;
    case MESH_SOCKET_RECEIVE:
    {
        /* Should switch to try_receive and set non-blocking */
        int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        // Error occurred during receiving
        if (len < 0)
        {
            ESP_LOGE(MESH_TAG, "recv failed: errno %d", errno);
            set_mState(MESH_SOCKET_SEND);
            break;
        }
        // Data received
        else
        {
            rx_buffer[len] = 0; // Null-terminate whatever we received and treat
                                // like a string
            ESP_LOGI(MESH_TAG, "Received %d bytes from %s:", len, host_ip);
            ESP_LOGI(MESH_TAG, "%s", rx_buffer);
        }
    }
    break;
    case MESH_SOCKET_CLOSE:
    {

        ESP_LOGE(MESH_TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);

        sock = -1;

        set_mState(MESH_DO_NOTHING);
    }
    break;
    case MESH_LEAF_ROOT:
    {
        /* Leaf-nodes interact with root-node */
    }
    break;
    default:
        break;
    }
}

const char *IP_EVENT_HANDLER_TAG = "IP_EVENT_HANDLER";

void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                      void *event_data)
{
    const char *IP_EVENT_HANDLER_TAG = "IP EVENT HANDLER";
    switch (event_id)
    {
    case IP_EVENT_STA_GOT_IP:
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(IP_EVENT_HANDLER_TAG, "got ip:" IPSTR,
                 IP2STR(&event->ip_info.ip));
    }
    break;
    default:
        ESP_LOGI(IP_EVENT_HANDLER_TAG, "event_base: %s\tevent_id%d", event_base,
                 event_id);
        break;
    }
}