/*
********************************************************************************
文件名	：ymodem.c
文件描述：Y-MODEM通信协议
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __YMODEM_GLOBAL
#include "ymodem.h"                 


/*
********************************************************************************
函数名称：ymodem_init
函数描述：Y-MODEM初始化函数
参数描述：
返回值	：
********************************************************************************
*/
void ymodem_init(void)
{
	uint32_t i;
	uint8_t *pt;

	pt = (uint8_t *)(&ymodem_comm_msg.rx_cnt);
	for(i = 0; i < sizeof(YMODEM_COMM_TYPE_DEF); i++)
		*pt = 0;
	
}

