#ifndef __FILTER_H
#define __FILTER_H
#include "stdint.h"
#include "math.h"
/*----------------------------------------------------------------------------------------------------------------------/
        *               
        *               一阶低通滤波器
        *               
----------------------------------------------------------------------------------------------------------------------*/
/*一阶低通滤波器模式*/
#define LPFO_MODE_N 0
#define LPFO_MODE_D 1       //使用动态调节


/*一阶低通滤波器结构体类型*/
typedef struct
{
    uint8_t mode : 1;       //是否使用动态调节
    float K;                //滤波系数

    float last_data;        //上一次获取数据
    float now_data;         //本次获取的数据

    float last_after_data;        //上一次获取数据
    float now_after_data;         //本次获取的数据

    float Kw;               //数据稳定时系数
    uint8_t dir_last;   //数据变化方向
    uint8_t dir_now;
    uint8_t cnt;            //稳定增长（减少）持续时间
    uint8_t Threshold_A;    //阈值1用于一阶带参滤波器，变化角度大于此值时，计数增加
    uint8_t Threshold_T;    //阈值2用于一阶带参滤波器，计数值大于此值时，增大参数，增强滤波跟随
} LPFOfilter_t;


/*一阶低通滤波器初始化结构体类型*/
typedef struct {
    uint8_t mode : 1;
    float K;
    float Kw;
    uint8_t Threshold_A;
    uint8_t Threshold_T;
} LPFO_initstruct;


float LPFOfilter_cal(LPFOfilter_t* pft, float get);
void LPFOfilter_init(LPFOfilter_t* pft,LPFO_initstruct* pftinit);
/*----------------------------------------------------------------------------------------------------------------------/
        *               
        *               平均滤波器
        *               
----------------------------------------------------------------------------------------------------------------------*/
#ifndef __AVG_FILTER_H
#define __AVG_FILTER_H


#define AVG_FILTER_NUM 10


typedef struct{
    uint8_t num;
    float array[AVG_FILTER_NUM];
    uint8_t index;
    float after_data;
}AVGfilter_t;


void avg_filter_init(void);
float AVGfilter_cal(AVGfilter_t* avg, float get);
float GetMedianNum(float in, int iFilterLen);
#endif



/*----------------------------------------------------------------------------------------------------------------------/
        *		无名科创开源飞控 V1.1	武汉科技大学  By.YuYi
        *		        CSDN博客: http://blog.csdn.net/u011992534
        *               优酷ID：NamelessCotrun无名小哥
        *               无名科创开源飞控QQ群：540707961
        *               https://shop348646912.taobao.com/?spm=2013.1.1000126.2.5ce78a88ht1sO2
        *               百度贴吧:无名科创开源飞控
        *               Copyright(C) 武汉科技大学无名科创团队 2017-2019
        *               All rights reserved
----------------------------------------------------------------------------------------------------------------------*/
//#ifndef _ADRC_H_
//#define _ADRC_H_
//typedef struct
//{
//    /*****安排过度过程*******/
//    float x1;   //跟踪微分期状态量
//    float x2;   //跟踪微分期状态量微分项
//    float r;    //时间尺度
//    float h;    //ADRC系统积分时间
//    uint16_t N0;//跟踪微分器解决速度超调h0=N*h

//    float h0;
//    float fh;   //最速微分加速度跟踪量
//    /*****扩张状态观测器*******/
//    /******已系统输出y和输入u来跟踪估计系统状态和扰动*****/
//    float z1;
//    float z2;
//    float z3;   //根据控制对象输入与输出，提取的扰动信息
//    float e;    //系统状态误差
//    float y;    //系统输出量
//    float fe;
//    float fe1;
//    float beta_01;
//    float beta_02;
//    float beta_03;

//    /**********系统状态误差反馈率*********/
//    float e0;   //状态误差积分项
//    float e1;   //状态偏差
//    float e2;   //状态量微分项
//    float u0;   //非线性组合系统输出
//    float u;    //带扰动补偿后的输出


//    /*********第一种组合形式*********/
//    float beta_0;   //线性
//    float beta_1;   //非线性组合参数
//    float beta_2;   //u0=beta_1*e1+beta_2*e2+(beta_0*e0);
//    /*********第二种组合形式*********/
//    float alpha1;   //u0=beta_1*fal(e1,alpha1,zeta)+beta_2*fal(e2,alpha2,zeta)
//    float alpha2;   //0<alpha1<1<alpha2
//    float zeta;     //线性段的区间长度
//    /*********第三种组合形式*********/
//    float h1;       //u0=-fhan(e1,e2,r,h1);
//    uint16_t N1;    //跟踪微分器解决速度超调h0=N*h
//    /*********第四种组合形式*********/
//    float c;        //u0=-fhan(e1,c*e2*e2,r,h1);
//    float b0;       //扰动补偿
//} Fhan_Data;



//void ADRC_Init(Fhan_Data *fhan_Input1,Fhan_Data *fhan_Input2);
//void Fhan_ADRC(Fhan_Data *fhan_Input,float expect_ADRC);
//void ADRC_Control(Fhan_Data *fhan_Input,float expect_ADRC,float feedback);

//extern Fhan_Data ADRC_Pitch_Controller,ADRC_Roll_Controller;
//#endif


#endif



