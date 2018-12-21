/*
********************************************************************************
文件名	：lcd_if.h
文件描述：LCD IF硬件接口头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __LCD_IF_H
#define __LCD_IF_H

#ifdef __LCD_IF_GLOBAL
	#define __LCD_IF_EXE 
#else
	#define __LCD_IF_EXE extern
#endif

#ifdef __cplusplus
	extern "C" {
#endif 

//PC6		PC7			PC8			PB14		PB15
//LCD_A0	LCD_RST		LCD_CS		LCD_SCK		LCD_DI
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
#define	LCD_BL_ON_DEF		0
#define	LCD_BL_OFF_DEF		1         

#ifdef __OLD_MCB_VER__
	#define	LCD_BL_ON		(GPIOB->BRR = GPIO_Pin_13)			//OLD_MCB BL PB13
	#define	LCD_BL_OFF	    (GPIOB->BSRR = GPIO_Pin_13)     
#else
	#define	LCD_BL_ON			lcd_bl_set(LCD_BL_ON_DEF)    	//背光亮
	#define	LCD_BL_OFF			lcd_bl_set(LCD_BL_OFF_DEF)		//背光灭
#endif

#define LCD_A0_H			(GPIOC->BSRR = GPIO_Pin_6)		//LCD硬件接口定义
#define LCD_A0_L            (GPIOC->BRR = GPIO_Pin_6)     

#define LCD_RST_H			(GPIOC->BSRR = GPIO_Pin_7)		//同上...
#define LCD_RST_L           (GPIOC->BRR = GPIO_Pin_7) 

#define LCD_CS_H			(GPIOC->BSRR = GPIO_Pin_8)		//同上...
#define LCD_CS_L            (GPIOC->BRR = GPIO_Pin_8) 

#define LCD_SCK_H			(GPIOB->BSRR = GPIO_Pin_14)		//同上...
#define LCD_SCK_L           (GPIOB->BRR = GPIO_Pin_14) 

#define LCD_DI_H			(GPIOB->BSRR = GPIO_Pin_15)		//同上...
#define LCD_DI_L            (GPIOB->BRR = GPIO_Pin_15) 
/*
********************************************************************************
公共变量定义
********************************************************************************
*/


/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void lcd_if_init(void);				//LCD硬件接口初始化
void lcd_bl_set(uint8_t on_off);	//LCD背光控制


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
void lcd_gpio_init(void);

#ifdef __cplusplus
	}
#endif

#endif
