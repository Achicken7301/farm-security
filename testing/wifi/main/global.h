#ifndef __GLOBAL_VARIABLES__
#define __GLOBAL_VARIABLES__
#include <stdio.h>

typedef enum
{
    /* WIFI STATES */
    WIFI_INIT,
    WIFI_START,
    WIFI_IDLE, /* I DONT KNOW WHY THIS EXIST??? */

    /* AP CONFIGURATION */
    WIFI_AP_INIT,
    WIFI_AP_CONFIGURATION,

    /* HTTP SERVER */
    WEB_SERVER,
    CLOSE_WEB_SERVER,

    /* CONNECT TO ROUTER APSTA MODE */
    WIFI_STA_INIT,
    WIFI_STA_CONFIGURATION,

} TransitionState;

extern TransitionState currentState;

void setState(TransitionState);
TransitionState getState();
#endif // End #ifndef __GLOBAL_VARIABLES__