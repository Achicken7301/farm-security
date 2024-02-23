#include "fsm_http_server.h"

esp_event_handler_instance_t event_handler_instance;
static HttpServerState http_state = HTTP_SERVER_INIT;
esp_netif_t *netif_default_wifi;
char *HTTP_SERVER_TAG = "HTTP_SERVER_TAG";
httpd_handle_t server = NULL;
int s_retry_num = 0;

void set_HttpServerState(HttpServerState state)
{
    ESP_LOGI(HTTP_SERVER_TAG, "Current state is %s",
             get_HttpServerState(state));
    http_state = state;
}

const char *get_HttpServerState(HttpServerState state)
{
    switch (state)
    {
    case HTTP_SERVER_INIT:
        return "HTTP_SERVER_INIT";
    case HTTP_SERVER_DEINIT:
        return "HTTP_SERVER_DEINIT";
    case HTTP_SERVER_IDLE:
        return "HTTP_SERVER_IDLE";
    default:
        return "Unknown State";
    }
}
/* Send response with custom headers and body set as the
 * string passed in user context*/
const char *resp_str =
    "<html>\n"
    "<body>\n"
    "<h1>ESP32 WIFI CONFIGURATION</h1>\n"
    "<form action='/submit'>\n"
    "  <label for='router_ssid'>ROUTER SSID:</label>\n"
    "  <input type='text' id='router_ssid' name='router_ssid'><br><br>\n"

    "  <label for='router_pass'>ROUTER PASSWORD:</label>\n"
    "  <input type='text' id='router_pass' name='router_pass'><br><br>\n"

    "  <input type='submit' value='Submit'>\n"
    "</form>\n"
    "</body>\n"
    "</html>\n";

/* An HTTP GET handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

httpd_uri_t root_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
};

static esp_err_t post_handler(httpd_req_t *req)
{
    /* TODO: GUI when connect to router fail,
    We should let user know by edit resp_str.
     */
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    // httpd_query_key_value(query, "ssid", router_ssid, sizeof(router_ssid));
    const char *query = req->uri + strlen("/submit?");
    httpd_query_key_value(query, "router_ssid", ROUTER_SSID,
                          sizeof(ROUTER_SSID));
    httpd_query_key_value(query, "router_pass", ROUTER_PASS,
                          sizeof(ROUTER_PASS));

    /* Filter user input, idk if there're better ways,
    but for know im using this sImPlE tEcHnIqUiE☜(ﾟヮﾟ☜)
    https://www.asciitable.com/
    Space replace with +
    0x20  replace with 0x2B
    */
    for (int i = 0; i < sizeof(ROUTER_SSID); i++)
    {
        // 0x20 is space, 0x2B is +
        if (ROUTER_SSID[i] == 0x2B)
        {
            ROUTER_SSID[i] = 0x20;
        }
    }

    printf("HTTP_SERVER_TAG: ROUTER's ssid: %s, pass: %s\n", ROUTER_SSID,
           ROUTER_PASS);

    ESP_LOGI(HTTP_SERVER_TAG, "Received ssid & pass, wait for connection");

    set_router_config(1);

    set_HttpServerState(HTTP_SERVER_DEINIT);

    return ESP_OK;
}

httpd_uri_t post_uri = {
    .uri = "/submit",
    .method = HTTP_GET,
    .handler = post_handler,
    .user_ctx = NULL,
};

void fsm_http_server()
{
    switch (http_state)
    {
    case HTTP_SERVER_INIT:
    {
        /* ENABLE WIFI_EVENT HANDLER ALSO DHCP SERVER -> VERY IMPORTANT*/
        netif_default_wifi = esp_netif_create_default_wifi_ap();

        ESP_ERROR_CHECK(esp_event_handler_instance_register(
            WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFI_EVENT_handler, NULL,
            &event_handler_instance));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

        wifi_config_t ap_config = {
            .ap =
                {
                    .ssid = AP_SSID,
                    .ssid_len = strlen(AP_SSID),
                    .password = AP_PASS,
                    .max_connection = MAX_STA_CONNECTION,
                    .authmode = WIFI_AUTH_WPA_WPA2_PSK,
                },
        };

        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        /* Init HTTP server configuration */
        static httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.lru_purge_enable = true;
        // Start the httpd server
        ESP_LOGI(HTTP_SERVER_TAG, "Starting server on port: '%d'",
                 config.server_port);
        if (httpd_start(&server, &config) == ESP_OK)
        {
            // Set URI handlers
            ESP_LOGI(HTTP_SERVER_TAG, "Registering URI handlers");
            httpd_register_uri_handler(server, &root_uri);
            httpd_register_uri_handler(server, &post_uri);
        }

        set_HttpServerState(HTTP_SERVER_IDLE);
    }
    break;

    case HTTP_SERVER_DEINIT:
    {
        /* Close WEB server */
        ESP_LOGI(HTTP_SERVER_TAG, "UnRegistering URI handlers");
        httpd_stop(server);
        httpd_unregister_uri_handler(server, "/", HTTP_GET);
        httpd_unregister_uri_handler(server, "/submit", HTTP_GET);

        ESP_LOGI(HTTP_SERVER_TAG, "esp wifi stop");
        esp_wifi_stop();

        ESP_LOGI(HTTP_SERVER_TAG, "Unregister WIFI_HANDLER_handler");
        esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                     event_handler_instance);

        free(netif_default_wifi);

        set_HttpServerState(HTTP_SERVER_IDLE);
    }
    break;

    case HTTP_SERVER_IDLE:
    {
    }
    break;
    default:
        ESP_LOGI(HTTP_SERVER_TAG, "UNKNOWN STATE %d", http_state);
        break;
    }
}

/**
 * @brief WIFI callback
 *
 *
 * @param arg
 * @param event_base
 * @param event_id
 * @param event_data
 */
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
            ESP_LOGI(HTTP_SERVER_TAG, "retry to connect to the AP");
        }
        ESP_LOGI(HTTP_SERVER_TAG, "connect to the AP fail");
    }
    break;
    case WIFI_EVENT_AP_STACONNECTED:
    {
        wifi_event_ap_staconnected_t *event =
            (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(HTTP_SERVER_TAG, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    break;

    case WIFI_EVENT_AP_STADISCONNECTED:
    {
        wifi_event_ap_stadisconnected_t *event =
            (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(HTTP_SERVER_TAG, "station " MACSTR " leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    break;
    default:
        break;
    }
}
