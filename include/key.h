/*
********************************************************************************
文件名	：key.h
文件描述：按键头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __KEY_H
#define __KEY_H

#ifdef __KEY_GLOBAL
	#define __KEY_EXE 
#else
	#define __KEY_EXE extern
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

//PB0		PB1		PB2		PB10	PB11	PC12
//KEY1		KEY2	KEY3	KEY4	KEY5	KEY6
//S1		S2		S3		S4		S5		S6
//UP		RT		OK		LT		DW		RETURN		//1、2、5号机
//2s		2s		200ms	2s		2s		200ms

//RETURN	-		UP		DW		OK		-			//3号机?
//200ms				2s		2s		200ms
/*
********************************************************************************
宏定义
********************************************************************************
*/

#define		KEY_NO			0xFFFF				//KEY IO 值
#define		KEY_UP			0x0C06				//KEY IO 值
#define		KEY_RIGHT		0x0C05				//...
#define		KEY_OK			0x0C03
#define		KEY_LEFT		0x0807
#define		KEY_DOWN		0x0407
#define		KEY_RETURN		0x0000				//0x1000
#define		KEY_UPDW		0x0406				//组合键
#define		KEY_LFRI		0x0805				//...

#define		KEY_PBIO		0x0C07				//检测用IO值
#define		KEY_PCIO		0x1000				//...
#define		RD_GPIOB		(GPIOB->IDR & KEY_PBIO)	//...
#define		RD_GPIOC		(GPIOC->IDR & KEY_PCIO)	//...

#define		KEY_JITTER_CNT	2					//消抖时间2*10ms，可改
#define		KEY_200MS_CNT	20					//短按键时间20*10ms
#define		KEY_2S_CNT		200					//长按键时间200*10ms

typedef enum 
{
	NO_KEY = 0,
	UP_S_KEY,									//S:200ms short key
	DOWN_S_KEY,
	LEFT_S_KEY,
	RIGHT_S_KEY,
	OK_S_KEY,
	RETURN_S_KEY,
	UD_S_KEY,
	LR_S_KEY,
	UP_L_KEY,									//L:2s long key
	DOWN_L_KEY,
	LEFT_L_KEY,
	RIGHT_L_KEY,
	OK_L_KEY,
	RETURN_L_KEY,
	UD_L_KEY,									//up down组合键
	LR_L_KEY,									//left right组合键
	
}KEY_VALUE_DEF;

/*
********************************************************************************
公共变量定义
********************************************************************************
*/
//mcb_hmu_task_flag = 1;						//按键属于人机任务的一部分
__KEY_EXE KEY_VALUE_DEF key_value;				//键值
__KEY_EXE uint8_t	key_press_flag;				//key按下标志，如有此标志，则mcb_hmu_task_flag = 1; key_scan()
__KEY_EXE uint8_t	key_timer_en;				//key计数器允许标志
__KEY_EXE uint16_t	key_timer_cnt;				//key计数器
__KEY_EXE uint8_t	key_timer_en1;				//key计数器允许标志
__KEY_EXE uint16_t	key_timer_cnt1;				//key计数器
__KEY_EXE uint16_t	key_press_val;				//key按下对应IO值

/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void key_init(void);							//key初始化
void key_scan(void);							//key扫描程序,若判断出按键则key_press_flag=0;mcb_hmu_task_flag=1;key_timer_en=0;key_timer_cnt=0;key_press_val=0;

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
void key_gpio_int_init(void);

#ifdef __cplusplus
	}
#endif

#endif
