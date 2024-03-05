#ifndef __FSM_CAMERA_H__
#define __FSM_CAMERA_H__

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include "esp_camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm_sta.h"

#include "scheduler.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#define BOARD_ESP32CAM_AITHINKER 1

#if BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 // software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22
#endif

#define UNKNOWN_STATE "-1"
#define RE_TAKE (10 * 1000)
#define ONCE (0)

typedef enum
{
    CAM_INIT,
    CAM_TAKE_PIC,
    CAM_CLEAR_PIC,
    CAM_DEINIT,
    CAM_DO_NOTHING,
} CameraState;

extern const char *FSM_CAMERA_TAG;
extern camera_fb_t *pic;
extern bool camIsReady;

void set_cState(CameraState);
void fsm_camera();

#endif // End #ifndef __FSM_CAMERA_H__