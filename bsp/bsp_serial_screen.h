#ifndef _SERIAL_SCREEN_TASK_H__
#define _SERIAL_SCREEN_TASK_H__

/* 板级支持文件 */
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"


#define FRIC_ADDR     ((uint32_t)0x08060000) 	//摩擦轮VEL存放起始地址 	8byte
#define GIMBAL_ADDR   ((uint32_t)0x08060008) 	//云台POS起始地			16byte

extern uint16_t serial_fric_vel[4];

void Serial_Screen_Task(const void* argu);
void Serial_Screen_Handle(uint8_t* screen_buf);


#endif 
