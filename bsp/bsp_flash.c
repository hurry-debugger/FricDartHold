#include "bsp_flash.h"
#include <assert.h>

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

/*
  * @brief  将 uint16_t 数组转换为 uint32_t 数组（大端序模式）
  * @param  addr_32: 目标缓冲区地址（需4字节对齐）
  * @param  addr_16: 源数据地址（需2字节对齐）
  * @param  byte: 转换的字节数（需为4的倍数）
  * @retval 无
  * @note 函数假设系统为小端模式，需确保地址对齐以避免硬件异常[7,8](@ref)
  */
void u16_To_u32(uint32_t *addr_32, uint16_t *addr_16, uint16_t byte)
{
    // 参数合法性检查
    if (!addr_16 || !addr_32 || byte%4 != 0) return;
    
    // 循环合并数据（小端序）
    for (uint16_t i = 0; i < byte/4; i++) {
        // 将两个连续的uint16_t合并为一个uint32_t
        addr_32[i] = (uint32_t)(addr_16[2*i]) | ((uint32_t)(addr_16[2*i + 1]) << 16);
    }
}
/*
  * @brief  将 uint32_t 数组转换为 uint16_t 数组（支持大端/小端模式）
  * @param  dest_u16: 目标缓冲区地址（需2字节对齐）
  * @param  src_u32: 源数据地址（需4字节对齐）
  * @param  byte: 转换的字节数（需为4的倍数）
  * @param  endian: 字节序（0=小端，1=大端）
  * @retval 无
  * @note 函数强制内存对齐检查，避免硬件异常
  */
void u32_To_u16(uint16_t *addr_16, uint32_t *addr_32, uint32_t byte)
{
    // 参数合法性检查
    if (!addr_16 || !addr_32 || byte%4 != 0) return;

    for (uint32_t i = 0; i < byte/4; i++) {
     // 小端模式（低字节在前）
            addr_16[2*i]   = (uint16_t)(addr_32[i] & 0xFFFF);       // 低16位
            addr_16[2*i+1] = (uint16_t)((addr_32[i] >> 16) & 0xFFFF); // 高16位
    }
}

/**
  * @brief  将数据写入Flash存储器
  * @param  WriteAddress: 写入起始地址（需4字节对齐）
  * @param  data: 待写入数据的指针（需指向32位对齐数据）
  * @param  byte: 写入字节数（需为4的倍数）
  * @retval HAL状态码（HAL_OK=成功，HAL_ERROR=失败）
  *
  * @note 关键操作流程：
  *       1. 参数校验 -> 2. 中断锁定 -> 3. Flash解锁 -> 4. 扇区擦除
  *       -> 5. 数据写入 -> 6. Flash上锁 -> 7. 中断恢复
  * @warning 擦除操作会清除整个扇区数据，建议存储关键数据前备份原始内容
  */
HAL_StatusTypeDef WriteFlashData(uint32_t WriteAddress, uint32_t *data, uint32_t word)
{
    /* 关闭全局中断（防止Flash操作被中断打断）*/
    __disable_irq();
    
    FLASH_EraseInitTypeDef FlashEraseInit;
    uint32_t SectorError = 0;
    HAL_StatusTypeDef status = HAL_OK;

    /* 参数有效性检查（三重验证机制）*/
    if( (WriteAddress % 4 != 0) ||                // 地址4字节对齐检查
        (WriteAddress < FMC_FLASH_BASE) ||         // 起始地址下限检查
        (WriteAddress + word*4 >= FMC_FLASH_END) ||  // 结束地址越界检查
        (word == 0) )                              // 零长度写入检查
    {
        __enable_irq();
        return HAL_ERROR;
    }

    /* Flash解锁（必须成对使用HAL_FLASH_Unlock/Lock) */
    HAL_FLASH_Unlock();

    /* 计算需要擦除的扇区范围（优化原逐个地址检查的低效逻辑） */
    uint32_t start_sector = STMFLASH_GetFlashSector(WriteAddress);
    uint32_t end_sector = STMFLASH_GetFlashSector(WriteAddress + word*4 - 1);
    
    /* 配置擦除参数（注意电压范围需与实际硬件匹配）*/
    FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
    FlashEraseInit.Sector = start_sector;
    FlashEraseInit.NbSectors = end_sector - start_sector + 1;
    FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3; // 2.7V-3.6V适用

    /* 执行扇区擦除（注意擦除后的数据全为0xFFFFFFFF）*/
    if((status = HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError)) != HAL_OK) {
        HAL_FLASH_Lock();
        __enable_irq();
        return status;
    }

    /* 数据写入*/
    for(uint32_t i=0; i<word; i++) 
    {
        /* 单字编程（FLASH_TYPEPROGRAM_WORD要求地址4字节对齐） */
        if((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
                                    WriteAddress, 
                                    data[i])) != HAL_OK) 
        {
            HAL_FLASH_Lock();
            __enable_irq();
            return status;
        }
        WriteAddress += 4; // 地址递增步长与编程类型匹配
    }

    /* Flash上锁（防止意外写入）*/
    HAL_FLASH_Lock();
    
    /* 恢复全局中断 */
    __enable_irq();

    return HAL_OK;
}
