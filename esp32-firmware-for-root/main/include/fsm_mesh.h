#ifndef __FSM_MESH_H__
#define __FSM_MESH_H__

#include "global.h"
#include "sch_task.h"
#include "scheduler.h"
#include <esp_err.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_mesh.h>
#include <esp_mesh_internal.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <string.h>

#if FSM_CAMERA
#include "fsm_camera.h"
#endif // End #if FSM_CAMERA

#if USE_HTTP_SERVER
#include "fsm_http_server.h"
#endif // End #ifdef USE_HTTP_SERVER

#define MESH_MAX_RX_BUFF 1024
#define MESH_MAX_TX_BUFF 1024
#define UNKNOWN_STATE "-1"
#define mSTATE_DEBUG 0
#define DUMP_DATA_BUFF 16 // 16 bytes per line
#define DUMP_DATA_LINES 4 // 4 lines * 16 bytes each

#define MESH_AP_CONNECTION 6

typedef enum
{
  /* In-order to send message to external IP, root node need
   * to config TCP/IP on Station Interface
   * https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/esp-wifi-mesh.html#mesh-data-transmission:~:text=A%20TCP/IP%20layer%20is%20only%20required%20on%20the%20root%20node%20when%20it%20transmits/receives%20a%20packet%20to/from%20an%20external%20IP%20network.
   */
  MESH_STA_INTERFACE,
  /* Mesh from root send to server */
  MESH_SOCKET_INIT,
  MESH_SOCKET_SEND,
  MESH_SOCKET_RECEIVE,
  MESH_SOCKET_CLOSE,
  /* Mesh from leaf send data to root */
  MESH_LEAF_ROOT,

  MESH_INIT,
  MESH_SEND,
  MESH_RECEIVE,       // Receive image size
  MESH_RECEIVE_IMAGE, // Execute command code
  MESH_CLEAR_IMAGE_BUFF,
  MESH_DO_NOTHING,
} MeshState;

extern MeshState mState;
extern int bytes_receive;
extern uint8_t *pic_from_mesh;

void mesh_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);
void ip_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data);
const char *get_mState();
void set_mState(MeshState state);
void fsm_mesh();
#endif // End #ifndef __FSM_MESH_H__