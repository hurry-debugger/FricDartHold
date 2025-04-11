#ifndef _CONTROL_DEF_H_
#define _CONTROL_DEF_H_

#include "stdint.h"

/********更改YAW中值***********/
#define GIMBAL_YAW_CENTER_OFFSET 4769   // 3701   //新舵轮

/*----------------------------- player preference ----------------------------- */
#define KEY_CHASSIS_FIGHT 		KB_V
#define KEY_CHASSIS_ROTATE 		KB_SHIFT 
#define KEY_CHASSIS_POWER 		KB_C

#define KEY_Q_GIMBAL_TURN_LEFT 	KB_Q // KB_Q
#define KEY_E_GIMBAL_TURN_RIGHT KB_E // KB_E
#define KEY_F_GIMBAL_TURN_BACK 	KB_F

#define KEY_SHOOT_HZ_CHANGE 	KB_B

#define KEY_VISION_sENERGY 		KB_Z
#define KEY_VISION_bENERGY 		KB_X

/*-----------------------------shoot-----------------------------*/

/* 拨盘 PID 参数 */
#define PID_TRIGGER_ECD_P 0.3f
#define PID_TRIGGER_ECD_I 0.0f
#define PID_TRIGGER_ECD_D 0.3f

#define PID_TRIGGER_SPD_P 5.0f
#define PID_TRIGGER_SPD_I 0.005f
#define PID_TRIGGER_SPD_D 0.0f



/*-----------------------------chassis---------------------------*/
#define RC_CH4_SCALE 12
#define RC_CH3_SCALE 12

#define SPEED_40W 3200.0f
#define SPEED_45W 4900.0f // 4000.0f     //3400
#define SPEED_50W 5300.0f // 5300.0f     //5500
#define SPEED_55W 5500.0f // 5500.0f     //5700
#define SPEED_60W 5600.0f // 6200.0f     //6700
#define SPEED_65W 5800.0f // 6200.0f     //6700
#define SPEED_70W 5900.0f // 6200.0f     //
#define SPEED_75W 6100.0f // 6200.0f     //
#define SPEED_80W 6300.0f // 6850.0f     //
#define SPEED_85W 6900.0f // 6850.0f     //
#define SPEED_90W 7000.0f // 6200.0f     //
#define SPEED_95W 7500.0f // 6850.0f     //
#define SPEED_100W 8050.f // 7650.0f     //8150
#define SPEED_120W 8350.f // 7950.0f     //8150
#define SPEED_SUPERCAP 8600.0f
#define SPEED_0W SPEED_50W // 裁判系统底盘功率调为规则外时
#define SPEED_SUPPLY 2000.0f

#define SUPERCAP_CHAGER_VOLAGE 23.5f
#define SUPERCAP_DISCHAGER_VOLAGE 13.5f // 超级电容放电电压下限

/*-----------------------------gimbal----------------------------*/
#define Reduction_ratio 1.0f // pit轴减速比
#define RC_CH2_SCALE 0.003f
#define RC_CH1_SCALE (-0.0005f)

#define KEYBOARD_SCALE_PIT (-0.001f)         //-0.03f
#define KEYBOARD_SCALE_PIT_SUPPLY (-0.0006f) // 0.008f
#define KEYBOARD_SCALE_YAW (-0.001f)
#define KEYBOARD_SCALE_YAW_SUPPLY (-0.0006f)

#define GIMBAL_PIT_CENTER_OFFSET 1346
#define GIMBAL_PIT_MAX 2340
#define GIMBAL_PIT_MAX_SUPPLY 2340
#define GIMBAL_PIT_MIN 945

#define GIMBAL_YAW_BETWEEN_ECD (8192 / 8)
#define FIGHT_OFFSET_ERR  0//(1.0f * GIMBAL_YAW_BETWEEN_ECD / 8191 * 2 * PI) // 坐标系变换 -1为右45度，1为左45度

/* YAW轴PID系数 */
// #define pid_yaw_angle_P 170.0f
// #define pid_yaw_angle_P 180.0f
// #define pid_yaw_angle_I 0.00f
// #define pid_yaw_angle_D 10.0f

// #define pid_yaw_spd_P   15.0f
// #define pid_yaw_spd_I   0.10f
// #define pid_yaw_spd_D   0.00f


#define pid_yaw_angle_P 200.0f // 200
#define pid_yaw_angle_I 1.00f
#define pid_yaw_angle_D 20.0f

#define pid_yaw_spd_P 20.0f
#define pid_yaw_spd_I 0.1f
#define pid_yaw_spd_D 0.00f

/* PIT轴PID系数 */
// #define pid_pit_ecd_P   12.0f
#define pid_pit_ecd_P 10.0f
#define pid_pit_ecd_I 0.0f
#define pid_pit_ecd_D 0.0f

#define pid_pit_angle_P 130.0f
#define pid_pit_angle_I 0.00f
#define pid_pit_angle_D 20.0f

#define pid_pit_spd_P 8.0f
#define pid_pit_spd_I 0.1f
#define pid_pit_spd_D 0.0f

// 编码器ecd环系数
#define pid_yaw_mecd_P 8.0f
#define pid_yaw_mecd_I 0.0f
#define pid_yaw_mecd_D 0.0f

#define pid_yaw_mspd_P 10.0f
#define pid_yaw_mspd_I 0.1f
#define pid_yaw_mspd_D 0.0f

/* 弹舱盖 */
#define PWM_SEND_TIM12      (&htim12)
#define PWM_SEND_CH1        TIM12->CCR1		//PB14
#define COVER_PWM_CLOSE      1700			//500
#define COVER_PWM_OPEN     860				//2350
#define Magazine_PWM        PWM_SEND_CH1	//pwm设置占空比
#define Magazine_Time_CH    PWM_SEND_TIM12, TIM_CHANNEL_1

enum
{
    NOW = 0,
    LAST = 1,
    LLAST = 2
};

#endif


