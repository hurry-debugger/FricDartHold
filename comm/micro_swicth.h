#ifndef __MICRO_SWICTH_H__
#define __MICRO_SWICTH_H__

#include "stm32f4xx_hal.h"

typedef enum
{
	NOFOUND = 0,
	FOUND
}micro_switch_state_e;

extern micro_switch_state_e gimbal_zero;
extern micro_switch_state_e reload_zero;

extern void MicroSwicth_Callback(void);

#endif
