/*
********************************************************************************
文件名	：lcd_api.h
文件描述：LCD API头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __LCD_API_H
#define __LCD_API_H

#ifdef __LCD_API_GLOBAL
	#define __LCD_API_EXE 
#else
	#define __LCD_API_EXE extern
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
#define	DISPLAY_BUF_SIZE	1024					//128*64/8=1024
#define NULL				((void*)0)              //null

//机型1字体大小选择
//#define ZITI_4HAO_1
#define ZITI_5HAO_1
//#define ZITI_XIAO5_1

//机型2字体大小选择
//#define ZITI_4HAO_2
//#define ZITI_5HAO_2
#define ZITI_XIAO5_2

#ifdef 	ZITI_4HAO_1
	#define ZITI_NUM_1		16
#endif	
#ifdef 	ZITI_5HAO_1
	#define ZITI_NUM_1		14
#endif	
#ifdef 	ZITI_XIAO5_1
	#define ZITI_NUM_1		12
#endif	

#ifdef 	ZITI_4HAO_2
	#define ZITI_NUM_2		16
#endif	
#ifdef 	ZITI_5HAO_2
	#define ZITI_NUM_2		14
#endif	
#ifdef 	ZITI_XIAO5_2
	#define ZITI_NUM_2		12
#endif	

/*
********************************************************************************
公共变量定义
********************************************************************************
*/
__LCD_API_EXE char display_buf[DISPLAY_BUF_SIZE];	//显示缓冲区

/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void lcd_api_init(void);							//LCD API初始化
void lcd_clear_(void);
void lcd_clear(void);
void lcd_display(void);

void lcd_draw_char_5x7(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);
void lcd_draw_char_8x16(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);
void lcd_draw_hz_nx16_1(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);		//机型1-五号
void lcd_draw_hz_nx16_2(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);		//机型2-小五
void lcd_draw_hz_32x32(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);

void lcd_draw_graph(uint8_t line, uint8_t column, uint8_t width, uint8_t high, const uint8_t *pt);
void lcd_draw_hline(uint8_t line, uint8_t column, uint8_t len);
void lcd_clr_hline(uint8_t line, uint8_t column, uint8_t len);
void lcd_draw_vline(uint8_t line, uint8_t column, uint8_t len);
void lcd_draw_rect(uint8_t line, uint8_t column, uint8_t width, uint8_t high);

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
void lcd_set_xy(uint8_t line, uint8_t column);
void lcd_draw_dot(uint8_t line, uint8_t column, uint8_t color);



#ifdef __cplusplus
	}
#endif

#endif
