/**
  * @brief 视觉信息解算与预测计算
  */
#ifndef	__VISION_RX_H__
#define __VISION_RX_H__

#include "stdint.h"
#include "math.h"

#define VISION_RX_NUM 23

#define NAN_PROCESS(now, last)      \
    do {                            \
        if (isnan(now)) {           \
            (now) = (last);         \
        } else {                    \
            (last) = (now);         \
        }                           \
    } while (0)

/* 视觉驱动状态枚举类型 */
typedef enum {
    AIMING = 0,
    UNAIMING = 1,
    TAIL_ERROR = 2,    
    REPEAT_ERROR = 3
} vsn_rx_status_e;

/* 视觉驱动结构体类型 */
typedef struct {
    uint32_t repeat_cnt;    /* 收到重复帧计数 */
    vsn_rx_status_e status; /* 视觉接收状态 */
    union {
        uint8_t buff[VISION_RX_NUM];/* 视觉缓存区 */
        __packed struct {           /* 视觉数据处理共用体 */    
            float yaw;              /* 视觉相对yaw角度 */
            float pit;              /* 视觉相对pit角度 */
            float dis;              /* 视觉发枪时机 */
            float fire_flag;              /* 火控 */
            float pos;              /* 装甲板姿态角 发0*/
            uint8_t shoot_flag;     /*英雄击打位*/ 
            uint8_t cnt     :6;     /* 视觉侧发送自加位 */
            uint8_t ist_flag:1;     /* 视觉侧开始补帧 */
            uint8_t aim_flag:1;     /* 视觉识别到目标 */
            uint8_t eof;            /* 视觉发送帧帧尾 */
        } data;                     /* 视觉解码 */
    } rx[2];
} vsn_drive_t;

extern vsn_drive_t vsn_drive;

void vision_data_handler(uint8_t *Vision_Data); /* 视觉串口信息接收回调 */

#endif

