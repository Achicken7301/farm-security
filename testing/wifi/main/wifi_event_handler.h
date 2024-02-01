#ifndef __WIFI_EVENT_HANDLER_H__
#define __WIFI_EVENT_HANDLER_H__

#include "global.h"
#include "wifi_event_handler.h"

#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <string.h>

extern httpd_uri_t root_uri;
extern httpd_uri_t post_uri;

#define WIFI_TESTING 1

#define MAX_ROUTER_SSID 32
#define MAX_ROUTER_PASS 64
extern char ROUTER_SSID[MAX_ROUTER_SSID];
extern char ROUTER_PASS[MAX_ROUTER_PASS];
extern uint8_t ROUTER_CHANNEL;

/* WIFI_event_handler */
#define EXAMPLE_ESP_MAXIMUM_RETRY 5

int is_router_config();
int is_router_connected();

// void AP_event_handler(void *, esp_event_base_t, int32_t, void *);
void WIFI_EVENT_handler(void *, esp_event_base_t, int32_t, void *);
void IP_EVENT_handler(void *, esp_event_base_t, int32_t, void *);

#endif // End #ifndef __WIFI_EVENT_HANDLER_H__