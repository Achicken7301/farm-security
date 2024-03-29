#include "tcp_processing.h"

/**
 * @brief TCP server process user input
 *
 *
 * @param rx_buff
 * @return
 */
// TcpMess_Err_t process_rx_buffer(TcpMessageStructure *rx_buff)
TcpMess_Err_t process_rx_buffer(char *rx_buff, int len)
{

  /* TODO: I HAVE NO IDEA WHAT IM DOING */
  char *cmd;
  char *var;
  char *var_data;
  char *token;
  /* This is really a messsssssssssssssssssssssssssssssssssssssssss */
  token = strtok(rx_buff, " ");
  if (token == NULL)
    return TCP_MESH_WRONG_FORMAT;
  cmd = token;
  token = strtok(NULL, " ");
  if (token == NULL)
    return TCP_MESH_WRONG_FORMAT;
  var = token;
  token = strtok(NULL, "\r\n");
  if (token == NULL)
    return TCP_MESH_WRONG_FORMAT;
  var_data = token;

  ESP_LOGI(TCP_TAG, "Code:%s,var:%s,var_data:%s", cmd, var, var_data);

  /* Real data of every tcp is some thing like <user-input>\r\n. THERE always
   * \r\n at the end, so be carefull.
   * This is a sImPle fUnCtIon, I'll Implement this later.
   */

  /* THIS IS A MESS, I"LL FIX THIS LATER */
  if (!strIsEqual(cmd, CODE_SET))
  {
    ESP_LOGI(TCP_TAG, "Wrong Error Code");
    return TCP_MESH_WRONG_FORMAT;
  }

  if (strIsEqual(var, VAR_SSID))
  {
    memcpy(ROUTER_SSID, var_data, sizeof(ROUTER_SSID));
    ESP_LOGI(TCP_TAG, "Success set ROUTER_SSID to %s", var_data);
    return TCP_MESH_OK;
  }

  if (strIsEqual(var, VAR_PASS))
  {
    strcpy(ROUTER_PASS, var_data);
    ESP_LOGI(TCP_TAG, "Success set ROUTER_PASS to %s", var_data);
    return TCP_MESH_OK;
  }

  if (strIsEqual(var, VAR_APPLY))
  {
    return TCP_MESH_APPLY;
  }

  return TCP_MESH_WRONG_FORMAT;
}

/**
 * @brief Process server reponse
 *
 * @return TcpMess_Err_t
 */
TcpMess_Err_t process_server_data(char *rx_buff) { return TCP_MESH_OK; }
