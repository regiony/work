/*
********************************************************************************
�ļ���	��pca9555.h
�ļ�������PCA9555�ӿ�ͷ�ļ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#ifndef __PCA9555_H
#define __PCA9555_H

#ifdef __PCA9555_GLOBAL
	#define __PCA9555_EXE 
#else
	#define __PCA9555_EXE extern
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
//PCA9555�����Ų�������
#define PCA9555_I2C_ADDR_1		0x40						//I2C��ַ����R/nW������0x20<<1��
#define PCA9555_REG_IN0         0x00                        //����Ĵ���0��ַ    
#define PCA9555_REG_IN1         0x01                        //����Ĵ���1��ַ    
#define PCA9555_REG_OUT0        0x02                        //����Ĵ���0��ַ    
#define PCA9555_REG_OUT1        0x03                        //����Ĵ���1��ַ    
#define PCA9555_REG_POL0        0x04                        //���Է�ת�Ĵ���0��ַ    
#define PCA9555_REG_POL1        0x05                        //���Է�ת�Ĵ���1��ַ 
#define PCA9555_REG_CFG0        0x06                        //�������üĴ���0��ַ    
#define PCA9555_REG_CFG1        0x07                        //�������üĴ���1��ַ   

//ʵ��Ӧ�ö���
#define PORT_OUT_RESET			0x00						//PORT�����
#define PORT_OUT_SET			0x01						//PORT�����
#define PCA9555_PORT0			0x00						//PORT0 FLAG
#define PCA9555_PORT1			0x01						//PORT1 FLAG
#define PCA9555_DIR_VAL0		0xE0						//ʵ��Ӧ��IO��������ֵ0
#define PCA9555_DIR_VAL1		0x00						//ʵ��Ӧ��IO��������ֵ1
#define PCA9555_POL_VAL0		0x00						//ʵ��Ӧ��POL��������ֵ0
#define PCA9555_POL_VAL1		0x00						//ʵ��Ӧ��POL��������ֵ1
//ʵ��Ӧ��PORT0 �źŶ���
#define HW_VER_21				0x60						//Ӳ���汾��21��ӦIO����
#define LCD_BL_CTRL				0x10						//LCD������ƶ�ӦIO����
#define ALM4_CTRL				0x08						//��������4��ӦIO����
#define ALM3_CTRL				0x04						//3ͬ��
#define ALM2_CTRL				0x02						//2ͬ��
#define ALM1_CTRL				0x01						//1ͬ��
//ʵ��Ӧ��PORT1 �źŶ���
#define RLED1_CTRL				0x80						//RED LED1���ƶ�ӦIO����
#define GLED1_CTRL				0x40						//GREEN LED1���ƶ�ӦIO����
#define RLED2_CTRL				0x20						//RED LED2���ƶ�ӦIO����
#define GLED2_CTRL				0x10						//GREEN LED2���ƶ�ӦIO����
#define RLED3_CTRL				0x08						//RED LED3���ƶ�ӦIO����
#define GLED3_CTRL				0x04						//GREEN LED3���ƶ�ӦIO����
#define RS485_EN1_CTRL			0x02						//RS485_EN1���ƶ�ӦIO����
#define RS485_EN2_CTRL			0x01						//RS485_EN2���ƶ�ӦIO����

/*
********************************************************************************
������������
********************************************************************************
*/
//PORT0		IO07	IO06	IO05	IO04	IO03	IO02	IO01	IO00
//DIR		1-IN	1-IN	1-IN	0-OUT	0-OUT	0-OUT	0-OUT	0-OUT
//POL		0		0		0		0		0		0		0		0
//NAME		NO USE	VER2	VER1	LCD_BL	ALM4	ALM3	ALM2	ALM1
__PCA9555_EXE uint8_t port0_buf;							//PCA9555 port0����������ֵ0��������λ��λ�����

//PORT1		IO17	IO16	IO15	IO14	IO13	IO12	IO11	IO10
//DIR		0-OUT	0-OUT	0-OUT	0-OUT	0-OUT	0-OUT	0-OUT	0-OUT
//POL		0		0		0		0		0		0		0		0
//NAME		RLED1	GLED1	RLED2	GLED2	RLED3	GLED3	485EN1	485EN2
__PCA9555_EXE uint8_t port1_buf;							//PCA9555port1����������ֵ0��������λ��λ�����
/*
********************************************************************************
������������
********************************************************************************
*/
void pca9555_init(void);									//pca9555��ʼ��
//port0
void rd_hw_ver21(uint8_t *val);								//��Ӳ���汾��
void lcd_bl_ctrl(uint8_t io, uint8_t set_reset);			//LCD�������
void alarm_out_ctrl(uint8_t io, uint8_t set_reset);			//�����������
//port1
void led_out_ctrl(uint8_t io, uint8_t set_reset);			//LED�������
void rs485_en_ctrl(uint8_t io, uint8_t set_reset);			//RS485 EN�������

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
//pca9555 ������������
void pca9555_set_dir(uint8_t i2c_addr, uint8_t portx, uint8_t dir);
void pca9555_set_pol(uint8_t i2c_addr, uint8_t portx, uint8_t pol);
void pca9555_rd_port(uint8_t i2c_addr, uint8_t portx, uint8_t *val);
void pca9555_wr_port(uint8_t i2c_addr, uint8_t portx, uint8_t val);

#ifdef __cplusplus
	}
#endif

#endif
