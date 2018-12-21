/*
********************************************************************************
�ļ���	��lcd_api.h
�ļ�������LCD APIͷ�ļ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
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
ͷ�ļ�
********************************************************************************
*/
#include "stm32f0xx.h"

/*
********************************************************************************
�궨��
********************************************************************************
*/
#define	DISPLAY_BUF_SIZE	1024					//128*64/8=1024
#define NULL				((void*)0)              //null

//����1�����Сѡ��
//#define ZITI_4HAO_1
#define ZITI_5HAO_1
//#define ZITI_XIAO5_1

//����2�����Сѡ��
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
������������
********************************************************************************
*/
__LCD_API_EXE char display_buf[DISPLAY_BUF_SIZE];	//��ʾ������

/*
********************************************************************************
������������
********************************************************************************
*/
void lcd_api_init(void);							//LCD API��ʼ��
void lcd_clear_(void);
void lcd_clear(void);
void lcd_display(void);

void lcd_draw_char_5x7(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);
void lcd_draw_char_8x16(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);
void lcd_draw_hz_nx16_1(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);		//����1-���
void lcd_draw_hz_nx16_2(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);		//����2-С��
void lcd_draw_hz_32x32(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt);

void lcd_draw_graph(uint8_t line, uint8_t column, uint8_t width, uint8_t high, const uint8_t *pt);
void lcd_draw_hline(uint8_t line, uint8_t column, uint8_t len);
void lcd_clr_hline(uint8_t line, uint8_t column, uint8_t len);
void lcd_draw_vline(uint8_t line, uint8_t column, uint8_t len);
void lcd_draw_rect(uint8_t line, uint8_t column, uint8_t width, uint8_t high);

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
void lcd_set_xy(uint8_t line, uint8_t column);
void lcd_draw_dot(uint8_t line, uint8_t column, uint8_t color);



#ifdef __cplusplus
	}
#endif

#endif
