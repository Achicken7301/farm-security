#ifndef __SCAN_WIFI_H__
#define __SCAN_WIFI_H__

#include <esp_event.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <nvs_flash.h>
#include <string.h>

void wifi_scan();
#endif // End #ifndef __SCAN_WIFI_H__