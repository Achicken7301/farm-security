#ifndef __FSM_STA_H__
#define __FSM_STA_H__

#include <errno.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <lwip/dns.h>
#include <lwip/err.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <netdb.h>
#include <nvs_flash.h>
#include <string.h>

#include "fsm_camera.h"
#include "fsm_mesh.h"
#include "global.h"
#include "sch_task.h"
#include "tcp_processing.h"

#define HOST_IP_ADDR "192.168.1.13"
#define PORT 8844
#define RE_SEND_MESS_TIME 5000

#define TX_BUFF_MAX (1 << 10)

void fsm_sta_init();
MeshError_t root_sendImage2server();

#endif // End #ifndef __FSM_STA_H__