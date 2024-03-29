/**
 * @file fsm_camera.c
 * @author Khang (banhbeovodung01@gmail.com)
 * @brief Required INIT Watch Dog (WD) and Enable PSRAM (component config ->
 * ESP32-spercific -> Support for external, SPI-connected RAM)
 * @version 0.1
 * @date 2024-03-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "fsm_camera.h"

const char *FSM_CAMERA_TAG = "CAMERA_TAG";

#if USE_CAMERA
CameraState cState = CAM_INIT;
#else
CameraState cState = CAM_DO_NOTHING;
#endif

camera_fb_t *pic = NULL;
int isCamReady = 1;

char *get_cState(CameraState state)
{
  switch (state)
  {
  case CAM_INIT:
    return "CAM_INIT";

  case CAM_TAKE_PIC:
    return "CAM_TAKE_PIC";
  default:
    return UNKNOWN_STATE;
  }
}

/**
 * @brief Set Camera State
 *
 *
 * @param CAM_INIT Initilize camera
 * @param CAM_TAKE_PIC Take picture and store to global `pic` variable
 * @param CAM_CLEAR_PIC
 * @param CAM_DEINIT
 * @param CAM_DO_NOTHING
 */
void set_cState(CameraState state)
{
  if (!strIsEqual(get_cState(state), UNKNOWN_STATE))
  {
    ESP_LOGI(FSM_CAMERA_TAG, "Current State %s", get_cState(state));
  }
  else
  {
    ESP_LOGI(FSM_CAMERA_TAG, "Unknown State %d", state);
  }
  cState = state;
}

void takePic() { set_cState(CAM_TAKE_PIC); }
void reInitCamera() { set_cState(CAM_INIT); }

void fsm_camera()
{
  switch (cState)
  {
  case CAM_INIT:
  {
    camera_config_t camera_config = {
        .pin_pwdn = CAM_PIN_PWDN,
        .pin_reset = CAM_PIN_RESET,
        .pin_xclk = CAM_PIN_XCLK,
        .pin_sccb_sda = CAM_PIN_SIOD,
        .pin_sccb_scl = CAM_PIN_SIOC,

        .pin_d7 = CAM_PIN_D7,
        .pin_d6 = CAM_PIN_D6,
        .pin_d5 = CAM_PIN_D5,
        .pin_d4 = CAM_PIN_D4,
        .pin_d3 = CAM_PIN_D3,
        .pin_d2 = CAM_PIN_D2,
        .pin_d1 = CAM_PIN_D1,
        .pin_d0 = CAM_PIN_D0,
        .pin_vsync = CAM_PIN_VSYNC,
        .pin_href = CAM_PIN_HREF,
        .pin_pclk = CAM_PIN_PCLK,

        // XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
        .xclk_freq_hz = 20000000,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = PIXFORMAT_JPEG, // YUV422,GRAYSCALE,RGB565,JPEG

        // QQVGA-UXGA, For ESP32, do not use sizes above QVGA
        // when not JPEG. The performance of the ESP32-S series
        // has improved a lot, but JPEG mode always gives
        // better frame rates.
        .frame_size = FRAMESIZE_VGA,
        // .frame_size = FRAMESIZE_QVGA,
        // .frame_size = FRAMESIZE_HD,
        // 0-63, for OV series camera sensors, lower number
        // means higher quality
        .jpeg_quality = 8,
        // When jpeg mode is used, if fb_count more than one, the
        // driver will work in continuous mode.
        .fb_count = 1,
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
    };

    // initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
      isCamReady = 0;
      ESP_LOGE(FSM_CAMERA_TAG, "Camera Init Failed");
      set_cState(CAM_DEINIT);
      break;
    }

    /* Transision base on module */
#if USE_MESH || USE_HTTP_CLIENT
    set_cState(CAM_DO_NOTHING);
#else
    set_cState(CAM_TAKE_PIC);
#endif
  }
  break;
  case CAM_CLEAR_PIC:
  {
    esp_camera_fb_return(pic);
    ESP_LOGI(FSM_CAMERA_TAG, "Cam is ready");
    isCamReady = 1;

    /* TODO: After taking picture, return to deepsleep mode, Fix this later */
    // SCH_Add(startDeepSleep, 1000, ONCE);

    set_cState(CAM_DO_NOTHING);
  }
  break;
  case CAM_TAKE_PIC:
  {
    if (isCamReady == 0)
    {
      // ESP_LOGE(FSM_CAMERA_TAG, "Camera is NOT ready");
      // set_cState(CAM_DO_NOTHING);
      break;
    }
#if USE_MESH
    if (connected2Root == 0)
    {
      // ESP_LOGE(FSM_CAMERA_TAG, "Not connected to root");
      // set_cState(CAM_DO_NOTHING);
      break;
    }
#endif // End #if USE_MESH

    ESP_LOGI(FSM_CAMERA_TAG, "Taking picture...");
    for (int i = 0; i < 4; i++)
    {
      pic = esp_camera_fb_get();
      esp_camera_fb_return(pic);
    }

    pic = esp_camera_fb_get();
    ESP_LOGI(FSM_CAMERA_TAG, "Picture taken! Its size was: %zu bytes", pic->len);

    isCamReady = 0;

    set_cState(CAM_DO_NOTHING);
  }
  break;
  case CAM_DEINIT:
  {
  }
  break;
  case CAM_DO_NOTHING:
  {
  }
  break;
  default:
    break;
  }
}

void sendPic2Mesh()
{
  if (isCamReady)
  {
    set_cState(CAM_TAKE_PIC);
  }
  else
  {
    ESP_LOGW(FSM_CAMERA_TAG, "Camera is not ready");
  }
}