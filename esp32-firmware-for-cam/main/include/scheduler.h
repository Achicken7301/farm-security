#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <stdint.h>
#include <stdio.h>

/**
 * @brief This Scheduler with O(n) complexity
 * @Todo: Make this to O(1)
 *
 */
#define CLOCK 10
#define SCH_MAX_TASK 10

typedef struct
{
  void *pFn;
  uint16_t delay;
  uint16_t period;
  uint8_t flag;
} vTask;

typedef enum
{
  SCH_ERR_OK,
  SCH_ERR_NOT_OK,
  SCH_ERR_TASK_NOT_FOUND,
} SchedulerError_t;

vTask SCH_Tasks[SCH_MAX_TASK];

void SCH_Update();
void SCH_Dispatch();
void SCH_Add(void *task, uint16_t delay, uint16_t period);
void SCH_Delele(int);
int SCH_Get(void *);

#endif