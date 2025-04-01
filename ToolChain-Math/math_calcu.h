#ifndef _MATH_CALCU_H_
#define _MATH_CALCU_H_

#include "stdint.h"

#define N2 100
#define e  2.718282f
#define SIGMOID_PERIOD 0.133333f
#define SIGMOID_MAX    10

/* 圆周率 */
#ifndef PI
    #define PI 3.14159265358979323846f
#endif

/* 符号 */
#define SIGN(x) ( (x) > 0? (1): ( (x) < 0? (-1): 0 ) )

/* 绝对值 */
#ifndef ABS
    #define ABS(x) ((x>0)? (x): (-(x)))
#endif

/* 输出限幅 */
#ifndef OUTPUT_LIMIT
#define OUTPUT_LIMIT(output,max,min) \
    ( (output) <= (max) && (output)>=(min)? output: \
    ( (output) > (max)? (output = max):(output = min)) )
#endif

typedef struct
{
    float change_scale;
    uint16_t real_target;
    uint16_t limit_target;
    uint32_t ticks;
    uint32_t last_ticks;
} slope_t;

typedef __packed struct
{
    float input;        //输入数据
    float out;          //滤波输出的数据
    float num[1];       //滤波参数
    float frame_period; //滤波的时间间隔 单位 s
} first_order_filter_type_t;

void slope_calc(slope_t *V);

typedef struct
{
    float input;        //输入数据
    float out;          //输出数据
    float min_value;    //限幅最小值
    float max_value;    //限幅最大值
    float frame_period; //时间间隔
} ramp_function_source_t;

/* 滞环死区控制 */
typedef struct {
    float max;
    float min;
    float now;
    float last;
    int8_t out;  //发生跳变的方向
} delay_loop_t;

void ramp_calc(ramp_function_source_t *ramp_source_type, float frame_period, float input, float max, float min);
void Bubble_Sort(float *a, uint8_t n);
float GildeAverageValueFilter(float NewValue, float *Data);
float Sigmoid_function(float x);
float circle_error(float set, float get, float circle_para);
float data_limit(float data, float max, float min);
void least_square_linear_fit(float x[], float y[], const int num, float* a, float* b);

float vector_arg(float x, float y, float x_th, float y_th);

void delay_loop_init(delay_loop_t* dlp, float max, float min);
int8_t delay_loop_calc(delay_loop_t* dlp, float now_data);

#endif
