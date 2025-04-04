#ifndef _SERIAL_SCREEN_TASK_H__
#define _SERIAL_SCREEN_TASK_H__

/* 板级支持文件 */
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"

#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//扇区7起始地址, 128 Kbytes  

#define NUM_0_POS ((uint32_t)0x08060000)   //编号一的镖位置信息存放的位置，16字节
#define NUM_1_POS ((uint32_t)0x08060010)   //编号二的镖位置信息存放的位置，16字节
#define NUM_2_POS ((uint32_t)0x08060020)   //编号三的镖位置信息存放的位置，16字节
#define NUM_3_POS ((uint32_t)0x08060030)   //编号四的镖位置信息存放的位置，16字节 

#endif 
