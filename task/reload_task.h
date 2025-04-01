#ifndef __RELOAD_TASK_H__
#define __RELOAD_TASK_H__

#include "stm32f4xx_hal.h"
#include "modeswitch_task.h"

#define RELOAD_PERIOD 		2
#define MG2006_MAX_CURRENT 	10000
#define MG2006_MAX_ECD 		8191
#define MG2006_MAX_SPEED 	450	

typedef struct {
	float spd_fbd;
	float spd_ref;
	
	int32_t pos_fbd;
	int32_t pos_ref;
	
	int16_t current;
	
	ctrl_mode_e mode;
	int32_t max_ecd;
}reload_t;


void Reload_Task(void const* argu);


#endif
