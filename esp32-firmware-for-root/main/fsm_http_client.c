#include "fsm_http_client.h"

HttpClientState hcState = HTTP_CLIENT_DO_NOTHING;

const char *HTTP_CLIENT_TAG = "HTTP_CLIENT";
const char *get_hcState(HttpClientState state)
{
    switch (state)
    {
    case HTTP_CLIENT_INIT:
        return "HTTP_CLIENT_INIT";
    default:
        return UNKNOWN_STATE;
    }
}

void set_hcState(HttpClientState state)
{
    if (strcmp(get_hcState(state), UNKNOWN_STATE))
    {
        ESP_LOGI(HTTP_CLIENT_TAG, "Current State: %s", get_hcState(state));
    }
    else
    {
        ESP_LOGI(HTTP_CLIENT_TAG, "Unknown State: %d", state);
    }
    hcState = state;
}

char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
esp_http_client_handle_t client;

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from
                                // event handler
    static int output_len;      // Stores number of bytes read
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s",
                 evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        /*
         *  Check for chunked encoding is added as the URL for chunked encoding
         * used in this example returns binary data. However, event handler can
         * also be used in case chunked encoding is used.
         */
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // If user_data buffer is configured, copy the response into the
            // buffer
            if (evt->user_data)
            {
                memcpy(evt->user_data + output_len, evt->data, evt->data_len);
            }
            else
            {
                if (output_buffer == NULL)
                {
                    output_buffer = (char *)malloc(
                        esp_http_client_get_content_length(evt->client));
                    output_len = 0;
                    if (output_buffer == NULL)
                    {
                        ESP_LOGE(HTTP_CLIENT_TAG,
                                 "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                memcpy(output_buffer + output_len, evt->data, evt->data_len);
            }
            output_len += evt->data_len;
        }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_FINISH");
        if (output_buffer != NULL)
        {
            // Response is accumulated in output_buffer. Uncomment the below
            // line to print the accumulated response
            // ESP_LOG_BUFFER_HEX(HTTP_CLIENT_TAG, output_buffer, output_len);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(HTTP_CLIENT_TAG, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err =
            esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            ESP_LOGI(HTTP_CLIENT_TAG, "Last esp error code: 0x%x", err);
            ESP_LOGI(HTTP_CLIENT_TAG, "Last mbedtls failure: 0x%x",
                     mbedtls_err);
        }
        if (output_buffer != NULL)
        {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    }
    return ESP_OK;
}

void taskHttpGet() { set_hcState(HTTP_CLIENT_GET); }
void taskHttpPost() { set_hcState(HTTP_CLIENT_POST); }
void fsm_http_client()
{
    switch (hcState)
    {
    case HTTP_CLIENT_INIT:
    {
        /**
         * NOTE: All the configuration parameters for http_client must be
         * spefied either in URL or as host and path parameters. If host and
         * path parameters are not set, query parameter will be ignored. In such
         * cases, query parameter should be specified in URL.
         *
         * If URL as well as host and path parameters are specified, values of
         * host and path will be considered.
         */
        esp_http_client_config_t config = {
            .host = CONFIG_EXAMPLE_HTTP_ENDPOINT,
            .port = 8080,
            .path = "/api",
            // .query = "esp",
            .event_handler = _http_event_handler,
            .user_data = local_response_buffer, // Pass address of local buffer
                                                // to get response
            .disable_auto_redirect = true,
        };
        client = esp_http_client_init(&config);

        // SCH_Add(taskHttpGet, 5000, ONCE);
        // SCH_Add(taskHttpPost, 5000, ONCE);
        set_hcState(HTTP_CLIENT_DO_NOTHING);
    }
    break;
    case HTTP_CLIENT_GET:
    {
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK)
        {
            ESP_LOGI(HTTP_CLIENT_TAG,
                     "HTTP GET Status = %d, content_length = %d",
                     esp_http_client_get_status_code(client),
                     esp_http_client_get_content_length(client));
        }
        else
        {
            ESP_LOGE(HTTP_CLIENT_TAG, "HTTP GET request failed: %s",
                     esp_err_to_name(err));
        }
        ESP_LOG_BUFFER_HEX(HTTP_CLIENT_TAG, local_response_buffer,
                           strlen(local_response_buffer));

        // SCH_Add(taskHttpGet, 5000, ONCE);
        set_hcState(HTTP_CLIENT_DO_NOTHING);
    }
    break;
    case HTTP_CLIENT_POST:
    {
        esp_http_client_config_t config = {
            .host = CONFIG_EXAMPLE_HTTP_ENDPOINT,
            .path = "/get",
            .port = 8080,
            .transport_type = HTTP_TRANSPORT_OVER_TCP,
            .event_handler = _http_event_handler,
        };
        esp_http_client_handle_t client_post = esp_http_client_init(&config);

        // POST
        // const char *post_data = "{\"field1\":\"value1\"}";
        esp_http_client_set_url(client_post, "/api/upload");
        esp_http_client_set_method(client_post, HTTP_METHOD_POST);

#if USE_CAMERA
        esp_http_client_set_header(client_post, "Content-Type", "image/jpg");
#else
        esp_http_client_set_header(client_post, "Content-Type",
                                   "application/json");
#endif
        esp_http_client_set_post_field(client_post, (const char *)pic->buf,
                                       pic->len);
        esp_err_t err = esp_http_client_perform(client_post);
        if (err == ESP_OK)
        {
            ESP_LOGI(HTTP_CLIENT_TAG,
                     "HTTP POST Status = %d, content_length = %d",
                     esp_http_client_get_status_code(client_post),
                     esp_http_client_get_content_length(client_post));
        }
        else
        {
            ESP_LOGE(HTTP_CLIENT_TAG, "HTTP POST request failed: %s",
                     esp_err_to_name(err));
        }

#if USE_CAMERA
        set_cState(CAM_CLEAR_PIC);
        set_hcState(HTTP_CLIENT_DO_NOTHING);
#else
        set_hcState(HTTP_CLIENT_DO_NOTHING);
#endif
    }
    break;
    case HTTP_CLIENT_DO_NOTHING:
    {
    }
    break;
    default:
        break;
    }
}