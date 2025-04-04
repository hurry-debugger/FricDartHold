#include "bsp_flash.h"

#define FMC_FLASH_BASE      0x08000000   // FLASH的起始地址
#define FMC_FLASH_END       0x08080000   // FLASH的结束地址

#define FLASH_WAITETIME     50000        //FLASH等待超时时间

//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//扇区7起始地址, 128 Kbytes  
//   stm32f407vetx   512k  只有8个扇区
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//扇区11起始地址,128 Kbytes 

//读取指定地址的字(32位数据) 
//faddr:读地址 
//返回值:对应数据.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
    return *(uint32_t*)faddr; 
}

//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
static uint8_t STMFLASH_GetFlashSector(uint32_t addr)
{
    if(addr<ADDR_FLASH_SECTOR_1)return FLASH_SECTOR_0;
    else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_SECTOR_1;
    else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_SECTOR_2;
    else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_SECTOR_3;
    else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_SECTOR_4;
    else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_SECTOR_5;
    else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_SECTOR_6;
    else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_SECTOR_7;
    else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_SECTOR_8;
    else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_SECTOR_9;
    else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_SECTOR_10;   
    return FLASH_SECTOR_11;	
}

/**
 *@功能：向内部Flash写入数据
 *@参数1：WriteAddress：数据要写入的目标地址（偏移地址）
 *@参数2：*data： 写入的数据首地址
 *@参数3：length：写入数据的个数
 */
void WriteFlashData(uint32_t WriteAddress, uint8_t *data, uint32_t length)
{
    FLASH_EraseInitTypeDef FlashEraseInit;
    HAL_StatusTypeDef FlashStatus=HAL_OK;
    uint32_t SectorError=0;
    uint32_t addrx=0;
    uint32_t endaddr=0;
    
    if( (WriteAddress < FMC_FLASH_BASE) || ( WriteAddress + length >= FMC_FLASH_END) || (length <= 0) )
    return;

    HAL_FLASH_Unlock();              //解锁
    addrx = WriteAddress;            //写入的起始地址
    endaddr = WriteAddress+length;   //写入的结束地址


        while(addrx<endaddr)  //扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
        {
             if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
            {   
                FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
                FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //要擦除的扇区
                FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
                FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
                if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
                {
                    break;//发生错误了
                }
                }else addrx+=1;
                FLASH_WaitForLastOperation(FLASH_WAITETIME);
				//等待上次操作完成
        }
    
    FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //等待上次操作完成
    if(FlashStatus==HAL_OK)
    {
         while(WriteAddress<endaddr)//写数据
         {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,WriteAddress,*data)!=HAL_OK)//写入数据
            { 
                break;	//写入异常
            }
            WriteAddress+=1;
            data++;
        }  
    }
    HAL_FLASH_Lock();           //上锁
}
