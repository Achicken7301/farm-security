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

#include "driver/rtc_io.h"
#include "global.h"
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_sleep.h>
#include <string.h>

#define I_AM_LEAF 1
#define WAKE_UP_PIN 15

typedef enum PowerSaveState
{
  POWER_SAVE_INIT,
  POWER_SAVE_DEINIT,
  POWER_SAVE_DO_NOTHING,
  POWER_SAVE_START,
} PowerSaveState;

void set_psState(PowerSaveState);
void startDeepSleep();
void fsm_power_save();
#endif // End #ifndef __FSM_POWER_SAVE_H__