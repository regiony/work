/*
********************************************************************************
�ļ���	��mcb_wdt.c
�ļ����������ذ忴�Ź�ģ��
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __MCB_WDT_GLOBAL
#include "mcb_wdt.h"                 


/*
********************************************************************************
�������ƣ�mcb_wdt_init
�������������ذ忴�Ź���ʼ��
����������30K 		~  	40K   	~ 	60k
����ֵ  : 2.666s�S  ~	2.0s    ~   1.333S	    IWDG_SetReload(LSI_VALUE/16);
//����ֵ��2.031S  	~	1.523s  ~ 	1.015S		IWDG_SetReload(LSI_VALUE/21);
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
�������ƣ�mcb_wdt_task
�������������ذ忴�Ź�����
����������
����ֵ	��
********************************************************************************
*/
void mcb_wdt_task(void) 
{
	mcb_wdt_task_flag = 0;
    //Reload IWDG counter
    IWDG_ReloadCounter();  
}
