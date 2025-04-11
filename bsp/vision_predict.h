#ifndef __VISION_PREDICT_H__
#define __VISION_PREDICT_H__

#include "stdint.h"

/* �Ӿ���ģʽ */
typedef enum {
    vMODE_AUTO        = 0,    /* ���� */
    vMODE_bENERGY     = 1,    /* ���������� */
    vMODE_sENERGY     = 2,    /* С�������� */
} vision_mode_e;

/* ��̨��̬��ʷ��Ϣ */
typedef struct {
    float pit;
    float wy;
    float yaw;
    float wz;
} gim_msg_t;

/* �Ӿ�״̬ */
typedef enum {
    vUNAIMING   = 0,    /* û��ʶ��Ŀ�� */
    vAIMING     = 1,    /* ʶ��Ŀ�� */
    vFIRST_LOST = 2,     /* �״ζ�ʧĿ�� */
	vFIRST_LOCK = 3
} vsn_output_status_e;


/* �Ӿ��㷨�м���� */
typedef struct {

    float       period;         /* �Ӿ����� */
    float       test_period;    /* ʵ���Ӿ����� */
    uint8_t     valid_cnt;      /* �����յ���Ч���ݼ���λ */
} vsn_data_t;

/* �Ӿ�ģ��ӿ� */
typedef struct {
    vision_mode_e mode;  /* �Ӿ�ģʽ */
    volatile vsn_output_status_e status;    /* �Ӿ����״̬ */
    volatile uint8_t new_frame_flag;        /* ��һ֡���ݱ�־ */
    volatile uint8_t shoot_enable;          /* ���������־ */
	float shoot_enable_limit;
	uint8_t fire_flag;
	uint8_t stop_fire;
    
    float gimbal_pit_ecd; /* ʱ����ͬ��ʱ��̨pit�Ƕ� */
    float pit_angle_error;
    float pit_predict_angle;
    
    float gimbal_yaw_angle; /* ʱ����ͬ��ʱ��̨yaw�Ƕ� */
    float yaw_angle_error;
    float yaw_predict_angle;
} vsn_output_t;

extern vsn_output_t  vision; /* �Ӿ�ģ�����ӿ� */
extern vsn_data_t    vd;     /* �Ӿ�ԭʼ����������м����(����) */

void vsn_init(void);
void vsn_mode_switch_init(void);
void vsn_calc(void);
void vsn_gimbal_ref_calc(void);
int rotation_direction_judge(void);
void vsn_shoot_enable(void);
#endif
