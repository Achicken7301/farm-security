#ifndef __FSM_WIFI_H__
#define __FSM_WIFI_H__

#include "global.h"
#include "wifi_event_handler.h"

// #include "ets_sys.h"
#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <string.h>

typedef enum
{
    /* WIFI STATES */
    WIFI_INIT,
    WIFI_START,
    WIFI_IDLE, /* I DONT KNOW WHY THIS EXIST??? */

    /* AP CONFIGURATION */
    WIFI_AP_INIT,
    WIFI_AP_CONFIGURATION,

    /* TODO: Switch this into different fsm HTTP SERVER, NEW IDEA, I should use
       scan wifi, to select channel better, in case router been power lost ->
       auto-connected without config with http-server */
    WEB_SERVER,
    CLOSE_WEB_SERVER,

    /* CONNECT TO ROUTER APSTA MODE */
    WIFI_STA_INIT,
    WIFI_STA_CONFIGURATION,

    /* LOST MODE */
    LOST_INIT,

} TransitionState;

#define AP_SSID "ESP_AP"
#define AP_PASS "khang123"
#define DEFAULT_ESP_WIFI_CHANNEL (1)
#define EXAMPLE_MAX_STA_CONN 6

void fsm_wifi();
void set_wifiState(TransitionState);
#endif // End #ifndef __FSM_WIFI_H__