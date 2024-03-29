#ifndef __FSM_HC_501_H__
#define __FSM_HC_501_H__
#if USE_CAMERA
#include "fsm_camera.h"
#endif // End #if USE_CAMERA
#if USE_MESH
#include "fsm_mesh.h"
#endif // End #if USE_MESH
#include "fsm_ir_led.h"
#include "fsm_power_save.h"
#include "global.h"
#include "scheduler.h"

#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>

extern const char *HC_501_TAG;
extern int hc_501_buff;
extern int flag_hc501_long;

#define HC501_PIN 15

/* Input Pull-up high (hc_501_gpio_config()), so i need t switch this. */
#define HC501_HIGH 1
#define HC501_LOW 0

/* 30secs */
#define MAX_IDLE_TIME (30 * 1000 / CLOCK)

typedef enum
{
  HC501_FOUND,
  HC501_NOT_FOUND,
  HC501_FILTER_FOR_MULTI_FOUND, // i really dont know what is this LOL
} HC501state;

void set_hc501State(HC501state);
void hc_501_gpio_init();
void hc_501_reading();
int get_hc_501();
void fsm_hc501();
#endif // End #ifndef __FSM_HC_501_H__