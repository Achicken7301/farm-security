#ifndef __TCP_PROCESSING_H__
#define __TCP_PROCESSING_H__

#include "fsm_tcp_server.h"
#include "global.h"

#include <esp_log.h>
#include <stdio.h>
#include <string.h>

#define CODE_SET "set"
#define VAR_SSID "ssid"
#define VAR_PASS "password"
#define VAR_APPLY "apply"

// TcpMess_Err_t process_rx_buffer(TcpMessageStructure *);
TcpMess_Err_t process_rx_buffer(char *, int len);
TcpMess_Err_t process_server_data(char *);
int strIsEqual(char *, char *);

#endif // End #ifndef __TCP_PROCESSING_H__