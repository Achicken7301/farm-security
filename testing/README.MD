# Directory Structure

## Wifi (ESP32 to ESP8266)
Goals: User can config ESP32 connect to Router using HTTPD (Apache HTTP Server).

![fsm_wifi](../docs/fsm_wifi.png)*<center>X is dont care. Example: (0,X) is no matter `connected` value if `config` is 0 WIFI_START will switch to STA_INIT.</center>*

## EPS MESH (ESP to ESP)

![esp_mesh](../docs/mesh_output.png)*<center>**ESP_MESH:** OUTPUT ESP ROOT MESH.</center>*

## CAMERA_EXAMPLE

This is an example using `esp_camera.h` module to test OV2640 camera.


## Camera_tcp_client

Take picture from camera as formated as jpeg, then send to server Ipv4 ip_addr
![cam_tcp_client](../docs//received_picture.jpg)