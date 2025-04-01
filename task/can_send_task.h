#ifndef __CAN_SEND_TASK_H__
#define __CAN_SEND_TASK_H__

#include "stm32f4xx_hal.h"

#define SHOOT_MOTOR_MSG_SEND 	1<<0
#define RELAOD_MOTOR_MSG_SEND   1<<1
#define GIMBAL_MOTOR_MSG_SEND 	1<<2

#define MG3508_CAN_TX_ID		0x200//  ID  1--4
#define MG2006_CAN_TX_ID		0x1FF//  ID  5--8
#define MG3508_CAN_TX_ID		0x200//  ID  1--4

void Can_Send_Task(void const *argu);

#endif
