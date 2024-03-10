#include "fsm_ap.h"

const char *AP_TAG = "AP_TAG";
esp_netif_t *esp_netif_create_default_wifi;

void fsm_ap()
{
    switch (tcpState)
    {
    case AP_INIT:
    {

        /* ENABLE WIFI_EVENT HANDLER ALSO DHCP SERVER -> VERY IMPORTANT*/
        esp_netif_create_default_wifi = esp_netif_create_default_wifi_ap();
        // ESP_ERROR_CHECK(esp_event_handler_instance_register(
        //     WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_EVENT_handler, NULL, NULL));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        /* CONFIGURATION */
        wifi_config_t ap_config = {
            .ap =
                {
                    .ssid = AP_SSID,
                    .ssid_len = strlen(AP_SSID),
                    // .channel = DEFAULT_ESP_WIFI_CHANNEL,
                    .password = AP_PASS,
                    .max_connection = MAX_STA_CONN,
                    .authmode = WIFI_AUTH_WPA_WPA2_PSK,
                },
        };

        if (strlen(AP_PASS) == 0)
        {
            ap_config.ap.authmode = WIFI_AUTH_OPEN;
        }

        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
        ESP_LOGI(AP_TAG, "wifi_init_softap finished. SSID:%s password:%s",
                 AP_SSID, AP_PASS);

        ESP_ERROR_CHECK(esp_wifi_start());

        set_tcpState(TS_INIT);
    }
    break;

    case AP_DEINIT:
    {
        /* No need to Unregister when we DONT register it at the begining */
        // esp_err_t err = esp_event_handler_instance_unregister(
        //     WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_EVENT_handler);
        // if (err != ESP_OK)
        // {
        //     // Handle error (e.g., log error message)
        // }
        ESP_LOGI(AP_TAG, "Stop Esp Wifi");
        esp_wifi_stop();
        ESP_LOGI(AP_TAG, "Stop netif");
        esp_netif_deinit();

        ESP_LOGI(AP_TAG, "Start Mesh fsm");
        set_router_config(1);

        ESP_LOGI(AP_TAG, "Set AP to Idle");
        set_tcpState(AP_IDLE);
    }
    break;

    case AP_IDLE:
    {
        break;
    }
    default:
        break;
    }
}