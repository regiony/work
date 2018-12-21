/*
********************************************************************************
文件名	：mcb_wdt.c
文件描述：主控板看门狗模块
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __MCB_WDT_GLOBAL
#include "mcb_wdt.h"                 


/*
********************************************************************************
函数名称：mcb_wdt_init
函数描述：主控板看门狗初始化
参数描述：30K 		~  	40K   	~ 	60k
返回值  : 2.666s  ~	2.0s    ~   1.333S	    IWDG_SetReload(LSI_VALUE/16);
//返回值：2.031S  	~	1.523s  ~ 	1.015S		IWDG_SetReload(LSI_VALUE/21);
********************************************************************************
*/
void mcb_wdt_init(void) 
{
//
  	// Enable write access to IWDG_PR and IWDG_RLR registers
  	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  	// IWDG counter clock: LSI/32
  	IWDG_SetPrescaler(IWDG_Prescaler_32);
  	
	//Set counter reload value to obtain 1500ms IWDG TimeOut.
    // Counter Reload Value = 2000ms/IWDG counter clock period
    //                     = 2000ms / (LSI/32)
    //                     = 2s / (LsiFreq/32)
    //                     = LsiFreq/(32 / 2)
    //                     = LsiFreq/16
  	//
  	IWDG_SetReload(LSI_VALUE/16);

  	//Reload IWDG counter
  	IWDG_ReloadCounter();

  	//Enable IWDG (the LSI oscillator will be enabled by hardware)
 	IWDG_Enable();  	
//
	mcb_wdt_task_flag = 0;
}

/*
********************************************************************************
函数名称：mcb_wdt_task
函数描述：主控板看门狗任务
参数描述：
返回值	：
********************************************************************************
*/
void mcb_wdt_task(void) 
{
	mcb_wdt_task_flag = 0;
    //Reload IWDG counter
    IWDG_ReloadCounter();  
}
