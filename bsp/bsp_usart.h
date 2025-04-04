#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "usart.h"
#include "main.h"

#define DBUS_HUART    		huart1
#define JUDGE_HUART   		huart3
#define SERIAL_SCREEN_HUART huart5

#define DMA_DBUS_LEN    25 
#define DMA_JUDGE_LEN 	100
#define DMA_SCREEN_LEN 	6

void USER_UART_Init(void);
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);

#endif

