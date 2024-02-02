#include "global.h"
TransitionState currentState;

void setState(TransitionState state)
{
    printf("Current State is:\t%s\n", getState(state));
    currentState = state;
}

const char *getState(TransitionState state)
{
    switch (state)
    {
    case WIFI_INIT:
        return "WIFI_INIT";
    case WIFI_START:
        return "WIFI_START";
    case WIFI_IDLE:
        return "WIFI_IDLE";
    case WIFI_AP_INIT:
        return "WIFI_AP_INIT";
    case WIFI_AP_CONFIGURATION:
        return "WIFI_AP_CONFIGURATION";
    case WEB_SERVER:
        return "WEB_SERVER";
    case CLOSE_WEB_SERVER:
        return "CLOSE_WEB_SERVER";
    case WIFI_STA_INIT:
        return "WIFI_STA_INIT";
    case WIFI_STA_CONFIGURATION:
        return "WIFI_STA_CONFIGURATION";
    default:
        return "UNKNOWN_STATE";
    }
}