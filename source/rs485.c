/*
********************************************************************************
�ļ���	��rs485.c
�ļ�������RS485ͨ��������API
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __RS485_GLOBAL
#include "rs485.h"                 
#include "i2c.h"  
#include "pca9555.h"  
#include "public.h"

/*
********************************************************************************
�������ƣ�rs485_init
����������RS485��ʼ��
����������
����ֵ	��
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

	rs485_1_en(RS485_EN_RECV);						//RS485-1Ĭ�Ͻ���״̬
	rs485_2_en(RS485_EN_RECV);						//RS485-2Ĭ�Ͻ���״̬
	usart2_init(RS485_BR);
	usart3_init(RS485_BR);
		
	TIM3_PWM_init(rs485_2_clk_out_period, rs485_2_clk_out_pulse);	
}

/*
********************************************************************************
�������ƣ�rs485_1_schedule
����������RS485-1����
����������
����ֵ	��
********************************************************************************
*/
void rs485_1_schedule(void)							
{
    //�������ݴ���
    if(rs485_1_comm_recv_end_flag)
    {
        rs485_1_comm_recv_end_flag = 0;              
        rs485_1_comm_data_deal();                    //���ݽ���
        rs485_1_comm_data_deal_flag = 0;
        rs485_1_comm_link_tx_flag = 1;
    }
    //��·����
    if(rs485_1_comm_link_tx_flag)
    {
        rs485_1_comm_link_tx_flag = 0;   
        rs485_1_comm_data_send();                    //���ݷ���
    }	
}

/*
********************************************************************************
�������ƣ�rs485_2_schedule
����������RS485-2����
����������
����ֵ	��
********************************************************************************
*/
void rs485_2_schedule(void)						
{
    //�������ݴ���
    if(rs485_2_comm_recv_end_flag)
    {
        rs485_2_comm_recv_end_flag = 0;              
        rs485_2_comm_data_deal();                    //���ݽ���
        rs485_2_comm_data_deal_flag = 0;
        rs485_2_comm_link_tx_flag = 1;
    }
    //��·����
    if(rs485_2_comm_link_tx_flag)
    {
        rs485_2_comm_link_tx_flag = 0;   
        rs485_2_comm_data_send();                   //���ݷ���
    }	
    //��������CLK OUT
    if(rs485_2_comm_clk_out_flag)	
    {
    	rs485_2_comm_clk_out_flag = 0;
    	TIM3_PWM_init(rs485_2_clk_out_period, rs485_2_clk_out_pulse);	
    }
}

/*
********************************************************************************
�������ƣ�rs485_1_comm_data_deal
����������rs485_1���ݽ���������
����������
����ֵ	��
********************************************************************************
*/
void rs485_1_comm_data_deal(void)					
{
	
}

/*
********************************************************************************
�������ƣ�rs485_1_comm_data_send
����������rs485_1��·�������ݺ���
����������
����ֵ	��
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
�������ƣ�rs485_2_comm_data_deal
����������rs485_2���ݽ���������
����������
����ֵ	��
********************************************************************************
*/
void rs485_2_comm_data_deal(void)
{
	
}

/*
********************************************************************************
�������ƣ�rs485_2_comm_data_send
����������rs485_2��·�������ݺ���
����������
����ֵ	��
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
�������ƣ�rs485_1_en
����������RS485-1�շ�����
����������recv_or_send: RS485_EN_RECV, RS485_EN_SEND
����ֵ	��
********************************************************************************
*/
void rs485_1_en(uint8_t recv_or_send)
{
	rs485_en_ctrl(RS485_EN1_CTRL, recv_or_send);
}

/*
********************************************************************************
�������ƣ�rs485_2_en
����������RS485-2�շ�����
����������recv_or_send: RS485_EN_RECV, RS485_EN_SEND
����ֵ	��
********************************************************************************
*/
void rs485_2_en(uint8_t recv_or_send)
{
	rs485_en_ctrl(RS485_EN2_CTRL, recv_or_send);
}

/*
********************************************************************************
�������ƣ�TIM3_PWM_init
����������TIM3-CH4 PWM������ã���������ʱ��ͣ��ʱ���������������ú�������
����������
����ֵ	��
********************************************************************************
*/
void TIM3_PWM_init(uint16_t period, uint16_t pulse)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE); 	//ʹ��GPIOCʱ��
	//CLKOUT ʱ�������Ӧ����PC9 TIM3_CH4_PWM		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);					//��ʼ��GPIO	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_0);	
	
	/* TIM3 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  	//ʹ��TIM3ʱ��	
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
	TIM3->CR1 |= TIM_CR1_CEN;								//ɶʱ���������64K��������Ŀǰ�ϵ����
}

/*
********************************************************************************
�������ƣ�usart2_init
����������usart2��ʼ��,RS485-1
����������
����ֵ	��
********************************************************************************
*/
void usart2_init(uint32_t br)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE); 	//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//ʹ��USART2ʱ��
	//����2���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1); 		//GPIOA2����ΪGPIO_AF_1   
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1); 		//GPIOA3����ΪGPIO_AF_1
	//USART2    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 	//GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 			//����
	GPIO_Init(GPIOA,&GPIO_InitStructure); 					//��ʼ��PA2��PA3
 	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = br;				//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;		//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); 				//��ʼ������2
	USART_Cmd(USART2, ENABLE);  							//ʹ�ܴ��� 2
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			//���������ж�
	//Usart2 NVIC ����
  	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;    		//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStruct);							//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

/*
********************************************************************************
�������ƣ�usart3_init
����������usart3��ʼ����RS485-2��CLKOUT
����������
����ֵ	��
********************************************************************************
*/
void usart3_init(uint32_t br)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE); 	//ʹ��GPIOCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	//ʹ��USART3ʱ��
	//����3���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource4,GPIO_AF_1); 		//GPIOC4����ΪGPIO_AF_1  
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource5,GPIO_AF_1); 		//GPIOC5����ΪGPIO_AF_1
	//USART3    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; 	//GPIOC4��GPIOC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;		//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 			//����
	GPIO_Init(GPIOC,&GPIO_InitStructure); 					//��ʼ��PC4��PC5
	
	USART_InitStructure.USART_BaudRate = br;				//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;		//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART3, &USART_InitStructure); 				//��ʼ������3
	USART_Cmd(USART3, ENABLE);  							//ʹ�ܴ��� 3
	USART_ClearFlag(USART3, USART_FLAG_TC);
}

