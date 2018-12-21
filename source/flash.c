/*
********************************************************************************
�ļ���	��flash.c
�ļ��������ڲ�flash��������NVM������ʹ�ã��ṩflash����API
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __FLASH_GLOBAL
#include "flash.h"                 


/*
********************************************************************************
�������ƣ�flash_init
����������flash��ʼ��
����������
����ֵ	��
********************************************************************************
*/
void flash_init(void)
{
	GPS_CN_save_flag = 0;			//��Ϣ�洢FLASH��־
	erase_flash_flag = 0;    		//����FLASH��־
	rd_cur_page_num = 0;			//����ǰҳ��
	rd_cur_page_offset = 0;			//����ǰҳ��ƫ�Ƶ�ַ
	wr_cur_page_num = 0;			//д��ǰҳ��
	wr_cur_page_offset = 0;			//д��ǰҳ��ƫ�Ƶ�ַ	
}

/*
********************************************************************************
�������ƣ�erase_flash
��������������flash����ָ��ҳ
����������
����ֵ	��1:ok, -1:fail
********************************************************************************
*/
int erase_flash(uint8_t start_page, uint8_t page_num)
{
	uint8_t i;
	
	FLASH_Status FlashStatus = FLASH_COMPLETE;
	
	wr_cur_page_num = 0;
	wr_cur_page_offset = 0;
	
	/* Unlock the Flash Program Erase controller */
	FLASH_Unlock();	
	
	for(i = 0; i < page_num; i++)
	{
	  /* Erase Page i */
	  FlashStatus = FLASH_ErasePage((uint32_t)(EEPROM_BASE + (PAGE_SIZE * (i + start_page))));
	
	  /* If erase operation was failed, a Flash error code is returned */
	  if (FlashStatus != FLASH_COMPLETE)
	  {
	    FLASH_Lock();
	    return -1;
	  }  	
	}
	
	FLASH_Lock();
	return 1;
}

/*
********************************************************************************
�������ƣ�read_flash
������������flash
����������
����ֵ	��1:read ok, -1:no data read 
********************************************************************************
*/
int read_flash(uint8_t stp, uint8_t enp, uint8_t *pt)
{
	uint32_t addr, i;
	uint16_t *pt16;
	
	if(rd_cur_page_num > enp)
		return -1;
		
	if(rd_cur_page_offset > MAX_OFFSET) //ÿҳ������һ�����ݵ�4���ֽ��������������
 	{
  		rd_cur_page_num++;	
  		rd_cur_page_offset = 0;
  		if(rd_cur_page_num >= enp)
  			return -1;
  	}
		
	addr = (uint32_t)(EEPROM_BASE + (PAGE_SIZE * rd_cur_page_num) + rd_cur_page_offset);
	pt16 = (uint16_t *)addr;
	
	if(*pt16 == 0xFFFF) 	//���жϣ�ʱ��̫��������Ҫ���õñ������ݣ��ҵ�ͬ��ͷ���Ҳ���
	{
		//����Ч����
		return -1;
	}	
	else
	if(*pt16 == 0xAA55)  	//���жϣ�ʱ��̫��������Ҫ���õñ������ݣ��ҵ�ͬ��ͷ���Ҳ���
	{
		for(i = 0; i < 10; i++)
		{
			*pt++ = (uint8_t)((*pt16 >> 8) & 0xFF);
			*pt++ = (uint8_t)(*pt16 & 0xFF);
			pt16++;
		}
		rd_cur_page_offset += 20;
	}
	else  //�ݴ�����ʱ�򵥴���
	{
		//
		return -1;
	}
	return 1;
}

/*
********************************************************************************
�������ƣ�write_flash
����������дflash
����������
����ֵ	��1:write ok, -1:wirte overflow, 2:fail
********************************************************************************
*/
int write_flash(uint16_t *pt)
{
	uint32_t i;
	
	if(wr_cur_page_num >= MAX_PAGE_NUM)
		return -1;
	
	/* Unlock the Flash Program Erase controller */
	FLASH_Unlock();	
	
	//FLASH_Status FlashStatus = FLASH_COMPLETE;	
	
	if(wr_cur_page_offset > MAX_OFFSET) //ÿҳ������һ�����ݵ�4���ֽ��������������
	{
			wr_cur_page_num++;	
			wr_cur_page_offset = 0;
			if(wr_cur_page_num >= MAX_PAGE_NUM)
				return -1;
	}
	
	for(i = 0; i < 10; i++)
	{
	  if(FLASH_ProgramHalfWord((uint32_t)(EEPROM_BASE + (PAGE_SIZE * wr_cur_page_num) + wr_cur_page_offset), *pt++) == FLASH_COMPLETE)
	  	wr_cur_page_offset += 2;
	  else
	  	return 2;
	}
	
	FLASH_Lock();
	return 1;
}

