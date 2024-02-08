#ifndef __GLOBAL_VARIABLES__
#define __GLOBAL_VARIABLES__
#include <stdint.h>
#include <stdio.h>

// static const char *TAG_STA = "wifi STA";
extern const char *TAG_softAP;
extern const char *TAG_webserver;
extern const char *TAG_STA;

#define MAX_ROUTER_SSID 32
#define MAX_ROUTER_PASS 64
extern char ROUTER_SSID[MAX_ROUTER_SSID];
extern char ROUTER_PASS[MAX_ROUTER_PASS];
extern uint8_t ROUTER_CHANNEL;

#endif // End #ifndef __GLOBAL_VARIABLES__