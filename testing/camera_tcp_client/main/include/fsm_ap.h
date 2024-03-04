#ifndef __FSM_AP_H__
#define __FSM_AP_H__

#include "fsm_tcp_server.h"
#include "global.h"
// #include "wifi_event_handler.h"

// #include "ets_sys.h"
#include <esp_err.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <string.h>

char AP_BIND_ADDRESS[32];

#define MAX_STA_CONN 6

void fsm_ap_init();

#endif // End #ifndef __FSM_AP_H__