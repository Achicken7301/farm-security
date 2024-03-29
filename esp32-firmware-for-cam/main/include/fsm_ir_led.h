#ifndef __FSN_IR_LED_H__
#define __FSN_IR_LED_H__

#include <driver/gpio.h>

#define IR_LED_PIN GPIO_NUM_14

void ir_led_gpio_config();
void ir_led_on();
void ir_led_off();
#endif // End #ifndef __FSN_IR_LED_H__