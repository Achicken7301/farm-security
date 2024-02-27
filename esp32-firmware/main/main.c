#include "main.h"

void timerInit();

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    /* Timer Init */
    timerInit();

    set_tcpState(AP_INIT);

    while (1)
    {
#if USE_HTTP_SERVER
        /* HTPP SERVER && MESH cant switch between these, YET!!! */
        fsm_http_server();
#endif // End #ifdef USE_HTTP_SERVER
#if USE_TCP_SERVER
        fsm_ap_init();
        fsm_tcp_server_nonblocking();
#endif
        fsm_sta_init();
        fsm_mesh();
        SCH_Dispatch();
    }
}

/**
 * @brief Hardware timer call this function every 10ms
 *
 * @param arg
 */
bool TIM0_GROUP0_Callback(void *arg)
{
    // timerrun();
    SCH_Update();
    // Need to enable intr again
    // https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/api-reference/peripherals/timer.html#introduction:~:text=once%20triggered%2c%20the%20alarm%20is%20disabled%20automatically%20and%20needs%20to%20be%20re%2denabled%20to%20trigger%20again.
    ESP_ERROR_CHECK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));

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

    ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, &ti_cfg));
    ESP_ERROR_CHECK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00));

    /**
     * @brief 1 tick = 1us
     * Timer trigger alarm when counter == 10 * 1000 (10ms)
     *
     */
    ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 10 * 1000));
    ESP_ERROR_CHECK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
    ESP_ERROR_CHECK(timer_isr_callback_add(TIMER_GROUP_0, TIMER_0,
                                           TIM0_GROUP0_Callback, NULL, 0));

    ESP_ERROR_CHECK(timer_start(TIMER_GROUP_0, TIMER_0));
}
