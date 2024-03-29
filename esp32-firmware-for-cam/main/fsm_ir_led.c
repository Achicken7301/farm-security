#include "fsm_ir_led.h"

void ir_led_gpio_config()
{
  gpio_config_t led_cfg = {
      .intr_type = 0,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = 1ULL << IR_LED_PIN,
      .pull_down_en = 0,
      .pull_up_en = 0,
  };

  gpio_config(&led_cfg);

  gpio_set_level(IR_LED_PIN, 0);
}

void ir_led_on() { gpio_set_level(IR_LED_PIN, 1); }
void ir_led_off() { gpio_set_level(IR_LED_PIN, 0); }