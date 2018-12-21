/*
********************************************************************************
�ļ���	��mcb_alm.h
�ļ����������ذ屨�����ͷ�ļ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#ifndef __MCB_ALM_H
#define __MCB_ALM_H

#ifdef __MCB_ALM_GLOBAL
	#define __MCB_ALM_EXE 
#else
	#define __MCB_ALM_EXE extern
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
#include "mcb_rfb.h"

/*
********************************************************************************
�궨��
********************************************************************************
*/
#define	__DAN_SE_LED__			//��ɫled��˸�ƣ�˫ɫ����ɫ������Ҳ�����õ�ɫ��

#define	ALARM_CHECK_TIME		20			//20 * 10 = 200ms
//BEEP NEED 2731HZ����
#define TIM1_PERIOD_BEEP		17576		//48000000 / 2731 = 17575.9	��0.366ms��
#define TIM1_PULSE_BEEP			8788

#define BEEP_ALARM_TIME			6000		//6000 * 10 = 60s

#ifdef __OLD_MCB_VER__
	#define BEEP_H				(GPIOC->BSRR = GPIO_Pin_10)		//SET
	#define BEEP_L				(GPIOC->BRR = GPIO_Pin_10)		//RESET
#endif

#define	MCB_ALM_UP_ARRAY_NUM	10			//��������������
#define	MCB_ALM_UP_DATA_NUM		100			//ÿ��������������Ա����

//WARN
#define WARN_DEF_NUM			13
#define WARN_MATA				100			//�����ߣ�����1       
#define WARN_SATA				101         //�����ߣ�����2         
#define WARN_RCH1				102			//��Ƶ��ͨ��1          
#define WARN_RCH2				103         //��Ƶ��ͨ��2   
#define WARN_MAPW				104         //����Դ����Դ1           
#define WARN_SAPW				105         //�ӵ�Դ����Դ2 
#define WARN_RLY1				106         //�̵���1           
#define WARN_RLY2				107         //�̵���2      
#define WARN_RLY3				108         //�̵���3           
#define WARN_RLY4				109         //�̵���4 
#define WARN_PTBD				154         //Э��塢��Ƶ��
#define WARN_OPPT				155         //����˿� �����ӻ��˹��ְ壩  
#define WARN_CTBD				156         //���ư� �����ذ�   
/*
********************************************************************************
������������
********************************************************************************
*/
__MCB_ALM_EXE uint32_t mcb_alm_task_flag;		//������������־
__MCB_ALM_EXE uint8_t mcb_alm_upwifi_flag;		//���������¼�ϴ�WIFI��־
__MCB_ALM_EXE uint8_t mcb_alm_0hour_clr;		//0�������־


/*
********************************************************************************
������������
********************************************************************************
*/
void mcb_alm_init(void);						//���������ʼ��
void mcb_alm_task(void);						//����������� 

/*
********************************************************************************
˽�б�������
********************************************************************************
*/
__MCB_ALM_EXE uint8_t beep_ctrl_en;				//BEEP�����أ�0:�رգ�1:����
__MCB_ALM_EXE uint8_t beep_alarm_flag;			//��ҪBEEP������־��0:����Ҫ���У�1:��Ҫ����
__MCB_ALM_EXE uint8_t beep_alarm_cnt_en;		//BEEP������ʱ������
__MCB_ALM_EXE uint16_t beep_alarm_cnt;			//BEEP������ʱ��

__MCB_ALM_EXE char mcb_alm_up_buf[MCB_ALM_UP_ARRAY_NUM][MCB_ALM_UP_DATA_NUM]; //�����ϴ����ݻ���������
__MCB_ALM_EXE uint8_t mcb_alm_cur_num;			//��ǰ������������
__MCB_ALM_EXE uint8_t mcb_alm_in_index;			//��ǰ��������in����
__MCB_ALM_EXE uint8_t mcb_alm_out_index;		//��ǰ��������out����
__MCB_ALM_EXE uint8_t mcb_alm_cur_index;		//��ǰ�����������������ڱ����ϴ�������ȡ

__MCB_ALM_EXE uint16_t ch1_alm_cnt;				//ͨ��1�澯������
__MCB_ALM_EXE uint16_t ch2_alm_cnt;				//ͨ��2�澯������
__MCB_ALM_EXE uint16_t ant1_alm_cnt;			//����1�澯������
__MCB_ALM_EXE uint16_t ant2_alm_cnt;			//����2�澯������
__MCB_ALM_EXE uint16_t pwr1_alm_cnt;			//��Դ1�澯������
__MCB_ALM_EXE uint16_t pwr2_alm_cnt;			//��Դ2�澯������

__MCB_ALM_EXE uint8_t cur_gps_max_star;			//��ǰͨ��GPS�������
__MCB_ALM_EXE uint8_t cur_gps_min_star;			//��ǰͨ��GPS��С����
__MCB_ALM_EXE uint8_t cur_bd_max_star;			//��ǰͨ��BD�������
__MCB_ALM_EXE uint8_t cur_bd_min_star;			//��ǰͨ��BD��С����
__MCB_ALM_EXE uint8_t cur_gps_star;				//��ǰͨ��GPS����
__MCB_ALM_EXE uint8_t cur_bd_star;				//��ǰͨ��BD����

__MCB_ALM_EXE uint8_t ch1_rf_state;				//��Ƶ��ͨ��1״̬
__MCB_ALM_EXE uint8_t ch2_rf_state;				//��Ƶ��ͨ��2״̬
__MCB_ALM_EXE uint8_t ant1_rf_state;			//��Ƶ������1״̬
__MCB_ALM_EXE uint8_t ant2_rf_state;			//��Ƶ������2״̬
__MCB_ALM_EXE uint8_t pwr1_pd8_state;			//��Դ1״̬
__MCB_ALM_EXE uint8_t pwr2_pd8_state;			//��Դ2״̬
__MCB_ALM_EXE uint8_t old_ch1_rf_state;			//��Ƶ��ͨ��1״̬
__MCB_ALM_EXE uint8_t old_ch2_rf_state;			//��Ƶ��ͨ��2״̬
__MCB_ALM_EXE uint8_t old_ant1_rf_state;		//��Ƶ������1״̬
__MCB_ALM_EXE uint8_t old_ant2_rf_state;		//��Ƶ������2״̬
__MCB_ALM_EXE uint8_t old_pwr1_pd8_state;		//��Դ1״̬
__MCB_ALM_EXE uint8_t old_pwr2_pd8_state;		//��Դ2״̬

__MCB_ALM_EXE uint8_t relay1_alm_state_rf;		//�̵���1�澯״̬
__MCB_ALM_EXE uint8_t relay1_alm_state_mpd8;	//�̵���1�澯״̬
__MCB_ALM_EXE uint8_t relay1_alm_state_spd8[8];
__MCB_ALM_EXE uint8_t relay2_alm_state;			//�̵���2�澯״̬
__MCB_ALM_EXE uint8_t relay3_alm_state;			//�̵���3�澯״̬
__MCB_ALM_EXE uint8_t relay4_alm_state;			//�̵���4�澯״̬

__MCB_ALM_EXE uint8_t old_relay1_alm_state_rf;	//�̵���1
__MCB_ALM_EXE uint8_t old_relay1_alm_state_mpd8;//�̵���1
__MCB_ALM_EXE uint8_t old_relay1_alm_state_spd8[8];
__MCB_ALM_EXE uint8_t old_relay2_alm_state;		//�̵���2
__MCB_ALM_EXE uint8_t old_relay3_alm_state;		//�̵���3
__MCB_ALM_EXE uint8_t old_relay4_alm_state;		//�̵���4

__MCB_ALM_EXE uint8_t rf_pd8_alm;				//0-rf,1-mpd8,0x81~0x88-spd8

__MCB_ALM_EXE uint8_t mpd8_out_state;			//�����˹��ְ����״̬
__MCB_ALM_EXE uint8_t spd8_out_state[8];		//�ӻ��˹��ְ����״̬
__MCB_ALM_EXE uint8_t old_mpd8_out_state;		//�����˹��ְ����״̬
__MCB_ALM_EXE uint8_t old_spd8_out_state[8];	//�ӻ��˹��ְ����״̬

__MCB_ALM_EXE char rf_date_time_str[20];		//����ʱ���ַ���
__MCB_ALM_EXE char mp_date_time_str[20];		//����ʱ���ַ���

//��Ƶ��Ͱ˹��ְ�״̬������ģ�飬���Ӱ˹��ְ�˿����״̬���˹���ģ��


/*
********************************************************************************
˽�к�������
********************************************************************************
*/
void alarm_check_deal(void);
void gps_bd_max_min_star_judge(void);
void alarm_data_in(uint8_t alm_type);
void alarm_data_out(void);
void beep_en_on_off(void);
void rfb_date_time_to_str(MR_COMM_BT_TIME_DEF bt_time);
void pd8_date_time_to_str(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

void warn_mata_deal(void);
void warn_sata_deal(void);
void warn_rch1_deal(void);
void warn_rch2_deal(void);
void warn_mapw_deal(void);
void warn_sapw_deal(void);
void warn_rly1_deal(uint8_t rf_pd8);
void warn_rly2_deal(void);
void warn_rly3_deal(void);
void warn_rly4_deal(void);
void warn_ptbd_deal(void);
void warn_oppt_deal(void);
void warn_ctbd_deal(void);

//LED1:����1��ͨ��1���ɽӵ�1�������ӻ���� 	LED2:����2��ͨ��2���ɽӵ�2  LED3:˫��Դָʾ
//���ϵ� LED1 LED2��LED3�̵���
//�����и���״̬����������������⣬�̵����������Ƶ��ǹ���
//
//���ݹ��ܶ���
void ALL_LED_OFF(void);							//д��������
void LED_1_NORMAL(void);						//ͬ��
void LED_2_NORMAL(void);
void LED_3_NORMAL(void);
void LED_1_ALARM(void);
void LED_2_ALARM(void);
void LED_1_FAULT(void);
void LED_2_FAULT(void);
void LED_3_ALARM(void);
void LED_IO_UPDATE(void);						//����ˢ��IO���ж���LED״̬�����������һ��

/*
  �̵���1(�ɽӵ�1��ָʾ����1��ͨ��1����������ʹӻ������
  �̵���2(�ɽӵ�2��ָʾ����2��ͨ��2��
  �̵���3(�ɽӵ�3��ָʾ��Դ1��
  �̵���4(�ɽӵ�4��ָʾ��Դ2��
*/
void ALL_RELAY_NORMAL(void);					//д��������
void RELAY_1_NORMAL(void);						//ͬ��
void RELAY_2_NORMAL(void);
void RELAY_3_NORMAL(void);
void RELAY_4_NORMAL(void);
void RELAY_1_ALARM(void);
void RELAY_2_ALARM(void);
void RELAY_3_ALARM(void);
void RELAY_4_ALARM(void);
void RELAY_IO_UPDATE(void);						//����ˢ��IO���ж���RELAY״̬�����������һ��
//
void BEEP_init(void);							//��������ʼ��

void TIM1_Config(void);							//TIM1ʱ��IO����
void TIM1_PWM_init(uint16_t period, uint16_t pulse);//TIM1 PWM��ʼ��
    
#ifdef __cplusplus
	}
#endif

#endif
