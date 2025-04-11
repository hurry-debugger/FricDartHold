#ifndef __RELOAD_TASK_H__
#define __RELOAD_TASK_H__

#include "stm32f4xx_hal.h"
#include "modeswitch_task.h"

#define RELOAD_PERIOD 		2
#define MG2006_MAX_CURRENT 	10000
#define MG2006_MAX_ECD 		8191
#define MG2006_MAX_SPEED 	450	

#define RELOAD_INIT_SPEED 	-4500
#define RELOAD_SPEED 		6000

#define ONE_DART_ECD      8191*36*16.1      //8191*36*13.5------6000.0f//

typedef enum{
	POS,
	SPEED
}pid_type_e;

typedef enum{
	RELOAD_PROTECT_MODE,
	RELOAD_INIT_MODE,
	RELOAD_SINGLE_MODE,
	RELOAD_SERIES_MODE,
	RELOAD_REINIT_MODE,
}reload_mode_e;

typedef struct {
	float spd_fbd;
	float spd_ref;
	
	int32_t pos_ref;
	int32_t pos_fbd;
	
	int32_t start_ecd;
	int32_t now_ecd;
	
	int16_t current;
	
	reload_mode_e reload_mode;
}reload_t;

/************* fsm *****************/

typedef enum{
    RELOAD_LOCK = 0,
	RELOAD_INIT,
    RELOAD_PROTECT,
    RELOAD_REMOTE,
    RELOAD_AUTO,
} reload_status_e;

typedef enum{
    RELOAD_NULL_EVENT = 0,
    RELOAD_UNLOCK_OPERATION,
	RELOAD_INIT_OPERATION,
    RELOAD_PROTECT_OPERATION,
    RELOAD_REMOTE_OPERATION,
    RELOAD_AUTO_OPERATION
} reload_event_e;

typedef struct{
    reload_status_e status;
    reload_event_e event;
    void (*reload_status_handler)(void);
} reload_fsm_t;

/******************************/

void Reload_Task(void const* argu);
void Reload_Tick(void);

#endif
