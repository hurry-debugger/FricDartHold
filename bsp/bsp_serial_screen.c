#include "cmsis_os.h"

#include "string.h"

#include "bsp_serial_screen.h"
#include "bsp_usart.h"
#include "bsp_flash.h"

/**								
* @brief 接收串口屏数据解析   t  num val1 val2 g(imbal) val1 val2 FF
* @param  无				head  type  num   val   tail
* @retval 无
*/
uint16_t serial_fric_vel[4];
uint32_t serial_gimbal_pos[4];
uint8_t serial_protect_flag;
uint8_t serial_num;
uint8_t flash_flag;
uint32_t flash_temp[6];
uint8_t screen_update_flag;
static uint8_t lcd_init_flag;//内部使用
uint8_t screen_init_flag;///外部使用

#define SERIAL_SCREEN_PERIOD 10

void Serial_Screen_Task(const void* argu)
{
	uint32_t mode_wake_time = osKernelSysTick();
	u32_To_u16(serial_fric_vel, (uint32_t*)FRIC_ADDR, 8);
	memcpy(serial_gimbal_pos, (uint32_t*)GIMBAL_ADDR, sizeof(serial_gimbal_pos));
	while (1)
	{
		if(lcd_init_flag)
		{
			lcd_init_flag = 0;
			Uart_printf(&SERIAL_SCREEN_HUART, "start.j0.val=100\xff\xff\xff");
			osDelay(4);
			Uart_printf(&SERIAL_SCREEN_HUART, "test.n0.val=%d\xff\xff\xff", serial_fric_vel[0]);	
			osDelay(4);
			Uart_printf(&SERIAL_SCREEN_HUART, "test.n2.val=0\xff\xff\xff", serial_gimbal_pos[0]);
			screen_init_flag = 1;
		}
		if(flash_flag)//保存串口屏数据进flash
		{
			u16_To_u32(flash_temp, serial_fric_vel, 8);
			memcpy(flash_temp+2, serial_gimbal_pos, 16);
			WriteFlashData(FRIC_ADDR, flash_temp, 6);
			flash_flag = 0;
		}
		if(screen_update_flag)
		{
			Uart_printf(&SERIAL_SCREEN_HUART, "test.n1.val=%d\xff\xff\xff", serial_gimbal_pos[serial_num]);
			osDelay(1);
			Uart_printf(&SERIAL_SCREEN_HUART, "test.n0.val=%d\xff\xff\xff", serial_fric_vel[serial_num]);
			screen_update_flag = 0;
		}
		osDelayUntil(&mode_wake_time, SERIAL_SCREEN_PERIOD);
	}
}


void Serial_Screen_Handle(uint8_t* screen_buf)
{
		switch (screen_buf[0])
		{
			case 'p':				//保护模式
				serial_protect_flag = 1;
				break;
			case 'e':				//exit退出保护模式
				serial_protect_flag = 0;
				break;
			case 't':				//串口屏发送数据更新单片机数据  先发低八位
			{				//镖体编号
				serial_fric_vel[screen_buf[1]] = screen_buf[2] | screen_buf[3]<<8;
				serial_gimbal_pos[screen_buf[1]] = screen_buf[4] | screen_buf[5]<<8 | screen_buf[6]<<16 | screen_buf[7]<<24;
			}	break;
			case 's':				//保存串口屏数据进flash
			{
				flash_flag = 1;
			}	break;		
			case 'r':				//在task更新单片机数据到串口屏
			{				 //镖体编号
				serial_num = screen_buf[1];
				screen_update_flag = 1;
			}	break;	
			case 'i':				//串口屏初始化成功
			{
				lcd_init_flag = 1;
			}	break;

			default:
				break;
		}
}
