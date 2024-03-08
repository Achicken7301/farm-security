#ifndef __GLOBAL_H__
#define __GLOBAL_H__

/* Config router ssid&password using tcp server, if root, config IPv4 server*/
#define USE_TCP_SERVER 0

/* Connect esp to router */
#define USE_STA 1

/* Connect esp - esp */
#define USE_MESH 1

/* Camera module */
#define USE_CAMERA 0

/* Http Client connects to Router (root only) */
#define USE_HTTP_CLIENT 1

#define I_AM_ROOT 0

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
    TCP_MESH_OK,
    TCP_MESH_NOT_OK,
    TCP_MESH_APPLY,
    TCP_MESH_WRONG_FORMAT,
    TCP_MESH_WRONG_SSID_FORMAT,
    TCP_MESH_WRONG_PASS_FORMAT,
} TcpMess_Err_t;

#define MAX_ROUTER_SSID 32
#define MAX_ROUTER_PASS 64
extern char ROUTER_SSID[MAX_ROUTER_SSID];
extern char ROUTER_PASS[MAX_ROUTER_PASS];

#define AP_SSID "Esp_MyAP"
#define AP_PASS "khang123"
#define EXAMPLE_ESP_MAXIMUM_RETRY 3
#define MAX_STA_CONNECTION 2

extern const char *TCP_TAG;
extern const char *MESH_TAG;

int is_router_config();
void set_router_config(int);

int is_router_connected();
void set_router_connected(int);

#endif // End #ifndef __GLOBAL_H__