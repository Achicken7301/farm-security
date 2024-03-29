#include "global.h"
#include <stdint.h>
#include <stdio.h>

int NIGHT = 1;
int DAY = 0;

const char *TCP_TAG = "nonblocking-socket-server";
const char *MESH_TAG = "MESH_TAG";

#if USE_HTTP_SERVER || USE_TCP_SERVER
char ROUTER_SSID[MAX_ROUTER_SSID];
char ROUTER_PASS[MAX_ROUTER_PASS];
#else
char ROUTER_SSID[MAX_ROUTER_SSID] = "Vnpt - Nha";
char ROUTER_PASS[MAX_ROUTER_PASS] = "0984012265";
#endif

static int isRouterConfig = 0;
static int isRouterConnected = 0;

int is_router_config() { return isRouterConfig; }
void set_router_config(int value) { isRouterConfig = value; }

int is_router_connected() { return isRouterConnected; }
void set_router_connected(int value) { isRouterConnected = value; }
/**
 * @brief Compair 2 string
 *
 *
 * @param str1
 * @param str2
 * @return 1 is Equal 0 is NOT
 */
int strIsEqual(char *str1, char *str2) { return !strcmp(str1, str2); }