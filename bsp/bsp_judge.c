#include "bsp_judge.h"
#include "string.h"
#include "usart.h"
#include "bsp_usart.h"
#include "cmsis_os.h"

/*****************数据结构体定义**********************/
frame_header											FrameHeader;		//帧头

ext_game_state_t									Game_State;
ext_game_result_t									Game_Result;
ext_game_robot_HP_t               RobotHP;                   //机器血量
ext_dart_status_t									Dart_Status;               //飞镖发射状态
ext_event_data_t									Event_Data;                //事件数据
ext_supply_projectile_action_t		Supply_Projectile_Action;  //补弹数据
ext_referee_warning_t							Referee_Warning;           //受罚情况
ext_dart_info_t					                   Dart_Info;              //飞镖发射相关数据
ext_game_robot_status_t						Game_Robot_Status;
ext_power_heat_data_t							Power_Heat_Data;
ext_game_robot_pos_t							Robot_Position;
ext_buff_t												Buff;
ext_aerial_robot_energy_t					Aerial_Robot_Energy;
ext_robot_hurt_t									Robot_Hurt;
ext_shoot_data_t									Shoot_Data;
ext_bullet_remaining_t						Bullet_Remaining;
ext_rfid_status_t									RFID_Status;
ext_dart_client_cmd_t							Dart_Client;                //飞镖闸门状态
robot_interactive_receivedata_t		Comm_getdata;
/******************************************************/

Verify_CRC16

uint16_t Robot_Self_HP;//自身血量

/**
  * @brief  读取裁判数据函数，串口中断函数中直接调用进行读取
  * @param  对应串口的缓存区数据
  * @retval 是否对正误判断做处理
  * @attention  在此判断帧头和CRC校验,无误再写入数据，不重复判断帧头
  */
int Judge_Read_Data(uint8_t *ReadFromUsart)
{
	uint8_t  retval_tf = FALSE; //数据真实性标志位,每次读取时都默认为FALSE
	uint16_t judge_length;			//数据字节长度
	int      CmdID=0;				
	if(ReadFromUsart == NULL)
	{
		return -1;
		
	}
	
	memcpy(&FrameHeader, ReadFromUsart, LEN_HEADER);  //写入帧头
	//判断帧头数据是否为0xA5
	if(ReadFromUsart[ SOF ] == JUDGE_FRAME_HEADER)
	{
		//帧头CRC8校验
		if (Verify_CRC8_Check_Sum( ReadFromUsart, LEN_HEADER ) == TRUE)
		{
			//统计一帧数据长度,用于CR16校验
			judge_length = ReadFromUsart[ DATA_LENGTH ] + LEN_HEADER + LEN_CMDID + LEN_TAIL;;

			//帧尾CRC16校验
			if(Verify_CRC16_Check_Sum(ReadFromUsart,judge_length) == TRUE)
			{
				retval_tf = TRUE;//都校验过了则说明数据可用
				
				CmdID = (ReadFromUsart[6] << 8 | ReadFromUsart[5]);
				//解析数据命令码,将数据拷贝到相应结构体中(注意拷贝数据的长度)
				switch(CmdID)
				{
					case ID_game_state:     //0x0001
						memcpy(&Game_State, (ReadFromUsart + DATA), LEN_game_state);
					break;
					
					case ID_game_result:    //0x0002
						memcpy(&Game_Result, (ReadFromUsart + DATA), LEN_game_result);
					break;
					
					case ID_robot_HP:       //0x0003
						memcpy(&RobotHP, (ReadFromUsart + DATA), LEN_robot_HP);
					break;
					
					case ID_darts_status:		//0x0004
						memcpy(&Dart_Status, (ReadFromUsart + DATA), LEN_darts_status);
					break;
					
					case ID_event_data:    	 //0x0101
						memcpy(&Event_Data, (ReadFromUsart + DATA), LEN_event_data);
					break;
					
					case ID_supply_action:   //0x0102
						memcpy(&Supply_Projectile_Action, (ReadFromUsart + DATA), LEN_supply_action);
					break;
					
					case ID_judge_warning:  	//0x0104
						memcpy(&Referee_Warning, (ReadFromUsart + DATA), LEN_judge_warning);
					break;
					
					case ID_dart_info:  //0x0105
						memcpy(&Dart_Info, (ReadFromUsart + DATA), LEN_darts_info);
					break;
					
					case ID_robot_status:     //0x0201
						memcpy(&Game_Robot_Status, (ReadFromUsart + DATA), LEN_robot_status);
					break;
					
					case ID_robot_power:      //0x0202
						memcpy(&Power_Heat_Data, (ReadFromUsart + DATA), LEN_robot_power);
					break;
					
					case ID_robot_position:   //0x0203
						memcpy(&Robot_Position, (ReadFromUsart + DATA), LEN_robot_position);
					break;
					
					case ID_robot_buff:      	//0x0204
						memcpy(&Buff, (ReadFromUsart + DATA), LEN_robot_buff);
					break;
					
					case ID_AerialRobotEnergy:   //0x0205
						memcpy(&Aerial_Robot_Energy, (ReadFromUsart + DATA), LEN_AerialRobotEnergy);
					break;
					
					case ID_robot_hurt:      		//0x0206
						memcpy(&Robot_Hurt, (ReadFromUsart + DATA), LEN_robot_hurt);
					
					case ID_shoot_data:      			//0x0207，射击信息
						memcpy(&Shoot_Data, (ReadFromUsart + DATA), LEN_shoot_data);
					break;
					
					case ID_bullet_remaining:     //0x0208
						memcpy(&Bullet_Remaining, (ReadFromUsart + DATA), LEN_bullet_remaining);
					break;
					
					case ID_RFID_status:      		//0x0209
						memcpy(&RFID_Status, (ReadFromUsart + DATA), LEN_RFID_status);
					break;
					
					case ID_dart_client:      		//0x020A
						memcpy(&Dart_Client, (ReadFromUsart + DATA), LEN_dart_client);
					break;
					
					case ID_robot_interact:      //0x0301
						memcpy(&Comm_getdata, (ReadFromUsart + DATA), LEN_robot_interract);
					break;
				}
			}
		}
		//首地址加帧长度,指向CRC16下一字节,用来判断是否为0xA5,用来判断一个数据包是否有多帧数据
		if(*(ReadFromUsart + sizeof(FrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL) == 0xA5)
		{
			//如果一个数据包出现了多帧数据,则再次读取
			Judge_Read_Data(ReadFromUsart + sizeof(FrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL);
		}
	}
	return retval_tf;
}
