/*
********************************************************************************
�ļ���	��mcb_ru.c
�ļ����������ذ�Զ������ģ��
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __MCB_RU_GLOBAL
#include "mcb_ru.h"    
#include "crc.h"  
#include "ymodem.h"     
#include "flash.h"           


/*
********************************************************************************
�������ƣ�mcb_ru_init
�������������ذ�Զ��������ʼ��
����������
����ֵ	��
********************************************************************************
*/
void mcb_ru_init(void) 
{

	mcb_ru_task_flag = 0;
	ymodem_init();
}

/*
********************************************************************************
�������ƣ�mcb_ru_task
�������������ذ�Զ����������
����������
����ֵ	��
********************************************************************************
*/
void mcb_ru_task(void) 
{
	mcb_ru_task_flag = 0;

}