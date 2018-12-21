/*
********************************************************************************
�ļ���	��mcb_pd8.h
�ļ����������ذ���˹��ְ�ͨ��ͷ�ļ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#ifndef __MCB_PD8_H
#define __MCB_PD8_H

#ifdef __MCB_PD8_GLOBAL
	#define __MCB_PD8_EXE 
#else
	#define __MCB_PD8_EXE extern
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
#define MCB_PD8_NW_BR			115200		//��������������
#define MCB_PD8_RU_BR			1000000		//Զ������������
#define MP_COMM_RX_BUF_LEN		235			//�����ɸ���Э���֡�Ż���RAM����ʱ
#define MP_COMM_TX_BUF_LEN		235			//�����ɸ���Э���֡�Ż���RAM����ʱ
#define MP_BUF_MAX_LEN			230			//��������󳤶�230

#define MCB_MP_POLL_TIME		110			//110*10ms=1100ms

#define MP_COMM_SYN_CHAR_1		0xAA		//ͬ����12����
#define MP_COMM_SYN_CHAR_2		0x55
#define MP_COMM_OK				0xA5
#define MP_COMM_FAIL			0x5A

#define MP_KYX_MAX_NUM			12			//�������������
#define MP_SPD8_MAX_NUM			8			//һ����������8���ӻ�

#define	MP_RECV_TIME_OUT		0x01		//���ճ�ʱλ����
#define	MP_RECV_CHKSUM_ERR		0x02		//У��ʹ���λ����
#define MP_RECV_TIME_OUT_CNT	50			//50*10ms=500ms
#define MP_RECV_TO_ALLOW_NUM	3			//���ճ�ʱ�����ط�����
#define PD8_LINK_FAULT_NUM		9			//��·�����жϴ���

//���������
#define MP_HFZCJBXX_CMD_01		0x01		//�ظ������ӻ�����Ϣ��״̬
#define MP_HFQR_CMD_02			0x02		//�ظ�ȷ�����豸������
#define MP_HFLGSJ_CMD_03		0x03		//�ظ�2��˥��ֵ
#define MP_HFKYXZZB_CMD_05		0x05		//�ظ������Ǽ��������
#define MP_ZDTZSJCG_CMD_7C		0x7C		//����֪ͨ�����ɹ�
#define MP_HFJRSJQR_CMD_7D		0x7D		//�ظ���������ȷ��
#define MP_ZDTZTCYCSJ_CMD_7E	0x7E		//����֪ͨ�˳�Զ������
#define MP_HFSBWJM_CMD_7F		0x7F		//�ظ��豸�ļ���
//���������                          	
#define MP_QRSBFSSS_CMD_80		0x80		//ȷ���豸���͵���������
#define MP_QQSBSCGLSZ_CMD_83	0x83		//�����豸�����������
#define MP_QQDQSBLSJZ_CMD_84	0x84		//�����ȡ�豸2��˥��ֵ
#define MP_QQSZSBLSJZ_CMD_85	0x85		//���������豸2��˥��ֵ
#define MP_QQKYXZZB_CMD_87		0x87		//��������Ǽ��������
#define MP_QQCXBXXZT_CMD_88		0x88		//�����ѯ����Ϣ״̬
#define MP_QQDQSBWJM_CMD_FE		0xFE		//�����ȡ�豸�ļ���
#define MP_QQJRYCSJ_CMD_FF		0xFF		//�������Զ������

//ͨ�Žṹ�嶨��
typedef struct
{
	uint8_t	rx_cnt;
	uint8_t	rx_len;
	uint8_t	tx_cnt;
	uint8_t	tx_len;	
	uint8_t	rx[MP_COMM_RX_BUF_LEN];
	uint8_t	tx[MP_COMM_TX_BUF_LEN];
	uint8_t comm_state;
}MP_COMM_TYPE_DEF;
//�����˹��ְ���Ϣ״̬�ṹ�嶨��
typedef struct
{
	uint8_t	mpd8_gps_kjx;		//GPS�ɼ���
	uint8_t	mpd8_bd_kjx;		//BD�ɼ���
	uint8_t	mpd8_gps_kyx;		//GPS������
	uint8_t	mpd8_bd_kyx;		//BD������
	uint8_t	mpd8_port_state1;	//�˿�״̬1, port8~port5����λһ��״̬��00�����գ�01���ӻ���10��BBU��11������
	uint8_t	mpd8_port_state2;	//�˿�״̬2, port4~port1����λһ��״̬��00�����գ�01���ӻ���10��BBU��11������
	uint8_t	year;				//utc date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;				//utc time...
	uint8_t	min;
	uint8_t	sec;	
	//��Ϣ��״̬��
	uint8_t	mpd8_pwr_state;		//���ӵ�Դ״̬��bit7~bit4��������bit3 		bit2 		bit1 			bit0
								//0��������1���澯				 pwr1״̬	pwr2״̬	pwr1�ɽӵ�3    	pwr2�ɽӵ�4
	//�豸״̬��Ϣ��
	uint8_t	mpd8_out_state;		//�����˹��ְ����״̬��port8 ~ port1, bit7 ~ bit0, 0��������1���澯	
	uint8_t	mpd8_alm_state;		//�����˹��ְ�ɽڵ�1�澯״̬��port8 ~ port1 �� bit7 ~ bit0	0��������1���ɽӵ�1�澯	
	uint8_t	mpd8_dev_state;		//bit7 ����GPSģ��ͨ�Ų��� 0��OK, 1�����ϣ�bi6~bit0 ����
}MPD8_COMM_MSG_DEF;
//�ӻ��˹��ְ���Ϣ״̬�ṹ�嶨��
typedef struct
{
	uint8_t	spd8_addr;			//�ӻ���ַ
	uint8_t	spd8_gps_kjx;		//GPS�ɼ���
	uint8_t	spd8_bd_kjx;		//BD�ɼ���
	uint8_t	spd8_gps_kyx;		//GPS������
	uint8_t	spd8_bd_kyx;		//BD������
	uint8_t	spd8_port_state;	//�˿�״̬, port8~port1��bit7~bit0��0�����գ�1��BBU
	uint8_t	year;				//utc date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;				//utc time...
	uint8_t	min;
	uint8_t	sec;	
	//�豸״̬��Ϣ��
	uint8_t	spd8_out_state;		//�ӻ��˹��ְ����״̬��port8 ~ port1, bit7 ~ bit0,	 0��������1���澯	
	uint8_t	spd8_alm_state;		//�ӻ��˹��ְ�ɽڵ�1�澯״̬��port8 ~ port1, bit7 ~ bit0, 0��������1���ɽӵ�1�澯	
	uint8_t	spd8_dev_state;		//bit7 ����GPSģ��ͨ�Ų��� 0��OK, 1�����ϣ�bi6~bit0 ����
}SPD8_COMM_MSG_DEF;
//����ʱ��
typedef struct
{
	uint16_t year;				//BEIJING date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;				//BEIJING time...
	uint8_t	min;
	uint8_t	sec;	
}MP_COMM_BT_TIME_DEF;
//�������Ǻż��������
typedef struct
{
	uint8_t GPS_KYX_CN[MP_KYX_MAX_NUM][2];		//[X][0]: �������Ǻ� ��[X][1]: �����������
	uint8_t BDS_KYX_CN[MP_KYX_MAX_NUM][2];		//ͬ��...
}MP_COMM_KYX_CN_DEF;

/*
********************************************************************************
������������
********************************************************************************
*/
__MCB_PD8_EXE uint32_t mcb_pd8_task_flag;			//���ذ���˹��ְ�ͨ�������־
__MCB_PD8_EXE MPD8_COMM_MSG_DEF mp_mpd8_comm_msg;	//������Ϣ��״̬�ṹ�����
__MCB_PD8_EXE MP_COMM_BT_TIME_DEF mp_comm_bt_time;	//����ʱ��ṹ�����

__MCB_PD8_EXE uint8_t mcb_cur_rw_slave_index;		//��ǰ��д�Ĵӻ������� = ���ӻ���ַ - 1�����ӻ���ַ������1~8֮��
__MCB_PD8_EXE uint8_t mcb_cur_valid_slave_addr[MP_SPD8_MAX_NUM];	//��ǰ��Ч�ӻ���ַ�б�������Ҫ��̬�����£����ڿ��ٲ��ҵ�����Ϣ���ʹӻ��������ʹ��
__MCB_PD8_EXE uint8_t mcb_cur_pd8_slave_num;		//��ǰͳ�ƵĴӻ��˹��ְ�����0~8��ֻ�Ǹ�������������ʵ�ʲ�����//ʵ�ʲ���������8���ӻ���ַ�����в�ѯ��Ч��ַ
//ע���N���ӻ�����Ϣ�Ͷ�Ӧ���´ӻ���������Ԫ��[N-1]����Ч�ӻ���ַ1~8��0��Ч��ֻҪ֪���ӻ���N�Ϳ���������������ֱ��ȡ����Ӧ��Ϣ
//���������ĵ�2���͵�5���ڷֱ����2���ӻ�����ô��������������ӻ��ĵ�ַ�ֱ���2,5���ӻ�����Ϣ�ͷֱ�洢���������������[2-1]��[5-1]Ԫ����
//�������õ����������Ч������һ���ӻ���һ�����ݱ�����Ч��ȥ��һ���ӻ�����Ӧ�����ݱ�����Ч
//��Ҫ�Ǵӻ���������ֵ�������ʹӻ���ַ����ֵ�ı仯��Ҫô�ӻ���ַ��Ҫô0��Ч��ַ����ʵ�ʳ�����Ҫ��ѯ �ӻ��˹��ְ��ַ����[] ��֪����Щ��Ч,����ͨ������
__MCB_PD8_EXE uint8_t mcb_pd8_slave_addr[MP_SPD8_MAX_NUM];	//�ӻ��˹��ְ��ַ���飬��Ч�ӻ���ַ1~8��0��Ч��ַ
__MCB_PD8_EXE SPD8_COMM_MSG_DEF mp_spd8_comm_msg[MP_SPD8_MAX_NUM];	//�ӻ���Ϣ��״̬�ṹ�����������[0]~[7]�˸��ӻ���Ϣ��Ӧ�ӻ���ַ1~8
__MCB_PD8_EXE MP_COMM_KYX_CN_DEF spd8_kyx_cn_array[MP_SPD8_MAX_NUM];//�ӻ��������Ǻż�������Ƚṹ�����飬[0]~[7]�˸��ӻ������Ǽ�������ȶ�Ӧ�ӻ���ַ1~8
#ifdef __MCB_PD8_GLOBAL
	char spd8_file[MP_SPD8_MAX_NUM][13] =			//�ӻ��˹��ְ��ļ���
	{
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
	};			
#else
	extern char spd8_file[MP_SPD8_MAX_NUM][13];
#endif

#ifdef __MCB_PD8_GLOBAL
	char mpd8_file[13] = "PD8M_1.0_1.0";			//�����˹��ְ��ļ���
#else
	extern char mpd8_file[13];
#endif

__MCB_PD8_EXE uint8_t mp_mpd8_or_spd8;				//��ȡ�������Ǵӻ���0-������1-�ӻ�
__MCB_PD8_EXE uint8_t mp_gps_or_bd;					//��ȡGPS����bd���ǿ����Ǻż�����ȱ��� 0 - gps��1 - bd
__MCB_PD8_EXE MP_COMM_KYX_CN_DEF mpd8_kyx_cn_array;	//�����������Ǻż�������Ƚṹ��

__MCB_PD8_EXE uint8_t mp_ch_att_sel;				//����ͨ��ѡ��, 			bit7~bit2��		bit1  			bit0
													//Ϊ1��ʾ���ô�ͨ����˥��ֵ	����	   		1�����ͨ��		1�����ͨ��
__MCB_PD8_EXE uint8_t mp_det_att;					//���ͨ��˥��ֵ0~30dB
__MCB_PD8_EXE uint8_t mp_out_att;					//���ͨ��˥��ֵ0~30dB

__MCB_PD8_EXE uint8_t mp_pd8_sel;					//4-�����˹��ְ壬5-�ӻ��˹��ְ�, 6-�����ӻ��˹��ְ�
__MCB_PD8_EXE uint8_t mp_pd8_in_out_ru_flag;		//���롢0�˳�Զ��������־��0x01-�����˹��ְ壬0x02-�ӻ��˹��ְ�, 0x04-�����ӻ��˹��ְ�
__MCB_PD8_EXE uint8_t mp_pd8_ru_succ_flag;			//Զ�������ɹ���־��0x01-�����˹��ְ壬0x02-�ӻ��˹��ְ�, 0x04-�����ӻ��˹��ְ�

/*
********************************************************************************
������������
********************************************************************************
*/
void mcb_pd8_init(void);							//���ذ���˹��ְ�ͨ�ų�ʼ��
void mcb_pd8_task(void);							//���ذ���˹��ְ�ͨ������

/*
********************************************************************************
˽�б�������
********************************************************************************
*/
__MCB_PD8_EXE MP_COMM_TYPE_DEF mp_comm_data;		//ͨ�Žṹ�����
__MCB_PD8_EXE uint8_t mp_comm_recv_end_flag;		//������ɱ�־
__MCB_PD8_EXE uint8_t mp_comm_link_tx_flag;			//��·���ͱ�־
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_flag;			//ϵͳ���ͱ�־
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_to_flag;		//ϵͳ���ͳ�ʱ��־
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_cmd;			//ϵͳ�������������
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_cmd1;			//ϵͳ���Ͷ�ȡ�豸��Ϣ��ѯ����
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_cmd2;			//ϵͳ�����������á���ȡ����
__MCB_PD8_EXE uint8_t mp_comm_data_deal_flag;		//���ݴ����б�־
__MCB_PD8_EXE uint8_t mp_comm_time_out_cnt;			//ϵͳ���ͳ�ʱ������
__MCB_PD8_EXE uint8_t mp_comm_time_out_en;			//ϵͳ���ͳ�ʱ�������־
__MCB_PD8_EXE uint8_t mp_comm_send_recv_end;		//һ�η������������־����������֡����
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_end;			//��·�������
__MCB_PD8_EXE uint8_t mp_to_cnt1;					//��ʱ������1
__MCB_PD8_EXE uint8_t mp_to_cnt2;					//��ʱ������2

/*
********************************************************************************
˽�к�������
********************************************************************************
*/
void UART5_init(uint32_t br);						//����5��ʼ������
void mp_comm_data_deal(void);						//���ݽ���������
void mp_comm_data_send(void);						//�������ݺ���
void mp_comm_sys_send(uint8_t cmd);					//ϵͳ�������ݺ���

//����
void mp_cmd_88_deal(void);							//����������֡����
void mp_cmd_87_deal(void);							//ͬ��...
void mp_cmd_85_deal(void);
void mp_cmd_84_deal(void);
void mp_cmd_83_deal(void);
void mp_cmd_80_deal(uint8_t type, uint8_t flag);
void mp_cmd_FE_deal(void);
void mp_cmd_FF_deal(void);

//����
void mp_cmd_01_deal(void);							//���������֡����
void mp_cmd_02_deal(void);							//ͬ��...
void mp_cmd_03_deal(void);
void mp_cmd_05_deal(void);
void mp_cmd_7C_deal(void);
void mp_cmd_7D_deal(void);
void mp_cmd_7E_deal(void);
void mp_cmd_7F_deal(void);

void MP_UTCToBeijing(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

#ifdef __cplusplus
	}
#endif

#endif
