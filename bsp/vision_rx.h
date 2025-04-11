/**
  * @brief �Ӿ���Ϣ������Ԥ�����
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

/* �Ӿ�����״̬ö������ */
typedef enum {
    AIMING = 0,
    UNAIMING = 1,
    TAIL_ERROR = 2,    
    REPEAT_ERROR = 3
} vsn_rx_status_e;

/* �Ӿ������ṹ������ */
typedef struct {
    uint32_t repeat_cnt;    /* �յ��ظ�֡���� */
    vsn_rx_status_e status; /* �Ӿ�����״̬ */
    union {
        uint8_t buff[VISION_RX_NUM];/* �Ӿ������� */
        __packed struct {           /* �Ӿ����ݴ������� */    
            float yaw;              /* �Ӿ����yaw�Ƕ� */
            float pit;              /* �Ӿ����pit�Ƕ� */
            float dis;              /* �Ӿ���ǹʱ�� */
            float fire_flag;              /* ��� */
            float pos;              /* װ�װ���̬�� ��0*/
            uint8_t shoot_flag;     /*Ӣ�ۻ���λ*/ 
            uint8_t cnt     :6;     /* �Ӿ��෢���Լ�λ */
            uint8_t ist_flag:1;     /* �Ӿ��࿪ʼ��֡ */
            uint8_t aim_flag:1;     /* �Ӿ�ʶ��Ŀ�� */
            uint8_t eof;            /* �Ӿ�����֡֡β */
        } data;                     /* �Ӿ����� */
    } rx[2];
} vsn_drive_t;

extern vsn_drive_t vsn_drive;

void vision_data_handler(uint8_t *Vision_Data); /* �Ӿ�������Ϣ���ջص� */

#endif

