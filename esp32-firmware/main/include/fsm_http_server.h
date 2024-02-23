#ifndef __FSM_HTTP_SERVER_H__
#define __FSM_HTTP_SERVER_H__

#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <string.h>

#include "global.h"

typedef enum
{
    HTTP_SERVER_INIT,
    HTTP_SERVER_DEINIT,
    HTTP_SERVER_IDLE,
} HttpServerState;

void set_HttpServerState(HttpServerState);
const char *get_HttpServerState(HttpServerState);
void WIFI_EVENT_handler(void *event_handler_arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);

void fsm_http_server();
#endif // End #ifndef __FSM_HTTP_SERVER_H__