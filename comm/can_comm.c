#include "can_comm.h"

#include "string.h"
#include "cmsis_os.h"

#include "bsp_can.h"
#include "dji_motor.h"

#include "shoot_task.h"
#include "gimbal_task.h"
#include "reload_task.h"

motor_measure_t fric_motor_data[2];
motor_measure_t reload_motor_data;
motor_measure_t gimbal_motor_data;

motor_current_t motor_current;

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
				can1_send_message(MG3508_CAN_TX_ID, motor_current.fric[R], motor_current.fric[L], 0, 0);	
			
			if(event.value.signals & RELAOD_MOTOR_MSG_SEND || \
				event.value.signals & GIMBAL_MOTOR_MSG_SEND)//reload
				can2_send_message(0x1FF, 0, motor_current.gimbal, motor_current.reload, 0);	
	
//			if(event.value.signals & GIMBAL_MOTOR_MSG_SEND)//gimbal	
//				can2_send_message(MG6020_CAN_TX_ID, motor.fric_current[R],motor.fric_current[L], 0, 0);	
			taskEXIT_CRITICAL();
        }
     }
}



static void User_Can1_CallBack(uint32_t ID, uint8_t* CAN_RxData);
static void User_Can2_CallBack(uint32_t ID, uint8_t* CAN_RxData);

void Can_Device_Init(void)
{
    uint32_t can_id1[] = {FRICTION_L_ID, FRICTION_R_ID, 0x203, 0x204, 0xFFF};
    uint32_t can_id2[] = {0x001, 0x002, 0x205, GIMBAL_ID, RELOAD_ID, 0xFFF};
	
	canx_init(&hcan1, can_id1, User_Can1_CallBack);
	canx_init(&hcan2, can_id2, User_Can2_CallBack);
}


static void User_Can1_CallBack(uint32_t ID, uint8_t* CAN_RxData)
{
	switch (ID)
	{
		case FRICTION_L_ID:
		case FRICTION_R_ID:
		{
			static uint8_t i;
			i = ID - FRICTION_R_ID;
			fric_motor_data[i].msg_cnt++ < 50 ? get_moto_offset(&fric_motor_data[i], CAN_RxData) \
												:encoder_data_handler(&fric_motor_data[i], CAN_RxData);
			memset(CAN_RxData, 0 ,8);
		}
			break;
		case 0x203:
			
			break;
		case 0x204:
			
			break;
		
		default:	break;
	}
}

static void User_Can2_CallBack(uint32_t ID, uint8_t* CAN_RxData)
{
	switch (ID)
	{
		case 0x001:
			
			break;
		case 0x002:
			break;
		case 0x205:
			
			break;
		case GIMBAL_ID:
			gimbal_motor_data.msg_cnt++ < 50 ? get_moto_offset(&gimbal_motor_data, CAN_RxData) \
												:encoder_data_handler(&gimbal_motor_data, CAN_RxData);
			memset(CAN_RxData, 0 ,8);				
			break;
		case RELOAD_ID:
			reload_motor_data.msg_cnt++ < 50 ? get_moto_offset(&reload_motor_data, CAN_RxData) \
												:encoder_data_handler(&reload_motor_data, CAN_RxData);
			memset(CAN_RxData, 0 ,8);
			break;

		default:	break;
	}
}

//static void Can_Msg_To_Buf(uint8_t* rx_buff, uint8_t* CAN_RX_Data)
//{
//	memcpy(rx_buff, CAN_RX_Data, 8);
//}
