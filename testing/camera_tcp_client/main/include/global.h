#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define USE_HTTP_SERVER 0
#define USE_TCP_SERVER 1
#define USE_MESH 1
#define USE_CAMERA 0

#define IS_ROOT 1

/* This is for TCP server config if device is root or not,
 * default config is NON-Root deivce.
 */
extern int isRoot;

#define MAX_ROUTER_SSID 32
#define MAX_ROUTER_PASS 64
extern char ROUTER_SSID[MAX_ROUTER_SSID];
extern char ROUTER_PASS[MAX_ROUTER_PASS];

#define AP_SSID "Esp_MyAP"
#define AP_PASS "khang123"
#define EXAMPLE_ESP_MAXIMUM_RETRY 3
#define MAX_STA_CONNECTION 2

/* The compiler wont let me define in `fsm_mesh.h` */
typedef enum
{
    MESH_NOT_ROOT,
    MESH_IS_ROOT,
    MESH_SUCCESS,
    MESH_ROOT_SEND2SERVER_FAIL,
    MESH_ROOT_SEND2SERVER_SUCCESS,
} MeshError_t;

typedef enum
{
    TCP_MESS_OK,
    TCP_MESS_NOT_OK,
    TCP_MESS_APPLY,
} TcpMess_Err_t;

extern const char *TCP_TAG;
extern const char *MESH_TAG;

int is_router_config();
void set_router_config(int);

int is_router_connected();
void set_router_connected(int);

#endif // End #ifndef __GLOBAL_H__