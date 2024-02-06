#ifndef __FSM_TCP_SERVER_H__
#define __FSM_TCP_SERVER_H__

#include "global.h"
#include "wifi_event_handler.h"

#include <errno.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <netdb.h>
#include <nvs_flash.h>
#include <string.h>
#include <sys/socket.h>

/*
TcpServer NonBlocking (prefix TS_)
The whole process base on this video
https://youtu.be/XXfdzwEsxFk?si=3Ti67II62hmWGJh0&t=130
 */
typedef enum
{
    TS_INIT,
    TS_CONFIG,
    TS_ERROR,
    TS_CREATE_SOCKET,
    TS_LISTEN,
    TS_BIND,
    TS_ACCEPT,
    TS_RECEIVE,
    TS_SEND,
    TS_DEINIT,
    TS_CLOSE
} TcpServerState;

#ifdef TCP_SERVER_DEFAULT_SETTING
#define CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS "0.0.0.0"
#define CONFIG_EXAMPLE_TCP_SERVER_BIND_PORT "5544"
#else
#define MAX_TCP_SERVER_BIND_ADDRESS 10
#define MAX_TCP_SERVER_BIND_PORT 10
extern char TCP_SERVER_BIND_ADDRESS[MAX_TCP_SERVER_BIND_ADDRESS];
extern char TCP_SERVER_BIND_PORT[MAX_TCP_SERVER_BIND_PORT];
#endif // End #ifdef TCP_SERVER_DEFAULT_SETTING

/**
 * @brief Indicates that the file descriptor represents an invalid
 * (uninitialized or closed) socket
 *
 * Used in the TCP server structure `sock[]` which holds list of active clients
 * we serve.
 */
#define INVALID_SOCK (-1)

void fsm_tcp_server_nonblocking();

#endif // End #ifndef __FSM_TCP_SERVER_H__