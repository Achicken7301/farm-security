#include "main.h"
#include "string.h"

int s_retry_num = 0;

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
    case WIFI_EVENT_STA_DISCONNECTED:
    {
        if (s_retry_num < 10)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI("WIFI_STATION", "retry to connect to the AP");
        }
        ESP_LOGI("WIFI_STATION", "connect to the AP fail");
    }
    break;
    case WIFI_EVENT_AP_STACONNECTED:
    {
        wifi_event_ap_staconnected_t *event =
            (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI("WIFI_STATION", "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    break;

    case WIFI_EVENT_AP_STADISCONNECTED:
    {
        wifi_event_ap_stadisconnected_t *event =
            (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI("WIFI_STATION", "station " MACSTR " leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    break;
    default:
        break;
    }
}

void IP_EVENT_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                      void *event_data)
{
    switch (event_id)
    {
    case IP_EVENT_STA_GOT_IP:
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI("WIFI_STATION", "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;

        ESP_LOGI("EVENT_TAG", "Unregister WIFI_EVENT_handler");
        ESP_ERROR_CHECK(esp_event_handler_unregister(
            WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_EVENT_handler));
        break;
    }

    default:
        printf("IP_EVENT_handler: event_base: %s\tevent_id:%d\n", event_base,
               event_id);
        break;
    }
}

uint8_t ROUTER_SSID[32] = "Vnpt - Nha";
uint8_t ROUTER_PASS[64] = "0984012265";

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* ENABLE IP EVENT HANDLER -> EVERY IMPORTANT */
    ESP_LOGI("WIFI_STATION", "Set esp_netif_create_default_wifi_sta");
    esp_netif_create_default_wifi_sta();

    wifi_config_t sta_config = {
        .sta =
            {
                // .ssid = "Vnpt - Nha",
                // .password = "0984012265",
                .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK,
                .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            },
    };

    memcpy(sta_config.sta.ssid, ROUTER_SSID, sizeof(ROUTER_SSID));
    memcpy(sta_config.sta.password, ROUTER_PASS, sizeof(ROUTER_PASS));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &IP_EVENT_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    while (1)
    {
    }
}