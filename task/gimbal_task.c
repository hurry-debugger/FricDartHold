#include "gimbal_task.h"

#include "stdlib.h"
#include "math.h"

#include "cmsis_os.h"

#include "pid.h"
#include "bsp_remote_ET08.h"
#include "math_calcu.h"

#include "can_comm.h"
#include "micro_swicth.h"

extern osThreadId can_send_task_handle;

gimbal_t gimbal;

PID_TypeDef gimbal_spd_pid_t;
PID_TypeDef gimbal_pos_pid_t;

ramp_function_source_t gimbal_ramp;

static void Gimbal_Motor_Init(void);
static void Gimbal_Control(void);
static void Gimbal_Data_Update(void);
static void Gimbal_Current_Set(void);
static void Gimbal_Found_Zero(void);

static void GimbalAutoControl(void);
			
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

float t = 0.002;

static void Gimbal_Pid_Cal(void)
{
	OUTPUT_LIMIT(gimbal.pos_ref, 0, GIMBAL_MAX_RIGHT);
	
	(gimbal.pos_ref >= 0) ? (ramp_calc(&gimbal_ramp, t, gimbal.pos_ref, gimbal.pos_ref, -gimbal.pos_ref))
						: (ramp_calc(&gimbal_ramp, t, gimbal.pos_ref, -gimbal.pos_ref, gimbal.pos_ref));
	
	gimbal.spd_ref = PID_Calculate(&gimbal_pos_pid_t, gimbal.pos_fbd, gimbal_ramp.out);	
	
	OUTPUT_LIMIT(gimbal.spd_ref, VISION_SPEED_LIMIT, -VISION_SPEED_LIMIT);
	gimbal.current = PID_Calculate(&gimbal_spd_pid_t, gimbal.spd_fbd, gimbal.spd_ref);
}

uint8_t version_flag;
float version_pos;

static void Gimbal_Control(void)
{
	Gimbal_Data_Update();
	
	switch (gimbal.gimbal_mode)
	{
		case GIMBAL_INIT_MODE:
		{
			Gimbal_Found_Zero();		
		}	break;
		case GIMBAL_PROTECT_MODE:
		{
			gimbal.current = 0;
		}	break;
		case GIMBAL_REMOTE_MODE://向右为负
		{
			gimbal.pos_ref -= (GIMBAL_MAX_SPEED/500.0)*(SBUS.Ch1/670.0);
			
			Gimbal_Pid_Cal();
		}	break;
		case GIMBAL_KEYBOARD_MODE:			
			break;//待完善
		case GIMBAL_VISION_MODE:
		{
			GimbalAutoControl();
		}	break;
		default:
			break;
	}
	Gimbal_Current_Set();
}

uint8_t vision_round = 6;

static void GimbalAutoControl(void)
{
	if (version_flag == NOFOUND)
	{
		if(gimbal.pos_fbd < GIMBAL_MAX_RIGHT+1000)//左右限幅范围6020转五圈，从最右边开始
			gimbal.pos_ref = GIMBAL_MAX_RIGHT + MG6020_MAX_ECD * vision_round+1000;
		else if(gimbal.pos_fbd > GIMBAL_MAX_RIGHT + MG6020_MAX_ECD * vision_round)
			gimbal.pos_ref = GIMBAL_MAX_RIGHT;
		}else if (version_flag == FOUND)
		{
			gimbal.pos_ref = version_pos;
		}
		
	Gimbal_Pid_Cal();
}

static void Gimbal_Found_Zero(void)
{
	if(gimbal_zero == NOFOUND)
	{
		gimbal.spd_ref = 500;
		gimbal.current = PID_Calculate(&gimbal_spd_pid_t, gimbal.spd_fbd, gimbal.spd_ref);
	}
	else if(gimbal_zero == FOUND)
	{
		//设置零点
		taskENTER_CRITICAL();
		gimbal_motor_data.offset_ecd = gimbal_motor_data.ecd;
		gimbal_motor_data.round_cnt = 0;
		taskEXIT_CRITICAL();
		
		gimbal.current = 0;
		gimbal.start_ecd = gimbal_motor_data.total_ecd;
	}
}

static void Gimbal_Current_Set(void)
{
	motor_current.gimbal = gimbal.current;
}

static void Gimbal_Motor_Init(void)
{	
	gimbal.gimbal_mode = GIMBAL_PROTECT_MODE;
	PID_Init(&gimbal_pos_pid_t, MG6020_MAX_ECD/2, 20, 1, \
				1.04999995, 0.0199999996, 0, \
				100, 20, \
				0, 0, ChangingIntegralRate | ErrorHandle);
	PID_Init(&gimbal_spd_pid_t, MG6020_MAX_CURRENT, 0, 0, \
				40.2, 0, 0, \
				0, 0, \
				0, 0, Integral_Limit | ErrorHandle);

}

static void Gimbal_Mode_Change(ctrl_mode_e gimbal_ctrl_mode)
{
	switch (gimbal_ctrl_mode)
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
			if (SBUS.Ch4 > 600)	//左拨杆向下拨解锁云台 
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

