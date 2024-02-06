#include "wifi_event_handler.h"

char ROUTER_SSID[MAX_ROUTER_SSID];
char ROUTER_PASS[MAX_ROUTER_PASS];

/* How to store this variables in flash??? */
static int is_router_config_t = 0;
static int is_router_connected_t = 0;

static const char *TAG_softAP = "wifi softAP";
static const char *TAG_STA = "wifi STA";
uint8_t ROUTER_CHANNEL = 0;
int s_retry_num = 0;

/**
 * @brief Public function - return if router is configuration
 *
 * @return (is config) ? true : false
 */
int is_router_config() { return is_router_config_t; }

/**
 * @brief Public function - return if router is configuration
 *
 * @return (is config) ? true : false
 */
int is_router_connected() { return is_router_connected_t; }

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
    "  <label for='router_channel'>ROUTER CHANNEL:</label>\n"
    "  <input type='text' id='router_channel'name='router_channel'><br><br>\n"
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
    httpd_query_key_value(query, "router_channel", (char *)ROUTER_CHANNEL,
                          sizeof(ROUTER_CHANNEL));

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

    is_router_config_t = 1;

    return ESP_OK;
}

httpd_uri_t post_uri = {
    .uri = "/submit",
    .method = HTTP_GET,
    .handler = post_handler,
    .user_ctx = NULL,
};

void WIFI_EVENT_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{

    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
    {
        esp_wifi_connect();
        is_router_connected_t = 1;
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
            ESP_LOGI(TAG_STA, "retry to connect to the AP");
        }
        ESP_LOGI(TAG_STA, "connect to the AP fail");
        is_router_config_t = 0;
        is_router_connected_t = 0;
        // currentState = WIFI_STA_CONFIGURATION;
        set_wifiState(WIFI_STA_CONFIGURATION);
    }
    break;
    case WIFI_EVENT_AP_STACONNECTED:
    {
        wifi_event_ap_staconnected_t *event =
            (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG_softAP, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    break;

    case WIFI_EVENT_AP_STADISCONNECTED:
    {
        wifi_event_ap_stadisconnected_t *event =
            (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG_softAP, "station " MACSTR " leave, AID=%d",
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
        ESP_LOGI(TAG_STA, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        break;
    }

    default:
        printf("IP_EVENT_handler: event_base: %s\tevent_id:%d\n", event_base,
               event_id);
        break;
    }
}