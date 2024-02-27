#include "global.h"

#include <stdint.h>
#include <stdio.h>

const char *TCP_TAG = "nonblocking-socket-server";
const char *MESH_TAG = "MESH_TAG";

#if USE_HTTP_SERVER || USE_TCP_SERVER
char ROUTER_SSID[MAX_ROUTER_SSID];
char ROUTER_PASS[MAX_ROUTER_PASS];
#else
char ROUTER_SSID[MAX_ROUTER_SSID] = "ESP_AP_TEST";
char ROUTER_PASS[MAX_ROUTER_PASS] = "khang123";
#endif // End #ifdef USE_HTTP_SERVER

static int isRouterConfig = 0;
static int isRouterConnected = 0;

int is_router_config() { return isRouterConfig; }
void set_router_config(int value) { isRouterConfig = value; }

int is_router_connected() { return isRouterConnected; }
void set_router_connected(int value) { isRouterConnected = value; }