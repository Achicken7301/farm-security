#include "main.h"

void app_main(void)
{
    while (1)
    {
        fsm_wifi();
        fsm_tcp_server_nonblocking();
    }
}
