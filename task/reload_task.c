#include "reload_task.h"

#include <stdlib.h>
#include "math.h"

#include "cmsis_os.h"

#include "pid.h"
#include "bsp_remote_ET08.h"

#include "can_comm.h"
#include "micro_swicth.h"


extern osThreadId can_send_task_handle;

uint8_t dart_num;
uint8_t fric_run_flag;

reload_t reload;

reload_fsm_t reload_fsm;

PID_TypeDef reload_spd_pid_t;//速度环

PID_TypeDef reload_spd_Cpid_t;//位置串级环
PID_TypeDef reload_pos_Cpid_t;

uint8_t rel_one_flag;
uint8_t rel_all_flag;

static void Reload_Motor_Init(void);
static void Reload_Control(void);
static void Reload_Data_Update(void);
static void Reload_Current_Set(void);

static void Relaod_Found_Zero(void);
static void Reload_Reinit(void);
static void Reload_Serial(void);

static void reload_mode_sw(reload_fsm_t *reload_fsm); 
static void reload_status_transition(reload_fsm_t *reload_fsm);
			
static void Reload_Status_Init(void);
static void reload_protect_handler(void);
static void reload_remote_handler(void);
static void reload_auto_handler(void);
static void reload_init_handler(void);

static void Reload_Mode_Change(reload_mode_e reload_mode);

void Reload_Task(void const* argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	Reload_Motor_Init();
	while (1)
	{
		reload_mode_sw(&reload_fsm);
		Reload_Control();
		osSignalSet( can_send_task_handle, RELAOD_MOTOR_MSG_SEND);
		osDelayUntil(&mode_wake_time, RELOAD_PERIOD);
	}
}

static void Reload_Data_Update(void)
{
	reload.now_ecd = reload_motor_data.total_ecd;
	reload.spd_fbd = reload_motor_data.speed_rpm;
}

static void Reload_Pid_Cal(void)
{
	reload.current = PID_Calculate(&reload_spd_pid_t, reload.spd_fbd, reload.spd_ref);
}

static void Check_Dark_Num(void)
{
	if(abs(reload.now_ecd) >= 4 * ONE_DART_ECD)
		dart_num = 4;
	else if(abs(reload.now_ecd) >= 3 * ONE_DART_ECD)
		dart_num = 3;
	else if(abs(reload.now_ecd) >= 2 * ONE_DART_ECD)
		dart_num = 2;
	else if(abs(reload.now_ecd) >= 1 * ONE_DART_ECD)
		dart_num = 1;
	else if(abs(reload.now_ecd) < reload.start_ecd+60000)
		dart_num = 0;
}

uint8_t reload_tick_start;
uint8_t reload_lanuch_flag;

static void Reload_Control(void)
{
	Reload_Data_Update();
	Check_Dark_Num();
	switch (reload.reload_mode)
	{
		case RELOAD_PROTECT_MODE:
		{
			reload.current = 0;
			
		}	break;
		case RELOAD_INIT_MODE:
		{
			Relaod_Found_Zero();		
		}	break;
		case RELOAD_SINGLE_MODE:
		{
			if(dart_num == 4)
			{
				fric_run_flag = 0;
				reload.spd_ref = 0;
			}else{
				fric_run_flag = 1;
				reload.spd_ref = RELOAD_SPEED;
			}
			
			Reload_Pid_Cal();
		}	break;
		case RELOAD_SERIES_MODE:
		{
			Reload_Serial();
		}	break;
		case RELOAD_REINIT_MODE:
		{
			Reload_Reinit();
		}	break;

		default:
			break;
	}
	
	Reload_Current_Set();
}

uint8_t flag;

static void Reload_Serial(void)
{
	switch (dart_num)
	{
		case 0:
		case 2://每次开闸门的第一发镖
		{
			if(flag)
			{
				fric_run_flag = 1;
				reload_lanuch_flag = 0;
				reload.spd_ref = RELOAD_SPEED;			
			}else
			{
				reload.spd_ref = 0;
				fric_run_flag = 0;
			}
		}	break;
		case 1:
		case 3://每次开闸门的第二发镖
		{
			flag = 0;
			reload.spd_ref = 0;
			reload_tick_start = 1;

			if(reload_lanuch_flag)
			{
				fric_run_flag = 1;
				reload.spd_ref = RELOAD_SPEED;
			}else{
				reload.spd_ref = 0;
				fric_run_flag = 0;
			}
		}	break;
		case 4:
		{
			reload_lanuch_flag = 0;
			fric_run_flag = 0;
			reload.spd_ref = 0;
		}	break;
		default:
			break;
	}
	Reload_Pid_Cal();
}

void Reload_Tick(void)//放入1ms定时器中
{
	static uint16_t reload_tick_cnt = 0;
	
	if (reload_tick_start)
	{
		if (reload_tick_cnt ++ >= 2000)//每隔两秒发一镖
		{
			reload_lanuch_flag = 1;
			reload_tick_cnt = 0;
			reload_tick_start = 0;
		}
	}
}


static void Reload_Reinit(void)
{
	fric_run_flag = 0;
	switch (SBUS.SD)
	{
		case SW_UP:{//重装填一发
			reload.spd_ref = -RELOAD_SPEED;
			if(dart_num <= 3-1)//小于第三发镖的位置
			{
				reload.spd_ref = 0;
			}
			
			Reload_Pid_Cal();
		}
			break;
		case SW_DN:{//重装填四发
			reload.spd_ref = -RELOAD_SPEED;
			if(dart_num == 0)
			{
				reload.spd_ref = 0;
			}
				
			Reload_Pid_Cal();
		}
			break;
		default:
			break;
	}
}

static void Reload_Current_Set(void)
{
	motor_current.reload = reload.current;
}

static void Relaod_Found_Zero(void)
{
	if(reload_zero == NOFOUND)
	{
		reload.spd_ref = RELOAD_INIT_SPEED;
		reload.current = PID_Calculate(&reload_spd_pid_t, reload.spd_fbd, reload.spd_ref);
	}
	else if(reload_zero == FOUND)
	{
		//设置零点
		taskENTER_CRITICAL();
		reload_motor_data.offset_ecd = reload_motor_data.ecd;
		reload_motor_data.round_cnt = 0;
		taskEXIT_CRITICAL();
		
		reload.spd_ref = 0;
		Reload_Pid_Cal();
		reload.start_ecd = reload_motor_data.total_ecd;
	}
}

static void Reload_Motor_Init(void)
{
	Reload_Status_Init();
////位置环
//	PID_Init(&reload_pos_Cpid_t, MG2006_MAX_ECD/2, 0, 100, \
//				0.0649, 0.0008, 0, \
//				3000, 1000, \
//				0, 0, ChangingIntegralRate);	
//	PID_Init(&reload_spd_Cpid_t, MG2006_MAX_CURRENT, 0, 0, \
//				2.7, 0, 0, \
//				100, 300, \
//				0, 0, ChangingIntegralRate);	

//速度环	
	PID_Init(&reload_spd_pid_t, MG2006_MAX_CURRENT, 0, 0, \
				3, 0.319999993, 1.79999995, \
				500, 1000, \
				0, 0, ChangingIntegralRate | Derivative_On_Measurement | ErrorHandle);	
}

static void reload_mode_sw(reload_fsm_t *fsm) //SC上保护中单发下连发
{
	fsm->reload_status_handler();
	reload_status_transition(fsm);
    /* 模式切换 */
    switch (fsm->status)
	{
		case RELOAD_LOCK:
        case RELOAD_PROTECT: 
			Reload_Mode_Change(RELOAD_PROTECT_MODE);
            break;
		case RELOAD_INIT:
			Reload_Mode_Change(RELOAD_INIT_MODE);
            break;
        case RELOAD_REMOTE: 
			switch(SBUS.SC)
			{
				case SW_MI:
					Reload_Mode_Change(RELOAD_SINGLE_MODE);
					if(SBUS.SA == SW_UP)
						Reload_Mode_Change(RELOAD_REINIT_MODE);
					break;
				case SW_DN:
					Reload_Mode_Change(RELOAD_SERIES_MODE);
					if(SBUS.SA == SW_UP)
						Reload_Mode_Change(RELOAD_REINIT_MODE);
					break;
				case SW_UP:
					Reload_Mode_Change(RELOAD_PROTECT_MODE);
					break;
				default:
					break;
			}
			break;
        case RELOAD_AUTO: 
				//自动
			break;		
		default:
			break;
	}
}	

static void reload_status_transition(reload_fsm_t *fsm)
{
	switch(fsm->status)
	{
		case RELOAD_LOCK:
			if(fsm->event == RELOAD_UNLOCK_OPERATION){
				fsm->status = RELOAD_PROTECT;
				fsm->reload_status_handler = reload_protect_handler;
			}
			break;
		case RELOAD_PROTECT:
			if (fsm->event == RELOAD_INIT_OPERATION){
				fsm->status = RELOAD_INIT;
				fsm->reload_status_handler = reload_init_handler;
			}else if(fsm->event == RELOAD_REMOTE_OPERATION){
				fsm->status = RELOAD_REMOTE;
				fsm->reload_status_handler = reload_remote_handler;
			}else if(fsm->event == RELOAD_AUTO_OPERATION){
				fsm->status = RELOAD_AUTO;
				fsm->reload_status_handler = reload_auto_handler;
			}	break;
		case RELOAD_INIT:
			if (fsm->event == RELOAD_PROTECT_OPERATION){
				fsm->status = RELOAD_PROTECT;
				fsm->reload_status_handler = reload_protect_handler;
			}else if(fsm->event == RELOAD_REMOTE_OPERATION){
				fsm->status = RELOAD_REMOTE;
				fsm->reload_status_handler = reload_remote_handler;
			}else if(fsm->event == RELOAD_AUTO_OPERATION){
				fsm->status = RELOAD_AUTO;
				fsm->reload_status_handler = reload_auto_handler;
			}	break;
		case RELOAD_REMOTE:
			if(fsm->event == RELOAD_PROTECT_OPERATION){
				fsm->status = RELOAD_PROTECT;
				fsm->reload_status_handler = reload_protect_handler;
			}else if(fsm->event == RELOAD_AUTO_OPERATION){
				fsm->status = RELOAD_AUTO;
				fsm->reload_status_handler = reload_auto_handler;
			}
			break;
		case RELOAD_AUTO:
			if(fsm->event == RELOAD_PROTECT_OPERATION){
				fsm->status = RELOAD_PROTECT;
				fsm->reload_status_handler = reload_protect_handler;
			}else if(fsm->event == RELOAD_REMOTE_OPERATION){
				fsm->status = RELOAD_REMOTE;
				fsm->reload_status_handler = reload_remote_handler;
			}
			break;
		default:
			break;
	}
	fsm->event = RELOAD_NULL_EVENT;
}

static void reload_lock_handler(void)
{
	if(lock_flag == UNLOCK)
		reload_fsm.event = RELOAD_UNLOCK_OPERATION;
}

static void reload_init_handler(void)
{
	if(ctrl_mode == PROTECT_MODE)
		reload_fsm.event = RELOAD_PROTECT_OPERATION;
	
	else if(ctrl_mode == REMOTE_MODE)
		reload_fsm.event = RELOAD_REMOTE_OPERATION;

	else if (ctrl_mode == VISION_MODE)
		reload_fsm.event = RELOAD_AUTO_OPERATION;
}

static void reload_protect_handler(void)
{
	if (ctrl_mode == INIT_MODE)
		reload_fsm.event = RELOAD_INIT_OPERATION;
	
	else if(ctrl_mode == REMOTE_MODE)
		reload_fsm.event = RELOAD_REMOTE_OPERATION;
	
	else if (ctrl_mode == VISION_MODE)
		reload_fsm.event = RELOAD_AUTO_OPERATION;
}

static void reload_remote_handler(void)
{
	if(ctrl_mode == PROTECT_MODE)
		reload_fsm.event = RELOAD_PROTECT_OPERATION;
	
	else if(ctrl_mode == VISION_MODE)
		reload_fsm.event = RELOAD_AUTO_OPERATION;
}

static void reload_auto_handler(void)
{	
	if(ctrl_mode == PROTECT_MODE)
		reload_fsm.event = RELOAD_PROTECT_OPERATION;

	else if(ctrl_mode == REMOTE_MODE||ctrl_mode == KEYBOARD_MODE)
		reload_fsm.event = RELOAD_REMOTE_OPERATION;
}

static void Reload_Mode_Change(reload_mode_e relaod_mode)
{	
	reload.reload_mode = relaod_mode;
}

static void Reload_Status_Init(void)
{
	reload_fsm.status = RELOAD_LOCK;
	reload_fsm.event = RELOAD_NULL_EVENT;
	reload_fsm.reload_status_handler = reload_lock_handler;
}
