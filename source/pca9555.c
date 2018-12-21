/*
********************************************************************************
�ļ���	��pca9555.c
�ļ�������PCA9555 API
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __PCA9555_GLOBAL
#include "i2c.h"  
#include "pca9555.h"                 


/*
********************************************************************************
�������ƣ�pca9555_init
����������pca9555��ʼ��
����������
����ֵ	��
********************************************************************************
*/
void pca9555_init(void) 
{
	port0_buf = 0;
	port1_buf = 0;
	i2c_init();
	pca9555_set_pol(PCA9555_I2C_ADDR_1, PCA9555_PORT0, PCA9555_POL_VAL0);
	pca9555_set_pol(PCA9555_I2C_ADDR_1, PCA9555_PORT1, PCA9555_POL_VAL1);
	pca9555_set_dir(PCA9555_I2C_ADDR_1, PCA9555_PORT0, PCA9555_DIR_VAL0);
	pca9555_set_dir(PCA9555_I2C_ADDR_1, PCA9555_PORT1, PCA9555_DIR_VAL1);
} 

//PORT0
/*
********************************************************************************
�������ƣ�rd_hw_ver21
������������Ӳ���汾��
����������
����ֵ	��
********************************************************************************
*/
void rd_hw_ver21(uint8_t *val)
{
	pca9555_rd_port(PCA9555_I2C_ADDR_1, PCA9555_PORT0, val);		
}

/*
********************************************************************************
�������ƣ�lcd_bl_ctrl
����������LCD�������
����������io:LCD_BL_CTRL; set_reset :PORT_OUT_RESET / PORT_OUT_SET
����ֵ	��
********************************************************************************
*/
void lcd_bl_ctrl(uint8_t io, uint8_t set_reset)
{
	io &= LCD_BL_CTRL;
	
	if(set_reset == PORT_OUT_SET)
		port0_buf |= io;
	else
	if(set_reset == PORT_OUT_RESET)	
		port0_buf &= ~io;
		
	pca9555_wr_port(PCA9555_I2C_ADDR_1, PCA9555_PORT0, port0_buf);
}

/*
********************************************************************************
�������ƣ�alarm_out_ctrl
���������������������
����������io:ALM4_CTRL...ALM1_CTRL; set_reset :PORT_OUT_RESET / PORT_OUT_SET
����ֵ	��
********************************************************************************
*/
void alarm_out_ctrl(uint8_t io, uint8_t set_reset)
{
	io &= (ALM4_CTRL | ALM3_CTRL | ALM2_CTRL | ALM1_CTRL);

	if(set_reset == PORT_OUT_SET)
		port0_buf |= io;
	else
	if(set_reset == PORT_OUT_RESET)	
		port0_buf &= ~io;
		
	pca9555_wr_port(PCA9555_I2C_ADDR_1, PCA9555_PORT0, port0_buf);	
}

/*
********************************************************************************
�������ƣ�led_out_ctrl
����������LED�������
����������io:RLED1_CTRL / GLED1_CTRL... RLED3_CTRL / GLED3_CTRL; set_reset :PORT_OUT_RESET / PORT_OUT_SET
����ֵ	��
********************************************************************************
*/
//PORT1
void led_out_ctrl(uint8_t io, uint8_t set_reset)
{
	io &= (RLED1_CTRL | GLED1_CTRL | RLED2_CTRL | GLED2_CTRL | RLED3_CTRL | GLED3_CTRL);
	
	if(set_reset == PORT_OUT_SET)
		port1_buf |= io;
	else
	if(set_reset == PORT_OUT_RESET)	
		port1_buf &= ~io;
		
	pca9555_wr_port(PCA9555_I2C_ADDR_1, PCA9555_PORT1, port1_buf);		
}

/*
********************************************************************************
�������ƣ�rs485_en_ctrl
����������RS485 EN�������
����������io:RS485_EN1_CTRL / RS485_EN2_CTRL; set_reset :PORT_OUT_RESET / PORT_OUT_SET
����ֵ	��
********************************************************************************
*/
void rs485_en_ctrl(uint8_t io, uint8_t set_reset)
{
	io &= (RS485_EN1_CTRL | RS485_EN2_CTRL);
	
	if(set_reset == PORT_OUT_SET)
		port1_buf |= io;
	else
	if(set_reset == PORT_OUT_RESET)	
		port1_buf &= ~io;
		
	pca9555_wr_port(PCA9555_I2C_ADDR_1, PCA9555_PORT1, port1_buf);			
}

/*
********************************************************************************
�������ƣ�pca9555_set_dir
����������pca9555���÷���
����������
����ֵ	��
********************************************************************************
*/
void pca9555_set_dir(uint8_t i2c_addr, uint8_t portx, uint8_t dir)
{
	if(portx == PCA9555_PORT0)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_CFG0, dir) == I2C_FAIL)
		{
			//������ʧ�ܣ���κ���Ϊ���ϣ����崦�������ƣ�Ŀǰʱ��̫��
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
	else
	if(portx == PCA9555_PORT1)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_CFG1, dir) == I2C_FAIL)
		{
			//������ʧ�ܣ���κ���Ϊ���ϣ����崦�������ƣ�Ŀǰʱ��̫��	
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
}

/*
********************************************************************************
�������ƣ�pca9555_set_pol
����������pca9555���ü���
����������
����ֵ	��
********************************************************************************
*/
void pca9555_set_pol(uint8_t i2c_addr, uint8_t portx, uint8_t pol)
{
	if(portx == PCA9555_PORT0)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_CFG0, pol) == I2C_FAIL)
		{
			//������ʧ�ܣ���κ���Ϊ���ϣ����崦�������ƣ�Ŀǰʱ��̫��
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
	else
	if(portx == PCA9555_PORT1)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_POL1, pol) == I2C_FAIL)
		{
			//������ʧ�ܣ���κ���Ϊ���ϣ����崦�������ƣ�Ŀǰʱ��̫��	
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
}

/*
********************************************************************************
�������ƣ�pca9555_rd_port
����������pca9555��IO
����������
����ֵ	��
********************************************************************************
*/
void pca9555_rd_port(uint8_t i2c_addr, uint8_t portx, uint8_t *val)
{
	if(portx == PCA9555_PORT0)
	{
		if(I2C_Read_Byte(i2c_addr, PCA9555_REG_IN0, val) == I2C_FAIL)
		{
			//������ʧ�ܣ���κ���Ϊ���ϣ����崦�������ƣ�Ŀǰʱ��̫��
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
	else
	if(portx == PCA9555_PORT1)
	{
		if(I2C_Read_Byte(i2c_addr, PCA9555_REG_IN1, val) == I2C_FAIL)
		{
			//������ʧ�ܣ���κ���Ϊ���ϣ����崦�������ƣ�Ŀǰʱ��̫��	
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
}

/*
********************************************************************************
�������ƣ�pca9555_wr_port
����������pca9555дIO
����������
����ֵ	��
********************************************************************************
*/
void pca9555_wr_port(uint8_t i2c_addr, uint8_t portx, uint8_t val)
{
	if(portx == PCA9555_PORT0)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_OUT0, val) == I2C_FAIL)
		{
			//������ʧ�ܣ���κ���Ϊ���ϣ����崦�������ƣ�Ŀǰʱ��̫��
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
	else
	if(portx == PCA9555_PORT1)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_OUT1, val) == I2C_FAIL)
		{
			//������ʧ�ܣ���κ���Ϊ���ϣ����崦�������ƣ�Ŀǰʱ��̫��	
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
}

