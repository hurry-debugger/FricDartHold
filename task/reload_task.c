#include "reload_task.h"

#include "cmsis_os.h"

#include "pid.h"

#include "can_comm.h"
#include "micro_swicth.h"

extern osThreadId can_send_task_handle;

reload_t reload;

PID_TypeDef reload_spd_pid_t;

static void Reload_Motor_Init(void);
static void Reload_Control(void);
static void Reload_Data_Update(void);
static void Reload_Current_Set(void);
static void Relaod_Found_Zero(void);

void Reload_Task(void const* argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	Reload_Motor_Init();
	while (1)
	{
		Reload_Control();
		osSignalSet( can_send_task_handle, RELAOD_MOTOR_MSG_SEND);
		osDelayUntil(&mode_wake_time, RELOAD_PERIOD);
	}
}

static void Reload_Data_Update(void)
{
	reload.spd_fbd = reload_motor_data.speed_rpm;
}

static void Reload_Pid_Cal(void)
{
	reload.current = PID_Calculate(&reload_spd_pid_t, reload.spd_fbd, reload.spd_ref);
}

static void Reload_Control(void)
{
	Reload_Data_Update();
	switch (reload.mode)
	{
		case PROTECT_MODE:
		{
			reload.current = 0;
			break;
		}
		case INIT_MODE:
		{
			Relaod_Found_Zero();
			break;
		}
			case REMOTE_MODE:
		{
			Reload_Pid_Cal();
			break;
		}
		default:
			break;
	}
	Reload_Current_Set();							
}

static void Reload_Current_Set(void)
{
	motor_current.reload = reload.current;
}

static void Relaod_Found_Zero(void)
{
	if(reload_zero == NOFOUND)
	{
		reload.spd_ref = -5000;
		reload.current = PID_Calculate(&reload_spd_pid_t, reload.spd_fbd, reload.spd_ref);
	}
	else if(reload_zero == FOUND)
	{		
		reload.spd_ref = 0;
		reload.current = PID_Calculate(&reload_spd_pid_t, reload.spd_fbd, reload.spd_ref);
		reload.max_ecd = reload_motor_data.total_ecd;
	}
}

static void Reload_Motor_Init(void)
{
	PID_Init(&reload_spd_pid_t, MG2006_MAX_CURRENT, 0, 0, \
				3, 0.319999993, 1.79999995, \
				500, 1000, \
				0, 0, ChangingIntegralRate | Derivative_On_Measurement);	
}
