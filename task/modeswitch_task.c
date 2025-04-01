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

ctrl_mode_e ctrl_mode;
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
			if(gimbal_zero != FOUND && reload_zero != FOUND)	//判断电机初始化是否完成
				ctrl_mode = INIT_MODE;
			else 
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
	last_vision_flag = ctrl_mode;
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
			ctrl_mode = KEYBOARD_MODE;
			break;
		}
	}
	
	if ( gimbal_zero == NOFOUND || reload_zero == NOFOUND)
		ctrl_mode =INIT_MODE;
	
  	if((last_vision_flag == VISION_MODE)&&(ctrl_mode != VISION_MODE))
	{	
		vision_flag = 1;
	}
}

////右拨杆模式切换
//static void sw2_mode_handler(void)	
//{
//    if( gimbal.mode == REMOTE_MODE )
//    {
//      switch( SBUS.Ch7)
//      {
//				case SW_UP:                     //上，摩擦轮停止
//				{
//					shoot.mode = SHOOT_STOP_MODE;
//					shoot.fire_mode = SINGLE_MODE;
//					break;
//				}
//				case SW_MI:                     //中，单发模式
//				{
//					shoot.mode = SHOOT_FIRE_MODE;
//					shoot.fire_mode = SINGLE_MODE;
//					break;
//				}
//				case SW_DN:                     //下，连发模式
//				{
//					shoot.mode = SHOOT_FIRE_MODE;
//					shoot.fire_mode = SERIES_MODE;
//					break;
//				}
//      }
//    }
//}
