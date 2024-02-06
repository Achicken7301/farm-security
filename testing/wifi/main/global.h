#ifndef __GLOBAL_VARIABLES__
#define __GLOBAL_VARIABLES__
#include <stdint.h>
#include <stdio.h>

#define MAX_ROUTER_SSID 32
#define MAX_ROUTER_PASS 64
extern char ROUTER_SSID[MAX_ROUTER_SSID];
extern char ROUTER_PASS[MAX_ROUTER_PASS];
extern uint8_t ROUTER_CHANNEL;

#endif // End #ifndef __GLOBAL_VARIABLES__