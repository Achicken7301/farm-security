#ifndef __FSM_WIFI_H__
#define __FSM_WIFI_H__

#include "global.h"
#include "wifi_event_handler.h"

#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <ets_sys.h>
#include <nvs_flash.h>
#include <string.h>

/* The examples use WiFi configuration that you can set via project
   configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define AP_SSID "ESP_AP"
#define AP_PASS "khang123"
#define DEFAULT_ESP_WIFI_CHANNEL (1)
#define EXAMPLE_MAX_STA_CONN 6

void fsm_wifi();

#endif // End #ifndef __FSM_WIFI_H__