#include "tcp_processing.h"

/**
 * @brief TCP server process user input
 *
 *
 * @param rx_buff
 * @return
 */
TcpMess_Err_t process_rx_buffer(TcpMessageStructure *rx_buff)
{
    /* TODO: I HAVE NO IDEA WHAT IM DOING */
    char code[3 + 1];
    char var[4 + 1];
    char var_data[64 - 4 - 3 - 2];

    memcpy(code, rx_buff->code, sizeof(rx_buff->code));
    memcpy(var, rx_buff->var, sizeof(rx_buff->var));
    memcpy(var_data, rx_buff->var_data, sizeof(rx_buff->var_data));

    ESP_LOGI(TCP_TAG, "Code:%s\tvar:%s\tvar_data:%s", code, var, var_data);

    /* Real data of every tcp is some thing like <user-input>\r\n. THERE
    always
     * \r\n at the end, so be carefull.
     * This is a sImPle fUnCtIon, I'll Implement this later.
     */
    for (int i = 0; i < sizeof(var_data); i++)
    {
        if (var_data[i] == '\r' || var_data[i] == '\n')
        {
            var_data[i] = '\0';
        }
    }

    /* THIS IS A MESS, I"LL FIX THIS LATER */
    if (!strIsEqual(code, CODE_SET))
    {
        ESP_LOGI(TCP_TAG, "Wrong Error Code");
        return TCP_MESS_NOT_OK;
    }

    if (strIsEqual(var, VAR_SSID))
    {

        // strcpy(ROUTER_SSID, var_data);
        memcpy(ROUTER_SSID, var_data, sizeof(ROUTER_SSID));
        ESP_LOGI(TCP_TAG, "Success set ROUTER_SSID to %s", var_data);

        return TCP_MESS_OK;
    }

    if (strIsEqual(var, VAR_PASS))
    {
        strcpy(ROUTER_PASS, var_data);
        ESP_LOGI(TCP_TAG, "Success set ROUTER_PASS to %s", var_data);
        return TCP_MESS_OK;
    }

    if (strIsEqual(var, VAR_APPLY))
    {
        return TCP_MESS_APPLY;
    }

    return TCP_MESS_NOT_OK;
}

/**
 * @brief Process server reponse
 *
 * @return TcpMess_Err_t
 */
TcpMess_Err_t process_server_data(char *rx_buff) { return TCP_MESS_OK; }

/**
 * @brief Compair 2 string
 *
 *
 * @param str1
 * @param str2
 * @return 1 is Equal 0 is NOT
 */
int strIsEqual(char *str1, char *str2) { return !strcmp(str1, str2); }
