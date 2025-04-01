/**
  ******************************************************************************
  * @file	 pid.h
  * @author  Hongxi Wong
  * @version V1.0.6
  * @date    2019/12/17
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#ifndef _PID_H
#define _PID_H

#include "stdint.h"
#ifndef ABS
#define ABS(x) ((x > 0) ? x : -x)
#endif
/**
 * @brief Enumeration of PID improvements.
 * 
 * This enumeration represents different improvements that can be applied to a PID controller.
 * Each improvement is represented by a bit flag, allowing multiple improvements to be combined using bitwise OR.
 * 
 * The available improvements are:
 * - NONE: No improvement.
 * - Integral_Limit: Integral limit improvement.
 * - Derivative_On_Measurement: Derivative on measurement improvement.
 * - Trapezoid_Intergral: Trapezoid integral improvement.
 * - Proportional_On_Measurement: Proportional on measurement improvement.
 * - OutputFilter: Output filter improvement.
 * - ChangingIntegralRate: Changing integral rate improvement.
 * - DerivativeFilter: Derivative filter improvement.
 * - ErrorHandle: Error handle improvement.
 */
typedef enum pid_Improvement_e
{
    NONE = 0X00,                        //0000 0000
    Integral_Limit = 0x01,              //0000 0001
    Derivative_On_Measurement = 0x02,   //0000 0010
    Trapezoid_Intergral = 0x04,         //0000 0100
    Proportional_On_Measurement = 0x08, //0000 1000
    OutputFilter = 0x10,                //0001 0000
    ChangingIntegralRate = 0x20,        //0010 0000
    DerivativeFilter = 0x40,            //0100 0000
    ErrorHandle = 0x80,                 //1000 0000
} PID_Improvement_e;

typedef enum errorType_e
{
    PID_ERROR_NONE = 0x00U,
    Motor_Blocked = 0x01U
} ErrorType_e;

typedef struct
{
    uint64_t ERRORCount;
    ErrorType_e ERRORType;
} PID_ErrorHandler_t;

typedef struct _PID_TypeDef
{
    float Target;
    float LastNoneZeroTarget;
    float Kp;
    float Ki;
    float Kd;

    float Measure;
    float Last_Measure;
    float Err;
    float Last_Err;

    float Pout;
    float Iout;
    float Dout;
    float ITerm;

    float Output;
    float Last_Output;
    float Last_Dout;

    float MaxOut;
    float IntegralLimit;
    float DeadBand;
    float ControlPeriod;
    float MaxErr;
    float ScalarA; //For Changing Integral
    float ScalarB; //ITerm = Err*((A-abs(err)+B)/A)  when B<|err|<A+B
    float Output_Filtering_Coefficient;
    float Derivative_Filtering_Coefficient;

    uint32_t thistime;
    uint32_t lasttime;
    uint8_t dtime;

    uint8_t Improve;

    PID_ErrorHandler_t ERRORHandler;

    void (*PID_param_init)(
        struct _PID_TypeDef *pid,
        uint16_t maxOut,
        uint16_t integralLimit,
        float deadband,
        float Kp,
        float ki,
        float kd,
        float A,
        float B,
        float output_filtering_coefficient,
        float derivative_filtering_coefficient,
        uint8_t improve);

    void (*PID_reset)(
        struct _PID_TypeDef *pid,
        float Kp,
        float ki,
        float kd);
} PID_TypeDef;

static void f_Trapezoid_Intergral(PID_TypeDef *pid);
static void f_Integral_Limit(PID_TypeDef *pid);
static void f_Derivative_On_Measurement(PID_TypeDef *pid);
static void f_Changing_Integral_Rate(PID_TypeDef *pid);
static void f_Output_Filter(PID_TypeDef *pid);
static void f_Derivative_Filter(PID_TypeDef *pid);
static void f_Output_Limit(PID_TypeDef *pid);
static void f_Proportion_Limit(PID_TypeDef *pid);
static void f_PID_ErrorHandle(PID_TypeDef *pid);

void PID_Init(
    PID_TypeDef *pid,
    uint16_t max_out,
    uint16_t intergral_limit,
    float deadband,

    float kp,
    float ki,
    float kd,

    float A,
    float B,

    float output_filtering_coefficient,
    float derivative_filtering_coefficient,

    uint8_t improve);
float PID_Calculate(PID_TypeDef *pid, float measure, float target);
	
// YAW轴 角度速度串级
extern PID_TypeDef pid_yaw_agl;
extern PID_TypeDef pid_yaw_spd;
	
	
////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    float output;
    float lag_filter;
} LagCompensator;	
	

/*定义前馈控制器的结构体*/
typedef struct{
  float rin;
  float lastRin;
  float perrRin;
}FFC_t;

extern FFC_t yaw_ffc;

float FeedforwardController(FFC_t *vFFC,float a,float b);

////////////////////////////////////////////////////////////////////////////////////////////////
/*定义结构体和公用体*/
typedef struct
{
  float setpoint;               /*设定值*/
  float kp;                     /*比例系数*/
  float ki;                     /*积分系数*/
  float kd;                     /*微分系数*/
  float lasterror;              /*前一拍偏差*/
  float preerror;               /*前两拍偏差*/
  float result;                 /*PID控制器结果*/
  float output;                 /*输出值，0-100，为百分比值*/
  float maximum;                /*输出值上限*/
  float minimum;                /*输出值下限*/
  float errorabsmax;            /*偏差绝对值最大值*/
  float errorabsmid;            /*偏差绝对值中位值*/
  float errorabsmin;            /*偏差绝对值最小值*/
}EXPERTPID;
		
#endif
