/*
********************************************************************************
文件名	：i2c.c
文件描述：i2c接口驱动
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __I2C_GLOBAL
#include "i2c.h"                 


/*
********************************************************************************
函数名称：i2c_init
函数描述：i2c初始化
参数描述：
返回值	：
********************************************************************************
*/
void i2c_init(void)
{
	I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);  		//使能GPIOA的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	
	/* GPIO configuration */
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_5);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_5);      
															
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);   
	
	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	//48MHZ: 0xB042C3C7 10K 0xB0420F13 100K 0x50330309 400K 
	// 8MHZ: 0x1042c3c7 10K   0x40B22536 400K
	I2C_InitStructure.I2C_Timing = 0xB042C3C7;
	
	/* Apply sEE_I2C configuration after enabling it */
	I2C_Init(I2C2, &I2C_InitStructure);
	 
	/* sEE_I2C Peripheral Enable */
	I2C_Cmd(I2C2, ENABLE);
}

/*
********************************************************************************
函数名称：I2C_Read_Byte
函数描述：读字节
参数描述：
返回值	：
********************************************************************************
*/
uint8_t I2C_Read_Byte(uint8_t i2c_addr, uint8_t data_addr, uint8_t *pt_data)
{
	uint32_t i; 
	
  	i = 0;
	while(I2C2->ISR & I2C_FLAG_BUSY)
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}
  
  	I2C_TransferHandling(I2C2, i2c_addr, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  
  	i = 0;
	while(!(I2C2->ISR & I2C_FLAG_TXIS))
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}  
  
  	I2C2->TXDR = data_addr;
  	
  	i = 0;
	while(!(I2C2->ISR & I2C_FLAG_TC))
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}   
		
  	I2C_TransferHandling(I2C2, i2c_addr, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  
  	i = 0;
	while(!(I2C2->ISR & I2C_FLAG_RXNE))
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}   
	
	*pt_data = I2C2->RXDR;

  	i = 0;
	while(!(I2C2->ISR & I2C_FLAG_STOPF))
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}   

	return I2C_OK;
}

/*
********************************************************************************
函数名称：I2C_Write_Byte
函数描述：写字节
参数描述：
返回值	：
********************************************************************************
*/
uint8_t I2C_Write_Byte(uint8_t i2c_addr, uint8_t data_addr, uint8_t data_val)
{
	uint32_t i; 
	
  	i = 0;
	while(I2C2->ISR & I2C_FLAG_BUSY)
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}
  
	I2C_TransferHandling(I2C2, i2c_addr, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
  
  	i = 0;
	while(!(I2C2->ISR & I2C_FLAG_TXIS))
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}  

  	I2C2->TXDR = data_addr;
  	
  	i = 0;
	while(!(I2C2->ISR & I2C_FLAG_TCR))
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}   
 
  	I2C_TransferHandling(I2C2, i2c_addr, 1, I2C_AutoEnd_Mode, I2C_No_StartStop);
  	
  	i = 0;
	while(!(I2C2->ISR & I2C_FLAG_TXIS))
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}  
  	
	I2C2->TXDR = data_val;
	
  	i = 0;
	while(!(I2C2->ISR & I2C_FLAG_STOPF))
	{
		i++;
		if(i > I2C_TIME_OUT)
			return I2C_FAIL;
	}   

	return I2C_OK;
}


//-----
/*
********************************************************************************
函数名称：
函数描述：I2C1_Read_NBytes
参数描述：
返回值	：
********************************************************************************
*/
uint8_t I2C1_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer)
{
  //static unsigned int tick_tmp;
  unsigned char read_Num;
	
//	tick_tmp = sys_tick;
  
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET)
  {
//    if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//    {
//      return I2C_FAIL;
//    }
  }
  
  I2C_TransferHandling(I2C2, driver_Addr, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET)
  {
//    if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//    {
//      return I2C_FAIL;
//    }
  }
  
  I2C_SendData(I2C2, start_Addr);
  
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TC) == RESET)
  {
//    if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//    {
//      return I2C_FAIL;
//    }
  }
  
  I2C_TransferHandling(I2C2, driver_Addr, number_Bytes,  I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  
  for(read_Num = 0; read_Num < number_Bytes; read_Num++)
  {
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
    {
//      if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//      {
//        return I2C_FAIL;
//      }
    }
    
    read_Buffer[read_Num] = I2C_ReceiveData(I2C2);
  }
  
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF) == RESET)
  {
//    if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//    {
//      return I2C_FAIL;
//    }
  }
 
  return I2C_OK;
}

/*
********************************************************************************
函数名称：i2c_init
函数描述：i2c初始化
参数描述：
返回值	：
********************************************************************************
*/
uint8_t  I2C1_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer)
{
	//static unsigned int tick_tmp;
	unsigned char write_Num;

//	tick_tmp = sys_tick;
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET)
  {
//    if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//    {
//      return I2C_FAIL;
//    }
  }
  
  I2C_TransferHandling(I2C2, driver_Addr, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
  
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET)
  {
//    if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//    {
//      return I2C_FAIL;
//    }
  }
  
  I2C_SendData(I2C2, start_Addr);
  
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TCR) == RESET)
  {
//    if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//    {
//      return I2C_FAIL;
//    }
  }
 
  I2C_TransferHandling(I2C2, driver_Addr, number_Bytes, I2C_AutoEnd_Mode, I2C_No_StartStop);
  
  for(write_Num = 0; write_Num <  number_Bytes; write_Num++)
  {
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_TXIS) == RESET)
    {
//      if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//      {
//        return I2C_FAIL;
//      }
    }
    
    I2C_SendData(I2C2, write_Buffer[write_Num]);
  }

  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF) == RESET)
  {
//    if((sys_tick-tick_tmp) > I2C_TIMEOUT)
//    {
//      return I2C_FAIL;
//    }
  }
 
  return I2C_OK;
}
