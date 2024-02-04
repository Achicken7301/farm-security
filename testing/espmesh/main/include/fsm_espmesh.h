#ifndef __FSM_ESP_MESH_H__
#define __FSM_ESP_MESH_H__

#include <esp_event.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_mesh.h>
#include <esp_mesh_internal.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <string.h>

#define DEFAULT_MESH_CONFIG 1

/* Use HTTPD to configure this */
#define CONFIG_MESH_CHANNEL 5
#define CONFIG_MESH_ROUTER_SSID "Vnpt - Nha"
#define CONFIG_MESH_ROUTER_PASSWD "0984012265"
#define CONFIG_MESH_AP_CONNECTIONS 2
#define CONFIG_MESH_AP_PASSWD "0984012265"

typedef enum
{
    MESH_PREREQUISITE,
    MESH_INIT,
    MESH_CONFIGURATION,
    MESH_START,
    MESH_SEND,
    MESH_DEINIT,
    MESH_RELAX,
} EspMeshState;

extern const char *MESH_TAG;

void mesh_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);

void ip_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data);

void set_EspMeshState(EspMeshState);
const char *get_EspMeshState();

int get_IsMeshConnect();
void set_IsMeshConnect(int);

void fsm_espmesh();

#endif // End #ifndef __FSM_ESP_MESH_H__