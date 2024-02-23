#ifndef __WIFI_EVENT_HANDLER_H__
#define __WIFI_EVENT_HANDLER_H__

#include "fsm_wifi.h"
#include "global.h"

#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <string.h>

#define EXAMPLE_ESP_MAXIMUM_RETRY 3

extern httpd_uri_t root_uri;
extern httpd_uri_t post_uri;

int is_router_config();
int is_router_connected();

/* WIFI_event_handler */
void WIFI_EVENT_handler(void *, esp_event_base_t, int32_t, void *);
void IP_EVENT_handler(void *, esp_event_base_t, int32_t, void *);

#endif // End #ifndef __WIFI_EVENT_HANDLER_H__