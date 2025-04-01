#include "micro_swicth.h"
#include "main.h"
#include "bsp_remote_ET08.h"

micro_switch_state_e gimbal_zero;
micro_switch_state_e reload_zero;

static uint16_t key_tick;

void MicroSwicth_Callback(void)
{
	//Î¢¶¯¿ª¹Ø£¨ºìNO£¬ºÚCOM£©
	if(key_tick ++ > 20)
	{
		key_tick = 0;
		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_15) == GPIO_PIN_RESET) 
			gimbal_zero = FOUND;
		
		if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_14) == GPIO_PIN_RESET) 
			reload_zero = FOUND;
		
		if (gimbal_zero == FOUND && reload_zero == FOUND)
		{}
	}
}
	
