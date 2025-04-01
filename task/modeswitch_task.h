#ifndef _MODESWITCH_TASK_H__
#define _MODESWITCH_TASK_H__

/* 板级支持文件 */
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"

#define MODESWITCH_PERIOD 10

typedef enum{
	LOCK = 0,
	UNLOCK
}lock_flag_e;

typedef enum
{
    PROTECT_MODE = 0,   //保护模式
	INIT_MODE,			//初始化模式
    REMOTE_MODE,   		//遥控模式
    KEYBOARD_MODE,  	//键盘模式
    VISION_MODE     	//视觉模式(鼠标右键开启)
} ctrl_mode_e;

extern lock_flag_e lock_flag;
extern ctrl_mode_e ctrl_mode;

void ModeSwitch_Task(void const *argu);


#endif 
