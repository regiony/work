/*
********************************************************************************
�ļ���	��lcd_if.h
�ļ�������LCD IFӲ���ӿ�ͷ�ļ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
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
ͷ�ļ�
********************************************************************************
*/
#include "stm32f0xx.h"

/*
********************************************************************************
�궨��
********************************************************************************
*/
#define	LCD_BL_ON_DEF		0
#define	LCD_BL_OFF_DEF		1         

#ifdef __OLD_MCB_VER__
	#define	LCD_BL_ON		(GPIOB->BRR = GPIO_Pin_13)			//OLD_MCB BL PB13
	#define	LCD_BL_OFF	    (GPIOB->BSRR = GPIO_Pin_13)     
#else
	#define	LCD_BL_ON			lcd_bl_set(LCD_BL_ON_DEF)    	//������
	#define	LCD_BL_OFF			lcd_bl_set(LCD_BL_OFF_DEF)		//������
#endif

#define LCD_A0_H			(GPIOC->BSRR = GPIO_Pin_6)		//LCDӲ���ӿڶ���
#define LCD_A0_L            (GPIOC->BRR = GPIO_Pin_6)     

#define LCD_RST_H			(GPIOC->BSRR = GPIO_Pin_7)		//ͬ��...
#define LCD_RST_L           (GPIOC->BRR = GPIO_Pin_7) 

#define LCD_CS_H			(GPIOC->BSRR = GPIO_Pin_8)		//ͬ��...
#define LCD_CS_L            (GPIOC->BRR = GPIO_Pin_8) 

#define LCD_SCK_H			(GPIOB->BSRR = GPIO_Pin_14)		//ͬ��...
#define LCD_SCK_L           (GPIOB->BRR = GPIO_Pin_14) 

#define LCD_DI_H			(GPIOB->BSRR = GPIO_Pin_15)		//ͬ��...
#define LCD_DI_L            (GPIOB->BRR = GPIO_Pin_15) 
/*
********************************************************************************
������������
********************************************************************************
*/


/*
********************************************************************************
������������
********************************************************************************
*/
void lcd_if_init(void);				//LCDӲ���ӿڳ�ʼ��
void lcd_bl_set(uint8_t on_off);	//LCD�������


/*
********************************************************************************
˽�б�������
********************************************************************************
*/


/*
********************************************************************************
˽�к�������
********************************************************************************
*/
void lcd_gpio_init(void);

#ifdef __cplusplus
	}
#endif

#endif
