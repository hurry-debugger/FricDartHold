#include "bsp_flash.h"
#include <assert.h>

#define FMC_FLASH_BASE      0x08000000   // FLASH����ʼ��ַ
#define FMC_FLASH_END       0x08080000   // FLASH�Ľ�����ַ

#define FLASH_WAITETIME     50000        //FLASH�ȴ���ʱʱ��

//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
//   stm32f407vetx   512k  ֻ��8������
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes 

//��ȡָ����ַ����(32λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
    return *(uint32_t*)faddr; 
}

//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
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
  * @brief  �� uint16_t ����ת��Ϊ uint32_t ���飨�����ģʽ��
  * @param  addr_32: Ŀ�껺������ַ����4�ֽڶ��룩
  * @param  addr_16: Դ���ݵ�ַ����2�ֽڶ��룩
  * @param  byte: ת�����ֽ�������Ϊ4�ı�����
  * @retval ��
  * @note ��������ϵͳΪС��ģʽ����ȷ����ַ�����Ա���Ӳ���쳣[7,8](@ref)
  */
void u16_To_u32(uint32_t *addr_32, uint16_t *addr_16, uint16_t byte)
{
    // �����Ϸ��Լ��
    if (!addr_16 || !addr_32 || byte%4 != 0) return;
    
    // ѭ���ϲ����ݣ�С����
    for (uint16_t i = 0; i < byte/4; i++) {
        // ������������uint16_t�ϲ�Ϊһ��uint32_t
        addr_32[i] = (uint32_t)(addr_16[2*i]) | ((uint32_t)(addr_16[2*i + 1]) << 16);
    }
}
/*
  * @brief  �� uint32_t ����ת��Ϊ uint16_t ���飨֧�ִ��/С��ģʽ��
  * @param  dest_u16: Ŀ�껺������ַ����2�ֽڶ��룩
  * @param  src_u32: Դ���ݵ�ַ����4�ֽڶ��룩
  * @param  byte: ת�����ֽ�������Ϊ4�ı�����
  * @param  endian: �ֽ���0=С�ˣ�1=��ˣ�
  * @retval ��
  * @note ����ǿ���ڴ�����飬����Ӳ���쳣
  */
void u32_To_u16(uint16_t *addr_16, uint32_t *addr_32, uint32_t byte)
{
    // �����Ϸ��Լ��
    if (!addr_16 || !addr_32 || byte%4 != 0) return;

    for (uint32_t i = 0; i < byte/4; i++) {
     // С��ģʽ�����ֽ���ǰ��
            addr_16[2*i]   = (uint16_t)(addr_32[i] & 0xFFFF);       // ��16λ
            addr_16[2*i+1] = (uint16_t)((addr_32[i] >> 16) & 0xFFFF); // ��16λ
    }
}

/**
  * @brief  ������д��Flash�洢��
  * @param  WriteAddress: д����ʼ��ַ����4�ֽڶ��룩
  * @param  data: ��д�����ݵ�ָ�루��ָ��32λ�������ݣ�
  * @param  byte: д���ֽ�������Ϊ4�ı�����
  * @retval HAL״̬�루HAL_OK=�ɹ���HAL_ERROR=ʧ�ܣ�
  *
  * @note �ؼ��������̣�
  *       1. ����У�� -> 2. �ж����� -> 3. Flash���� -> 4. ��������
  *       -> 5. ����д�� -> 6. Flash���� -> 7. �жϻָ�
  * @warning ������������������������ݣ�����洢�ؼ�����ǰ����ԭʼ����
  */
HAL_StatusTypeDef WriteFlashData(uint32_t WriteAddress, uint32_t *data, uint32_t word)
{
    /* �ر�ȫ���жϣ���ֹFlash�������жϴ�ϣ�*/
    __disable_irq();
    
    FLASH_EraseInitTypeDef FlashEraseInit;
    uint32_t SectorError = 0;
    HAL_StatusTypeDef status = HAL_OK;

    /* ������Ч�Լ�飨������֤���ƣ�*/
    if( (WriteAddress % 4 != 0) ||                // ��ַ4�ֽڶ�����
        (WriteAddress < FMC_FLASH_BASE) ||         // ��ʼ��ַ���޼��
        (WriteAddress + word*4 >= FMC_FLASH_END) ||  // ������ַԽ����
        (word == 0) )                              // �㳤��д����
    {
        __enable_irq();
        return HAL_ERROR;
    }

    /* Flash����������ɶ�ʹ��HAL_FLASH_Unlock/Lock) */
    HAL_FLASH_Unlock();

    /* ������Ҫ������������Χ���Ż�ԭ�����ַ���ĵ�Ч�߼��� */
    uint32_t start_sector = STMFLASH_GetFlashSector(WriteAddress);
    uint32_t end_sector = STMFLASH_GetFlashSector(WriteAddress + word*4 - 1);
    
    /* ���ò���������ע���ѹ��Χ����ʵ��Ӳ��ƥ�䣩*/
    FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
    FlashEraseInit.Sector = start_sector;
    FlashEraseInit.NbSectors = end_sector - start_sector + 1;
    FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3; // 2.7V-3.6V����

    /* ִ������������ע������������ȫΪ0xFFFFFFFF��*/
    if((status = HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError)) != HAL_OK) {
        HAL_FLASH_Lock();
        __enable_irq();
        return status;
    }

    /* ����д��*/
    for(uint32_t i=0; i<word; i++) 
    {
        /* ���ֱ�̣�FLASH_TYPEPROGRAM_WORDҪ���ַ4�ֽڶ��룩 */
        if((status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
                                    WriteAddress, 
                                    data[i])) != HAL_OK) 
        {
            HAL_FLASH_Lock();
            __enable_irq();
            return status;
        }
        WriteAddress += 4; // ��ַ����������������ƥ��
    }

    /* Flash��������ֹ����д�룩*/
    HAL_FLASH_Lock();
    
    /* �ָ�ȫ���ж� */
    __enable_irq();

    return HAL_OK;
}
