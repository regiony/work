/*
********************************************************************************
文件名	：main.c
文件描述：系统主函数，负责系统初始化、自检、调度
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/
#define __MAIN_GLOBAL
#include "main.h"
#include "public.h"                 

#define __DEBUG__					//调试状态宏定义开关

/*
********************************************************************************
函数名称：main
函数描述：系统主函数
参数描述：
返回值	：
********************************************************************************
*/
int main(void) 
{
	__disable_irq();				//关闭总中断
	sys_init();						//系统初始化
	__enable_irq();					//打开总中断
	
	sys_selfchk();					//系统自检
	
	while(1)
	{
		sys_schedule();				//系统调度
	}
}

/*
********************************************************************************
函数名称：sys_init
函数描述：系统初始化函数
参数描述：
返回值	：
********************************************************************************
*/
void sys_init(void)
{
#ifdef __DEBUG__
	DBGMCU_APB1PeriphConfig(DBGMCU_TIM14_STOP, ENABLE);
#endif
	
	public_init();					//各部分初始化
	main_init();
	mcb_alm_init();
	mcb_rfb_init();					//射频板和协议板按兼容软件处理
	mcb_pd8_init();
	mcb_hmu_init();					//射频板和协议板开机画面不同,机型不同显示方案不同
	mcb_wifi_init();
	mcb_pes_init();	
	mcb_ru_init();
	
#ifndef __DEBUG__
	mcb_wdt_init();	
#endif

	TIM14->CR1 |= TIM_CR1_CEN;		//启动节拍定时器
}

/*
********************************************************************************
函数名称：sys_schedule
函数描述：系统调度函数
参数描述：
返回值	：
********************************************************************************
*/
void sys_schedule(void)
{
	if(mcb_rfb_task_flag)
		mcb_rfb_task();				//与射频板和协议板通信任务，按兼容软件处理
	else
	if(mcb_pd8_task_flag)			//与八功分板通信任务
		mcb_pd8_task();
	else
	if(mcb_wifi_task_flag)			//与wifi模块通信任务
		mcb_wifi_task();
	else
	if(mcb_pes_task_flag)			//动环监控通信任务
		mcb_pes_task();
	
	if(mcb_ru_task_flag)			//远程升级任务
		mcb_ru_task();
	
	if(mcb_alm_task_flag)			//报警输出任务
		mcb_alm_task();
		
	if(mcb_hmu_task_flag)			//人机任务
		mcb_hmu_task();	
		
#ifndef __DEBUG__
	if(mcb_wdt_task_flag)			//看门狗任务
		mcb_wdt_task();
#endif
}

/*
********************************************************************************
函数名称：sys_selfchk
函数描述：系统自检函数
参数描述：
返回值	：
********************************************************************************
*/
void sys_selfchk(void)
{
	//有需要时，加入系统需要上电自检的部分
	#ifdef __OLD_MCB_VER__
        uint8_t i;
        for(i = 0; i < 200; i++)
        {
            BEEP_H;
            delay_xus(300);
            BEEP_L;	
            delay_xus(300);	
        }
        
        //lcd_draw_rect(0, 0, 128, 64);
        //lcd_display();
        /*
        LED_1_NORMAL();
        LED_2_NORMAL();
        LED_IO_UPDATE();
        delay_xms(500);	
        
        LED_1_ALARM();
        LED_2_ALARM();
        LED_IO_UPDATE();
        delay_xms(500);	
        
        LED_1_FAULT();    
        LED_3_ALARM();
        LED_IO_UPDATE();
        delay_xms(500);	
        */
        /*
        RELAY_1_ALARM();
        RELAY_2_ALARM();
        RELAY_3_ALARM();
        RELAY_4_ALARM();
        RELAY_IO_UPDATE();
        */
    #else
 		TIM1->CNT = 0;
        TIM1->CR1 |= TIM_CR1_CEN; 
		delay_xms(100);	 
        TIM1->CR1 &= ~TIM_CR1_CEN;   
        TIM1->CNT = 0;        
        
	#endif
}

/*
********************************************************************************
函数名称：main_init
函数描述：main初始化函数
参数描述：
返回值	：
********************************************************************************
*/
void main_init(void)
{ 
	sys_fault_word = 0;
	
}

/*
********************************************************************************
函数名称：sys_fault_deal
函数描述：系统故障处理函数
参数描述：
返回值	：
********************************************************************************
*/
void sys_fault_deal(void)
{
	//sys_fault_word在系统中判断处理，此处故障处理函数不作处理
}
