#include "can_send_task.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "shoot_task.h"
#include "gimbal_task.h"
#include "can_comm.h"

void Can_Send_Task(void const *argu)
{
    osEvent event;
    for(;;)
    {
        event = osSignalWait(SHOOT_MOTOR_MSG_SEND, osWaitForever);

        if (event.status == osEventSignal)
        {
			taskENTER_CRITICAL();//fric
			if(event.value.signals & SHOOT_MOTOR_MSG_SEND)
				can1_send_message(MG3508_CAN_TX_ID, shoot.fric_current[R],shoot.fric_current[L], 0, 0);	
			
			if(event.value.signals & RELAOD_MOTOR_MSG_SEND || \
				event.value.signals & GIMBAL_MOTOR_MSG_SEND)//reload
				can2_send_message(0x1FF, 0, gimbal.current, RELAOD_MOTOR_MSG_SEND, 0);	
			
//			if(event.value.signals & GIMBAL_MOTOR_MSG_SEND)//gimbal	
//				can2_send_message(MG6020_CAN_TX_ID, motor.fric_current[R],motor.fric_current[L], 0, 0);	
			taskEXIT_CRITICAL();
        }
     }
}
