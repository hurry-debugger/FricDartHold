/**
  * @file     math_calcu.c
  * @version  v2.0
  * @date     July,6th 2019
	*
  * @brief    数学计算函数
	*
  *	@author   Fatmouse
  *
  */
#include "math_calcu.h"
#include "string.h"
#include "math.h"
#include "cmsis_os.h"

/**
  * @brief          斜波函数计算，根据输入的值进行叠加，输入单位为 /s 即一秒后增加输入的值
  * @author         RM
  * @param[in]      斜波函数结构体
  * @param[in]      输入值
  * @param[in]      滤波参数
  * @retval         返回空
  */
void ramp_calc(ramp_function_source_t *ramp_source_type, float frame_period, float input, float max, float min)
{
    ramp_source_type->max_value = max;
    ramp_source_type->min_value = min;
    ramp_source_type->frame_period = frame_period;

    ramp_source_type->input = input;

    ramp_source_type->out += ramp_source_type->input * ramp_source_type->frame_period;

    if (ramp_source_type->out > ramp_source_type->max_value)
    {
        ramp_source_type->out = ramp_source_type->max_value;
    }
    else if (ramp_source_type->out < ramp_source_type->min_value)
    {
        ramp_source_type->out = ramp_source_type->min_value;
    }
}

void slope_calc(slope_t *V)
{
    V->last_ticks = V->ticks;
    V->ticks = osKernelSysTick();
	if((V->ticks-V->last_ticks)<10)		//小蜜蜂初始化防止重新校准
	{
		if(V->real_target !=V->limit_target)
		{
			if(V->real_target < V->limit_target)//加操作
			{
				V->real_target = V->real_target + V->change_scale* (V->ticks-V->last_ticks);
				if(V->real_target > V->limit_target)//限幅
				{
					V->real_target =  V->limit_target;
				}
			}
			else if(V->real_target > V->limit_target)//减操作
			{
				V->real_target =  V->real_target - V->change_scale* (V->ticks-V->last_ticks);
				if(V->real_target < V->limit_target)//限幅
				{
					V->real_target =  (short)V->limit_target;
				}
			}
		}
	}
}

/**
  * @brief          冒泡排序函数
  * @param[in]      数组从大到小排序
  * @retval         void
  */
void Bubble_Sort(float *a,uint8_t n)
{
    float buf;
    for(uint8_t i=0; i<n; ++i)
    {
        for(uint8_t j=0; j<n-1-i; ++j)
        {
            if(a[j]<a[j+1])
            {
                buf=a[j];
                a[j] = a[j+1];
                a[j+1] = buf;
            }
        }
    }
}

/**
  * @brief          滑动滤波函数   注：N2为滑动窗口大小
  * @author
  * @param[in]      滤波前的值、滤波缓存数组
  * @retval         滤波后的值
  */
float GildeAverageValueFilter(float NewValue,float *Data)
{
    float max,min;
    float sum;
    uint16_t i;
    Data[0]=NewValue;
    max=Data[0];
    min=Data[0];
    sum=Data[0];
    for(i=N2-1; i!=0; i--)
    {
        if(Data[i]>max) max=Data[i];
        else if(Data[i]<min) min=Data[i];
        sum+=Data[i];
        Data[i]=Data[i-1];
    }
    i=N2-2;
    sum=sum-max-min;
    sum=sum/i;
    return(sum);
}

/**
  * @brief          Sigmoid函数
  * @author
  * @param[in]      自变量
  * @retval         映射后的函数值
  */
float Sigmoid_function(float x)
{
    float y;
    float temp_x=ABS(x) - SIGMOID_MAX;			//将sigmoid函数向右平移最大区间值
    y= 1 / (1 + powf(e,-temp_x));
    return y;
}


/**
	*@func   float Circle_error(float set ,float get ,float circle_para)
	*@bref		环形数据计算偏差值
	*@param[in] set 设定值 get采样值 circle_para 一圈数值
	*@note	环形数据下，直接计算出PID中的偏差值
*/
float circle_error(float set,float get,float circle_para)
{
    float error;
    if(set > get)
    {
        if(set - get> circle_para/2)
            error = set - get - circle_para;
        else
            error = set - get;
    }
    else if(set < get)
    {
        if(set - get<-1*circle_para/2)
            error = set - get +circle_para;
        else
            error = set - get;
    }
    else	error = 0;

    return error;
}

float data_limit(float data, float max, float min)
{
    if(data >= max)					return max;
    else if(data <= min)		return min;
    else 										return data;
}

/**
 * @brief 最小二乘法直线拟合 y=ax+b，计算系数a，b
 * 
 * @param x[] 拟合点横坐标  
 * @param y[] 拟合点纵坐标
 * @param num 拟合点数
 * @param a   系数a返回指针
 * @param b   系数b返回指针
 * @attention num 是数组包含的元素个数，x[]和y[]的元素个数必须相等
 */
void least_square_linear_fit(float x[], float y[], const int num, float* a, float* b)
{
    float sum_x2 = 0.0f;
    float sum_y  = 0.0f;
    float sum_x  = 0.0f;
    float sum_xy = 0.0f;

    for (int i = 0; i < num; ++i)
    {
        sum_x2 += x[i]*x[i];
        sum_y  += y[i];
        sum_x  += x[i];
        sum_xy += x[i]*y[i];
    }

    *a = (num*sum_xy - sum_x*sum_y)/(num*sum_x2 - sum_x*sum_x);
    *b = (sum_x2*sum_y - sum_x*sum_xy)/(num*sum_x2-sum_x*sum_x);
}

/**
 * @brief 复数辐角计算
 * 
 * @param x     复数x坐标
 * @param y     复数y坐标
 * @param x_th  x坐标浮点死区，边界属于死区
 * @param y_th  y坐标浮点死区，边界属于死区
 * @return float 复数辐角(-PI,PI] (rad)，零向量辐角默认为0
 */
float vector_arg(float x, float y, float x_th, float y_th) {
    float arg = 0;
    x_th = x_th > 0? x_th: -x_th;
    y_th = y_th > 0? y_th: -y_th;

    if (x > x_th) {
        if (y > y_th || y < -y_th) {
            arg = atanf(y/x);           /* 第一、四象限 */
        } else {
            arg = 0;                    /* x正半轴 */
        }
    } else if (x >= -x_th && x <= x_th) {
        if (y > y_th) {
            arg = PI / 2;               /* y正半轴 */
        } else if (y < -y_th) {
            arg = -PI / 2;              /* y负半轴 */
        } else {
            arg = 0;                    /* 原点死区 */
        }
    } else if (x < -x_th) {
        if (y > y_th) {
            arg = atanf(y/x) + PI;      /* 第二象限 */
        } else if (y < -y_th) {
            arg = atanf(y/x) - PI;      /* 第三象限 */
        } else {
            arg = PI;                   /* x负半轴 */
        }
    }
    return arg;
}

/* 滞环死区控制 */
void delay_loop_init(delay_loop_t* dlp, float max, float min) {
    memset(dlp, 0, sizeof(delay_loop_t));
    if (max < min) {
        float temp = max;
        max = min;
        min = temp;
        dlp->max = max;
        dlp->min = min;
    }
}

int8_t delay_loop_calc(delay_loop_t* dlp, float now_data) {
    dlp->now = now_data;
    if (dlp->now > dlp->max && dlp->last < dlp->max) {  /* 正跳变 */
       dlp->out = 1;
    } else if (dlp->now < dlp->min && dlp->last > dlp->min) {  /* 负跳变 */
        dlp->out = -1;
    }
    dlp->last = dlp->now;
    return dlp->out;
}





