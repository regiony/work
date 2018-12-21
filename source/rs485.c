/*
********************************************************************************
文件名	：rs485.c
文件描述：RS485通信驱动，API
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __RS485_GLOBAL
#include "rs485.h"                 
#include "i2c.h"  
#include "pca9555.h"  
#include "public.h"

/*
********************************************************************************
函数名称：rs485_init
函数描述：RS485初始化
参数描述：
返回值	：
********************************************************************************
*/
void rs485_init(void) 
{
	uint8_t *pt; 
    uint32_t i;
	
	rs485_1_comm_recv_end_flag = 0;
	rs485_1_comm_link_tx_flag = 0; 
	rs485_1_comm_data_deal_flag = 0; 
	rs485_2_comm_recv_end_flag = 0; 
	rs485_2_comm_link_tx_flag = 0;  
	rs485_2_comm_data_deal_flag = 0;                       
	rs485_2_comm_clk_out_flag = 0; 
	
	rs485_2_clk_out_period = CLK_OUT_64K_PERIOD;
	rs485_2_clk_out_pulse = CLK_OUT_64K_PUSLE;
	
    pt = (uint8_t *)(&rs485_1_comm_data.rx_cnt);
    for(i = 0; i < sizeof(RS485_COMM_TYPE_DEF); i++)
        *pt = 0;	
        
    pt = (uint8_t *)(&rs485_2_comm_data.rx_cnt);
    for(i = 0; i < sizeof(RS485_COMM_TYPE_DEF); i++)
        *pt = 0;	

	rs485_1_en(RS485_EN_RECV);						//RS485-1默认接收状态
	rs485_2_en(RS485_EN_RECV);						//RS485-2默认接收状态
	usart2_init(RS485_BR);
	usart3_init(RS485_BR);
		
	TIM3_PWM_init(rs485_2_clk_out_period, rs485_2_clk_out_pulse);	
}

/*
********************************************************************************
函数名称：rs485_1_schedule
函数描述：RS485-1调度
参数描述：
返回值	：
********************************************************************************
*/
void rs485_1_schedule(void)							
{
    //接收数据处理
    if(rs485_1_comm_recv_end_flag)
    {
        rs485_1_comm_recv_end_flag = 0;              
        rs485_1_comm_data_deal();                    //数据解析
        rs485_1_comm_data_deal_flag = 0;
        rs485_1_comm_link_tx_flag = 1;
    }
    //链路发送
    if(rs485_1_comm_link_tx_flag)
    {
        rs485_1_comm_link_tx_flag = 0;   
        rs485_1_comm_data_send();                    //数据发送
    }	
}

/*
********************************************************************************
函数名称：rs485_2_schedule
函数描述：RS485-2调度
参数描述：
返回值	：
********************************************************************************
*/
void rs485_2_schedule(void)						
{
    //接收数据处理
    if(rs485_2_comm_recv_end_flag)
    {
        rs485_2_comm_recv_end_flag = 0;              
        rs485_2_comm_data_deal();                    //数据解析
        rs485_2_comm_data_deal_flag = 0;
        rs485_2_comm_link_tx_flag = 1;
    }
    //链路发送
    if(rs485_2_comm_link_tx_flag)
    {
        rs485_2_comm_link_tx_flag = 0;   
        rs485_2_comm_data_send();                   //数据发送
    }	
    //重新配置CLK OUT
    if(rs485_2_comm_clk_out_flag)	
    {
    	rs485_2_comm_clk_out_flag = 0;
    	TIM3_PWM_init(rs485_2_clk_out_period, rs485_2_clk_out_pulse);	
    }
}

/*
********************************************************************************
函数名称：rs485_1_comm_data_deal
函数描述：rs485_1数据解析处理函数
参数描述：
返回值	：
********************************************************************************
*/
void rs485_1_comm_data_deal(void)					
{
	
}

/*
********************************************************************************
函数名称：rs485_1_comm_data_send
函数描述：rs485_1链路发送数据函数
参数描述：
返回值	：
********************************************************************************
*/
void rs485_1_comm_data_send(void)				
{
    rs485_1_en(RS485_EN_SEND);	
    delay_xms(1);
    rs485_1_comm_data.tx_cnt = 0;
    USART2->ICR |= 0x40;
    USART2->CR1 |= 0x40;
    USART2->TDR = rs485_1_comm_data.tx[rs485_1_comm_data.tx_cnt++];	
}

/*
********************************************************************************
函数名称：rs485_2_comm_data_deal
函数描述：rs485_2数据解析处理函数
参数描述：
返回值	：
********************************************************************************
*/
void rs485_2_comm_data_deal(void)
{
	
}

/*
********************************************************************************
函数名称：rs485_2_comm_data_send
函数描述：rs485_2链路发送数据函数
参数描述：
返回值	：
********************************************************************************
*/
void rs485_2_comm_data_send(void)
{
    rs485_2_en(RS485_EN_SEND);	
    delay_xms(1);
    rs485_2_comm_data.tx_cnt = 0;
    USART3->ICR |= 0x40;
    USART3->CR1 |= 0x40;
    USART3->TDR = rs485_2_comm_data.tx[rs485_2_comm_data.tx_cnt++];	
}

/*
********************************************************************************
函数名称：rs485_1_en
函数描述：RS485-1收发控制
参数描述：recv_or_send: RS485_EN_RECV, RS485_EN_SEND
返回值	：
********************************************************************************
*/
void rs485_1_en(uint8_t recv_or_send)
{
	rs485_en_ctrl(RS485_EN1_CTRL, recv_or_send);
}

/*
********************************************************************************
函数名称：rs485_2_en
函数描述：RS485-2收发控制
参数描述：recv_or_send: RS485_EN_RECV, RS485_EN_SEND
返回值	：
********************************************************************************
*/
void rs485_2_en(uint8_t recv_or_send)
{
	rs485_en_ctrl(RS485_EN2_CTRL, recv_or_send);
}

/*
********************************************************************************
函数名称：TIM3_PWM_init
函数描述：TIM3-CH4 PWM输出配置，重新配置时先停定时器，清计数器，配好后再启动
参数描述：
返回值	：
********************************************************************************
*/
void TIM3_PWM_init(uint16_t period, uint16_t pulse)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE); 	//使能GPIOC时钟
	//CLKOUT 时钟输出对应引脚PC9 TIM3_CH4_PWM		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);					//初始化GPIO	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_0);	
	
	/* TIM3 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  	//使能TIM3时钟	
    TIM3->CR1 &= ~TIM_CR1_CEN;  
    
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = period;  	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	/* Output Compare Timing Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_Pulse = pulse;
	
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
    //TIM_ARRPreloadConfig(TIM3, ENABLE);
    
	TIM3->CNT = 0;
	TIM3->CR1 |= TIM_CR1_CEN;								//啥时候允许输出64K，待定，目前上电就有
}

/*
********************************************************************************
函数名称：usart2_init
函数描述：usart2初始化,RS485-1
参数描述：
返回值	：
********************************************************************************
*/
void usart2_init(uint32_t br)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE); 	//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//使能USART2时钟
	//串口2引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1); 		//GPIOA2复用为GPIO_AF_1   
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1); 		//GPIOA3复用为GPIO_AF_1
	//USART2    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 	//GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 			//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); 					//初始化PA2，PA3
 	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = br;				//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;		//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); 				//初始化串口2
	USART_Cmd(USART2, ENABLE);  							//使能串口 2
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			//开启接受中断
	//Usart2 NVIC 配置
  	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;    		//IRQ通道使能
	NVIC_Init(&NVIC_InitStruct);							//根据指定的参数初始化VIC寄存器、
}

/*
********************************************************************************
函数名称：usart3_init
函数描述：usart3初始化，RS485-2，CLKOUT
参数描述：
返回值	：
********************************************************************************
*/
void usart3_init(uint32_t br)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE); 	//使能GPIOC时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	//使能USART3时钟
	//串口3引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource4,GPIO_AF_1); 		//GPIOC4复用为GPIO_AF_1  
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource5,GPIO_AF_1); 		//GPIOC5复用为GPIO_AF_1
	//USART3    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; 	//GPIOC4与GPIOC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 			//上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); 					//初始化PC4，PC5
	
	USART_InitStructure.USART_BaudRate = br;				//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;		//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); 				//初始化串口3
	USART_Cmd(USART3, ENABLE);  							//使能串口 3
	USART_ClearFlag(USART3, USART_FLAG_TC);
}

