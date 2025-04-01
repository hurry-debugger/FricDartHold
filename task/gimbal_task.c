#include "gimbal_task.h"

#include "cmsis_os.h"

#include "pid.h"
#include "bsp_remote_ET08.h"

#include "can_comm.h"
#include "micro_swicth.h"

extern osThreadId can_send_task_handle;

gimbal_t gimbal;

PID_TypeDef gimbal_spd_pid_t;
PID_TypeDef gimbal_pos_pid_t;

gimbal_t gimbal;
gimbal_fsm_t gimbal_fsm;

static void Gimbal_Motor_Init(void);
static void Gimbal_Control(void);
static void Gimbal_Data_Update(void);
static void Gimbal_Current_Set(void);
static void Gimbal_Found_Zero(void);
			
static void Gimbal_Mode_Change(ctrl_mode_e ctrl_mode);

void Gimbal_Task(void const* argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	Gimbal_Motor_Init();
	while (1)
	{
		Gimbal_Mode_Change(ctrl_mode);
		Gimbal_Control();
		osSignalSet(can_send_task_handle, GIMBAL_MOTOR_MSG_SEND);
		osDelayUntil(&mode_wake_time, GIMBAL_PERIOD);
	}
}

static void Gimbal_Data_Update(void)
{
	gimbal.spd_fbd = gimbal_motor_data.speed_rpm;
	gimbal.pos_fbd = gimbal_motor_data.total_ecd;
}

static void Gimbal_Pid_Cal(void)
{
	gimbal.spd_ref = PID_Calculate(&gimbal_pos_pid_t, gimbal.pos_fbd, gimbal.pos_ref);	
	gimbal.current = PID_Calculate(&gimbal_spd_pid_t, gimbal.spd_fbd, gimbal.spd_ref);
}

float a, b, c;

static void Gimbal_Control(void)
{
	Gimbal_Data_Update();
	
	switch (gimbal.gimbal_mode)
	{
		case GIMBAL_INIT_MODE:
		{
			Gimbal_Found_Zero();
			break;
		}
		case GIMBAL_PROTECT_MODE:
		{
			gimbal.current = 0;
			break;
		}
		case GIMBAL_REMOTE_MODE:
		{
			gimbal.pos_ref += (GIMBAL_MAX_SPEED/500.0)*(SBUS.Ch1/650.0);
			Gimbal_Pid_Cal();
			break;
		}
		case GIMBAL_KEYBOARD_MODE:		//待完善
		case GIMBAL_VISION_MODE:
		{
			gimbal.pos_ref = 0;
			break;
		}

		default:
			break;
	}
	Gimbal_Current_Set();
}

static void Gimbal_Found_Zero(void)
{
	if(gimbal_zero == NOFOUND)
	{
		gimbal.spd_ref = 800;
		gimbal.current = PID_Calculate(&gimbal_spd_pid_t, gimbal.spd_fbd, gimbal.spd_ref);
	}
	else if(gimbal_zero == FOUND)
	{
		gimbal.spd_ref = 0;
		gimbal.current = PID_Calculate(&gimbal_spd_pid_t, gimbal.spd_fbd, gimbal.spd_ref);
//		taskENTER_CRITICAL();
//		reload_motor_data.offset_ecd = 0;
//		reload_motor_data.round_cnt  = 0;
//		taskEXIT_CRITICAL();
	}
}

static void Gimbal_Current_Set(void)
{
	motor_current.gimbal = gimbal.current;
}

static void Gimbal_Motor_Init(void)
{	
	PID_Init(&gimbal_pos_pid_t, MG6020_MAX_ECD/2, 20, 1, \
				1.04999995, 0.0199999996, 0, \
				100, 20, \
				0, 0, ChangingIntegralRate);
	PID_Init(&gimbal_spd_pid_t, MG6020_MAX_CURRENT, 0, 0, \
				40.2, 0, 0, \
				0, 0, \
				0, 0, Integral_Limit);

}

static void Gimbal_Mode_Change(ctrl_mode_e ctrl_mode)
{	
	switch (ctrl_mode)
	{
		case INIT_MODE:
		{
			gimbal.gimbal_mode = GIMBAL_INIT_MODE;
			break;
		}
		case PROTECT_MODE:
		{
			gimbal.gimbal_mode = GIMBAL_PROTECT_MODE;
			break;
		}
		case REMOTE_MODE:
		{
			if (SBUS.Ch4 > 600)	//左拨杆向下拨解锁云台  待找数据
				gimbal.gimbal_mode = GIMBAL_REMOTE_MODE;
			else 
				gimbal.gimbal_mode = GIMBAL_PROTECT_MODE;
			break;
		}
		case KEYBOARD_MODE:
		{
			gimbal.gimbal_mode = GIMBAL_KEYBOARD_MODE;
			break;
		}
		case VISION_MODE:
		{
			gimbal.gimbal_mode = GIMBAL_VISION_MODE;
			break;
		}
	}
}

