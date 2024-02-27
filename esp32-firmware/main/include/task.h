#ifndef __TASK_H__
#define __TASK_H__

#include "fsm_mesh.h"
#include "global.h"

#include <esp_log.h>
#include <esp_mesh.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_TX_BUFFER 1460
void test_sch();
void send2server();

#endif // End #ifndef __TASK_H__