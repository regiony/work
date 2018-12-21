/*
********************************************************************************
文件名	：flash.h
文件描述：内部flash访问头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __FLASH_H
#define __FLASH_H

#ifdef __FLASH_GLOBAL
	#define __FLASH_EXE 
#else
	#define __FLASH_EXE extern
#endif

#ifdef __cplusplus
	extern "C" {
#endif 

/*
********************************************************************************
头文件
********************************************************************************
*/
#include "stm32f0xx.h"

/*
********************************************************************************
宏定义
********************************************************************************
*/
#define MAX_PAGE_NUM		12				//读写页号必须在0~11之间

#define MAX_OFFSET			1019

#define PAGE_SIZE           ((uint32_t)0x0400) 

/* EEPROM start address in Flash */
#define EEPROM_BASE  		((uint32_t)0x08020000) 
/* Pages x base and end addresses */
//#define PAGE0_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x0000))
//#define PAGE0_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

/*
********************************************************************************
公共变量定义
********************************************************************************
*/
__FLASH_EXE	uint8_t	GPS_CN_save_flag;			//信息存储FLASH标志
__FLASH_EXE	uint8_t	erase_flash_flag;    		//擦除FLASH标志
__FLASH_EXE uint8_t start_page;					//读数据起始页
__FLASH_EXE uint8_t end_page;					//读数据结束页

/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void flash_init(void);
int erase_flash(uint8_t start_page, uint8_t page_num);
int read_flash(uint8_t stp, uint8_t enp, uint8_t *pt);	
int write_flash(uint16_t *pt);	

/*
********************************************************************************
私有变量定义
********************************************************************************
*/
__FLASH_EXE uint8_t 	rd_cur_page_num;		//读当前页号
__FLASH_EXE uint16_t 	rd_cur_page_offset;		//读当前页内偏移地址
__FLASH_EXE uint8_t		wr_cur_page_num;		//写当前页号
__FLASH_EXE uint16_t 	wr_cur_page_offset;		//写当前页内偏移地址

/*
********************************************************************************
私有函数定义
********************************************************************************
*/


#ifdef __cplusplus
	}
#endif

#endif
