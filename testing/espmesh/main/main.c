#include "main.h"

void app_main(void)
{
    set_EspMeshState(MESH_INIT);
    while (1)
    {
        fsm_espmesh();
    }
}
