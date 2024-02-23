#include "fsm_tcp_server.h"

TcpServerState tcpState;

char rx_buffer[128];
struct addrinfo hints = {.ai_socktype = SOCK_STREAM};

/* Just a little reminder, every things create with a start * in it, we should,
 * you know free it!!! */
struct addrinfo *address_info;
const size_t max_socks = CONFIG_LWIP_MAX_SOCKETS - 1;
int sock[CONFIG_LWIP_MAX_SOCKETS - 1];
int sock_pos = 0;
int try_receive_length = 0;
int listen_sock = INVALID_SOCK;

/* Getter-Setter */
const char *get_tcpState(TcpServerState status)
{
    switch (status)
    {
    case TS_INIT:
        return "TS_INIT";
    case TS_CONFIG:
        return "TS_CONFIG";
    case TS_ERROR:
        return "TS_ERROR";
    case TS_CREATE_SOCKET:
        return "TS_CREATE_SOCKET";
    case TS_LISTEN:
        return "TS_LISTEN";
    case TS_BIND:
        return "TS_BIND";
    case TS_ACCEPT:
        return "TS_ACCEPT";
    case TS_RECEIVE:
        return "TS_RECEIVE";
    case TS_SEND:
        return "TS_SEND";
    case TS_DEINIT:
        return "TS_DEINIT";
    case TS_CLOSE:
        return "TS_CLOSE";
    default:
        return "Unknown State";
    }
}

void set_tcpState(TcpServerState status)
{
    printf("Current state in FSM_TCP:\t%s\n", get_tcpState(status));
    tcpState = status;
}

/**
 * @brief Utility to log socket errors
 *
 * @param[in] tag Logging tag
 * @param[in] sock Socket number
 * @param[in] err Socket errno
 * @param[in] message Message to print
 */
static void log_socket_error(const char *tag, const int sock, const int err,
                             const char *message)
{
    ESP_LOGE(tag,
             "[sock=%d]: %s\n"
             "error=%d: %s",
             sock, message, err, strerror(err));
}

/**
 * @brief Tries to receive data from specified sockets in a non-blocking way,
 *        i.e. returns immediately if no data.
 *
 * @param[in] tag Logging tag
 * @param[in] sock Socket for reception
 * @param[out] data Data pointer to write the received data
 * @param[in] max_len Maximum size of the allocated space for receiving data
 * @return
 *          >0 : Size of received data
 *          =0 : No data available
 *          -1 : Error occurred during socket read operation
 *          -2 : Socket is not connected, to distinguish between an actual
 * socket error and active disconnection
 */
static int try_receive(const char *tag, const int sock, char *data,
                       size_t max_len)
{
    int len = recv(sock, data, max_len, 0);
    if (len < 0)
    {
        if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return 0; // Not an error
        }
        if (errno == ENOTCONN)
        {
            ESP_LOGW(tag, "[sock=%d]: Connection closed", sock);
            return -2; // Socket has been disconnected
        }
        log_socket_error(tag, sock, errno, "Error occurred during receiving");
        return -1;
    }

    return len;
}

/**
 * @brief Sends the specified data to the socket. This function blocks until all
 * bytes got sent.
 *
 * @param[in] tag Logging tag
 * @param[in] sock Socket to write data
 * @param[in] data Data to be written
 * @param[in] len Length of the data
 * @return
 *          >0 : Size the written data
 *          -1 : Error occurred during socket write operation
 */
static int socket_send(const char *tag, const int sock, const char *data,
                       const size_t len)
{
    int to_write = len;
    while (to_write > 0)
    {
        int written = send(sock, data + (len - to_write), to_write, 0);
        if (written < 0 && errno != EINPROGRESS && errno != EAGAIN &&
            errno != EWOULDBLOCK)
        {
            log_socket_error(tag, sock, errno, "Error occurred during sending");
            return -1;
        }
        to_write -= written;
    }
    return len;
}

/**
 * @brief Returns the string representation of client's address (accepted on
 * this server)
 */
static inline char *get_clients_address(struct sockaddr_storage *source_addr)
{
    static char address_str[128];
    char *res = NULL;
    // Convert ip address to string
    if (source_addr->ss_family == PF_INET)
    {
        res = inet_ntoa_r(((struct sockaddr_in *)source_addr)->sin_addr,
                          address_str, sizeof(address_str) - 1);
    }
    if (!res)
    {
        address_str[0] =
            '\0'; // Returns empty string if conversion didn't succeed
    }
    return address_str;
}

void fsm_tcp_server_nonblocking()
{
    switch (tcpState)
    {
    case TS_INIT:
    {
        // Prepare a list of file descriptors to hold client's sockets, mark all
        // of them as invalid, i.e. available
        for (int i = 0; i < max_socks; ++i)
        {
            sock[i] = INVALID_SOCK;
        }

        /* Translating the hostname or a string representation of an IP
           to address_info */
        int res = getaddrinfo(CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS,
                              CONFIG_EXAMPLE_TCP_SERVER_BIND_PORT, &hints,
                              &address_info);

        if (res != 0 || address_info == NULL)
        {
            ESP_LOGE(TCP_TAG,
                     "couldn't get hostname for `%s` "
                     "getaddrinfo() returns %d, addrinfo=%p",
                     CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS, res, address_info);
            // goto error;
        }
        // tcp_state = TS_CREATE_SOCKET;
        set_tcpState(TS_CREATE_SOCKET);
    }

    break;
    case TS_CREATE_SOCKET:
    {
        // Creating a listener socket
        listen_sock = socket(address_info->ai_family, address_info->ai_socktype,
                             address_info->ai_protocol);

        if (listen_sock < 0)
        {
            log_socket_error(TCP_TAG, listen_sock, errno,
                             "Unable to create socket");
            tcpState = TS_ERROR;
            break;
        }

        ESP_LOGI(TCP_TAG, "Listener socket created");

        /* CONFIG SOCKET. Do we need to seperate this into different state???
         * Marking the socket as non-blocking */
        int flags = fcntl(listen_sock, F_GETFL);
        if (fcntl(listen_sock, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            log_socket_error(TCP_TAG, listen_sock, errno,
                             "Unable to set socket non blocking");
            tcpState = TS_ERROR;
        }
        ESP_LOGI(TCP_TAG, "Socket marked as non blocking");
        tcpState = TS_BIND;
    }

    break;
    case TS_BIND:
    {
        // Binding socket to the given address
        int err =
            bind(listen_sock, address_info->ai_addr, address_info->ai_addrlen);
        if (err != 0)
        {
            log_socket_error(TCP_TAG, listen_sock, errno,
                             "Socket unable to bind");
            tcpState = TS_ERROR;
            break;
        }
        ESP_LOGI(TCP_TAG, "Socket bound on %s:%s",
                 CONFIG_EXAMPLE_TCP_SERVER_BIND_ADDRESS,
                 CONFIG_EXAMPLE_TCP_SERVER_BIND_PORT);

        tcpState = TS_LISTEN;
    }

    break;
    case TS_LISTEN:
    {
        // Set queue (backlog) of pending connections to one (can be more)
        int err = listen(listen_sock, 1);
        if (err != 0)
        {
            log_socket_error(TCP_TAG, listen_sock, errno,
                             "Error occurred during listen");
            tcpState = TS_ERROR;
            break;
        }
        ESP_LOGI(TCP_TAG, "Socket listening");
        tcpState = TS_ACCEPT;
    }

    break;
    case TS_ACCEPT:
    {
        // Large enough for both IPv4 or IPv6
        struct sockaddr_storage source_addr;
        socklen_t addr_len = sizeof(source_addr);

        int new_sock_index = 0;

        /**
         * @brief Similar to SCH_Add(&task);
         *
         */
        for (new_sock_index = 0; new_sock_index < max_socks; ++new_sock_index)
        {
            if (sock[new_sock_index] == INVALID_SOCK)
            {
                break;
            }
        }

        // We accept a new connection only if we have a free socket
        /**
         * @brief I NEED TO LEAVE SOME NOTES HERE
         * This if(), I consider to put in INTERUPT timer (Do uu tien cao nhat)
         * Most Priorities
         *
         */
        if (new_sock_index < max_socks)
        {
            /**
             * @brief NOTES for Interrupt
             * Listening for connection
             *
             */
            // Try to accept a new connections
            sock[new_sock_index] =
                accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);

            if (sock[new_sock_index] < 0)
            {
                if (errno == EWOULDBLOCK)
                {
                    /* The listener socket did not accepts any connection
                     * continue to serve open connections and try to accept
                     * again upon the next iteration */
                    ESP_LOGV(TCP_TAG, "No pending connections...");
                }
                else
                {
                    log_socket_error(TCP_TAG, listen_sock, errno,
                                     "Error when accepting connection");
                    tcpState = TS_ERROR;
                    break;
                }
            }
            else
            {
                /**
                 * @brief NOTES for Interrupt
                 * Write to buffer and do something else.
                 *
                 */
                // We have a new client connected -> print it's address
                ESP_LOGI(TCP_TAG, "[sock=%d]: Connection accepted from IP:%s",
                         sock[new_sock_index],
                         get_clients_address(&source_addr));

                // ...and set the client's socket non-blocking
                int flags = fcntl(sock[new_sock_index], F_GETFL);
                if (fcntl(sock[new_sock_index], F_SETFL, flags | O_NONBLOCK) ==
                    -1)
                {
                    log_socket_error(TCP_TAG, sock[new_sock_index], errno,
                                     "Unable to set socket non blocking");
                    tcpState = TS_ERROR;
                    break;
                }
                ESP_LOGI(TCP_TAG, "[sock=%d]: Socket marked as non blocking",
                         sock[new_sock_index]);
            }
            tcpState = TS_RECEIVE;
        }
    }

    break;
    case TS_RECEIVE:
    {
        int tcp_mess_apply = 0;
        for (int i = 0; i < max_socks; ++i)
        {
            if (sock[i] != INVALID_SOCK)
            {

                // This is an open socket -> try to serve it
                int len =
                    try_receive(TCP_TAG, sock[i], rx_buffer, sizeof(rx_buffer));
                if (len < 0)
                {
                    // Error occurred within this client's socket -> close and
                    // mark invalid
                    ESP_LOGI(TCP_TAG,
                             "[sock=%d]: try_receive() returned %d -> closing "
                             "the socket",
                             sock[i], len);
                    close(sock[i]);
                    sock[i] = INVALID_SOCK;
                }
                else if (len > 0)
                {
                    /* PROCESS DATA THROUGH THIS */
                    TcpMessageStructure tx_buff_temp;
                    strcpy(tx_buff_temp.data, rx_buffer);
                    if (process_rx_buffer(&tx_buff_temp) == TCPMESS_APPLY)
                    {
                        tcp_mess_apply = 1;
                        // set_tcpState(TS_DEINIT);
                        break;
                    }

                    // // Received some data -> echo back
                    // ESP_LOGI(TCP_TAG, "[sock=%d]: Received %.*s", sock[i],
                    // len,
                    //          rx_buffer);

                    // len = socket_send(TCP_TAG, sock[i], rx_buffer, len);
                    // if (len < 0)
                    // {
                    //     // Error occurred on write to this socket -> close it
                    //     // and mark invalid
                    //     ESP_LOGI(TCP_TAG,
                    //              "[sock=%d]: socket_send() returned %d -> "
                    //              "closing the socket",
                    //              sock[i], len);
                    //     close(sock[i]);
                    //     sock[i] = INVALID_SOCK;
                    // }
                    // else
                    // {
                    //     // Successfully echoed to this socket
                    //     ESP_LOGI(TCP_TAG, "[sock=%d]: Written %.*s", sock[i],
                    //              len, rx_buffer);
                    // }
                }

            } // one client's socket
        }     // for all sockets

        if (tcp_mess_apply == 1)
        {
            set_tcpState(TS_DEINIT);
            break;
        }
        tcpState = TS_ACCEPT;
    }
    break;
    case TS_ERROR:
    {
        if (listen_sock != INVALID_SOCK)
        {
            close(listen_sock);
        }

        for (int i = 0; i < max_socks; ++i)
        {
            if (sock[i] != INVALID_SOCK)
            {
                close(sock[i]);
            }
        }

        free(address_info);
        // TODO:  tcp_state point to where???
        tcpState = TS_INIT;
    }
    break;
    case TS_CLOSE:
    {
        ESP_LOGI(TCP_TAG,
                 "[sock=%d]: try_receive() returned %d -> closing "
                 "the socket",
                 sock[sock_pos], try_receive_length);
        close(sock[sock_pos]);
        sock[sock_pos] = INVALID_SOCK;
        tcpState = TS_ACCEPT;
    }
    break;
    case TS_DEINIT:
    {
        /* Close Tcp Server */
        ESP_LOGI(TCP_TAG, "Close all TCP connection");
        for (int i = 0; i < max_socks; ++i)
        {
            close(sock[i]);
        }

        set_tcpState(AP_DEINIT);
    }
    break;
    default:
        break;
    }
}
