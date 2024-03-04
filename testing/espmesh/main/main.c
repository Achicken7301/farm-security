#include "main.h"
// const char MESH_TAG = "ESP_MESH";
// Mesh task function
#define IMG_WIDTH (640)
#define IMG_LENGTH (400)
#define IMG_RESOLUTION (IMG_WIDTH * IMG_LENGTH)

void mesh_task()
{
    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (isMeshConnect())
        {
            if (esp_mesh_is_root())
            {
                // printf("I AM ROOT\n");
                uint8_t rx_buf[1500] = {
                    0,
                };
                mesh_addr_t from;
                mesh_data_t rx_data = {.size = 1500, .data = rx_buf};
                int flag;
                mesh_opt_t opt;
                mesh_rx_pending_t pending;
                esp_mesh_get_rx_pending(&pending);

                if (pending.toSelf > 0 || pending.toDS > 0)
                {
                    // ESP_LOGI(MESH_TAG, "Peding.toSelf %d\t Pending.toDs %d",
                    //  pending.toSelf, pending.toDS);
                    esp_err_t err = esp_mesh_recv(
                        &from, &rx_data, portMAX_DELAY, &flag, &opt, 1);
                    if (err == ESP_OK)
                    {

                        switch (rx_data.proto)
                        {
                        case MESH_PROTO_BIN:
                        {
                            ESP_LOGI(MESH_TAG, "MESH_PROTO_BIN");
                            if (rx_data.size == 0)
                            {
                                ESP_LOGI(MESH_TAG, "data.size: %d",
                                         rx_data.size);
                                break;
                            }
                            ESP_LOGI(MESH_TAG, "data.data: %s", rx_data.data);
                        }
                        break;
                        /* Node's station transmits datato root's AP */
                        case MESH_PROTO_AP:
                        {
                            ESP_LOGI(MESH_TAG, "MESH_PROTO_AP");
                        }
                        break;
                        /* sending from root AP -> Node's STA */
                        case MESH_PROTO_STA:
                        {
                            ESP_LOGI(MESH_TAG, "MESH_PROTO_STA");
                        }
                        break;
                        default:
                            break;
                        }
                    }
                    else
                    {
                        ESP_LOGI(MESH_TAG, "error 0x%x", err);
                    }
                }
            }
            else
            {
                // char *tx_data;

                // int size = asprintf(&tx_data,
                //                     "{"
                //                     "  \"from\": %s,"
                //                     "  \"with\": %s,"
                //                     "  \"hours\": %d"
                //                     "}",
                //                     "KHANG", "LOVE", 12);

                uint8_t tx_buffer[IMG_RESOLUTION];
                int size = sprintf(&tx_buffer, "From KHANG with LOVE\n");

                mesh_data_t data = {
                    .data = (uint8_t *)tx_buffer,
                    // data.proto = MESH_PROTO_BIN,
                    data.proto = MESH_PROTO_AP,
                    .size = size,
                    .tos = MESH_TOS_P2P,
                };
                ESP_LOGI(MESH_TAG, "data size: %d\t%s", data.size, data.data);
                esp_mesh_send(NULL, &data, MESH_PROTO_JSON, NULL, 1);
                // free(tx_data);
            }
        }
    }
}

void app_main(void)
{
    set_EspMeshState(MESH_INIT);
    // Create a task to handle mesh events
    // xTaskCreate(&mesh_task, "mesh_task", 4096, NULL, 5, &mesh_task);

    while (1)
    {
        fsm_espmesh();
    }
}
