#include "fsm_power_save.h"
PowerSaveState psState = POWER_SAVE_INIT;
const char *POWER_SAVE_TAG = "POWER SAVE";

const int ext_wakeup_pin_0 = WAKE_UP_PIN;

void startDeepSleep() { set_psState(POWER_SAVE_INIT); }

char *get_psState(PowerSaveState state)
{
  switch (state)
  {
  case POWER_SAVE_INIT:
    return "POWER_SAVE_INIT";
  case POWER_SAVE_DEINIT:
    return "POWER_SAVE_DEINIT";
  default:
    return UNKNOWN_STATE;
  }
}

/**
 * @brief Set Power Save state
 *
 *
 * @param POWER_SAVE_INIT
 * @param POWER_SAVE_DEINIT
 * @param POWER_SAVE_DO_NOTHING
 */
void set_psState(PowerSaveState state)
{
  char *temp = get_psState(state);
  if (strcmp(temp, UNKNOWN_STATE))
  {
    ESP_LOGI(POWER_SAVE_TAG, "Current State %s", get_psState(state));
  }
  else
  {
    ESP_LOGI(POWER_SAVE_TAG, "Current State %d", (state));
  }

  psState = state;
}

void fsm_power_save()
{
  switch (psState)
  {
  case POWER_SAVE_INIT:
  {
    ESP_LOGI(POWER_SAVE_TAG, "Enabling EXT0 wakeup on pin GPIO%d", ext_wakeup_pin_0);
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(ext_wakeup_pin_0, 1));

    // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during
    // deepsleep. EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO
    // pullup/downs. No need to keep that power domain explicitly, unlike EXT1.
    ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_0));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_0));

    // Isolate GPIO12 pin from external circuits. This is needed for modules
    // which have an external pull-up resistor on GPIO12 (such as ESP32-WROVER)
    // to minimize current consumption.
    rtc_gpio_isolate(GPIO_NUM_12);

    set_psState(POWER_SAVE_DO_NOTHING);
  }
  break;
  case POWER_SAVE_DEINIT:
  {
    rtc_gpio_deinit(ext_wakeup_pin_0);
  }
  break;
  case POWER_SAVE_START:
  {
    ESP_LOGI(POWER_SAVE_TAG, "Entering Deep Sleep mode");
    esp_deep_sleep_start();
  }
  break;
  case POWER_SAVE_DO_NOTHING:
  {
  }
  break;
  default:
    break;
  }
}