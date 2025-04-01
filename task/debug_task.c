#include "debug_task.h"

#include "cmsis_os.h"

#include "data_scope.h"
#include "pid.h"

#include "can_comm.h"
#include "shoot_task.h"
#include "gimbal_task.h"
#include "reload_task.h"

extern shoot_t shoot;
extern gimbal_t gimbal;
extern reload_t reload;

extern PID_TypeDef fric_motor_pid_t[2];

extern PID_TypeDef gimbal_spd_pid_t;
extern PID_TypeDef gimbal_pos_pid_t;

extern PID_TypeDef reload_spd_pid_t;

typedef enum
{
	fric_motor,		//0
	reload_motor,	//1
	gimbal_motor	//2
}motor_type_e;

motor_type_e motor_type = reload_motor;

void DataWavePkg(void)
{
	switch (motor_type)
	{
		case fric_motor:
		{
			DataScope_Get_Channel_Data(shoot.fric_spd_fbd[R]);
			DataScope_Get_Channel_Data(shoot.fric_spd_ref[R]);
			
			DataScope_Get_Channel_Data(shoot.fric_current[R]);
			
			DataScope_Get_Channel_Data(fric_motor_pid_t[R].Pout);
			DataScope_Get_Channel_Data(fric_motor_pid_t[R].Iout);
			
			DataScope_Get_Channel_Data(shoot.fric_spd_fbd[L]);
			DataScope_Get_Channel_Data(shoot.fric_spd_ref[L]);
					
			DataScope_Get_Channel_Data(shoot.fric_current[L]);
			
			DataScope_Get_Channel_Data(fric_motor_pid_t[L].Pout);
			DataScope_Get_Channel_Data(fric_motor_pid_t[L].Iout);			
			break;
		}
		case gimbal_motor:
		{
			DataScope_Get_Channel_Data(gimbal.pos_fbd);
			DataScope_Get_Channel_Data(gimbal.pos_ref);
			DataScope_Get_Channel_Data(gimbal.spd_fbd);
			DataScope_Get_Channel_Data(gimbal.spd_ref);	
			
			DataScope_Get_Channel_Data(gimbal.current);
			
			DataScope_Get_Channel_Data(gimbal_spd_pid_t.Iout);
			DataScope_Get_Channel_Data(gimbal_spd_pid_t.Pout);
			DataScope_Get_Channel_Data(gimbal_pos_pid_t.Iout);
			DataScope_Get_Channel_Data(gimbal_pos_pid_t.Pout);
			break;
		}
		case reload_motor:
		{
			DataScope_Get_Channel_Data(reload.pos_fbd);
			DataScope_Get_Channel_Data(reload.pos_ref);
			DataScope_Get_Channel_Data(reload.spd_fbd);
			DataScope_Get_Channel_Data(reload.spd_ref);	
									   
			DataScope_Get_Channel_Data(reload.current);
									   
			DataScope_Get_Channel_Data(reload_spd_pid_t.Iout);
			DataScope_Get_Channel_Data(reload_spd_pid_t.Pout);
			DataScope_Get_Channel_Data(reload_spd_pid_t.Dout);
			break;
		}

	}
}


void Debug_Task(void const* argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	while(1)
	{
		taskENTER_CRITICAL();
		
		DataWave(&DEBUG_HUART);
		
		taskEXIT_CRITICAL();
		osDelayUntil(&mode_wake_time, DEBUG_PERIOD);
	}
}

