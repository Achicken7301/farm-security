#ifndef __FSM_HTTP_CLIENT_H__
#define __FSM_HTTP_CLIENT_H__

#include <esp_event.h>
#include <esp_http_client.h>
#include <esp_log.h>
// #include <esp_netif.h>
#include <esp_system.h>
#include <esp_tls.h>
#include <nvs_flash.h>
#include <stdlib.h>
#include <string.h>

#include "fsm_camera.h"
#include "scheduler.h"

typedef enum
{
    HTTP_CLIENT_INIT,
    HTTP_CLIENT_DEINIT,
    HTTP_CLIENT_GET,
    HTTP_CLIENT_DO_NOTHING,
    HTTP_CLIENT_POST,
} HttpClientState;
#define UNKNOWN_STATE "-1"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
#define CONFIG_EXAMPLE_HTTP_ENDPOINT "192.168.1.13"

void fsm_http_client();
void set_hcState(HttpClientState);

#endif // End #ifndef __FSM_HTTP_CLIENT_H__
