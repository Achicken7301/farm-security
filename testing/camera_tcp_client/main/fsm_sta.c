/**
 * @file fsm_sta.c
 * @author
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
        // err = send(sock, root_tx_buff, sizeof(root_tx_buff), 0);
        int file_size;
        char payload[50];
        file_size = sprintf(payload, "%zu", pic->len);

        /* Send pic size in bytes */
        // err = send(sock, &root_tx_buff, file_size, 0);
        send(sock, &payload, file_size, 0);

        /* Send pic data every byte,
         * TODO: This is UN-Optimize, each package should
         * contain more data in single packet send*/

        for (size_t i = 0; i < pic->len; i++)
        {
            err = send(sock, &pic->buf[i], sizeof(pic->buf[i]), 0);
            if (err < 0)
            {
                ESP_LOGE(MESH_TAG, "Error occurred during sending: errno %d",
                         errno);

                // ESP_LOGE(MESH_TAG, "Try after %d seconds",
                // RE_SEND_MESS_TIME); SCH_Add(send2server, RE_SEND_MESS_TIME,
                // 0);
                set_mState(MESH_SOCKET_CLOSE);
                break;
            }
        }

        ESP_LOGI(MESH_TAG, "Send success");
        /* Check if server reponse */
        // set_mState(MESH_SOCKET_RECEIVE);

        /* No need to response  */
        set_mState(MESH_SOCKET_CLOSE);
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

            /* process tcp receive here */
            if (process_server_data(rx_buffer) == TCP_MESS_OK)
            {
                ESP_LOGI(MESH_TAG, "Server receive message, closing socket");
                set_mState(MESH_SOCKET_CLOSE);
                break;
            }
        }
    }
    break;
    case MESH_SOCKET_CLOSE:
    {
        ESP_LOGE(MESH_TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);

        sock = -1;

        set_cState(CAM_CLEAR_PIC);
        set_mState(MESH_DO_NOTHING);
    }
    break;
    case MESH_LEAF_ROOT:
    {
    }
    break;
    default:
        break;
    }
}
