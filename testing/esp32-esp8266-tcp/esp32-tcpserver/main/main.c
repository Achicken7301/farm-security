#include "main.h"

uint8_t ROUTER_CHANNEL = 9;
const char *test_tag = "test_tcp_tag";
#define EXAMPLE_ESP_MAXIMUM_RETRY 5
int s_retry_num = 0;
int is_router_connected = 0;

void IP_EVENT_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                      void *event_data)
{
    printf("IP_EVENT_handler: event_base: %s\tevent_id:%d\n", event_base,
           event_id);
    switch (event_id)
    {
    case IP_EVENT_STA_GOT_IP:
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(test_tag, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        is_router_connected = 1;
        break;
    }

    default:
        break;
    }
}

void WIFI_EVENT_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{

    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
    {
        esp_wifi_connect();
    }
    break;
    case WIFI_EVENT_STA_STOP:
    {
    }
    break;
    case WIFI_EVENT_STA_DISCONNECTED:
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(test_tag, "retry to connect to the AP");
        }
        ESP_LOGI(test_tag, "connect to the AP fail");
    }
    break;
    case WIFI_EVENT_AP_STACONNECTED:
    {
        wifi_event_ap_staconnected_t *event =
            (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(test_tag, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    break;

    case WIFI_EVENT_AP_STADISCONNECTED:
    {
        wifi_event_ap_stadisconnected_t *event =
            (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(test_tag, "station " MACSTR " leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    break;
    default:
        break;
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    /* ENABLE IP EVENT HANDLER -> EVERY IMPORTANT */
    ESP_LOGI(test_tag, "Set esp_netif_create_default_wifi_sta");
    esp_netif_t *esp_netif_create_default_wifi =
        esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    wifi_config_t sta_config = {
        .sta =
            {
                .ssid = "NHA",
                .password = "0984012265",
                .channel = 9,
                .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK,
                .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            },
    };
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_EVENT_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &IP_EVENT_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));

    ESP_ERROR_CHECK(esp_wifi_start());
    while (1)
    {
        fsm_tcp_server_nonblocking();
    }
}
