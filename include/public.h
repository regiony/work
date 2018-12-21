/*
********************************************************************************
�ļ���	��public.h
�ļ�����������ͷ�ļ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
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
ͷ�ļ�
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
�궨��
********************************************************************************
*/
#define TIM14_PERIOD	10000			//���� 1/1M * 10000 = 10ms
#define TIM14_PSC		47				//48��Ƶ��48M/48=1M
#define RFB				1				//��Ƶ��
#define XYB				2				//Э���
#define DEV_TYPE_1		0				//1�Ż�
#define DEV_TYPE_2		1				//2�Ż�
#define MCB_SW_VER		"1.0"			//����汾�Ŷ��壬�����ƣ���ʽx.x��������汾�ֶ��ڴ˴���
#define MCB_HW_VER		"1.0"			//Ӳ���汾�ų�ֵ��Ӳ��2 GPIO��ʾ��"1.0"~"1.3"����������жϵó�
#define MCB_FILE		"MCB_1.0_1.0"	//���ذ��ļ�����ֵ

//
#define GPIO_RFB_XYB_PIN 	(GPIOB->IDR & GPIO_Pin_7) 		//PB7��������
#define GPIO_DEV_TYPE_PIN 	(GPIOB->IDR & GPIO_Pin_6)		//PB6������
//pca9555 extend io
//#define GPIO_HW_IO1_PIN 	(?)			//PCA9555 IO0-5     //Ӳ���汾��PCA9555�ж���
//#define GPIO_HW_IO2_PIN 	(?)			//PCA9555 IO0-6


/*
********************************************************************************
������������
********************************************************************************
*/
__PUBLIC_EXE uint8_t rfb_or_xyb;		//1:��Ƶ��, 2:Э��壬Ӳ��GPIO������
__PUBLIC_EXE uint8_t dev_type;			//0:1�Ż�, 1:2�Ż���Ӳ��GPIO������
__PUBLIC_EXE uint8_t hw_ver21;			//Ӳ���汾�ţ�Ӳ��GPIO������ //ver2 ver1
										//IO2 IO1��00����1.0��, 01����1.1��, 10����1.2��, 11����1.3��

#ifdef __PUBLIC_GLOBAL
	char mcb_sw_ver[4] = MCB_SW_VER;	//��ֵ
	char mcb_hw_ver[4] = MCB_HW_VER;	
	char mcb_file[12] = MCB_FILE;	
#else
extern	char mcb_sw_ver[4];			
extern	char mcb_hw_ver[4];			
extern	char mcb_file[12];		
#endif
/*
********************************************************************************
������������
********************************************************************************
*/
void public_init(void);					//����ģ���ʼ��
void delay_xus(uint32_t val);			//�����ʱxus
void delay_xms(uint32_t val);			//�����ʱxms

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
void TIM14_init(uint16_t period, uint16_t prescaler);//10ms���Ķ�ʱ����ʼ��
void get_rfb_xyb(void);					//��ȡ�����Ƶ�廹��Э���
void get_mcb_file(void);				//��ȡ���ذ��ļ���������ֶ��ģ�Ӳ���Զ���ȡ
void get_dev_type(void);				//��ȡ�豸���ͣ�Ӳ��GPIO������
void sys_sw_reset(void);                //ϵͳ��λ
void dev_rfxy_io_init(void);

#ifdef __cplusplus
	}
#endif

#endif
