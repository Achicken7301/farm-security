#ifndef __FSM_STA_H__
#define __FSM_STA_H__

#include <errno.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <lwip/dns.h>
#include <lwip/err.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <nvs_flash.h>
#include <string.h>

#include "global.h"

#define HOST_IP_ADDR "192.168.1.13"
#define PORT 5544
#define RE_SEND_MESS_TIME 5000

typedef enum
{
    /* In-order to send message to external IP, root node need to config TCP/IP
     * on Station Interface
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
    MESH_DEINIT,
    MESH_START,
    MESH_DO_NOTHING,
} MeshState;

extern char *payload;

void fsm_sta_init();

void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                      void *event_data);
#endif // End #ifndef __FSM_STA_H__