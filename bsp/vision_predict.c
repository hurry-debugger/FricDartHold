#include "vision_predict.h"
#include "math.h"
#include "string.h"
#include "KalmanFilter.h"
#include "vision_rx.h"
#include "math_calcu.h"

/* 非视觉必要头文件 */
#include "control_def.h"
#include "remote.h"
#include "modeswitch_task.h"
#include "gimbal_task.h"
#include "pid.h"

/* 圆周率 */
#ifndef PI
#define PI 3.14159265358979323846f
#endif

/* 绝对值 */
#ifndef ABS
#define ABS(x) ( (x>0)? (x): (-(x)) )
#endif

/* 符号 */
#ifndef SIGN
#define SIGN(x) ( (x)>0? (1): ( (x)<0? (-1): 0 ) )
#endif

/* 输出限幅 */
#ifndef OUTPUT_LIMIT
#define OUTPUT_LIMIT(output,max,min) \
    ( (output) <= (max) && (output)>=(min)? output: \
    ( (output) > (max)? (output = max):(output = min)) )
#endif

vsn_output_t vision;    /* 视觉计算结果输出结构体 */
vsn_data_t    vd;  

/* 电控侧预测三档系统 */
float kp1 = 1.6f;
float kp2 = 0.0f;
//float kp3 = 0.7f;//0 0 1.8


/*	视觉来决定开枪时机	*/
/////////////////////////////////////////////////////////////////////////
float shoot_enable_test = 1.5f; //1.9f

float shoot_enable_ANTISPIN_pit = 2.0f; //1.9f
float shoot_enable_ANTISPIN_yaw = 2.0f; //2.1f
/////////////////////////////////////////////////////////////////////////



/* ----------------------------------------------------------------- */
/*                           局部全局变量                            */
/* ----------------------------------------------------------------- */
/* 预测量系数 */
float   predict_kp_yaw;     /* 1.8f  0.7f */
float   predict_kp_pit;


/* ----------------------------------------------------------------- */
/*                         缓存区与定时器                            */
/* ----------------------------------------------------------------- */

/* ----------------------------------------------------------------- */
/*                           自定义函数                              */
/* ----------------------------------------------------------------- */
/** 
 * @brief 根据视觉模式对部分参数重新初始化
 */
void vsn_mode_switch_init(void) {
    /* 根据视觉模式调整部分参数 */
    switch (vision.mode) {
        case vMODE_AUTO: {  
            break;
        }
        case vMODE_bENERGY:
        case vMODE_sENERGY: { 
            break;
        }
    }
}

/* 开枪时机控制 */
void vsn_shoot_enable(void) {
    switch (vision.mode) {
        case vMODE_AUTO:{
            if ((ABS(gimbal.pid.yaw_agl_ref - gimbal.pid.yaw_agl_fdb) < vision.shoot_enable_limit && 
                ABS(gimbal.pid.pit_agl_ref - gimbal.pid.pit_agl_fdb) < vision.shoot_enable_limit)&&vision.fire_flag&&!vision.stop_fire) {
                vision.shoot_enable = 1;
            } else {
                vision.shoot_enable = 0;
            }
            break;
        }
        case vMODE_bENERGY:
        case vMODE_sENERGY: {
            if (ABS(gimbal.pid.yaw_agl_ref - gimbal.pid.yaw_agl_fdb) < vision.shoot_enable_limit && 
                ABS(gimbal.pid.pit_agl_ref - gimbal.pid.pit_agl_fdb) < vision.shoot_enable_limit) {
                vision.shoot_enable = 1;
            } else {
                vision.shoot_enable = 0;
            }
            break;
        }
    }
}

/** 
 * @brief 视觉云台设定值计算函数
 */
uint8_t flag;
void vsn_gimbal_ref_calc(void) {
	vsn_shoot_enable();
    switch (vision.status) {
        case vAIMING: {  /* 识别到目标 */
			pid_yaw_agl.PID_reset(&pid_yaw_agl,300.0f,3.0f,0.0f);
			pid_yaw_spd.PID_reset(&pid_yaw_spd,35.0f,0.0f,0.0f);			
            if (vision.new_frame_flag) { /* 接收到新的数据 */
                /* 清除标志，防止重复处理同一帧 */
                vision.new_frame_flag = 0;
                /* 修改云台设定值 */
                 /****************绝对方案*********************/
                gimbal.pid.yaw_agl_ref  = vision.yaw_predict_angle;
                gimbal.pid.pit_agl_ref  = vision.pit_predict_angle;                
           }
            break;
        }
        case vFIRST_LOST: {  /* 首次丢失目标 */
            vision.status = vUNAIMING;
            gimbal.pid.pit_agl_ref  = gimbal.pid.pit_agl_fdb;
            gimbal.pid.yaw_agl_ref  = gimbal.pid.yaw_agl_fdb;
            break;
        }
        case vUNAIMING: {  /* 未识别到目标 */ //将云台控制权交还给操作手
			pid_yaw_agl.PID_reset(&pid_yaw_agl,250.0f,0.0f,0.0f);
			pid_yaw_spd.PID_reset(&pid_yaw_spd,25.0f,0.01f,10.0f);			
            if (ctrl_mode == REMOTER_MODE) {
                gimbal.pid.pit_agl_ref  += rc.ch2 * 0.0008f;
                gimbal.pid.yaw_agl_ref  += rc.ch1 * scale.ch1;
            } else {
                gimbal.pid.pit_agl_ref  += rc.mouse.y * KEYBOARD_SCALE_PIT*0.1f; 
                gimbal.pid.yaw_agl_ref  += rc.mouse.x * KEYBOARD_SCALE_YAW;
            }
            break;
        }
        default:break;
    }
}


/*********************************************************************/
/** 
 * @brief 视觉预测相关变量初始化
 */
void vsn_init(void) {
    vision_mode_e last_vision_mode = vision.mode;
    memset(&vision,     0, sizeof(vsn_output_t));
    memset(&vsn_drive,  0, sizeof(vsn_drive_t));
    memset(&vd,         0, sizeof(vsn_data_t));
    vision.mode = last_vision_mode;
}



/** 
 * @brief 视觉历史数据初始化
 */
static void vsn_deinit(void) {
    /* 清空数据 */
    vision_mode_e last_vision_mode = vision.mode;
    memset(vsn_drive.rx, 0, sizeof(vsn_drive.rx));
    memset(&vd,          0, sizeof(vsn_data_t));
    memset(&vision,      0, sizeof(vsn_output_t));
    vision.mode = last_vision_mode;
}



/**
 * @brief 视觉数据预处理与预测逻辑
 */
void vsn_calc(void) {
	
	static vsn_output_status_e last_vsn_status;
    /* 数据量纲统一、方向统一 */
    vision.pit_predict_angle  = vsn_drive.rx[NOW].data.pit;            /* 视觉PIT相对角度（°） */
    vision.yaw_predict_angle  = vsn_drive.rx[NOW].data.yaw;            /* 视觉YAW相对角度（°） */
	vision.shoot_enable_limit = vsn_drive.rx[NOW].data.dis;			//视觉决定开枪时机
    vision.fire_flag          = vsn_drive.rx[NOW].data.fire_flag;    	/* 火控 */
    vsn_drive.rx[0].data.aim_flag |= vsn_drive.rx[0].data.ist_flag; /*<! 状态暂时合并 */
    
    /* 根据视觉模式修改参数 */
    vsn_mode_switch_init();
    
    /* 视觉预测流程 */
    if (vd.valid_cnt && vsn_drive.status != AIMING) {  /* 首次丢帧或异常重力下坠无解 */
        vd.valid_cnt = 0;
        vsn_deinit();                 /* 视觉历史数据清空 */
        vision.status = vFIRST_LOST;  /* 输出为首次丢帧状态 */
    } else if (vsn_drive.status == AIMING) {  /* 识别到目标 */
        ++vd.valid_cnt;  /* 有效数据计数 */
        vsn_shoot_enable();  /* 开枪时机控制 */
        vision.new_frame_flag = 1;  /* 标注为新有效数据 */
        vision.status = vAIMING;  /* 输出为识别到目标状态 */
    }else {  /* 未识别到目标 */
        vision.status = vUNAIMING;  /* 输出为丢失目标模式 */
    }
	last_vsn_status = vision.status;
}

