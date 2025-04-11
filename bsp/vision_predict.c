#include "vision_predict.h"
#include "math.h"
#include "string.h"
#include "KalmanFilter.h"
#include "vision_rx.h"
#include "math_calcu.h"

/* ���Ӿ���Ҫͷ�ļ� */
#include "control_def.h"
#include "remote.h"
#include "modeswitch_task.h"
#include "gimbal_task.h"
#include "pid.h"

/* Բ���� */
#ifndef PI
#define PI 3.14159265358979323846f
#endif

/* ����ֵ */
#ifndef ABS
#define ABS(x) ( (x>0)? (x): (-(x)) )
#endif

/* ���� */
#ifndef SIGN
#define SIGN(x) ( (x)>0? (1): ( (x)<0? (-1): 0 ) )
#endif

/* ����޷� */
#ifndef OUTPUT_LIMIT
#define OUTPUT_LIMIT(output,max,min) \
    ( (output) <= (max) && (output)>=(min)? output: \
    ( (output) > (max)? (output = max):(output = min)) )
#endif

vsn_output_t vision;    /* �Ӿ�����������ṹ�� */
vsn_data_t    vd;  

/* ��ز�Ԥ������ϵͳ */
float kp1 = 1.6f;
float kp2 = 0.0f;
//float kp3 = 0.7f;//0 0 1.8


/*	�Ӿ���������ǹʱ��	*/
/////////////////////////////////////////////////////////////////////////
float shoot_enable_test = 1.5f; //1.9f

float shoot_enable_ANTISPIN_pit = 2.0f; //1.9f
float shoot_enable_ANTISPIN_yaw = 2.0f; //2.1f
/////////////////////////////////////////////////////////////////////////



/* ----------------------------------------------------------------- */
/*                           �ֲ�ȫ�ֱ���                            */
/* ----------------------------------------------------------------- */
/* Ԥ����ϵ�� */
float   predict_kp_yaw;     /* 1.8f  0.7f */
float   predict_kp_pit;


/* ----------------------------------------------------------------- */
/*                         �������붨ʱ��                            */
/* ----------------------------------------------------------------- */

/* ----------------------------------------------------------------- */
/*                           �Զ��庯��                              */
/* ----------------------------------------------------------------- */
/** 
 * @brief �����Ӿ�ģʽ�Բ��ֲ������³�ʼ��
 */
void vsn_mode_switch_init(void) {
    /* �����Ӿ�ģʽ�������ֲ��� */
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

/* ��ǹʱ������ */
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
 * @brief �Ӿ���̨�趨ֵ���㺯��
 */
uint8_t flag;
void vsn_gimbal_ref_calc(void) {
	vsn_shoot_enable();
    switch (vision.status) {
        case vAIMING: {  /* ʶ��Ŀ�� */
			pid_yaw_agl.PID_reset(&pid_yaw_agl,300.0f,3.0f,0.0f);
			pid_yaw_spd.PID_reset(&pid_yaw_spd,35.0f,0.0f,0.0f);			
            if (vision.new_frame_flag) { /* ���յ��µ����� */
                /* �����־����ֹ�ظ�����ͬһ֡ */
                vision.new_frame_flag = 0;
                /* �޸���̨�趨ֵ */
                 /****************���Է���*********************/
                gimbal.pid.yaw_agl_ref  = vision.yaw_predict_angle;
                gimbal.pid.pit_agl_ref  = vision.pit_predict_angle;                
           }
            break;
        }
        case vFIRST_LOST: {  /* �״ζ�ʧĿ�� */
            vision.status = vUNAIMING;
            gimbal.pid.pit_agl_ref  = gimbal.pid.pit_agl_fdb;
            gimbal.pid.yaw_agl_ref  = gimbal.pid.yaw_agl_fdb;
            break;
        }
        case vUNAIMING: {  /* δʶ��Ŀ�� */ //����̨����Ȩ������������
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
 * @brief �Ӿ�Ԥ����ر�����ʼ��
 */
void vsn_init(void) {
    vision_mode_e last_vision_mode = vision.mode;
    memset(&vision,     0, sizeof(vsn_output_t));
    memset(&vsn_drive,  0, sizeof(vsn_drive_t));
    memset(&vd,         0, sizeof(vsn_data_t));
    vision.mode = last_vision_mode;
}



/** 
 * @brief �Ӿ���ʷ���ݳ�ʼ��
 */
static void vsn_deinit(void) {
    /* ������� */
    vision_mode_e last_vision_mode = vision.mode;
    memset(vsn_drive.rx, 0, sizeof(vsn_drive.rx));
    memset(&vd,          0, sizeof(vsn_data_t));
    memset(&vision,      0, sizeof(vsn_output_t));
    vision.mode = last_vision_mode;
}



/**
 * @brief �Ӿ�����Ԥ������Ԥ���߼�
 */
void vsn_calc(void) {
	
	static vsn_output_status_e last_vsn_status;
    /* ��������ͳһ������ͳһ */
    vision.pit_predict_angle  = vsn_drive.rx[NOW].data.pit;            /* �Ӿ�PIT��ԽǶȣ��㣩 */
    vision.yaw_predict_angle  = vsn_drive.rx[NOW].data.yaw;            /* �Ӿ�YAW��ԽǶȣ��㣩 */
	vision.shoot_enable_limit = vsn_drive.rx[NOW].data.dis;			//�Ӿ�������ǹʱ��
    vision.fire_flag          = vsn_drive.rx[NOW].data.fire_flag;    	/* ��� */
    vsn_drive.rx[0].data.aim_flag |= vsn_drive.rx[0].data.ist_flag; /*<! ״̬��ʱ�ϲ� */
    
    /* �����Ӿ�ģʽ�޸Ĳ��� */
    vsn_mode_switch_init();
    
    /* �Ӿ�Ԥ������ */
    if (vd.valid_cnt && vsn_drive.status != AIMING) {  /* �״ζ�֡���쳣������׹�޽� */
        vd.valid_cnt = 0;
        vsn_deinit();                 /* �Ӿ���ʷ������� */
        vision.status = vFIRST_LOST;  /* ���Ϊ�״ζ�֡״̬ */
    } else if (vsn_drive.status == AIMING) {  /* ʶ��Ŀ�� */
        ++vd.valid_cnt;  /* ��Ч���ݼ��� */
        vsn_shoot_enable();  /* ��ǹʱ������ */
        vision.new_frame_flag = 1;  /* ��עΪ����Ч���� */
        vision.status = vAIMING;  /* ���Ϊʶ��Ŀ��״̬ */
    }else {  /* δʶ��Ŀ�� */
        vision.status = vUNAIMING;  /* ���Ϊ��ʧĿ��ģʽ */
    }
	last_vsn_status = vision.status;
}

