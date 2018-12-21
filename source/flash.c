/*
********************************************************************************
文件名	：flash.c
文件描述：内部flash空闲区当NVM数据区使用，提供flash操作API
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __FLASH_GLOBAL
#include "flash.h"                 


/*
********************************************************************************
函数名称：flash_init
函数描述：flash初始化
参数描述：
返回值	：
********************************************************************************
*/
void flash_init(void)
{
	GPS_CN_save_flag = 0;			//信息存储FLASH标志
	erase_flash_flag = 0;    		//擦除FLASH标志
	rd_cur_page_num = 0;			//读当前页号
	rd_cur_page_offset = 0;			//读当前页内偏移地址
	wr_cur_page_num = 0;			//写当前页号
	wr_cur_page_offset = 0;			//写当前页内偏移地址	
}

/*
********************************************************************************
函数名称：erase_flash
函数描述：擦除flash部分指定页
参数描述：
返回值	：1:ok, -1:fail
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
函数名称：read_flash
函数描述：读flash
参数描述：
返回值	：1:read ok, -1:no data read 
********************************************************************************
*/
int read_flash(uint8_t stp, uint8_t enp, uint8_t *pt)
{
	uint32_t addr, i;
	uint16_t *pt16;
	
	if(rd_cur_page_num > enp)
		return -1;
		
	if(rd_cur_page_offset > MAX_OFFSET) //每页不够存一包数据的4个字节区域空下来不用
 	{
  		rd_cur_page_num++;	
  		rd_cur_page_offset = 0;
  		if(rd_cur_page_num >= enp)
  			return -1;
  	}
		
	addr = (uint32_t)(EEPROM_BASE + (PAGE_SIZE * rd_cur_page_num) + rd_cur_page_offset);
	pt16 = (uint16_t *)addr;
	
	if(*pt16 == 0xFFFF) 	//简单判断，时间太紧，真正要做好得遍历数据，找到同步头或找不到
	{
		//无有效数据
		return -1;
	}	
	else
	if(*pt16 == 0xAA55)  	//简单判断，时间太紧，真正要做好得遍历数据，找到同步头或找不到
	{
		for(i = 0; i < 10; i++)
		{
			*pt++ = (uint8_t)((*pt16 >> 8) & 0xFF);
			*pt++ = (uint8_t)(*pt16 & 0xFF);
			pt16++;
		}
		rd_cur_page_offset += 20;
	}
	else  //容错处理，暂时简单处理
	{
		//
		return -1;
	}
	return 1;
}

/*
********************************************************************************
函数名称：write_flash
函数描述：写flash
参数描述：
返回值	：1:write ok, -1:wirte overflow, 2:fail
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
	
	if(wr_cur_page_offset > MAX_OFFSET) //每页不够存一包数据的4个字节区域空下来不用
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

