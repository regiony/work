/*
********************************************************************************
文件名	：public.c
文件描述：系统公共模块
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __PUBLIC_GLOBAL
#include "public.h"                 


/*
********************************************************************************
函数名称：public_init
函数描述：公共模块初始化函数
参数描述：
返回值	：
********************************************************************************
*/
void public_init(void) 
{
	rfb_or_xyb = RFB;				//初值是射频板
	dev_type = DEV_TYPE_1;			//初值是1号机
	hw_ver21 = 0;
	
	TIM14_init(TIM14_PERIOD, TIM14_PSC); //节拍定时器初始化
	pca9555_init();					//IO扩展芯片初始化
	dev_rfxy_io_init();
	
	get_rfb_xyb();					//获取外接射频板还是协议板
	get_mcb_file();					//获取主控板文件名
	get_dev_type();					//获取设备机型
    
    //dev_type = DEV_TYPE_1;	        //for test
}

/*
********************************************************************************
函数名称: sys_sw_reset
函数描述：系统软复位
参数描述：
返回值	：
********************************************************************************
*/
void sys_sw_reset(void)
{
    __disable_irq();				//关闭总中断？
    NVIC_SystemReset();
}

/*
********************************************************************************
函数名称: get_rfb_xyb
函数描述：获取外接射频板还是协议板
参数描述：
返回值	：
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
函数名称: get_mcb_file
函数描述：获取主控板文件名，软件头文件手动定义，硬件自动获取
参数描述：
返回值	：
********************************************************************************
*/
void get_mcb_file(void)			
{ 
	uint8_t i;
	
	//获取硬件版本号
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
	
	//获取主控板文件名	
	for(i = 0; i < 3; i++)		//add HW_VER
		mcb_file[i + 4] = mcb_hw_ver[i];
	for(i = 0; i < 3; i++)		//add SW_VER
		mcb_file[i + 8] = mcb_sw_ver[i];
}

/*
********************************************************************************
函数名称: get_dev_type
函数描述：获取设备机型，硬件GPIO读出来
参数描述：
返回值	：
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
函数名称：TIM14_init
函数描述：TIM14初始化，10ms节拍定时器
参数描述：
返回值	：
********************************************************************************
*/	
void TIM14_init(uint16_t period, uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM14_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  	//使能TIM14时钟
	
  	TIM14_TimeBaseInitStructure.TIM_Period = period; 		//自动重装载值
	TIM14_TimeBaseInitStructure.TIM_Prescaler = prescaler;  //定时器分频
	TIM14_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM14_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM14,&TIM14_TimeBaseInitStructure);	//初始化TIM14
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM14_IRQn; 			//定时器14中断
	NVIC_InitStructure.NVIC_IRQChannelPriority=0x03; 		//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE); 				//允许定时器14更新中断
	//TIM_Cmd(TIM14,DISABLE); 								//使能定时器14
}

/*
********************************************************************************
函数名称：dev_rfxy_io_init
函数描述：设备、板类型IO初始化
参数描述：
返回值	：
********************************************************************************
*/
void dev_rfxy_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
	
	/* 配置GPIO管脚复用 PC10 BEEP OUT*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

/*
********************************************************************************
函数名称：delay_xus
函数描述：软件延时xus，不考虑流水线循环体11个周期，待实测
参数描述：
返回值	：
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
函数名称：delay_xms
函数描述：软件延时xms，不考虑流水线循环体11个周期，待实测
参数描述：
返回值	：
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
