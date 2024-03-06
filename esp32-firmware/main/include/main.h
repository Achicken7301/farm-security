#ifndef __MAIN_H__
#define __MAIN_H__
#include <driver/timer.h>
#include <nvs_flash.h>

#include "fsm_camera.h"
#include "fsm_mesh.h"
#include "fsm_sta.h"
#include "fsm_tcp_server.h"
#include "global.h"
#include "sch_task.h"
#include "scheduler.h"

#if USE_HTTP_CLIENT
#include "fsm_http_client.h"
#endif

#if USE_HTTP_SERVER
#include "fsm_ap.h"
#include "fsm_http_server.h"
#endif

#endif // End #ifndef __MAIN_H__