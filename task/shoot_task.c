#include "shoot_task.h"

#include "string.h"
#include "cmsis_os.h"

#include "pid.h"

#include "bsp_remote_ET08.h"
#include "micro_swicth.h"

#include "can_comm.h"
#include "modeswitch_task.h"


PID_TypeDef fric_motor_pid_t[2];

extern osThreadId can_send_task_handle;
extern uint8_t fric_run_flag;

shoot_t shoot;
shoot_fsm_t shoot_fsm;

static void Shoot_Motor_Init(void);
static void Shoot_Control(void);
static void Shoot_data_update(void);
static void Shoot_Current_Set(void);

static void shoot_mode_sw(shoot_fsm_t *shoot_fsm); 
static void shoot_status_transition(shoot_fsm_t *shoot_fsm);

static void Shoot_Status_Init(void);
static void shoot_protect_handler(void);
static void shoot_remote_handler(void);
static void shoot_auto_handler(void);
static void shoot_init_handler(void);

void FricMotor_Mode_Change(shoot_mode_e shoot_mode);

void Shoot_Task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	Shoot_Motor_Init();
	while (1)
	{
		shoot_mode_sw(&shoot_fsm);
		Shoot_Control();
		Shoot_data_update();
		osSignalSet(can_send_task_handle, SHOOT_MOTOR_MSG_SEND);
		osDelayUntil(&mode_wake_time, SHOOT_PERIOD);
	}
}

static void Shoot_data_update(void)
{
	shoot.fric_spd_fbd[R] = fric_motor_data[R].speed_rpm;
	shoot.fric_spd_fbd[L] = fric_motor_data[L].speed_rpm;
}

static void Shoot_PID_calu(void)
{
	shoot.fric_current[R] = PID_Calculate(&fric_motor_pid_t[R], shoot.fric_spd_fbd[R], shoot.fric_spd_ref[R]);
	shoot.fric_current[L] = PID_Calculate(&fric_motor_pid_t[L], shoot.fric_spd_fbd[L], shoot.fric_spd_ref[L]);
}

int32_t r = 6000,l = -6000;

static void Shoot_Control(void)
{
	switch (shoot.shoot_mode)
	{
		case SHOOT_PROTECT_MODE:
		{
			shoot.fric_spd_ref[R] = 0;
			shoot.fric_spd_ref[L] = 0;
		}break;
		case SHOOT_SINGLE_MODE:
		{
			shoot.fric_spd_ref[R] = r;
			shoot.fric_spd_ref[L] = l;
			if (!fric_run_flag)
			{
				shoot.fric_spd_ref[R] = 0;
				shoot.fric_spd_ref[L] = 0;
			}
				
//			swtich(dart_num)
//			shoot.fric_spd_ref[R] = flash内部数据(dart_num)
//			shoot.fric_spd_ref[L] = flash内部数据(dart_num)
		}break;
		case SHOOT_SERIES_MODE:
		{
			shoot.fric_spd_ref[R] = r;
			shoot.fric_spd_ref[L] = l;
			if (!fric_run_flag)
			{
				shoot.fric_spd_ref[R] = 0;
				shoot.fric_spd_ref[L] = 0;
			}
//			swtich(dart_num)
//			shoot.fric_spd_ref[R] = flash内部数据(dart_num)
//			shoot.fric_spd_ref[L] = flash内部数据(dart_num)
		}break;
	}
	
	Shoot_PID_calu();
	Shoot_Current_Set();
}

static void Shoot_Current_Set(void)
{
	motor_current.fric[L] = shoot.fric_current[L];
	motor_current.fric[R] = shoot.fric_current[R];
}

static void Shoot_Motor_Init(void)
{
	Shoot_Status_Init();
	
	PID_Init(&fric_motor_pid_t[R], MG3508_MAX_CURRENT, 1500, 20, \
								12, 0.005, 0, \
								0, 0, \
								0, 0, Integral_Limit|ErrorHandle);
	
	PID_Init(&fric_motor_pid_t[L], MG3508_MAX_CURRENT, 1500, 20, \
								12, 0.005, 0, \
								0, 0, \
								0, 0, Integral_Limit|ErrorHandle);
}

static void shoot_mode_sw(shoot_fsm_t *shoot_fsm) //SC上保护中单发下连发
{
	shoot_fsm->shoot_status_handler();
	shoot_status_transition(shoot_fsm);
    /* 模式切换 */
    switch (shoot_fsm->status) 
	{
		case SHOOT_INIT:
        case SHOOT_PROTECT: 
		{
			FricMotor_Mode_Change(SHOOT_PROTECT_MODE);
		}
             break;
        case SHOOT_REMOTE: 
		{
			switch(SBUS.SC)
			{
				case SW_UP:
					FricMotor_Mode_Change(SHOOT_PROTECT_MODE);
					break;
				case SW_MI:
					FricMotor_Mode_Change(SHOOT_SINGLE_MODE);
					break;
				case SW_DN:
					FricMotor_Mode_Change(SHOOT_SERIES_MODE);
					break;
				default:
					break;
			}
		}
			break;
        case SHOOT_AUTO: 
		{
				//视觉
			break;
		}
		default:
			break;
	}
}	

static void shoot_status_transition(shoot_fsm_t *fsm)
{
		switch(fsm->status){
		case SHOOT_LOCK:
			if(fsm->event == SHOOT_UNLOCK_OPERATION){
				fsm->status = SHOOT_PROTECT;
				fsm->shoot_status_handler = shoot_init_handler;
			}
			break;
		case SHOOT_INIT:
			if(fsm->event == SHOOT_PROTECT_OPERATION){
				fsm->status = SHOOT_PROTECT;
				fsm->shoot_status_handler = shoot_protect_handler;
			}else if(fsm->event == SHOOT_REMOTE_OPERATION){
				fsm->status = SHOOT_REMOTE;
				fsm->shoot_status_handler = shoot_remote_handler;
			}else if(fsm->event == SHOOT_AUTO_OPERATION){
				fsm->status = SHOOT_AUTO;
				fsm->shoot_status_handler = shoot_auto_handler;
			}
			break;
		case SHOOT_PROTECT:
			if(fsm->event == SHOOT_REMOTE_OPERATION){
				fsm->status = SHOOT_REMOTE;
				fsm->shoot_status_handler = shoot_remote_handler;
			}else if(fsm->event == SHOOT_AUTO_OPERATION){
				fsm->status = SHOOT_AUTO;
				fsm->shoot_status_handler = shoot_auto_handler;
			}
			break;
		case SHOOT_REMOTE:
			if(fsm->event == SHOOT_PROTECT_OPERATION){
				fsm->status = SHOOT_PROTECT;
				fsm->shoot_status_handler = shoot_protect_handler;
			}else if(fsm->event == SHOOT_AUTO_OPERATION){
				fsm->status = SHOOT_AUTO;
				fsm->shoot_status_handler = shoot_auto_handler;
			}
			break;
		case SHOOT_AUTO:
			if(fsm->event == SHOOT_PROTECT_OPERATION){
				fsm->status = SHOOT_PROTECT;
				fsm->shoot_status_handler = shoot_protect_handler;
			}else if(fsm->event == SHOOT_REMOTE_OPERATION){
				fsm->status = SHOOT_REMOTE;
				fsm->shoot_status_handler = shoot_remote_handler;
			}
			break;
		default:
			break;
	}
	fsm->event = SHOOT_NULL_EVENT;
}

static void shoot_lock_handler(void)
{
	if(lock_flag == UNLOCK)
		shoot_fsm.event = SHOOT_UNLOCK_OPERATION;
}

static void shoot_init_handler(void)
{
	if(ctrl_mode == PROTECT_MODE)
		shoot_fsm.event = SHOOT_PROTECT_OPERATION;
	
	else if(ctrl_mode == REMOTE_MODE)
		shoot_fsm.event = SHOOT_REMOTE_OPERATION;
	
	else if(ctrl_mode == VISION_MODE)
		shoot_fsm.event = SHOOT_AUTO_OPERATION;
}

static void shoot_protect_handler(void)
{
	if(ctrl_mode == REMOTE_MODE)
		shoot_fsm.event = SHOOT_REMOTE_OPERATION;
	
	else if(ctrl_mode == VISION_MODE)
		shoot_fsm.event = SHOOT_AUTO_OPERATION;
}

static void shoot_remote_handler(void)
{
	if(ctrl_mode == PROTECT_MODE)
		shoot_fsm.event = SHOOT_PROTECT_OPERATION;
	
	else if(ctrl_mode == VISION_MODE)
		shoot_fsm.event = SHOOT_AUTO_OPERATION;
}

static void shoot_auto_handler(void)
{
	if(ctrl_mode == PROTECT_MODE)
		shoot_fsm.event = SHOOT_PROTECT_OPERATION;
	
	else if(ctrl_mode == REMOTE_MODE||ctrl_mode == KEYBOARD_MODE)
		shoot_fsm.event = SHOOT_REMOTE_OPERATION;
}

void FricMotor_Mode_Change(shoot_mode_e fric_mode)
{
	static shoot_mode_e last_fric_mode = SHOOT_PROTECT_MODE;
	UNUSED(last_fric_mode);	
	
	shoot.shoot_mode = fric_mode;
	last_fric_mode = fric_mode;
}

static void Shoot_Status_Init(void)
{
	shoot_fsm.status = SHOOT_LOCK;
	shoot_fsm.event = SHOOT_NULL_EVENT;
	shoot_fsm.shoot_status_handler = shoot_lock_handler;
}


static uint16_t shoot_interval_cnt;//每发镖的间隔时间，20s内打完
static uint16_t wait_gate_cnt;//闸门完全开启需要7s，需等待裁判系统开启的标志位，judge_flag;

void Shoot_Tick(void)
{
	shoot_interval_cnt ++;
}
