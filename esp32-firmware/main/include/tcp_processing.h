#ifndef __TCP_PROCESSING_H__
#define __TCP_PROCESSING_H__

#include "fsm_tcp_server.h"
#include "global.h"

#include <esp_log.h>
#include <stdio.h>
#include <string.h>

#define CODE_SET "set"
#define VAR_SSID "ssid"
#define VAR_PASS "pass"
#define VAR_APPLY "aply"

typedef union tcp_processing
{
    /* set ssid <ssid-data> */
    /* set pass <pass-data> */
    char data[64];
    struct
    {
        /* TODO: I"LL OPTIMIZE THIS LATER */
        char code[3];
        char _space01[1];
        char var[4];
        char _space02[1];
        char var_data[64 - 4 - 3 - 2];
    };
} TcpMessageStructure;

typedef enum
{
    TCPMESS_OK,
    TCPMESS_NOT_OK,
    TCPMESS_APPLY,
} TcpMess_Err_t;

TcpMess_Err_t process_rx_buffer(TcpMessageStructure *);
int strIsEqual(char *, char *);

#endif // End #ifndef __TCP_PROCESSING_H__