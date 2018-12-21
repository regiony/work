/*
********************************************************************************
文件名	：public.h
文件描述：公共头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __PUBLIC_H
#define __PUBLIC_H

#ifdef __PUBLIC_GLOBAL
	#define __PUBLIC_EXE 
#else
	#define __PUBLIC_EXE extern
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
#include "main.h"
#include "key.h"  
#include "lcd_if.h"
#include "lcd_drv.h" 
#include "lcd_api.h"  
#include "ziku.h"   
#include "i2c.h" 
#include "pca9555.h" 
#include "flash.h"  
#include "crc.h"   
#include "rs485.h"
#include "crc.h" 
#include "ymodem.h"
#include "mcb_hmu.h"   
#include "mcb_alm.h" 
#include "mcb_pd8.h"  
#include "mcb_pes.h"   
#include "mcb_rfb.h" 
//#include "mcb_xyb.h"    
#include "mcb_ru.h"
#include "mcb_wdt.h"  
#include "mcb_wifi.h" 
#include "stm32f0xx_it.h"

/*
********************************************************************************
宏定义
********************************************************************************
*/
#define TIM14_PERIOD	10000			//周期 1/1M * 10000 = 10ms
#define TIM14_PSC		47				//48分频，48M/48=1M
#define RFB				1				//射频板
#define XYB				2				//协议板
#define DEV_TYPE_1		0				//1号机
#define DEV_TYPE_2		1				//2号机
#define MCB_SW_VER		"1.0"			//软件版本号定义，不限制（格式x.x），软件版本手动在此处改
#define MCB_HW_VER		"1.0"			//硬件版本号初值：硬件2 GPIO表示（"1.0"~"1.3"），由软件判断得出
#define MCB_FILE		"MCB_1.0_1.0"	//主控板文件名初值

//
#define GPIO_RFB_XYB_PIN 	(GPIOB->IDR & GPIO_Pin_7) 		//PB7，板类型
#define GPIO_DEV_TYPE_PIN 	(GPIOB->IDR & GPIO_Pin_6)		//PB6，机型
//pca9555 extend io
//#define GPIO_HW_IO1_PIN 	(?)			//PCA9555 IO0-5     //硬件版本，PCA9555中定义
//#define GPIO_HW_IO2_PIN 	(?)			//PCA9555 IO0-6


/*
********************************************************************************
公共变量定义
********************************************************************************
*/
__PUBLIC_EXE uint8_t rfb_or_xyb;		//1:射频板, 2:协议板，硬件GPIO读出来
__PUBLIC_EXE uint8_t dev_type;			//0:1号机, 1:2号机，硬件GPIO读出来
__PUBLIC_EXE uint8_t hw_ver21;			//硬件版本号，硬件GPIO读出来 //ver2 ver1
										//IO2 IO1：00：“1.0”, 01：“1.1”, 10：“1.2”, 11：“1.3”

#ifdef __PUBLIC_GLOBAL
	char mcb_sw_ver[4] = MCB_SW_VER;	//初值
	char mcb_hw_ver[4] = MCB_HW_VER;	
	char mcb_file[12] = MCB_FILE;	
#else
extern	char mcb_sw_ver[4];			
extern	char mcb_hw_ver[4];			
extern	char mcb_file[12];		
#endif
/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void public_init(void);					//公共模块初始化
void delay_xus(uint32_t val);			//软件延时xus
void delay_xms(uint32_t val);			//软件延时xms

/*
********************************************************************************
私有变量定义
********************************************************************************
*/


/*
********************************************************************************
私有函数定义
********************************************************************************
*/
void TIM14_init(uint16_t period, uint16_t prescaler);//10ms节拍定时器初始化
void get_rfb_xyb(void);					//获取外接射频板还是协议板
void get_mcb_file(void);				//获取主控板文件名，软件手动改，硬件自动获取
void get_dev_type(void);				//获取设备机型，硬件GPIO读出来
void sys_sw_reset(void);                //系统软复位
void dev_rfxy_io_init(void);

#ifdef __cplusplus
	}
#endif

#endif
