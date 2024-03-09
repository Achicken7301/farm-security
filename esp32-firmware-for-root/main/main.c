#include "main.h"

void timerInit();
void startHttpClientGet() { set_hcState(HTTP_CLIENT_GET); }
void startHttpClientPost() { set_hcState(HTTP_CLIENT_POST); }

void testMesh()
{
  if (esp_mesh_is_root())
  {
    ESP_LOGI(MESH_TAG, "I AM ROOT");
    set_mState(MESH_RECEIVE);
  }
  else
  {
    // ESP_LOGI(MESH_TAG, "I AM CHILD");
    set_mState(MESH_SEND);
  }
}

void rootCheck()
{
  if (esp_mesh_is_root())
  {
    // ESP_LOGI(MESH_TAG, "I AM ROOT");
    set_mState(MESH_RECEIVE);
  }
}

void app_main(void)
{
#if USE_MESH || USE_HTTP_SERVER || USE_TCP_SERVER || USE_STA
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
#endif

  /* Timer Init */
  timerInit();

  // SCH_Add(testMesh, 15000, 5000);
  SCH_Add(rootCheck, 10000, 1000);

#if USE_TCP_SERVER
  set_tcpState(AP_INIT);
#endif

  while (1)
  {
#if USE_TCP_SERVER
    fsm_ap();
    fsm_tcp_server_nonblocking();
#endif
#if USE_STA
    fsm_sta();
#endif
#if USE_HTTP_CLIENT
    fsm_http_client();
#endif

#if USE_MESH
    fsm_mesh();
#endif
#if USE_CAMERA
    fsm_camera();
#endif
    SCH_Dispatch();
  }
}

/**
 * @brief Hardware timer call this function every 10ms
 *
 * @param arg
 */
bool TIM0_GROUP1_Callback(void *arg)
{
  // timerrun();
  SCH_Update();
  // Need to enable intr again
  // https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/api-reference/peripherals/timer.html#introduction:~:text=once%20triggered%2c%20the%20alarm%20is%20disabled%20automatically%20and%20needs%20to%20be%20re%2denabled%20to%20trigger%20again.
  ESP_ERROR_CHECK(timer_enable_intr(TIMER_GROUP_1, TIMER_0));

  return 1;
};

/**
 * @brief Config TIMER
 *
 */
void timerInit()
{

  timer_config_t ti_cfg = {
      .alarm_en = TIMER_ALARM_EN,
      .auto_reload = TIMER_AUTORELOAD_EN,
      .counter_dir = TIMER_COUNT_UP,
      .counter_en = TIMER_PAUSE,
      .divider = 80, // 16bit: pre-scaler
      .intr_type = TIMER_INTR_LEVEL,

  };

  ESP_ERROR_CHECK(timer_init(TIMER_GROUP_1, TIMER_0, &ti_cfg));
  ESP_ERROR_CHECK(timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0x00));

  /**
   * @brief 1 tick = 1us
   * Timer trigger alarm when counter == 10 * 1000 (10ms)
   *
   */
  ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_GROUP_1, TIMER_0, 10 * 1000));
  ESP_ERROR_CHECK(timer_enable_intr(TIMER_GROUP_1, TIMER_0));
  ESP_ERROR_CHECK(
      timer_isr_callback_add(TIMER_GROUP_1, TIMER_0, TIM0_GROUP1_Callback, NULL, 0));

  ESP_ERROR_CHECK(timer_start(TIMER_GROUP_1, TIMER_0));
}
