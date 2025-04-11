#ifndef __SHOOT_TAKS_H__
#define __SHOOT_TAKS_H__

#include "stm32f4xx_hal.h"

#define SHOOT_PERIOD 			2

#define MG3508_MAX_CURRENT 	9000

typedef enum{
	R,
	L
}fric_type_e;

typedef enum{
	SHOOT_PROTECT_MODE,
	SHOOT_SINGLE_MODE,
	SHOOT_SERIES_MODE,
}shoot_mode_e;

typedef struct{
	shoot_mode_e shoot_mode;
	
	float fric_spd_fbd[2];
	float fric_spd_ref[2];
	int16_t fric_current[2];
	
}shoot_t;

/*************  fsr  *****************/
typedef enum{
    SHOOT_LOCK = 0,
	SHOOT_INIT,
    SHOOT_PROTECT,
    SHOOT_REMOTE,
    SHOOT_AUTO,
} shoot_status_e;

typedef enum{
    SHOOT_NULL_EVENT = 0,
    SHOOT_UNLOCK_OPERATION,
    SHOOT_INIT_OPERATION,
    SHOOT_PROTECT_OPERATION,
    SHOOT_REMOTE_OPERATION,
    SHOOT_AUTO_OPERATION
} shoot_event_e;

typedef struct{
    shoot_status_e status;
    shoot_event_e event;
    void (*shoot_status_handler)(void);
} shoot_fsm_t;

/******************************/

extern shoot_t shoot;
void Shoot_Task(void const *argu);

#endif
