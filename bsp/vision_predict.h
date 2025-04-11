#ifndef __VISION_PREDICT_H__
#define __VISION_PREDICT_H__

#include "stdint.h"

/* 视觉侧模式 */
typedef enum {
    vMODE_AUTO        = 0,    /* 自瞄 */
    vMODE_bENERGY     = 1,    /* 大能量机关 */
    vMODE_sENERGY     = 2,    /* 小能量机关 */
} vision_mode_e;

/* 云台姿态历史信息 */
typedef struct {
    float pit;
    float wy;
    float yaw;
    float wz;
} gim_msg_t;

/* 视觉状态 */
typedef enum {
    vUNAIMING   = 0,    /* 没有识别到目标 */
    vAIMING     = 1,    /* 识别到目标 */
    vFIRST_LOST = 2,     /* 首次丢失目标 */
	vFIRST_LOCK = 3
} vsn_output_status_e;


/* 视觉算法中间变量 */
typedef struct {

    float       period;         /* 视觉周期 */
    float       test_period;    /* 实测视觉周期 */
    uint8_t     valid_cnt;      /* 连续收到有效数据计数位 */
} vsn_data_t;

/* 视觉模块接口 */
typedef struct {
    vision_mode_e mode;  /* 视觉模式 */
    volatile vsn_output_status_e status;    /* 视觉输出状态 */
    volatile uint8_t new_frame_flag;        /* 新一帧数据标志 */
    volatile uint8_t shoot_enable;          /* 发弹允许标志 */
	float shoot_enable_limit;
	uint8_t fire_flag;
	uint8_t stop_fire;
    
    float gimbal_pit_ecd; /* 时间轴同步时云台pit角度 */
    float pit_angle_error;
    float pit_predict_angle;
    
    float gimbal_yaw_angle; /* 时间轴同步时云台yaw角度 */
    float yaw_angle_error;
    float yaw_predict_angle;
} vsn_output_t;

extern vsn_output_t  vision; /* 视觉模块对外接口 */
extern vsn_data_t    vd;     /* 视觉原始数据与计算中间变量(调试) */

void vsn_init(void);
void vsn_mode_switch_init(void);
void vsn_calc(void);
void vsn_gimbal_ref_calc(void);
int rotation_direction_judge(void);
void vsn_shoot_enable(void);
#endif
