#include "global.h"

#include <stdint.h>
#include <stdio.h>

const char *TCP_TAG = "nonblocking-socket-server";

#if USE_HTTP_SERVER || USE_TCP_SERVER
/* Store this to flash */
#define MAX_ROUTER_SSID 32
#define MAX_ROUTER_PASS 64
char ROUTER_SSID[MAX_ROUTER_SSID];
char ROUTER_PASS[MAX_ROUTER_PASS];
#else
char ROUTER_SSID[MAX_ROUTER_SSID] = "Vnpt - Nha";
char ROUTER_PASS[MAX_ROUTER_PASS] = "0984012265";

#endif // End #ifdef USE_HTTP_SERVER

static int isRouterConfig = 0;
static int isRouterConnected = 0;

int is_router_config() { return isRouterConfig; }
void set_router_config(int value) { isRouterConfig = value; }

int is_router_connected() { return isRouterConnected; }
void set_router_connected(int value) { isRouterConnected = value; }