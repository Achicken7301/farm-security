#ifndef __MAIN_H__
#define __MAIN_H__

#include "fsm_power_save.h"
#include "global.h"
#include "sch_task.h"
#include "scheduler.h"

#include <driver/timer.h>
#include <nvs_flash.h>

#if USE_CAMERA
#include "fsm_camera.h"
#endif // End #if USE_CAMERA

#if USE_MESH
#include "fsm_mesh.h"
#endif // End #if USE_MESH

#if USE_TCP_SERVER
#include "fsm_ap.h"
#include "fsm_tcp_server.h"
#endif

#if USE_HTTP_CLIENT
#include "fsm_http_client.h"
#include "fsm_sta.h"
#endif

#if USE_HTTP_SERVER
#include "fsm_ap.h"
#include "fsm_http_server.h"
#endif

#endif // End #ifndef __MAIN_H__