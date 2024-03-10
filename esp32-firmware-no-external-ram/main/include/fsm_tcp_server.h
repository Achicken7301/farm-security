#ifndef __FSM_TCP_SERVER_H__
#define __FSM_TCP_SERVER_H__

#include <errno.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <netdb.h>
#include <nvs_flash.h>
// #include <socket.h>
#include <string.h>
#include <sys/socket.h>

#include "tcp_processing.h"

/*
TcpServer NonBlocking (prefix TS_)
The whole process base on this video
https://youtu.be/XXfdzwEsxFk?si=3Ti67II62hmWGJh0&t=130
 */
typedef enum
{
    /* Access Point FSM */
    AP_INIT,
    AP_DEINIT,
    AP_IDLE,

    /* TCP SEVER FSM */
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

extern TcpServerState tcpState;

/* This will be AP default gateway */
#define CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS "0.0.0.0"
#define CONFIG_EXAMPLE_TCP_SERVER_BIND_PORT "5544"

/**
 * @brief Indicates that the file descriptor represents an invalid
 * (uninitialized or closed) socket
 *
 * Used in the TCP server structure `sock[]` which holds list of active clients
 * we serve.
 */
#define INVALID_SOCK (-1)

void set_tcpState(TcpServerState);
void fsm_tcp_server_nonblocking();

#endif // End #ifndef __FSM_TCP_SERVER_H__