/*
********************************************************************************
�ļ���	��mcb_rfb.h
�ļ����������ذ�����Ƶ��ͨ��ͷ�ļ�����Ƶ���Э��尴�����������������ĵط����ֿ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#ifndef __MCB_RFB_H
#define __MCB_RFB_H

#ifdef __MCB_RFB_GLOBAL
	#define __MCB_RFB_EXE 
#else
	#define __MCB_RFB_EXE extern
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
#define MCB_RFB_NW_BR		57600		//��������������
#define MCB_RFB_RU_BR		1000000		//Զ������������
#define MR_COMM_RX_BUF_LEN	235			//�����ɸ���Э���֡�Ż���RAM����ʱ
#define MR_COMM_TX_BUF_LEN	235			//�����ɸ���Э���֡�Ż���RAM����ʱ
#define MR_BUF_MAX_LEN		230			//��������󳤶�230 

#define MCB_RFB_POLL_TIME	100			//100*10ms=1000ms
#define	HOUR00_MIN00_SECX	10			//ÿ��0��10s���ж���Ч

#define COMM_SYN_CHAR_1		0xAA		//ͬ����12����
#define COMM_SYN_CHAR_2		0x55
#define COMM_OK				0xA5
#define COMM_FAIL			0x5A

#define MR_KYX_MAX_NUM		12			//�������������

#define	RECV_TIME_OUT		0x01		//���ճ�ʱλ����
#define	RECV_CHKSUM_ERR		0x02		//У��ʹ���λ����
#define RECV_TIME_OUT_CNT	50			//50*10ms=500ms
#define RECV_TO_ALLOW_NUM	3			//���ճ�ʱ�����ط�����
#define RF_LINK_FAULT_NUM	9			//��·�����жϴ���

//���������
#define HFBXX_CMD_01		0x01		//�ظ���Ƶ����Ϣ��״̬
#define HFQR_CMD_02			0x02		//�ظ�ȷ�����豸������
#define HFSGSJ_CMD_03		0x03		//�ظ�����˥��ֵ
#define HFBLX_CMD_04		0x04		//�ظ�������
#define HFKYXZZB_CMD_05		0x05		//�ظ������Ǽ��������
#define ZDTZSJCG_CMD_7C		0x7C		//����֪ͨ�����ɹ�
#define HFJRSJQR_CMD_7D		0x7D		//�ظ���������ȷ��
#define ZDTZTCYCSJ_CMD_7E	0x7E		//����֪ͨ�˳�Զ������
#define HFSBWJM_CMD_7F		0x7F		//�ظ��豸�ļ���
//���������
#define QRSBFSSS_CMD_80		0x80		//ȷ���豸���͵���������
#define QQJRTCSDMS_CMD_81	0x81		//��������˳��ֶ�ģʽ
#define QQJRTCZDMS_CMD_82	0x82		//��������˳��Զ�ģʽ
#define QQSBSCGLSZ_CMD_83	0x83		//�����豸�����������
#define QQDQSBSSJZ_CMD_84	0x84		//�����ȡ�豸����˥��ֵ
#define QQSZSBSSJZ_CMD_85	0x85		//���������豸����˥��ֵ
#define QQDQSBBLX_CMD_86	0x86		//�����ȡ�豸������
#define QQKYXZZB_CMD_87		0x87		//��������Ǽ��������
#define QQCXBXXZT_CMD_88	0x88		//�����ѯ����Ϣ״̬
#define QQDQSBWJM_CMD_FE	0xFE		//�����ȡ�豸�ļ���
#define QQJRYCSJ_CMD_FF		0xFF		//�������Զ������

//ͨ�Žṹ�嶨��
typedef struct
{
	uint8_t	rx_cnt;
	uint8_t	rx_len;
	uint8_t	tx_cnt;
	uint8_t	tx_len;	
	uint8_t	rx[MR_COMM_RX_BUF_LEN];
	uint8_t	tx[MR_COMM_TX_BUF_LEN];
	uint8_t comm_state;
}MR_COMM_TYPE_DEF;
//��Ϣ״̬�ṹ�嶨��
typedef struct
{
	uint8_t	cur_mode;		//��ǰģʽ 0-���� 1-�ֶ� 2-�Զ� 4-ͨ���Լ�
	uint8_t	cur_chno;		//��ǰͨ�� 0-1ͨ�� 1-2ͨ�� 2-��·1 3-��·2
	char	e_or_w;			//������
	char	s_or_n;			//�ϱ�γ
	float	longitude;		//����
	float	latitude;		//γ��
	float	altitude;		//����
	uint8_t	ch1_gps_kjx;	//ͨ��1 GPS�ɼ���
	uint8_t	ch1_bd_kjx;		//ͨ��1 BD�ɼ���
	uint8_t	ch1_gps_kyx;	//ͨ��1 GPS������
	uint8_t	ch1_bd_kyx;		//ͨ��1 BD������
	uint8_t	ch2_gps_kjx;	//ͬ��...
	uint8_t	ch2_bd_kjx;
	uint8_t	ch2_gps_kyx;
	uint8_t	ch2_bd_kyx;
	uint8_t	year;			//utc date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;			//utc time...
	uint8_t	min;
	uint8_t	sec;	
	//��Ϣ��״̬��
	uint8_t	state1;			
	//bit7~bit6 ����1    bit5~bit4 ����2   	bit3 ͨ��1    bit2 ͨ��2	bit2 �ɽӵ�1 	bit1 �ɽӵ�2
	//00��ok   01:δ��   00��ok   01:δ��   0��ok         0��ok         0��ok       	0��ok     
	//10���澯 11:����   10���澯 11:����   1���澯       1���澯       1������     	1������  
	//�豸״̬��Ϣ��
	uint8_t	state2;			
	//bit7 ����GPSģ��ͨ�Ų���	bi6~bit0 ����
	//0:ok		
	//1:����
}MR_COMM_MSG_DEF;
//����ʱ��
typedef struct
{
	uint16_t year;			//BEIJING date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;			//BEIJING time...
	uint8_t	min;
	uint8_t	sec;	
}MR_COMM_BT_TIME_DEF;
//�������Ǻż��������
typedef struct
{
	uint8_t CH1_GPS_KYX_CN[MR_KYX_MAX_NUM][2];		//[X][0]: �������Ǻ� ��[X][1]: �����������
	uint8_t CH1_BDS_KYX_CN[MR_KYX_MAX_NUM][2];		//ͬ��...
	uint8_t CH2_GPS_KYX_CN[MR_KYX_MAX_NUM][2];
	uint8_t CH2_BDS_KYX_CN[MR_KYX_MAX_NUM][2];
}MR_COMM_KYX_CN_DEF;

/*
********************************************************************************
������������
********************************************************************************
*/
__MCB_RFB_EXE uint32_t mcb_rfb_task_flag;		//���ذ�����Ƶ��ͨ�������־

__MCB_RFB_EXE MR_COMM_MSG_DEF mr_comm_msg;		//��Ϣ��״̬�ṹ�����
__MCB_RFB_EXE MR_COMM_BT_TIME_DEF mr_comm_bt_time;	//����ʱ��ṹ�����

__MCB_RFB_EXE uint8_t mr_gps_or_bd;				//��ȡGPS����bd���ǿ����Ǻż�����ȱ��� 0 - gps��1 - bd
__MCB_RFB_EXE uint8_t mr_channel_no;			//��ȡ��Ƶ���ĸ�ͨ�������ǿ����Ǻż�����ȱ��� 1 - ͨ��1�� 2 - ͨ��2
__MCB_RFB_EXE MR_COMM_KYX_CN_DEF mr_kyx_cn_array;	//�������Ǻż��������

__MCB_RFB_EXE uint8_t mr_ch_att_sel;			//����ͨ��ѡ��, 			bit7~bit3��	bit2 	 		bit1  			bit0
												//Ϊ1��ʾ���ô�ͨ����˥��ֵ	����		1������ͨ��		1��ͨ��2		1��ͨ��1
__MCB_RFB_EXE uint8_t mr_ch1_att;				//ͨ��1˥��ֵ0~30dB
__MCB_RFB_EXE uint8_t mr_ch2_att;				//ͨ��2˥��ֵ0~30dB
__MCB_RFB_EXE uint8_t mr_com_att;				//����ͨ��˥��ֵ0~60dB

__MCB_RFB_EXE uint8_t  mr_in_out_auto_mode;		//�����˳��Զ�ģʽ 0-�˳���1-�����Զ�ģʽ
__MCB_RFB_EXE uint8_t  mr_in_out_manu_mode;		//�����˳��ֶ�ģʽ 0-�˳���1-ͨ��1�����ֶ�ģʽ��2-ͨ��2�����ֶ�ģʽ��3-��·1�����ֶ�ģʽ��4-��·2�����ֶ�ģʽ
#ifdef __MCB_RFB_GLOBAL
	char rfb_xybm_file[13] = "RFB_1.0_1.0";		//��Ƶ���Э���mcu�ļ���
	char rfb_xybf_file[13] = "XYBF_1.0_1.0";	//Э���fpga�ļ���
#else
	extern char rfb_xybm_file[13];
	extern char rfb_xybf_file[13];
	
#endif
__MCB_RFB_EXE uint8_t rfb_xybm_xybf_sel;		//1-��Ƶ�壬2-Э���MCU, 3-Э���FPGA
__MCB_RFB_EXE uint8_t rfb_xyb_in_out_ru_flag;	//���롢�˳�Զ��������־��0x01-��Ƶ�壬0x02-Э���MCU, 0x04-Э���FPGA
__MCB_RFB_EXE uint8_t rfb_xyb_ru_succ_flag;		//Զ�������ɹ���־��0x01-��Ƶ�壬0x02-Э���MCU, 0x04-Э���FPGA
/*
********************************************************************************
������������
********************************************************************************
*/
void mcb_rfb_init(void);						//���ذ�����Ƶ��ͨ�ų�ʼ��
void mcb_rfb_task(void);						//���ذ�����Ƶ��ͨ������

void UTCToBeijing(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second); //GPS UTCʱ��ת����ʱ��
/*
********************************************************************************
˽�б�������
********************************************************************************
*/
__MCB_RFB_EXE MR_COMM_TYPE_DEF mr_comm_data;	//ͨ�Žṹ�����
__MCB_RFB_EXE uint8_t mr_comm_recv_end_flag;	//������ɱ�־
__MCB_RFB_EXE uint8_t mr_comm_link_tx_flag;		//��·���ͱ�־
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_flag;		//ϵͳ���ͱ�־
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_to_flag;	//ϵͳ���ͳ�ʱ��־
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_cmd;		//ϵͳ�������������
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_cmd1;		//ϵͳ���Ͷ�ȡ�豸��Ϣ��ѯ����
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_cmd2;		//ϵͳ�����������á���ȡ����
__MCB_RFB_EXE uint8_t mr_comm_data_deal_flag;	//���ݴ����б�־
__MCB_RFB_EXE uint8_t mr_comm_time_out_cnt;		//ϵͳ���ͳ�ʱ������
__MCB_RFB_EXE uint8_t mr_comm_time_out_en;		//ϵͳ���ͳ�ʱ�������־
__MCB_RFB_EXE uint8_t mr_comm_send_recv_end;	//һ�η������������־����������֡����
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_end;		//��·�������
__MCB_RFB_EXE uint8_t mr_to_cnt1;				//��ʱ������1
__MCB_RFB_EXE uint8_t mr_to_cnt2;				//��ʱ������2

/*
********************************************************************************
˽�к�������
********************************************************************************
*/
void UART6_init(uint32_t br);					//����6��ʼ������
void mr_comm_data_deal(void);					//���ݽ���������
void mr_comm_data_send(void);					//��·�������ݺ���
void mr_comm_sys_send(uint8_t cmd);				//ϵͳ�������ݺ���

//����
void cmd_88_deal(void);							//����������֡����
void cmd_87_deal(void);							//ͬ��...
void cmd_86_deal(void);
void cmd_85_deal(void);
void cmd_84_deal(void);
void cmd_83_deal(void);
void cmd_82_deal(void);
void cmd_81_deal(void);   
void cmd_80_deal(uint8_t type, uint8_t flag);
void cmd_FE_deal(void);
void cmd_FF_deal(void);

//����
void cmd_01_deal(void);							//���������֡����
void cmd_02_deal(void);							//ͬ��...
void cmd_03_deal(void);
void cmd_04_deal(void);
void cmd_05_deal(void);
void cmd_7C_deal(void);
void cmd_7D_deal(void);
void cmd_7E_deal(void);
void cmd_7F_deal(void);


#ifdef __cplusplus
	}
#endif

#endif
