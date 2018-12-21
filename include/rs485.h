/*
********************************************************************************
文件名	：rs485.h
文件描述：RS485头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __RS485_H
#define __RS485_H

#ifdef __RS485_GLOBAL
	#define __RS485_EXE 
#else
	#define __RS485_EXE extern
#endif

#ifdef __cplusplus
	extern "C" {
#endif 

/*
********************************************************************************
头文件
********************************************************************************
*/
#include "stm32f0xx.h"

/*
********************************************************************************
宏定义
********************************************************************************
*/
#define	RS485_BR				9600
#define RS485_EN_RECV			0
#define RS485_EN_SEND			1

#define RS485_COMM_RX_BUF_LEN	100
#define RS485_COMM_TX_BUF_LEN	100

#define CLK_OUT_64K_PERIOD		750			//48000000 / 64000 = 750
#define CLK_OUT_64K_PUSLE		375			//CLK_OUT_64K_PERIOD / 2 = 375  50%duty

/*
********************************************************************************
公共变量定义
********************************************************************************
*/
//通信结构体定义
typedef struct
{
	uint8_t	rx_cnt;
	uint8_t	rx_len;
	uint8_t	tx_cnt;
	uint8_t	tx_len;	
	uint8_t	rx[RS485_COMM_RX_BUF_LEN];
	uint8_t	tx[RS485_COMM_TX_BUF_LEN];
	uint8_t comm_state;
}RS485_COMM_TYPE_DEF;

//mcb_pes_task_flag = 1;								//RS485隶属于动环监控任务
/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void rs485_init(void);									//RS485初始化
void rs485_1_schedule(void);							//RS485-1调度
void rs485_2_schedule(void);							//RS485-2调度

__RS485_EXE uint16_t rs485_2_clk_out_period;			//rs485_2 CLK OUT周期
__RS485_EXE uint16_t rs485_2_clk_out_pulse;				//rs485_2 CLK OUT脉冲时间
/*
********************************************************************************
私有变量定义
********************************************************************************
*/
__RS485_EXE RS485_COMM_TYPE_DEF rs485_1_comm_data;		//rs485-1通信结构体变量定义
__RS485_EXE RS485_COMM_TYPE_DEF rs485_2_comm_data;		//rs485-2通信结构体变量定义
__RS485_EXE uint8_t rs485_1_comm_recv_end_flag;			//rs485_1接收完成标志
__RS485_EXE uint8_t rs485_1_comm_link_tx_flag;			//rs485_1链路发送标志
__RS485_EXE uint8_t rs485_1_comm_data_deal_flag;		//rs485_1数据处理中标志
__RS485_EXE uint8_t rs485_2_comm_recv_end_flag;			//rs485_2接收完成标志
__RS485_EXE uint8_t rs485_2_comm_link_tx_flag;			//rs485_2链路发送标志
__RS485_EXE uint8_t rs485_2_comm_data_deal_flag;		//rs485_2数据处理中标志
__RS485_EXE uint8_t rs485_2_comm_clk_out_flag;			//rs485_2 CLK OUT标志


/*
********************************************************************************
私有函数定义
********************************************************************************
*/
void usart2_init(uint32_t br);							//usart初始化
void usart3_init(uint32_t br);
void rs485_1_en(uint8_t recv_or_send);					//recv_or_send: RS485_EN_RECV, RS485_EN_SEND
void rs485_2_en(uint8_t recv_or_send);					//同上
void rs485_1_comm_data_deal(void);						//rs485_1数据解析处理函数
void rs485_1_comm_data_send(void);						//rs485_1链路发送数据函数
void rs485_2_comm_data_deal(void);						//rs485_2数据解析处理函数
void rs485_2_comm_data_send(void);						//rs485_2链路发送数据函数
void TIM3_PWM_init(uint16_t period, uint16_t pulse);	//TIM3-CH4 PWM输出配置，重新配置时先停定时器，清计数器，配好后再启动

#ifdef __cplusplus
	}
#endif

#endif
