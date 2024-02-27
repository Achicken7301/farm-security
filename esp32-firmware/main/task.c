#include "task.h"

const char *MESH_SEND_TAG = "ESP_MESH_SEND";

void test_sch() { ESP_LOGI("TESH SCHEDULER TAG", "Testing----------------"); }

/**
 * @brief Receive a package from leafs
 *
 */
void receive_root()
{ // printf("I AM ROOT\n");
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
        esp_err_t err =
            esp_mesh_recv(&from, &rx_data, portMAX_DELAY, &flag, &opt, 1);
        if (err == ESP_OK)
        {
            switch (rx_data.proto)
            {
            case MESH_PROTO_BIN:
            {
                ESP_LOGI(MESH_TAG, "MESH_PROTO_BIN");
                if (rx_data.size == 0)
                {
                    ESP_LOGI(MESH_TAG, "data.size: %d", rx_data.size);
                    break;
                }
                ESP_LOGI(MESH_TAG, "recevie from," MACSTR ",  data: %s",
                         MAC2STR(from.addr), rx_data.data);
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
/**
 * @brief
 * Receive a packet targeted to external IP network.
 *
 */
void receiveDS_root() {}

/**
 * @brief
 * This function will receive data from leafs then use tcp-socket and
 * send to server
 */
void send2server() { set_mState(MESH_SOCKET_INIT); }

/**
 * @brief
 * For leafs, send data ro root
 *
 */
void send2root(char *data_send)
{
    uint8_t tx_buffer[MAX_TX_BUFFER];
    if (sizeof(data_send) > MAX_TX_BUFFER)
    {
        ESP_LOGI(MESH_SEND_TAG, "sizeof(data_send) > MAX_TX_BUFFER");
    }

    int buff_size = sprintf(&tx_buffer, data_send);

    mesh_data_t data = {
        .data = (uint8_t *)tx_buffer,
        .proto = MESH_PROTO_BIN,
        // .proto = MESH_PROTO_HTTP,
        .size = buff_size,
        .tos = MESH_TOS_P2P,
    };

    /* BIN */
    ESP_LOGI(MESH_SEND_TAG, "Sending BIN protocol");
    esp_err_t mesh_err = esp_mesh_send(NULL, &data, MESH_DATA_P2P, NULL, 1);

    switch (mesh_err)
    {
    case ESP_OK:
    {
        ESP_LOGI(MESH_SEND_TAG, "Send to server successfuly");
    }
    break;

    default:
        ESP_LOGI(MESH_SEND_TAG, "New Error Code %d", mesh_err);
        break;
    }
}