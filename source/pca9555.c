/*
********************************************************************************
文件名	：pca9555.c
文件描述：PCA9555 API
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __PCA9555_GLOBAL
#include "i2c.h"  
#include "pca9555.h"                 


/*
********************************************************************************
函数名称：pca9555_init
函数描述：pca9555初始化
参数描述：
返回值	：
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
函数名称：rd_hw_ver21
函数描述：读硬件版本号
参数描述：
返回值	：
********************************************************************************
*/
void rd_hw_ver21(uint8_t *val)
{
	pca9555_rd_port(PCA9555_I2C_ADDR_1, PCA9555_PORT0, val);		
}

/*
********************************************************************************
函数名称：lcd_bl_ctrl
函数描述：LCD背光控制
参数描述：io:LCD_BL_CTRL; set_reset :PORT_OUT_RESET / PORT_OUT_SET
返回值	：
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
函数名称：alarm_out_ctrl
函数描述：报警输出控制
参数描述：io:ALM4_CTRL...ALM1_CTRL; set_reset :PORT_OUT_RESET / PORT_OUT_SET
返回值	：
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
函数名称：led_out_ctrl
函数描述：LED输出控制
参数描述：io:RLED1_CTRL / GLED1_CTRL... RLED3_CTRL / GLED3_CTRL; set_reset :PORT_OUT_RESET / PORT_OUT_SET
返回值	：
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
函数名称：rs485_en_ctrl
函数描述：RS485 EN输出控制
参数描述：io:RS485_EN1_CTRL / RS485_EN2_CTRL; set_reset :PORT_OUT_RESET / PORT_OUT_SET
返回值	：
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
函数名称：pca9555_set_dir
函数描述：pca9555设置方向
参数描述：
返回值	：
********************************************************************************
*/
void pca9555_set_dir(uint8_t i2c_addr, uint8_t portx, uint8_t dir)
{
	if(portx == PCA9555_PORT0)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_CFG0, dir) == I2C_FAIL)
		{
			//若操作失败，多次后认为故障，具体处理后边完善，目前时间太紧
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
	else
	if(portx == PCA9555_PORT1)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_CFG1, dir) == I2C_FAIL)
		{
			//若操作失败，多次后认为故障，具体处理后边完善，目前时间太紧	
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
}

/*
********************************************************************************
函数名称：pca9555_set_pol
函数描述：pca9555设置极性
参数描述：
返回值	：
********************************************************************************
*/
void pca9555_set_pol(uint8_t i2c_addr, uint8_t portx, uint8_t pol)
{
	if(portx == PCA9555_PORT0)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_CFG0, pol) == I2C_FAIL)
		{
			//若操作失败，多次后认为故障，具体处理后边完善，目前时间太紧
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
	else
	if(portx == PCA9555_PORT1)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_POL1, pol) == I2C_FAIL)
		{
			//若操作失败，多次后认为故障，具体处理后边完善，目前时间太紧	
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
}

/*
********************************************************************************
函数名称：pca9555_rd_port
函数描述：pca9555读IO
参数描述：
返回值	：
********************************************************************************
*/
void pca9555_rd_port(uint8_t i2c_addr, uint8_t portx, uint8_t *val)
{
	if(portx == PCA9555_PORT0)
	{
		if(I2C_Read_Byte(i2c_addr, PCA9555_REG_IN0, val) == I2C_FAIL)
		{
			//若操作失败，多次后认为故障，具体处理后边完善，目前时间太紧
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
	else
	if(portx == PCA9555_PORT1)
	{
		if(I2C_Read_Byte(i2c_addr, PCA9555_REG_IN1, val) == I2C_FAIL)
		{
			//若操作失败，多次后认为故障，具体处理后边完善，目前时间太紧	
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
}

/*
********************************************************************************
函数名称：pca9555_wr_port
函数描述：pca9555写IO
参数描述：
返回值	：
********************************************************************************
*/
void pca9555_wr_port(uint8_t i2c_addr, uint8_t portx, uint8_t val)
{
	if(portx == PCA9555_PORT0)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_OUT0, val) == I2C_FAIL)
		{
			//若操作失败，多次后认为故障，具体处理后边完善，目前时间太紧
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
	else
	if(portx == PCA9555_PORT1)
	{
		if(I2C_Write_Byte(i2c_addr, PCA9555_REG_OUT1, val) == I2C_FAIL)
		{
			//若操作失败，多次后认为故障，具体处理后边完善，目前时间太紧	
			//N > 3; sys_fault_word |= PCA9555_FAULT;
		}		
	}
}

