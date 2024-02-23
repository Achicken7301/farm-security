#ifndef __FSM_MESH_H__
#define __FSM_MESH_H__

#include <esp_err.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_mesh.h>
#include <esp_mesh_internal.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <string.h>

#include "global.h"

#ifdef USE_HTTP_SERVER
#include "fsm_http_server.h"
#endif // End #ifdef USE_HTTP_SERVER

#define MESH_AP_CONNECTION 6

typedef enum
{
    MESH_INIT,
    MESH_DEINIT,
    MESH_DO_NOTHING,
} MeshState;

typedef enum
{
    MESH_NOT_ROOT,
    MESH_IS_ROOT,
    MESH_SUCCESS,
} MeshError;

void mesh_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);

void ip_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data);
const char *get_mState();
void set_mState(MeshState state);
void fsm_mesh();
#endif // End #ifndef __FSM_MESH_H__