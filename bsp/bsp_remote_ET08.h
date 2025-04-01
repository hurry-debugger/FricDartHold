#ifndef _BSP_REMOTE_ET08_H_
#define _BSP_REMOTE_ET08_H_

#include "main.h"

#ifndef ABS
#define ABS(x) ((x > 0) ? x : -x)
#endif

typedef enum{
	SW_UP = 353,
    SW_MI = 1024,
    SW_DN = 1694,
}rc_sw_mode_e;

typedef struct{
	uint8_t   Start;
	int16_t  Ch1;
	int16_t  Ch2;
	int16_t  Ch3;
	int16_t  Ch4;
	int16_t  Ch5;
	int16_t  Ch6;
	int16_t  Ch7;
	int16_t  Ch8;
	int16_t  Ch9;
	int16_t  Ch10;
	int16_t  Ch11;
	int16_t  Ch12;
	int16_t  Ch13;
	int16_t  Ch14;
	int16_t  Ch15;
	int16_t  Ch16;
	uint8_t   Flag;
	uint8_t   End;
	
	int16_t SA;
	int16_t SB;
	int16_t SC;
	int16_t SD;	
}SBUS_Buffer;

extern SBUS_Buffer SBUS;

void sbus_callback_handler(SBUS_Buffer *SBUS, uint8_t *SBUS_RXBuffer);

#endif
