/*
********************************************************************************
�ļ���	��rs485.h
�ļ�������RS485ͷ�ļ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
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
ͷ�ļ�
********************************************************************************
*/
#include "stm32f0xx.h"

/*
********************************************************************************
�궨��
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
������������
********************************************************************************
*/
//ͨ�Žṹ�嶨��
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

//mcb_pes_task_flag = 1;								//RS485�����ڶ����������
/*
********************************************************************************
������������
********************************************************************************
*/
void rs485_init(void);									//RS485��ʼ��
void rs485_1_schedule(void);							//RS485-1����
void rs485_2_schedule(void);							//RS485-2����

__RS485_EXE uint16_t rs485_2_clk_out_period;			//rs485_2 CLK OUT����
__RS485_EXE uint16_t rs485_2_clk_out_pulse;				//rs485_2 CLK OUT����ʱ��
/*
********************************************************************************
˽�б�������
********************************************************************************
*/
__RS485_EXE RS485_COMM_TYPE_DEF rs485_1_comm_data;		//rs485-1ͨ�Žṹ���������
__RS485_EXE RS485_COMM_TYPE_DEF rs485_2_comm_data;		//rs485-2ͨ�Žṹ���������
__RS485_EXE uint8_t rs485_1_comm_recv_end_flag;			//rs485_1������ɱ�־
__RS485_EXE uint8_t rs485_1_comm_link_tx_flag;			//rs485_1��·���ͱ�־
__RS485_EXE uint8_t rs485_1_comm_data_deal_flag;		//rs485_1���ݴ����б�־
__RS485_EXE uint8_t rs485_2_comm_recv_end_flag;			//rs485_2������ɱ�־
__RS485_EXE uint8_t rs485_2_comm_link_tx_flag;			//rs485_2��·���ͱ�־
__RS485_EXE uint8_t rs485_2_comm_data_deal_flag;		//rs485_2���ݴ����б�־
__RS485_EXE uint8_t rs485_2_comm_clk_out_flag;			//rs485_2 CLK OUT��־


/*
********************************************************************************
˽�к�������
********************************************************************************
*/
void usart2_init(uint32_t br);							//usart��ʼ��
void usart3_init(uint32_t br);
void rs485_1_en(uint8_t recv_or_send);					//recv_or_send: RS485_EN_RECV, RS485_EN_SEND
void rs485_2_en(uint8_t recv_or_send);					//ͬ��
void rs485_1_comm_data_deal(void);						//rs485_1���ݽ���������
void rs485_1_comm_data_send(void);						//rs485_1��·�������ݺ���
void rs485_2_comm_data_deal(void);						//rs485_2���ݽ���������
void rs485_2_comm_data_send(void);						//rs485_2��·�������ݺ���
void TIM3_PWM_init(uint16_t period, uint16_t pulse);	//TIM3-CH4 PWM������ã���������ʱ��ͣ��ʱ���������������ú�������

#ifdef __cplusplus
	}
#endif

#endif
