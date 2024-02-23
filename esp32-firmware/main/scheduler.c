#include "scheduler.h"

/**
 * @brief Similar to timerRun()
 *
 */
void SCH_Update()
{
    for (int i = 0; i < SCH_MAX_TASK; i++)
    {
        if (SCH_Tasks[i].pFn)
        {
            if (SCH_Tasks[i].delay == 0)
            {
                SCH_Tasks[i].flag += 1;
                if (SCH_Tasks[i].period)
                {
                    SCH_Tasks[i].delay = SCH_Tasks[i].period;
                }
            }
            else
            {
                SCH_Tasks[i].delay -= 1;
            }
        }
    }
}

/**
 * @brief Excecute function pointer from vtask array
 *
 */
void SCH_Dispatch()
{
    for (int i = 0; i < SCH_MAX_TASK; i++)
    {
        if (SCH_Tasks[i].flag > 1)
        {
            // Execute function
            // printf("Exe SCH_Tasks[%d].pFn\n", i);
            ((void (*)())SCH_Tasks[i].pFn)();
            SCH_Tasks[i].flag -= 1;

            // If period == 0 -> delele task from tasks
            if (SCH_Tasks[i].period == 0)
            {
                SCH_Delele(i);
            }
        }
    }
}

void SCH_Add(void *task, uint16_t delay, uint16_t period)
{
    // find .pFn is NULL
    uint8_t index = 0;
    while (SCH_Tasks[index].pFn)
    {
        index++;
    }

    // Found index in Task which is null, then add Task.
    SCH_Tasks[index].pFn = task;
    SCH_Tasks[index].delay = delay / CLOCK;
    SCH_Tasks[index].period = period / CLOCK;
    SCH_Tasks[index].flag = 0;
}

/**
 * @brief Delete task from vtask with given index
 *
 * @param index position of task in vtasks
 */
void SCH_Delele(int index)
{
    SCH_Tasks[index].pFn = NULL;
    SCH_Tasks[index].delay = 0;
    SCH_Tasks[index].flag = 0;
    SCH_Tasks[index].period = 0;
}