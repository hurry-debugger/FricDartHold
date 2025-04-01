#ifndef __DEBUG_TASK_H__
#define __DEBUG_TASK_H__

#include "stm32f4xx_hal.h"

#define DEBUG_PERIOD 2
#define DEBUG_HUART  huart2

void Debug_Task(void const* argu);

#endif
