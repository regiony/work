/*
********************************************************************************
文件名	：pca9555.h
文件描述：PCA9555接口头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
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
头文件
********************************************************************************
*/
#include "stm32f0xx.h"

/*
********************************************************************************
宏定义
********************************************************************************
*/
//PCA9555基本才操作定义
#define PCA9555_I2C_ADDR_1		0x40						//I2C地址（含R/nW），（0x20<<1）
#define PCA9555_REG_IN0         0x00                        //输入寄存器0地址    
#define PCA9555_REG_IN1         0x01                        //输入寄存器1地址    
#define PCA9555_REG_OUT0        0x02                        //输出寄存器0地址    
#define PCA9555_REG_OUT1        0x03                        //输出寄存器1地址    
#define PCA9555_REG_POL0        0x04                        //极性反转寄存器0地址    
#define PCA9555_REG_POL1        0x05                        //极性反转寄存器1地址 
#define PCA9555_REG_CFG0        0x06                        //方向配置寄存器0地址    
#define PCA9555_REG_CFG1        0x07                        //方向配置寄存器1地址   

//实际应用定义
#define PORT_OUT_RESET			0x00						//PORT输出低
#define PORT_OUT_SET			0x01						//PORT输出高
#define PCA9555_PORT0			0x00						//PORT0 FLAG
#define PCA9555_PORT1			0x01						//PORT1 FLAG
#define PCA9555_DIR_VAL0		0xE0						//实际应用IO方向配置值0
#define PCA9555_DIR_VAL1		0x00						//实际应用IO方向配置值1
#define PCA9555_POL_VAL0		0x00						//实际应用POL极性配置值0
#define PCA9555_POL_VAL1		0x00						//实际应用POL极性配置值1
//实际应用PORT0 信号定义
#define HW_VER_21				0x60						//硬件版本号21对应IO定义
#define LCD_BL_CTRL				0x10						//LCD背光控制对应IO定义
#define ALM4_CTRL				0x08						//报警控制4对应IO定义
#define ALM3_CTRL				0x04						//3同上
#define ALM2_CTRL				0x02						//2同上
#define ALM1_CTRL				0x01						//1同上
//实际应用PORT1 信号定义
#define RLED1_CTRL				0x80						//RED LED1控制对应IO定义
#define GLED1_CTRL				0x40						//GREEN LED1控制对应IO定义
#define RLED2_CTRL				0x20						//RED LED2控制对应IO定义
#define GLED2_CTRL				0x10						//GREEN LED2控制对应IO定义
#define RLED3_CTRL				0x08						//RED LED3控制对应IO定义
#define GLED3_CTRL				0x04						//GREEN LED3控制对应IO定义
#define RS485_EN1_CTRL			0x02						//RS485_EN1控制对应IO定义
#define RS485_EN2_CTRL			0x01						//RS485_EN2控制对应IO定义

/*
********************************************************************************
公共变量定义
********************************************************************************
*/
//PORT0		IO07	IO06	IO05	IO04	IO03	IO02	IO01	IO00
//DIR		1-IN	1-IN	1-IN	0-OUT	0-OUT	0-OUT	0-OUT	0-OUT
//POL		0		0		0		0		0		0		0		0
//NAME		NO USE	VER2	VER1	LCD_BL	ALM4	ALM3	ALM2	ALM1
__PCA9555_EXE uint8_t port0_buf;							//PCA9555 port0缓冲区，初值0，运行中位与位或操作

//PORT1		IO17	IO16	IO15	IO14	IO13	IO12	IO11	IO10
//DIR		0-OUT	0-OUT	0-OUT	0-OUT	0-OUT	0-OUT	0-OUT	0-OUT
//POL		0		0		0		0		0		0		0		0
//NAME		RLED1	GLED1	RLED2	GLED2	RLED3	GLED3	485EN1	485EN2
__PCA9555_EXE uint8_t port1_buf;							//PCA9555port1缓冲区，初值0，运行中位与位或操作
/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void pca9555_init(void);									//pca9555初始化
//port0
void rd_hw_ver21(uint8_t *val);								//读硬件版本号
void lcd_bl_ctrl(uint8_t io, uint8_t set_reset);			//LCD背光控制
void alarm_out_ctrl(uint8_t io, uint8_t set_reset);			//报警输出控制
//port1
void led_out_ctrl(uint8_t io, uint8_t set_reset);			//LED输出控制
void rs485_en_ctrl(uint8_t io, uint8_t set_reset);			//RS485 EN输出控制

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
//pca9555 基本操作函数
void pca9555_set_dir(uint8_t i2c_addr, uint8_t portx, uint8_t dir);
void pca9555_set_pol(uint8_t i2c_addr, uint8_t portx, uint8_t pol);
void pca9555_rd_port(uint8_t i2c_addr, uint8_t portx, uint8_t *val);
void pca9555_wr_port(uint8_t i2c_addr, uint8_t portx, uint8_t val);

#ifdef __cplusplus
	}
#endif

#endif
