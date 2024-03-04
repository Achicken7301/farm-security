#include "main.h"

void app_main(void)
{
    if (ESP_OK != init_camera())
    {
        return;
    }

    while (1)
    {
        fsm_sta_init();
        ESP_LOGI("CAMERA_TAG", "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();

        // use pic->buf to access the image
        ESP_LOGI("CAMERA_TAG", "Picture taken! Its size was: %zu bytes",
                 pic->len);
        esp_camera_fb_return(pic);
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}
