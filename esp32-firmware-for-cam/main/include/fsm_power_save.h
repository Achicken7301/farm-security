/**
 * @file fsm_power_save.h
 * @author your name (you@domain.com)
 * @brief In Daylight, switch to deep sleep mode which wake up when have interupt from
 * HC-501 IR sensor
 * @date 2024-03-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __FSM_POWER_SAVE_H__
#define __FSM_POWER_SAVE_H__

#include "fsm_hc_501.h"
#include "global.h"
#include "scheduler.h"

#if USE_MESH
#include "fsm_mesh.h"
#endif // End #if USE_MESH

#if USE_CAMERA
#include "fsm_camera.h"
#endif // End #if USE_CAMERA

#include <driver/gpio.h>
#include <driver/rtc_io.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_mesh.h>
#include <esp_sleep.h>
#include <esp_wifi.h>
#include <string.h>

#define I_AM_LEAF 1
#define TIMER_WAKEUP_TIME_IN_SEC 30

typedef enum PowerSaveState
{
  POWER_SAVE_INIT,
  POWER_SAVE_DEINIT,
  POWER_SAVE_GPIO_WAKEUP_CONFIG,
  POWER_SAVE_TIMER_WAKEUP_CONFIG,
  POWER_SAVE_DO_NOTHING,
  POWER_SAVE_START,
} PowerSaveState;

void set_psState(PowerSaveState);
void startDeepSleep();
void deep_sleep_wakeup_by_timer();
void gpio_init();
void fsm_power_save();
#endif // End #ifndef __FSM_POWER_SAVE_H__