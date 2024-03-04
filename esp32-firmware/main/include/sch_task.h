#ifndef __SCH_TASK_H__
#define __SCH_TASK_H__

#include "fsm_camera.h"
#include "fsm_mesh.h"
#include "fsm_sta.h"
#include "global.h"

#include <esp_log.h>
#include <esp_mesh.h>
#include <lwip/dns.h>
#include <lwip/err.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_TX_BUFFER 1460
void test_sch();
void send2server();

#endif // End #ifndef __TASK_H__