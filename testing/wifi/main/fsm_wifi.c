/**
 * @file fsm_wifi.c
 * @author achicken7301 (buiankhang130301@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "fsm_wifi.h"
TransitionState currentState = WIFI_INIT;

// static const char *TAG_STA = "wifi STA";
static const char *TAG_softAP = "wifi softAP";
static const char *TAG_webserver = "wifi webserver";
static httpd_handle_t server = NULL;

/* This should be AP or STA or BOTH, idk lol!!! (；′⌒`) */
esp_netif_t *esp_netif_create_default_wifi;

/* This for  WIFI_STA_CONFIGURATION*/
static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;

void fsm_wifi()
{
  switch (currentState)
  {
  case WIFI_INIT:
  {
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // currentState = WIFI_AP_INIT;
    setState(WIFI_AP_INIT);
  }

  break;
  case WIFI_AP_INIT:
  {
    /* ENABLE WIFI_EVENT HANDLER ALSO DHCP SERVER -> VERY IMPORTANT*/
    esp_netif_create_default_wifi = esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_EVENT_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    setState(WIFI_AP_CONFIGURATION);
  }
  break;
  case WIFI_AP_CONFIGURATION:
  {
    wifi_config_t ap_config = {
        .ap =
            {
                .ssid = AP_SSID,
                .ssid_len = strlen(AP_SSID),
                .channel = DEFAULT_ESP_WIFI_CHANNEL,
                .password = AP_PASS,
                .max_connection = EXAMPLE_MAX_STA_CONN,
                .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            },
    };

    if (strlen(AP_PASS) == 0)
    {
      ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));

    ESP_LOGI(TAG_softAP,
             "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             AP_SSID, AP_PASS, DEFAULT_ESP_WIFI_CHANNEL);
    currentState = WIFI_START;
  }
  break;
  case WIFI_START:
  {
    /*
    | is_router_config() | is_router_connected() | OUTPUT           |
    |--------------------|-----------------------|------------------|
    | 0                  | 0                     | WEB_SERVER       |
    | 0                  | 1                     | X                |
    | 1                  | 0                     | WIFI_STA_INIT    |
    | 1                  | 1                     | CLOSE_WEB_SERVER |
     */
    if (is_router_config() && is_router_connected())
    {
      ESP_ERROR_CHECK(esp_wifi_start());
      setState(CLOSE_WEB_SERVER);
      break;
    }

    if (!is_router_config() && !is_router_connected())
    {
      ESP_ERROR_CHECK(esp_wifi_start());
      setState(WEB_SERVER);
      break;
    }

    if (is_router_config() && !is_router_connected())
    {
      setState(WIFI_STA_INIT);
      break;
    }
  }
  break;
  case WEB_SERVER:
  {
    static httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    // Start the httpd server
    ESP_LOGI(TAG_webserver, "Starting server on port: '%d'",
             config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
      // Set URI handlers
      ESP_LOGI(TAG_webserver, "Registering URI handlers");
      httpd_register_uri_handler(server, &root_uri);
      httpd_register_uri_handler(server, &post_uri);
    }

    /* TESTING: NEED TO CHANGE THIS */
    // currentState = WIFI_STA_INIT;
    setState(WIFI_STA_INIT);
  }
  break;
  case CLOSE_WEB_SERVER:
  {
    if (is_router_config() && is_router_connected())
    {
      ESP_LOGI(TAG_webserver, "UnRegistering URI handlers");
      httpd_stop(server);
      httpd_unregister_uri_handler(server, "/", HTTP_GET);
      httpd_unregister_uri_handler(server, "/submit", HTTP_GET);
      ESP_LOGI(TAG_webserver, "From CLOSE_WEB_SERVER switch to WIFI_IDLE");
      // currentState = WIFI_IDLE;
      setState(WIFI_IDLE);
    }
  }
  break;
  case WIFI_STA_INIT:
  {
    if (!is_router_config())
      break;

    /* ENABLE IP EVENT HANDLER -> EVERY IMPORTANT */
    ESP_LOGI(TAG_webserver, "Set esp_netif_create_default_wifi_sta");
    esp_netif_create_default_wifi = esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    setState(WIFI_STA_CONFIGURATION);
  }
  break;
  case WIFI_STA_CONFIGURATION:
  {

    /* TEST: TURN THIS OFF IF IT WORKS  */
    esp_wifi_stop();
    wifi_config_t sta_config = {
        .sta =
            {
                /*.ssid = EXAMPLE_ESP_WIFI_SSID,
                 *.password = EXAMPLE_ESP_WIFI_PASS,
                 */
                .channel = ROUTER_CHANNEL,
                /* Authmode threshold resets to WPA2 as default if
                 * password matches WPA2 standards (pasword len => 8). If
                 * you want to connect the device to deprecated WEP/WPA
                 * networks, Please set the threshold value to
                 * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password
                 * with length and format matching to
                 * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
                 */
                .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK,
                .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            },
    };
    strcpy((char *)sta_config.sta.ssid, (char *)ROUTER_SSID);
    strcpy((char *)sta_config.sta.password, (char *)ROUTER_PASS);

    /* idk what 2 lines of code below are */
    // ESP_ERROR_CHECK(esp_event_handler_instance_register(
    //     WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_EVENT_handler, NULL,
    //     &instance_any_id));
    // ESP_ERROR_CHECK(esp_event_handler_instance_register(
    //     IP_EVENT, IP_EVENT_STA_GOT_IP, &IP_EVENT_handler, NULL,
    //     &instance_got_ip));
    /* idk what 2 lines of code below are */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_EVENT_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &IP_EVENT_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));

    // currentState = WIFI_AP_CONFIGURATION;
    setState(WIFI_START);
  }
  break;
  case WIFI_IDLE:
  {
  }
  break;
  default:
    break;
  }
}
