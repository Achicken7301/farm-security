# idf_component_register(SRCS "main.c"  "fsm_mesh.c" "scheduler.c" "fsm_tcp_server.c" "global.c" "tcp_processing.c" "fsm_sta.c" "fsm_camera.c" "sch_task.c"
#                     INCLUDE_DIRS "." "include")

file(GLOB_RECURSE SRCS "*.c")
idf_component_register(SRCS ${SRCS}
                        INCLUDE_DIRS "." "include")
