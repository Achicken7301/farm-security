#include "fsm_hc_501.h"
const char *HC_501_TAG = "HC_501";

HC501state hcState = HC501_NOT_FOUND;

int hc_501_buff;
int hc_501_buff1, hc_501_buff2;
int idle_counter = 0;

/**
 * @brief This reading every timer-interupt which is 10ms in this setting for
 * GROUP1_TIMER0
 *
 */
void hc_501_reading()
{
  idle_counter++;
  /* Avoid when setup hc501 gpio config, hc501_pin will get low and will trigger which
   * makes hc 501 state to 0 -> send image -> not good -> hard to debug (lol) */
  hc_501_buff2 = hc_501_buff1;
  hc_501_buff1 = gpio_get_level(HC501_PIN);
  if (hc_501_buff1 == hc_501_buff2)
  {
    hc_501_buff = hc_501_buff1;
  }
}

int get_hc_501() { return (hc_501_buff == HC501_HIGH); }

void hc_501_gpio_init()
{
  ESP_LOGI(HC_501_TAG, "Init hc501 sensor");
  gpio_config_t pir_hc501 = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_INPUT,
      .pin_bit_mask = 1ULL << HC501_PIN,
      .pull_down_en = 1,
      .pull_up_en = 0,
  };
  gpio_config(&pir_hc501);
}

void set_hc501State(HC501state state)
{
  ESP_LOGI(HC_501_TAG, "Current State %d", state);
  hcState = state;
}

void fsm_hc501()
{
  switch (hcState)
  {
  case HC501_FOUND:
  {
    if (DAY)
    {
      /* Days TODO: Wake up from sleep -> take picture -> send to root -> go back to
       * deepsleep */
#if USE_MESH || USE_CAMERA
      // set_cState(CAM_TAKE_PIC);
      // set_mState(MESH_SEND_IMAGE);
      // set_psState(POWER_SAVE_GPIO_WAKEUP_CONFIG);
#endif // End #if USE_MESH || USE_CAMERA
    }

    if (NIGHT)
    {
      /* Nights TODO: take picture -> send to root*/
      ir_led_on();
      SCH_Add(ir_led_off, 1000, ONCE);
#if USE_CAMERA
      set_cState(CAM_TAKE_PIC);
#endif // End #if USE_CAMERA
#if USE_MESH
      set_mState(MESH_SEND_IMAGE);
#endif // End #if USE_MESH || USE_CAMERA
    }

    /* Check condition to GET OUT of this state */
    if (get_hc_501())
    {
      set_hc501State(HC501_FILTER_FOR_MULTI_FOUND);
      break;
    }
    if (!get_hc_501())
    {
      set_hc501State(HC501_NOT_FOUND);
    }
  }
  break;
  case HC501_FILTER_FOR_MULTI_FOUND:
  {
    if (!get_hc_501())
    {
      set_hc501State(HC501_NOT_FOUND);
    }
  }
  break;
  case HC501_NOT_FOUND:
  {
    if (get_hc_501())
    {
      set_hc501State(HC501_FOUND);
      idle_counter = 0;
    }

    if (idle_counter > MAX_IDLE_TIME)
    {
      DAY = 0;
      NIGHT = 0;
      idle_counter = 0;
      SCH_Add(deep_sleep_wakeup_by_timer, 1000, ONCE);
    }
  }
  break;
  default:
    break;
  }
}