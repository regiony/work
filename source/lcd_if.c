/*
********************************************************************************
�ļ���	��lcd_if.c
�ļ�������LCDӲ����ؽӿ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __LCD_IF_GLOBAL
#include "lcd_if.h"                 
#include "pca9555.h"  

/*
********************************************************************************
�������ƣ�
����������
����������
����ֵ	��
********************************************************************************
*/
void lcd_if_init(void) 
{
	lcd_gpio_init();	
	
	LCD_CS_H;
	LCD_SCK_H;
	LCD_RST_H;
	LCD_BL_ON;
}

/*
********************************************************************************
�������ƣ�lcd_bl_set
����������LCD��������
����������on_off:0-LCD_BL_ON_DEF, 1-LCD_BL_OFF_DEF
����ֵ	��
********************************************************************************
*/
void lcd_bl_set(uint8_t on_off)
{
	lcd_bl_ctrl(LCD_BL_CTRL, on_off);	
}

/*
********************************************************************************
�������ƣ�lcd_gpio_init
����������LCD IO��ʼ��
����������
����ֵ	��
********************************************************************************
*/
void lcd_gpio_init(void)
{
	GPIO_InitTypeDef LCDGPIO_InitStruct;
  	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);

	LCDGPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8;
	LCDGPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	LCDGPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	LCDGPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	LCDGPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC,&LCDGPIO_InitStruct);
	
	LCDGPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	LCDGPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	LCDGPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	LCDGPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	LCDGPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC,&LCDGPIO_InitStruct);
	
	LCDGPIO_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	LCDGPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	LCDGPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	LCDGPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	LCDGPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB,&LCDGPIO_InitStruct);
	
	#ifdef __OLD_MCB_VER__
		LCDGPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
		LCDGPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		LCDGPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
		LCDGPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		LCDGPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
		GPIO_Init(GPIOB,&LCDGPIO_InitStruct);	
	#endif
}
