/* Deep sleep wake up example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "driver/adc.h"
#include "driver/rtc_io.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "esp32/ulp.h"

static RTC_DATA_ATTR struct timeval sleep_enter_time;

#define CONFIG_EXAMPLE_EXT0_WAKEUP 1

void app_main(void)
{

  struct timeval now;
  gettimeofday(&now, NULL);
  int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 +
                      (now.tv_usec - sleep_enter_time.tv_usec) / 1000;

  switch (esp_sleep_get_wakeup_cause())
  {
#if CONFIG_EXAMPLE_EXT0_WAKEUP
  case ESP_SLEEP_WAKEUP_EXT0:
  {
    printf("Wake up from ext0\n");
    break;
  }
#endif // CONFIG_EXAMPLE_EXT0_WAKEUP

  case ESP_SLEEP_WAKEUP_TIMER:
  {
    printf("Wake up from timer. Time spent in deep sleep: %dms\n", sleep_time_ms);
    break;
  }
  case ESP_SLEEP_WAKEUP_UNDEFINED:
  default:
    printf("Not a deep sleep reset\n");
  }

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  // const int wakeup_time_sec = 50;
  // printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
  // ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000));

#if CONFIG_EXAMPLE_EXT0_WAKEUP
#if CONFIG_IDF_TARGET_ESP32
  const int ext_wakeup_pin_0 = 15;
#else
  const int ext_wakeup_pin_0 = 3;
#endif

  printf("Enabling EXT0 wakeup on pin GPIO%d\n", ext_wakeup_pin_0);
  ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(ext_wakeup_pin_0, 1));

  // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during
  // deepsleep. EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO
  // pullup/downs. No need to keep that power domain explicitly, unlike EXT1.
  ESP_ERROR_CHECK(rtc_gpio_pullup_dis(ext_wakeup_pin_0));
  ESP_ERROR_CHECK(rtc_gpio_pulldown_en(ext_wakeup_pin_0));
#endif // CONFIG_EXAMPLE_EXT0_WAKEUP

#if CONFIG_IDF_TARGET_ESP32
  // Isolate GPIO12 pin from external circuits. This is needed for modules
  // which have an external pull-up resistor on GPIO12 (such as ESP32-WROVER)
  // to minimize current consumption.
  rtc_gpio_isolate(GPIO_NUM_12);
#endif

  printf("Entering deep sleep\n");
  gettimeofday(&sleep_enter_time, NULL);

  esp_deep_sleep_start();
}
