#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define USE_TCP_SERVER 1
#define USE_HTTP_SERVER 0

#define MAX_ROUTER_SSID 32
#define MAX_ROUTER_PASS 64

#if USE_HTTP_SERVER || USE_TCP_SERVER
/* Store this to flash */
extern char ROUTER_SSID[MAX_ROUTER_SSID];
extern char ROUTER_PASS[MAX_ROUTER_PASS];
#else
extern char ROUTER_SSID[MAX_ROUTER_SSID];
extern char ROUTER_PASS[MAX_ROUTER_PASS];
#endif // End #ifdef USE_HTTP_SERVER

#define AP_SSID "Esp_MyAP"
#define AP_PASS "khang123"
#define EXAMPLE_ESP_MAXIMUM_RETRY 3
#define MAX_STA_CONNECTION 2

extern const char *TCP_TAG;

int is_router_config();
void set_router_config(int);

int is_router_connected();
void set_router_connected(int);

#endif // End #ifndef __GLOBAL_H__