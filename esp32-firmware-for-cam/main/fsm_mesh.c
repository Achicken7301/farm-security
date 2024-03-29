#include "fsm_mesh.h"

#if USE_STA
MeshState mState = MESH_STA_INTERFACE;
#elif USE_MESH
MeshState mState = MESH_INIT;
#endif
int connected2Root = 0;
/* Data init */
uint8_t rx_buf[1460] = {
    0,
};

const uint8_t MESH_ID[6] = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
mesh_addr_t id;
uint16_t mesh_layer;

/* Root */
mesh_addr_t mesh_parent_addr;
int last_layer = -1;

void send_pic_from_mesh_to_server() {}

/**
 * @brief Set Mesh State
 *
 * @param MESH_STA_INTERFACE,
 * @param MESH_SOCKET_INIT,
 * @param MESH_SOCKET_SEND,
 * @param MESH_SOCKET_RECEIVE,
 * @param MESH_SOCKET_CLOSE,
 * @param MESH_LEAF_ROOT,
 * @param MESH_INIT,
 * @param MESH_DEINIT,
 * @param MESH_START,
 * @param MESH_DO_NOTHING,
 */
void set_mState(MeshState state)
{
  mState = state;
  if (strcmp(get_mState(), UNKNOWN_STATE))
  {
    ESP_LOGI(MESH_TAG, "Current State %s", get_mState());
  }
  else
  {
    ESP_LOGI(MESH_TAG, "Current State 0x%02x", state);
  }
}

void dumpData(uint8_t *buff)
{
  ESP_LOGI(MESH_TAG,
           "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x "
           "%02x %02x %02x",
           buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7],
           buff[8], buff[9], buff[10], buff[11], buff[12], buff[13], buff[14], buff[15]);
}

const char *get_mState()
{
  switch (mState)
  {
  case MESH_STA_INTERFACE:
    return "MESH_STA_INTERFACE";
  case MESH_SOCKET_INIT:
    return "MESH_SOCKET_INIT";
  case MESH_SOCKET_SEND:
    return "MESH_SOCKET_SEND";
  case MESH_SOCKET_CLOSE:
    return "MESH_SOCKET_CLOSE";
  case MESH_INIT:
    return "MESH_INIT";
  case MESH_SEND_IMAGE:
    return "MESH_SEND_IMAGE";
  case MESH_RECEIVE:
    return "MESH_RECEIVE";
  case MESH_DO_NOTHING:
    return "MESH_DO_NOTHING";
  default:
    return UNKNOWN_STATE;
  }
}

MeshError_t imageSendCheck(esp_err_t *err_type)
{
  switch (*err_type)
  {
  case ESP_OK:
  {
    return MESH_ROOT_SEND2SERVER_SUCCESS;
  }
  default:
  {
    ESP_LOGE(MESH_TAG, "Send Error Code 0x%02x", *err_type);
    return MESH_ROOT_SEND2SERVER_FAIL;
  }
  }
}

void fsm_mesh()
{
  switch (mState)
  {
  case MESH_INIT:
  {
#if USE_HTTP_SERVER || USE_TCP_SERVER
    if (!is_router_config())
    {
      break;
    }
#endif
#if !USE_STA
    /* Mesh need this to scan for leaves, BUT when init station mode, we
     * already use this. SO that use #if #endif here for */
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_start());
#endif
    /*  mesh initialization */
    ESP_ERROR_CHECK(esp_mesh_init());
    esp_mesh_enable_ps();

    /*  register mesh events handler */
    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID,
                                               &mesh_event_handler, NULL));

    /* Enable the Mesh IE encryption by default */
    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
    /* mesh ID */
    memcpy(&cfg.mesh_id, MESH_ID, 6);
    /* router */
    cfg.router.ssid_len = strlen(ROUTER_SSID);
    memcpy(&cfg.router.ssid, ROUTER_SSID, cfg.router.ssid_len);
    memcpy(&cfg.router.password, ROUTER_PASS, strlen(ROUTER_PASS));
    /* mesh softAP */
    cfg.mesh_ap.max_connection = MESH_AP_CONNECTION;
    memcpy(&cfg.mesh_ap.password, ROUTER_PASS, strlen(ROUTER_PASS));

    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));
    esp_mesh_set_self_organized(true, true);
    /* mesh start */
    ESP_ERROR_CHECK(esp_mesh_start());

    ESP_LOGI(MESH_TAG, "Start connecting....");
    esp_mesh_connect();

    set_mState(MESH_DO_NOTHING);
  }
  break;
  case MESH_SEND_IMAGE:
  {
    /* isCamReady = 0 which mean picture is taken wait for clear_pic */
    if (connected2Root == 0)
    {
      // ESP_LOGE(MESH_TAG, "Not connected to Root");
      // set_mState(MESH_DO_NOTHING);
      /* TODO: no connected to root, set power save mode to deep sleep */
      break;
    }

    /* Cam is ready which mean not taking pictures -> no data */
    if (isCamReady == 1)
    {
      // ESP_LOGE(MESH_TAG, "isCamReady = 1");
      // set_mState(MESH_DO_NOTHING);
      break;
    }

    esp_err_t mesh_send_err;
    int package = 0;
    int bytes_left = pic->len % MESH_MAX_TX_BUFF;

    mesh_data_t mesh_tx_data = {
        .tos = MESH_TOS_P2P,
    };

    /* Send Image Size for root allocate memories */
    uint8_t payload[50];

    memcpy(payload, &pic->len, sizeof(int));
    mesh_tx_data.proto = MESH_PROTO_HTTP;
    mesh_tx_data.data = (uint8_t *)payload;
    mesh_tx_data.size = sizeof(pic->len);
    mesh_send_err = esp_mesh_send(NULL, &mesh_tx_data, MESH_DATA_P2P, NULL, 0);
    if (imageSendCheck(&mesh_send_err) != MESH_ROOT_SEND2SERVER_SUCCESS)
    {
      set_cState(CAM_CLEAR_PIC);
      set_mState(MESH_DO_NOTHING);
      break;
    }

    ESP_LOGI(MESH_TAG, "Mesh Error Code 0x%02x",
             (mesh_send_err == ESP_OK) ? ESP_OK : mesh_send_err);

    /* Send Image Data */
    ESP_LOGI(MESH_TAG, "%d/%d=%d packages, %d bytes left", pic->len, MESH_MAX_TX_BUFF,
             pic->len / MESH_MAX_TX_BUFF, bytes_left);
    mesh_tx_data.proto = MESH_PROTO_BIN;
    for (package = 0; package < (pic->len / MESH_MAX_TX_BUFF); package++)
    {
      mesh_tx_data.data = &pic->buf[package * MESH_MAX_TX_BUFF];
      mesh_tx_data.size = MESH_MAX_TX_BUFF;
      mesh_send_err = esp_mesh_send(NULL, &mesh_tx_data, MESH_DATA_P2P, NULL, 0);
      if (imageSendCheck(&mesh_send_err) != MESH_ROOT_SEND2SERVER_SUCCESS)
      {
        set_cState(CAM_CLEAR_PIC);
        set_mState(MESH_DO_NOTHING);
        break;
      }
    }

    ESP_LOGI(MESH_TAG, "Last package %dbytes", bytes_left);
    mesh_tx_data.data = &pic->buf[package * MESH_MAX_TX_BUFF];
    mesh_tx_data.size = bytes_left;
    mesh_send_err = esp_mesh_send(NULL, &mesh_tx_data, MESH_DATA_P2P, NULL, 0);
    if (imageSendCheck(&mesh_send_err) != MESH_ROOT_SEND2SERVER_SUCCESS)
    {
      set_cState(CAM_CLEAR_PIC);
      set_mState(MESH_DO_NOTHING);
      break;
    }

#if USE_CAMERA
    set_cState(CAM_CLEAR_PIC);
    set_mState(MESH_DO_NOTHING);
#else
    set_mState(MESH_DO_NOTHING);
#endif
  }
  break;
  default:
    break;
  }
}

void mesh_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                        void *event_data)
{
  switch (event_id)
  {
  case MESH_EVENT_STARTED:
  {

    esp_mesh_get_id(&id);
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_MESH_STARTED>ID:" MACSTR "", MAC2STR(id.addr));
    mesh_layer = esp_mesh_get_layer();

    ESP_LOGI(MESH_TAG, "Allow Scan but forgo the election process");
#if !I_AM_ROOT
    esp_mesh_fix_root(true);
#endif // End #if !I_AM_ROOT
  }
  break;
  case MESH_EVENT_ROOT_ADDRESS:
  {
    mesh_event_root_address_t *root_addr = (mesh_event_root_address_t *)event_data;
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_ROOT_ADDRESS>root address:" MACSTR "",
             MAC2STR(root_addr->addr));

    connected2Root = 1;
  }
  break;
  case MESH_EVENT_TODS_STATE:
  {
    mesh_event_toDS_state_t *toDs_state = (mesh_event_toDS_state_t *)event_data;
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d", *toDs_state);
  }
  break;

  case MESH_EVENT_ROUTING_TABLE_ADD:
  {
    mesh_event_routing_table_change_t *routing_table =
        (mesh_event_routing_table_change_t *)event_data;
    ESP_LOGW(MESH_TAG, "<MESH_EVENT_ROUTING_TABLE_ADD>add %d, new:%d, layer:%d",
             routing_table->rt_size_change, routing_table->rt_size_new, mesh_layer);
  }
  break;
  case MESH_EVENT_CHILD_CONNECTED:
  {
    mesh_event_child_connected_t *child_connected =
        (mesh_event_child_connected_t *)event_data;
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_CHILD_CONNECTED>aid:%d, " MACSTR "",
             child_connected->aid, MAC2STR(child_connected->mac));
  }
  break;
  case MESH_EVENT_PARENT_CONNECTED:
  {
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_PARENT_CONNECTED>");
    mesh_event_connected_t *connected = (mesh_event_connected_t *)event_data;
    esp_mesh_get_id(&id);
    mesh_layer = connected->self_layer;
    strcpy((char *)mesh_parent_addr.addr, (char *)connected->connected.bssid);
    ESP_LOGI(MESH_TAG,
             "<MESH_EVENT_PARENT_CONNECTED>layer:%d-->%d, parent:" MACSTR "%s, ID:" MACSTR
             ", duty:%d",
             last_layer, mesh_layer, MAC2STR(mesh_parent_addr.addr),
             esp_mesh_is_root()  ? "<ROOT>"
             : (mesh_layer == 2) ? "<layer2>"
                                 : "",
             MAC2STR(id.addr), connected->duty);
    last_layer = mesh_layer;
  }
  break;
  case MESH_EVENT_PARENT_DISCONNECTED:
  {
    mesh_event_disconnected_t *disconnected = (mesh_event_disconnected_t *)event_data;
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_PARENT_DISCONNECTED>reason:%d", disconnected->reason);
    mesh_layer = esp_mesh_get_layer();

    /* TODO: Go to deep-sleep, wake-up 30s later to check */
    SCH_Add(deep_sleep_wakeup_by_timer, 1000, ONCE);
  }
  break;
  case MESH_EVENT_FIND_NETWORK:
  {
    mesh_event_find_network_t *find_network = (mesh_event_find_network_t *)event_data;
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_FIND_NETWORK>new channel:%d, router BSSID:" MACSTR "",
             find_network->channel, MAC2STR(find_network->router_bssid));
  }
  break;
  case MESH_EVENT_PS_PARENT_DUTY:
  {
    mesh_event_ps_duty_t *ps_duty = (mesh_event_ps_duty_t *)event_data;
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_PS_PARENT_DUTY>duty:%d", ps_duty->duty);
  }
  break;
  case MESH_EVENT_NETWORK_STATE:
  {
    mesh_event_network_state_t *network_state = (mesh_event_network_state_t *)event_data;
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_NETWORK_STATE>is_rootless:%d",
             network_state->is_rootless);
#if !I_AM_ROOT
    esp_mesh_fix_root(true);
    connected2Root = 0;
    /* TODO: Go to deep-sleep, wake-up 30s later to check */
    SCH_Add(deep_sleep_wakeup_by_timer, 1000, ONCE);
#endif // End #if !I_AM_ROOT
  }
  break;

  case MESH_EVENT_PS_CHILD_DUTY:
  {
    mesh_event_ps_duty_t *ps_duty = (mesh_event_ps_duty_t *)event_data;
    ESP_LOGI(MESH_TAG, "<MESH_EVENT_PS_CHILD_DUTY>cidx:%d, " MACSTR ", duty:%d",
             ps_duty->child_connected.aid - 1, MAC2STR(ps_duty->child_connected.mac),
             ps_duty->duty);
  }
  break;
#if !I_AM_ROOT
  case MESH_EVENT_ROOT_FIXED:
  {
    ESP_LOGI(MESH_TAG, "MESH_EVENT_ROOT_FIXED");

    /* TODO: Deinit all camera in scheduler */
  }
  break;
  case MESH_EVENT_NO_PARENT_FOUND:
  {
    ESP_LOGI(MESH_TAG, "MESH_EVENT_NO_PARENT_FOUND");

    /* TODO: Deep sleep mode for 30mins or something then wake up and scan for 5mins */
  }
  break;
#endif // End #if I_AM_ROOT
  default:
    ESP_LOGI(MESH_TAG, "mesh_event_handler event_id: %d", event_id);
    break;
  }
}

void meshSend() { set_mState(MESH_SEND_IMAGE); }