/**
  * @file     bsp_remote_ET08.c
  * @version  v1.0
  * @date     2024/4/16
  *
  * @brief    天地飞遥控器数据解算函数
  * @note     接USART1   Baud Rate：100000 bits/s
	*
  *	@author   佚名搬运工 & LZY
  *
**/

#include "bsp_remote_ET08.h"

SBUS_Buffer SBUS;

static void User_Decode(SBUS_Buffer *SBUS)
{
	SBUS->Ch1 -= 1024;
	SBUS->Ch1 = ABS(SBUS->Ch1)<30 ? 0 : SBUS->Ch1;
	SBUS->Ch2 -= 1024;
	SBUS->Ch2 = ABS(SBUS->Ch2)<30 ? 0 : SBUS->Ch2;
	SBUS->Ch3 -= 1024;
	SBUS->Ch3 = ABS(SBUS->Ch3)<30 ? 0 : SBUS->Ch3;
	SBUS->Ch4 -= 1024;
	SBUS->Ch4 = ABS(SBUS->Ch4)<30 ? 0 : SBUS->Ch4;
	SBUS->SA  = SBUS->Ch5;
	SBUS->SB  = SBUS->Ch6;
	SBUS->SC  = SBUS->Ch7;
	SBUS->SD  = SBUS->Ch8;
}

void sbus_callback_handler(SBUS_Buffer *SBUS, uint8_t *SBUS_RXBuffer)
{
		SBUS->Start=SBUS_RXBuffer[0];
		SBUS->Ch3=((uint16_t)((SBUS_RXBuffer[3]&0xc0)>>6))|((((uint16_t)SBUS_RXBuffer[4])<<2))|(((uint16_t)(SBUS_RXBuffer[5]&0x01))<<10);
		if(SBUS->Ch3 == 1)//遥控器关机后，状态维持原来
		{
			SBUS->Ch3 = 1024;
			return;
		}
		SBUS->Ch1=((uint16_t)SBUS_RXBuffer[1])|((uint16_t)((SBUS_RXBuffer[2]&0x07)<<8));
		SBUS->Ch2=((uint16_t)((SBUS_RXBuffer[2]&0xf8)>>3))|(((uint16_t)(SBUS_RXBuffer[3]&0x3f))<<6)/2;
		SBUS->Ch4=((uint16_t)((SBUS_RXBuffer[5]&0xfe)>>1))|(((uint16_t)(SBUS_RXBuffer[6]&0x0f))<<7);
		SBUS->Ch5=((uint16_t)((SBUS_RXBuffer[6]&0xf0)>>4))|(((uint16_t)(SBUS_RXBuffer[7]&0x7f))<<4);
		SBUS->Ch6=((uint16_t)((SBUS_RXBuffer[7]&0x80)>>7))|(((uint16_t)SBUS_RXBuffer[8])<<1)|(((uint16_t)(SBUS_RXBuffer[9]&0x03))<<9);
		SBUS->Ch7=((uint16_t)((SBUS_RXBuffer[9]&0xfc)>>2))|(((uint16_t)(SBUS_RXBuffer[10]&0x1f))<<6);
		SBUS->Ch8=((uint16_t)((SBUS_RXBuffer[10]&0xe0)>>5))|(((uint16_t)(SBUS_RXBuffer[11]))<<3);
		SBUS->Ch9=((uint16_t)SBUS_RXBuffer[12])|(((uint16_t)(SBUS_RXBuffer[13]&0x07))<<8);
		SBUS->Ch10=((uint16_t)((SBUS_RXBuffer[13]&0xf8)>>3))|(((uint16_t)(SBUS_RXBuffer[14]&0x3f))<<5);
		SBUS->Ch11=((uint16_t)((SBUS_RXBuffer[14]&0xc0)>>6))|(((uint16_t)SBUS_RXBuffer[15])<<2)|(((uint16_t)(SBUS_RXBuffer[16]&0x01))<<10);
		SBUS->Ch12=((uint16_t)((SBUS_RXBuffer[16]&0xfe)>>1))|(((uint16_t)(SBUS_RXBuffer[17]&0x0f))<<7);
		SBUS->Ch13=((uint16_t)((SBUS_RXBuffer[17]&0xf0)>>4))|(((uint16_t)(SBUS_RXBuffer[18]&0x7f))<<4);
		SBUS->Ch14=((uint16_t)((SBUS_RXBuffer[18]&0x80)>>7))|(((uint16_t)SBUS_RXBuffer[19])<<1)|(((uint16_t)(SBUS_RXBuffer[20]&0x03))<<9);
		SBUS->Ch15=((uint16_t)((SBUS_RXBuffer[20]&0xfc)>>2))|(((uint16_t)(SBUS_RXBuffer[21]&0x1f))<<6);
		SBUS->Ch16=((uint16_t)((SBUS_RXBuffer[21]&0xe0)>>5))|(((uint16_t)SBUS_RXBuffer[22])<<3);
		SBUS->Flag=SBUS_RXBuffer[23];
		SBUS->End=SBUS_RXBuffer[24];
		
		User_Decode(SBUS);
}
