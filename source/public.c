/*
********************************************************************************
�ļ���	��public.c
�ļ�������ϵͳ����ģ��
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __PUBLIC_GLOBAL
#include "public.h"                 


/*
********************************************************************************
�������ƣ�public_init
��������������ģ���ʼ������
����������
����ֵ	��
********************************************************************************
*/
void public_init(void) 
{
	rfb_or_xyb = RFB;				//��ֵ����Ƶ��
	dev_type = DEV_TYPE_1;			//��ֵ��1�Ż�
	hw_ver21 = 0;
	
	TIM14_init(TIM14_PERIOD, TIM14_PSC); //���Ķ�ʱ����ʼ��
	pca9555_init();					//IO��չоƬ��ʼ��
	dev_rfxy_io_init();
	
	get_rfb_xyb();					//��ȡ�����Ƶ�廹��Э���
	get_mcb_file();					//��ȡ���ذ��ļ���
	get_dev_type();					//��ȡ�豸����
    
    //dev_type = DEV_TYPE_1;	        //for test
}

/*
********************************************************************************
��������: sys_sw_reset
����������ϵͳ��λ
����������
����ֵ	��
********************************************************************************
*/
void sys_sw_reset(void)
{
    __disable_irq();				//�ر����жϣ�
    NVIC_SystemReset();
}

/*
********************************************************************************
��������: get_rfb_xyb
������������ȡ�����Ƶ�廹��Э���
����������
����ֵ	��
********************************************************************************
*/
void get_rfb_xyb(void)			
{
	if(GPIO_RFB_XYB_PIN)
		rfb_or_xyb = RFB;	
	else
		rfb_or_xyb = XYB;	
}

/*
********************************************************************************
��������: get_mcb_file
������������ȡ���ذ��ļ��������ͷ�ļ��ֶ����壬Ӳ���Զ���ȡ
����������
����ֵ	��
********************************************************************************
*/
void get_mcb_file(void)			
{ 
	uint8_t i;
	
	//��ȡӲ���汾��
	rd_hw_ver21(&hw_ver21);
	hw_ver21 &= HW_VER_21; 
	hw_ver21 >>= 5;
	//hw_ver21 &= 0x03;			//reliable deal
	
	if(hw_ver21 == 0x00) 
		mcb_hw_ver[2] = '0';		
	else
	if(hw_ver21 == 0x01) 
		mcb_hw_ver[2] = '1';	
	else	
	if(hw_ver21 == 0x02) 
		mcb_hw_ver[2] = '2';	
	else	
	if(hw_ver21 == 0x03) 
		mcb_hw_ver[2] = '3';	
	
	//��ȡ���ذ��ļ���	
	for(i = 0; i < 3; i++)		//add HW_VER
		mcb_file[i + 4] = mcb_hw_ver[i];
	for(i = 0; i < 3; i++)		//add SW_VER
		mcb_file[i + 8] = mcb_sw_ver[i];
}

/*
********************************************************************************
��������: get_dev_type
������������ȡ�豸���ͣ�Ӳ��GPIO������
����������
����ֵ	��
********************************************************************************
*/
void get_dev_type(void)	
{
	if(GPIO_DEV_TYPE_PIN)
		dev_type = DEV_TYPE_2;	
	else
		dev_type = DEV_TYPE_1;		
}

/*
********************************************************************************
�������ƣ�TIM14_init
����������TIM14��ʼ����10ms���Ķ�ʱ��
����������
����ֵ	��
********************************************************************************
*/	
void TIM14_init(uint16_t period, uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM14_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  	//ʹ��TIM14ʱ��
	
  	TIM14_TimeBaseInitStructure.TIM_Period = period; 		//�Զ���װ��ֵ
	TIM14_TimeBaseInitStructure.TIM_Prescaler = prescaler;  //��ʱ����Ƶ
	TIM14_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM14_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM14,&TIM14_TimeBaseInitStructure);	//��ʼ��TIM14
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM14_IRQn; 			//��ʱ��14�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority=0x03; 		//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE); 				//����ʱ��14�����ж�
	//TIM_Cmd(TIM14,DISABLE); 								//ʹ�ܶ�ʱ��14
}

/*
********************************************************************************
�������ƣ�dev_rfxy_io_init
�����������豸��������IO��ʼ��
����������
����ֵ	��
********************************************************************************
*/
void dev_rfxy_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
	
	/* ����GPIO�ܽŸ��� PC10 BEEP OUT*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

/*
********************************************************************************
�������ƣ�delay_xus
���������������ʱxus����������ˮ��ѭ����11�����ڣ���ʵ��
����������
����ֵ	��
********************************************************************************
*/
void delay_xus(uint32_t val)
{
	volatile uint32_t cnt;
	
	while(val--)
	{
		for(cnt = 0; cnt < 4; cnt++);
	}	
}

/*
********************************************************************************
�������ƣ�delay_xms
���������������ʱxms����������ˮ��ѭ����11�����ڣ���ʵ��
����������
����ֵ	��
********************************************************************************
*/
void delay_xms(uint32_t val)
{
	volatile uint32_t cnt;
	
	while(val--)
	{
		for(cnt = 0; cnt < 4363; cnt++);
	}		
}
