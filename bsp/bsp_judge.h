/**
  * @file bsp_judge.h
  * @version 1.0
  * @date  2022.3.14
	*
  * @brief 裁判系统串口协议v1.2的各种数据结构体
	*        v1.3的协议只是改了人工智能挑战赛的部分，对我们没有影响
	*				 所以没有写进去
	*
  *	@author  空中组抄强哥的
  *
  */
#ifndef BSP_JUDGE_H
#define BSP_JUDGE_H

#include "crc.h"
#include "stdint.h"

#define 	JUDGESEND_PERIOD 	10  //发送间隔，理论上越短越快
#define		JUDGESEND_TIME		10

#define   LEN_HEADER    5       //帧头长
#define   LEN_CMDID     2       //命令码长度
#define   LEN_TAIL      2	      //帧尾CRC16

//起始字节,协议固定为0xA5
#define    JUDGE_FRAME_HEADER         (0xA5)



typedef enum 
{
	FRAME_HEADER         = 0,
	CMD_ID               = 5,
	DATA                 = 7,
}JudgeFrameOffset;

//5字节帧头,偏移位置
typedef enum
{
	SOF          = 0,//起始位
	DATA_LENGTH  = 1,//帧内数据长度,根据这个来获取数据长度
	SEQ          = 3,//包序号
	CRC8         = 4 //CRC8
}FrameHeaderOffset;

typedef enum
{
	ID_game_state		  =		0x0001,		//比赛状态				1Hz
	ID_game_result		=		0x0002,		//比赛结果				结束时发送
	ID_robot_HP				=		0x0003,		//机器人血量			1Hz
	ID_darts_status		=		0x0004,		//飞镖发射状态		飞镖发射后发送
	
	ID_event_data			=		0x0101,		//场地事件数据		事件改变后发送
	ID_supply_action	=		0x0102,		//补给站动作标识	动作发生后发送
	ID_judge_warning	=		0x0104,		//裁判警告数据		警告发生后发送
	ID_dart_info        =0x0105,	//飞镖相关数据	1Hz
	
	ID_robot_status		=		0x0201,		//机器人状态数据	10Hz
	ID_robot_power		=		0x0202,		//实时功率热量		50Hz
	ID_robot_position =		0x0203,		//机器人位置数据	10Hz
	ID_robot_buff			=		0x0204,		//机器人增益数据	增益状态改变后发送
	ID_AerialRobotEnergy =	0x0205, //无人机能量			10Hz,只发送空中
	ID_robot_hurt			=		0x0206,		//伤害状态数据		伤害发生后发送
	ID_shoot_data			=		0x0207,		//实时射击数据		子弹发射后发送
	ID_bullet_remaining = 0x0208,   //弹丸剩余发射数	1Hz，空中/哨兵
	ID_RFID_status		=		0x0209,		//机器人RFID状态	1Hz
	ID_dart_client    =   0x020A,		//飞镖机器人客户端指令数据 10Hz
	
	ID_robot_interact=		0x0301,		//机器人间交互数据	发送方触发发送,上限10Hz
	ID_client_map			= 	0x0303		//客户端小地图交互数据	触发发送
}cmd_ID;

//数据段长度
typedef enum
{
	LEN_game_state			=		11,		
	LEN_game_result			=		1,		
	LEN_robot_HP				=		32,		
	LEN_darts_status		=		3,		
	LEN_event_data			=		4,		
	LEN_supply_action		=		4,
	LEN_judge_warning		=		2,		
	LEN_darts_info        	=		3, 		
	LEN_robot_status		=		27,		
	LEN_robot_power			=		16,		
	LEN_robot_position 	=		16,		
	LEN_robot_buff			=   1,
	LEN_AerialRobotEnergy	=	2,
	LEN_robot_hurt			=		1,		
	LEN_shoot_data			=		7,		
	LEN_bullet_remaining=		6,		
	LEN_RFID_status			=		4,	
	LEN_dart_client			=   6,
	LEN_robot_interract	=		20,			//机器人间交互数据数据段
}cmd_LEN;


//以下为具体数据结构体

/* 帧头 */
typedef __packed struct
{
	uint8_t  SOF;					//起始字节
	uint16_t DataLength;	//数据长度
	uint8_t  Seq;					//包序号
	uint8_t  CRC8;				//crc8校验
} frame_header;

/* 比赛状态数据：0x0001。发送频率：1Hz，发送范围：所有机器人。*/
typedef __packed struct 
{ 
	uint8_t game_type : 4;				//1机甲大师赛，2机甲大师单项赛，4RoboMaster 联盟赛3V3，5RoboMaster 联盟赛1V1
	uint8_t game_progress : 4;		//0未开始比赛，1准备阶段，2自检阶段，3五秒倒计时，4对战中，5比赛结算中
	uint16_t stage_remain_time;		//当前阶段剩余时间，单位秒
	uint64_t SyncTimeStamp;				//机器人接收到该指令的精确时间，当机载端收到有效的服务器授时后生效
} ext_game_state_t; 

/* 比赛结果数据：0x0002。发送频率：比赛结束后发送，发送范围：所有机器人。*/
typedef __packed struct 
{ 
	uint8_t winner;								//0 平局 1 红方胜利 2 蓝方胜利
} ext_game_result_t; 

/* 机器人血量数据：0x0003。发送频率：1Hz，发送范围：所有机器人。*/
typedef __packed struct
{
	uint16_t red_1_robot_HP;
	uint16_t red_2_robot_HP; 
	uint16_t red_3_robot_HP; 
	uint16_t red_4_robot_HP; 
	uint16_t red_5_robot_HP; 
	uint16_t red_7_robot_HP; 
	uint16_t red_outpost_HP;
	uint16_t red_base_HP; 
	uint16_t blue_1_robot_HP; 
	uint16_t blue_2_robot_HP; 
	uint16_t blue_3_robot_HP; 
	uint16_t blue_4_robot_HP; 
	uint16_t blue_5_robot_HP; 
	uint16_t blue_7_robot_HP; 	
	uint16_t blue_outpost_HP;
	uint16_t blue_base_HP;
}ext_game_robot_HP_t;

/* 飞镖发射状态：0x0004。发送频率：飞镖发射后发送，发送范围：所有机器人。*/
typedef __packed struct
{
	uint8_t dart_belong;							//1红方飞镖，2蓝方飞镖 
	uint16_t stage_remaining_time; 		//发射时的剩余比赛时间，单位秒
}ext_dart_status_t;

/* 场地事件数据：0x0101。发送频率：事件改变后发送，发送范围：己方机器人。*/
typedef __packed struct//v1.61修改
{
	uint32_t event_type;
	/*
	bit 0-2：
    bit0 己方补给站前补血点的占领状态 1为已占领；
	bit1 己方补给站内部补血点的占领状态 1为已占领；
	bit2 己方补给区的占领状态 1为已占领；（仅RMUL适用）
	
	bit 3-5：己方能量机关状态：
	bit 3 己方能量机关激活点的占领状态 1为已占领； 
	bit 4 己方小能量机关的激活状态，1为已激活；
	bit 5 己方小能量机关的激活状态 1为已激活；
	
	bit 6-11：己方高地占领状态；
	bit 6-7 己方环形高地的占领状态 1为被己方占领 2为被对方占领； 
	bit 8-9 己方梯形高地的占领状态 1为被己方占领 2为被对方占领；
	bit 10-11 己方梯形高地的占领状态 1为被己方占领 2为被对方占领； 
	
	bit 12-18 己方基地虚拟护盾的剩余值百分比（四舍五入，保留整数）；
	bit 19-27 飞镖最后一次击中己方前哨站或基地的时间（0-420，开局默认为0）； 
	bit 28-29 飞镖最后一次击中己方前哨站或基地的具体目标 开局默认为0 1为击中前哨站 2为击中基地固定目标 3为击中基地随机目标；
	bit 30-31：中心增益点的占领情况 0为未被占领 1为被己方占领 2为被对方占领 3为被双方占领（仅 RMUL 适用）；
	*/
}ext_event_data_t;

/* 补给站动作标识：0x0102。发送频率：动作触发后发送，发送范围：己方机器人。*/
typedef __packed struct
{
	uint8_t supply_projectile_id; 		//补给站口 ID：1 ，2
	uint8_t supply_robot_id; 					//补弹机器人 ID：0 为当前无机器人补弹，1 为红方英雄机器人补弹，2 为红方工程机器人补弹，3/4/5 为红方步兵机器人补弹，蓝方为101/102/103/104/105
	uint8_t supply_projectile_step; 	//出弹口开闭状态：0 为关闭，1 为子弹准备中，2 为子弹下落
	uint8_t supply_projectile_num;		//补弹数量：50 ，100 ，150 ，200
}ext_supply_projectile_action_t;

/* 裁判警告信息：cmd_id (0x0104)。发送频率：警告发生后发送，发送范围：己方机器人。*/
typedef __packed struct
{
	uint8_t level; 						//警告等级
	uint8_t foul_robot_id;		//犯规机器人ID：判负时，机器人ID为0,黄牌、红牌时，机器人ID为犯规机器人ID
}ext_referee_warning_t;

/* 飞镖发射口倒计时：cmd_id (0x0105)。发送频率：1Hz 周期发送，发送范围：己方机器人*/
/*飞镖发射口倒计时以及飞镖击中目标，次数，目标选择 1HZ*/
typedef __packed struct
{ 
	uint8_t dart_remaining_time;
	uint8_t dart_info_last_hit : 2;
	uint8_t dart_info_latest_hitcnt : 3;
	uint8_t dart_info_target : 2;
	
	uint8_t dart_info_remain_1 : 1;//保留位，不作用
	uint8_t dart_info_remain_2 : 8;
}ext_dart_info_t;

/* 比赛机器人状态：0x0201。发送频率：10Hz，发送范围：单一机器人。*/
typedef __packed struct
{ 
	uint8_t robot_id; 		//本机ID
	uint8_t robot_level;	//机器人等级：
	uint16_t remain_HP; 	//机器人剩余血量
	uint16_t max_HP; 			//机器人上限血量
	
	uint16_t shooter_id1_17mm_cooling_rate; 		//机器人1号17mm枪口每秒冷却值
	uint16_t shooter_id1_17mm_cooling_limit;		//机器人1号17mm枪口热量上限
	uint16_t shooter_id1_17mm_speed_limit;			//机器人1号17mm 枪口上限速度 单位 m/s
	uint16_t shooter_id2_17mm_cooling_rate;			//机器人2号17mm枪口每秒冷却值
	uint16_t shooter_id2_17mm_cooling_limit;		//机器人2号17mm枪口热量上限
	uint16_t shooter_id2_17mm_speed_limit;			//机器人2号17mm 枪口上限速度 单位 m/s
	uint16_t shooter_id1_42mm_cooling_rate; 		//机器人42mm枪口每秒冷却值
	uint16_t shooter_id1_42mm_cooling_limit;		//机器人42mm枪口热量上限
	uint16_t shooter_id1_42mm_speed_limit;			//机器人42mm枪口上限速度 单位 m/s
	uint16_t chassis_power_limit; 							//机器人底盘功率限制上限
	//主控电源输出情况
	uint8_t mains_power_gimbal_output : 1; 			//gimbal口输出 	1为有24V输出，0为无24v输出
	uint8_t mains_power_chassis_output : 1; 		//chassis口输出 1为有24V输出，0为无24v输出
	uint8_t mains_power_shooter_output : 1;			//shooter口输出 1为有24V输出，0为无24v输出
} ext_game_robot_status_t;

/* 实时功率热量数据：0x0202。发送频率：50Hz，发送范围：单一机器人。*/
typedef __packed struct
{
	uint16_t chassis_volt; 					//单位毫伏
	uint16_t chassis_current; 			//单位毫安
	float chassis_power;						//单位瓦 
	uint16_t chassis_power_buffer;	//单位焦耳，备注：飞坡根据规则增加至250J 
	uint16_t shooter_id1_17mm_cooling_heat; 		//1号17mm 枪口热量
	uint16_t shooter_id2_17mm_cooling_heat;			//2号17mm枪口热量
	uint16_t shooter_id1_42mm_cooling_heat;			//42mm 枪口热量
}ext_power_heat_data_t;

/* 机器人位置：0x0203。发送频率：10Hz，发送范围：单一机器人。*/
typedef __packed struct//v1.61
{
	float x;		//单位米
	float y;
	float z;
}ext_game_robot_pos_t;

/* 机器人增益：0x0204。发送频率：1Hz 周期发送，发送范围：单一机器人。*/
typedef __packed struct
{
 uint8_t power_rune_buff;
	/* 
	bit 0：机器人血量补血状态
	bit 1：枪口热量冷却加成
	bit 2：机器人防御加成
	bit 3：机器人攻击加成
	*/
}ext_buff_t;

/* 空中机器人能量状态：0x0205。发送频率：10Hz，发送范围：单一机器人。*/
typedef __packed struct
{ 
	uint8_t attack_time;		//可攻击时间，单位 s，30s 递减至 0	
}ext_aerial_robot_energy_t;

/* 伤害状态：0x0206。发送频率：伤害发生后发送，发送范围：单一机器人。*/
typedef __packed struct
{
	uint8_t armor_id : 4;
	uint8_t hurt_type : 4;
	/*
	bit 0-3：当血量变化类型为装甲伤害，代表装甲 ID，其中数值为 0-4 号代表机器人的五个装甲片，其他血量变化类型，该变量数值为 0。
	bit 4-7：血量变化类型:
	         0x0 装甲伤害扣血；
	         0x1 模块掉线扣血；
	         0x2 超射速扣血；
	         0x3 超枪口热量扣血；
	         0x4 超底盘功率扣血；
	         0x5 装甲撞击扣血
	*/
}ext_robot_hurt_t;

/* 实时射击信息：0x0207。发送频率：射击后发送，发送范围：单一机器人。*/
typedef __packed struct
{
  uint8_t bullet_type;		//子弹类型: 1：17mm 弹丸 2：42mm 弹丸
	uint8_t shooter_id;			//发射机构ID： 1:1号17mm发射机构	2:2号17mm发射机构		3：42mm 发射机构
  uint8_t bullet_freq;		//子弹射频 单位 Hz
  float bullet_speed;			//子弹射速 单位 m/s
}ext_shoot_data_t;

/* 子弹剩余发射数：0x0208。发送频率：10Hz 周期发送，所有机器人，*/

typedef __packed struct 
{ 
	uint16_t bullet_remaining_num_17mm;
	/*
	17mm子弹剩余发射数目 
	步兵机器人/英雄机器人:全队步兵与英雄剩余可发射17mm弹丸总量(联盟赛)		
	空中机器人/哨兵机器人:该机器人剩余可发射17mm弹丸总量(联盟赛/对抗赛)
	*/
	uint16_t bullet_remaining_num_42mm;	//42mm子弹剩余发射数目
	uint16_t coin_remaining_num; 				//剩余金币数量
} ext_bullet_remaining_t;

/* 机器人 RFID 状态：0x0209。发送频率：1Hz，发送范围：单一机器人。*/
typedef __packed struct
{
  uint32_t rfid_status;
	/*
	bit 0：基地增益点 RFID 状态；
  bit 1：高地增益点 RFID 状态；
  bit 2：能量机关激活点 RFID 状态；
  bit 3：飞坡增益点 RFID 状态；
  bit 4：前哨岗增益点 RFID 状态；
  bit 5：资源岛增益点 RFID 状态；
  bit 6：补血点增益点 RFID 状态；
  bit 7：工程机器人补血卡 RFID 状态；
  bit 8-31：保留
  RFID 状态不完全代表对应的增益或处罚状态，例如敌方已占领的高地增益点，不能获取对应的增益效果.
	
	*/
}ext_rfid_status_t;

/* 飞镖机器人客户端指令数据：0x020A。发送频率：10Hz，发送范围：单一机器人。*/
typedef __packed struct//v1.61
{
	uint8_t dart_launch_opening_status;
	uint8_t reserved;
	uint16_t target_change_time;
  uint16_t operate_launch_cmd_time;
}ext_dart_client_cmd_t;

/* 机器人间交互数据：0x0200~0x02ff。发送范围：单一机器人。*/
typedef __packed struct 
{ 
	uint8_t data[LEN_robot_interract+6];
} robot_interactive_receivedata_t;//机器人间交互数据

int Judge_Read_Data(uint8_t *ReadFromUsart);
extern ext_shoot_data_t Shoot_Data;
extern ext_game_state_t Game_State;
extern ext_dart_client_cmd_t	Dart_Client;                //飞镖闸门状态
extern ext_dart_info_t          Dart_Info;

#endif //版本定义
