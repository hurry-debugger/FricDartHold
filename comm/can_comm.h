#ifndef __CAN_COMM_H__
#define __CAN_COMM_H__

#include "stm32f4xx_hal.h"
#include "dji_motor.h"

//面对镖架发射的方向辨别摩擦轮左右
#define FRICTION_R_ID 0x201
#define FRICTION_L_ID 0x202
#define GIMBAL_ID     0x206
#define RELOAD_ID     0x207

#define SHOOT_MOTOR_MSG_SEND 	1<<0
#define RELAOD_MOTOR_MSG_SEND   1<<1
#define GIMBAL_MOTOR_MSG_SEND 	1<<2

#define MG3508_CAN_TX_ID		0x200//  ID  1--4
#define MG2006_CAN_TX_ID		0x1FF//  ID  5--8
#define MG3508_CAN_TX_ID		0x200//  ID  1--4

typedef struct 
{
	int16_t fric[2];
	int16_t gimbal;
	int16_t reload;
}motor_current_t;

extern motor_current_t motor_current;
extern motor_measure_t fric_motor_data[2];
extern motor_measure_t reload_motor_data;
extern motor_measure_t gimbal_motor_data;

void Can_Send_Task(void const *argu);

void Can_Device_Init(void);
static void Can_Msg_To_Buf(uint8_t* rx_buff, uint8_t* CAN_RX_Data);

#endif
