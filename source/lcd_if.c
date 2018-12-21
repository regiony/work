/*
********************************************************************************
文件名	：lcd_if.c
文件描述：LCD硬件相关接口
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __LCD_IF_GLOBAL
#include "lcd_if.h"                 
#include "pca9555.h"  

/*
********************************************************************************
函数名称：
函数描述：
参数描述：
返回值	：
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
函数名称：lcd_bl_set
函数描述：LCD背光设置
参数描述：on_off:0-LCD_BL_ON_DEF, 1-LCD_BL_OFF_DEF
返回值	：
********************************************************************************
*/
void lcd_bl_set(uint8_t on_off)
{
	lcd_bl_ctrl(LCD_BL_CTRL, on_off);	
}

/*
********************************************************************************
函数名称：lcd_gpio_init
函数描述：LCD IO初始化
参数描述：
返回值	：
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
