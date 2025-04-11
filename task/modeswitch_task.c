/**
  * @file     modeswitch_task.c
  * @version  v1.0
  * @date     
  *
  * @brief    遥控模式切换任务
  * @note     DT7
	*
  *	@author   罗嘉毅 & 空中组
  *
**/

#include "modeswitch_task.h"
#include <stdio.h>
#include "bsp_remote_ET08.h"

#include "micro_swicth.h"
#include "shoot_task.h"
#include "gimbal_task.h"

ctrl_mode_e ctrl_mode = PROTECT_MODE;
lock_flag_e lock_flag = LOCK;

uint8_t last_vision_flag;
uint8_t vision_flag;

static void sw1_mode_handler(void);
static void unlock_init(void);


void ModeSwitch_Task(void const *argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	for(;;)
	{
		//内八解锁
		if(lock_flag == LOCK)
		{
			 unlock_init();
		}
		else if(lock_flag == UNLOCK)
		{                           
			sw1_mode_handler();
		}
	 
		osDelayUntil(&mode_wake_time, MODESWITCH_PERIOD);
	}
}

//解锁函数
static void unlock_init(void)
{
	//SB拨杆居中，SC拨杆置上
	if(SBUS.SB == SW_MI && SBUS.SC == SW_UP)
	{
		if( SBUS.Ch3 > 600 && SBUS.Ch4 > 600)	//右遥杆内拨解锁
		{
			lock_flag = UNLOCK;  //左摇杆右下
		}
	}
}

//左拨杆模式切换
static void sw1_mode_handler(void)  
{
//	last_vision_flag = ctrl_mode;
	taskENTER_CRITICAL();
	switch( SBUS.SB )
	{
		case SW_UP:                     //上，遥控模式
		{
			ctrl_mode = REMOTE_MODE;
			break;
		}
		case SW_MI:                     //中，保护模式
		{
			ctrl_mode = PROTECT_MODE;
			break;
		}
		case SW_DN:                     //下，键鼠模式
		{
			ctrl_mode = VISION_MODE;
			break;
		}
	}
//				等待电机初始化											保护优先级最高
	if ((gimbal_zero == NOFOUND || reload_zero == NOFOUND) && ctrl_mode != PROTECT_MODE)
		ctrl_mode = INIT_MODE;
	
	taskEXIT_CRITICAL();
}

