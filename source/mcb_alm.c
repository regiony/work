/*
********************************************************************************
�ļ���	��mcb_alm.c
�ļ����������ذ��������ģ��,[led,beep,relay]
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __MCB_ALM_GLOBAL
#include "mcb_alm.h"                 
#include "i2c.h"  
#include "pca9555.h"  
#include "public.h"
#include <stdio.h>

/*
********************************************************************************
�������ƣ�mcb_alm_init
�������������������ʼ������
����������
����ֵ	��
********************************************************************************
*/
void mcb_alm_init(void) 
{
	uint8_t i, j;
	
	mcb_alm_task_flag = 0;
	mcb_alm_upwifi_flag = 0;
	mcb_alm_0hour_clr = 0;
	
	beep_alarm_cnt = 0;
	beep_alarm_cnt_en = 0;
	mcb_alm_cur_num = 0;
	mcb_alm_in_index = 0;
	mcb_alm_out_index = 0;
	
	ch1_alm_cnt = 0;		
	ch2_alm_cnt = 0;		
	ant1_alm_cnt = 0;	
	ant2_alm_cnt = 0;
	pwr1_alm_cnt = 0;
	pwr2_alm_cnt = 0;	
	                 
	cur_gps_max_star = 0;
	cur_gps_min_star = 0;
	cur_bd_max_star = 0;	
	cur_bd_min_star = 0;	
	cur_gps_star = 0;
	cur_bd_star = 0;
	                  
	ch1_rf_state = 0;	
	ch2_rf_state = 0;	
	ant1_rf_state = 0;
	ant2_rf_state = 0;
	pwr1_pd8_state = 0;
	pwr2_pd8_state = 0;
	old_ch1_rf_state = 0;	
	old_ch2_rf_state = 0;	
	old_ant1_rf_state = 0;
	old_ant2_rf_state = 0;
	old_pwr1_pd8_state = 0;
	old_pwr2_pd8_state = 0;
	                  
	relay1_alm_state_rf = 0;
	relay1_alm_state_mpd8 = 0;
	
	relay2_alm_state = 0;	
	relay3_alm_state = 0;	
	relay4_alm_state = 0;
	old_relay1_alm_state_rf = 0;
	old_relay1_alm_state_mpd8 = 0;
	
	old_relay2_alm_state = 0;	
	old_relay3_alm_state = 0;	
	old_relay4_alm_state = 0;
	
	rf_pd8_alm = 0;
                  
	mpd8_out_state = 0;
	old_mpd8_out_state = 0;
	
	for(i = 0; i < 8; i++)
	{
		spd8_out_state[i] = 0;
		old_spd8_out_state[i] = 0;
		relay1_alm_state_spd8[i] = 0;
		old_relay1_alm_state_spd8[i] = 0;
	}
	for(i = 0; i < 20; i++)	
    {
		rf_date_time_str[i] = 0;
        mp_date_time_str[i] = 0;
	}
	for(i = 0; i < MCB_ALM_UP_ARRAY_NUM; i++)
	{
		for(j = 0; j < MCB_ALM_UP_DATA_NUM; j++)
			mcb_alm_up_buf[i][j] = 0;
	}
	
	ALL_LED_OFF();				//�ϵ��ʼ���������еƻ���
	LED_3_NORMAL();				//�ϵ�LED3��ԴָʾĬ�����������̵����ƻ���
	LED_IO_UPDATE();			//LED״̬����
	
	ALL_RELAY_NORMAL();			//���ϵ�����RELAY�������棬�����ֶ���NO
	RELAY_IO_UPDATE();			//RELAY״̬����
	
	beep_ctrl_en = 1;			//�ϵ�Ĭ������BEEP����
	beep_alarm_flag = 0;		//BEEP������־���ϵ��ʼ���򱨾���ʧʱ������ֹͣ����
	#ifndef __OLD_MCB_VER__
		TIM1->CR1 &= ~TIM_CR1_CEN; 
		TIM1->CNT = 0;	
	#endif
	BEEP_init();				//BEEP��ʼ��
}

/*
********************************************************************************
�������ƣ�mcb_alm_task
���������������������
����������
����ֵ	��
********************************************************************************
*/
void mcb_alm_task(void) 
{
	mcb_alm_task_flag = 0;
		
	//�������
	alarm_check_deal();		
	
	//BEEP��������,ͬһ�澯Դһֱ�澯ֻ��һ�Σ���ͬ�澯Դ�澯����һ�Σ��澯�ָ��ٴθ澯����һ�Σ�������һֱ��
	if((beep_alarm_flag) && (!beep_alarm_cnt_en))
	{
		beep_alarm_flag = 0;
		if(beep_ctrl_en)
		{
			beep_alarm_cnt = 0;
			beep_alarm_cnt_en = 1;			
			TIM1->CNT = 0;	
			TIM1->CR1 |= TIM_CR1_CEN; 		
		}
	}
	
	//wifi�ϱ��澯����
	if(mcb_alm_cur_num > 0)
	{
		mcb_alm_upwifi_flag = 1;
		mcb_wifi_task_flag = 1;
	}	
	
	//��ǰͨ����GPS��BD���õ������С��
	gps_bd_max_min_star_judge();
				
	//0�㱨����Ԫ�����Ӧ����, ÿ��0���LCD������ʾ��ͨ�������ߺ͵�Դ�ĸ澯�����Լ�����/GPS�����С��������
	if(mcb_alm_0hour_clr)	
	{
		mcb_alm_0hour_clr = 0;
		
		ch1_alm_cnt = 0;		
		ch2_alm_cnt = 0;		
		ant1_alm_cnt = 0;	
		ant2_alm_cnt = 0;
		pwr1_alm_cnt = 0;
		pwr2_alm_cnt = 0;			
		cur_gps_max_star = 0;
		cur_gps_min_star = 0;
		cur_bd_max_star = 0;	
		cur_bd_min_star = 0;	
	}
}

/*
********************************************************************************
�������ƣ�gps_bd_max_min_star_judge
������������ǰͨ����GPS��BD���õ������С��
����������
����ֵ	��
********************************************************************************
*/
void gps_bd_max_min_star_judge(void)
{
	static uint8_t flag1 = 0, flag2 = 0;
	
	//��ǰͨ����GPS��BD���õ������С��
	if(mr_comm_msg.cur_chno == 0)		//��ǰ1ͨ��
	{
		if(flag1 == 0)
		{
			flag1 = 1;
			flag2 = 0;
			cur_gps_max_star = 0;
			cur_gps_min_star = 0;
			cur_bd_max_star = 0;	
			cur_bd_min_star = 0;					
		}		
		//GPS
		cur_gps_star = mr_comm_msg.ch1_gps_kyx;
		if(!cur_gps_max_star)	
			cur_gps_max_star = cur_gps_star;
		else
		if(cur_gps_max_star < cur_gps_star)
			cur_gps_max_star = cur_gps_star;
			
		if(!cur_gps_min_star)	
			cur_gps_min_star = cur_gps_star;
		else
		if(cur_gps_min_star > cur_gps_star)
			cur_gps_min_star = cur_gps_star;		
		//BD	
		cur_bd_star = mr_comm_msg.ch1_bd_kyx;
		if(!cur_bd_max_star)	
			cur_bd_max_star = cur_bd_star;
		else
		if(cur_bd_max_star < cur_bd_star)
			cur_bd_max_star = cur_bd_star;
			
		if(!cur_bd_min_star)	
			cur_bd_min_star = cur_bd_star;
		else
		if(cur_bd_min_star > cur_bd_star)
			cur_bd_min_star = cur_bd_star;					
	}
	else
	if(mr_comm_msg.cur_chno == 1)		//��ǰ2ͨ��
	{
		if(flag2 == 0)
		{
			flag2 = 1;
			flag1 = 0;
			cur_gps_max_star = 0;
			cur_gps_min_star = 0;
			cur_bd_max_star = 0;	
			cur_bd_min_star = 0;					
		}		
		//GPS
		cur_gps_star = mr_comm_msg.ch2_gps_kyx;
		if(!cur_gps_max_star)	
			cur_gps_max_star = cur_gps_star;
		else
		if(cur_gps_max_star < cur_gps_star)
			cur_gps_max_star = cur_gps_star;
			
		if(!cur_gps_min_star)	
			cur_gps_min_star = cur_gps_star;
		else
		if(cur_gps_min_star > cur_gps_star)
			cur_gps_min_star = cur_gps_star;		
		//BD	
		cur_bd_star = mr_comm_msg.ch2_bd_kyx;
		if(!cur_bd_max_star)	
			cur_bd_max_star = cur_bd_star;
		else
		if(cur_bd_max_star < cur_bd_star)
			cur_bd_max_star = cur_bd_star;
			
		if(!cur_bd_min_star)	
			cur_bd_min_star = cur_bd_star;
		else
		if(cur_bd_min_star > cur_bd_star)
			cur_bd_min_star = cur_bd_star;										
	}			
}

/*
********************************************************************************
�������ƣ�alarm_check_deal
��������������������, LED, BEEP, RELAY, ALARM UP WIFI
����������
����ֵ	��
********************************************************************************
*/
void alarm_check_deal(void) 
{
	uint8_t i, local_flag;
	static uint8_t flg1 = 0, flg2 = 0, j = 0;
	static uint16_t cnt = 0;
	
	//����1
	if((mr_comm_msg.state1 & 0xC0) == 0x00)
	{
		ant1_rf_state = 0;			//����
	}
	else
	if((mr_comm_msg.state1 & 0xC0) == 0x40)
	{
		ant1_rf_state = 1;			//δ��
	}
	else
	if((mr_comm_msg.state1 & 0xC0) == 0x80)
	{
		ant1_rf_state = 2;			//�澯
	}
	if(ant1_rf_state != old_ant1_rf_state) 	//״̬�仯
	{
		if(ant1_rf_state)
			ant1_alm_cnt++;
		if(ant1_alm_cnt > 999)
			ant1_alm_cnt = 0;
		alarm_data_in(WARN_MATA);
		old_ant1_rf_state = ant1_rf_state;
	}
	//����2
	if((mr_comm_msg.state1 & 0x30) == 0x00)
	{
		ant2_rf_state = 0;			//����
	}
	else
	if((mr_comm_msg.state1 & 0x30) == 0x10)
	{
		ant2_rf_state = 1;			//δ��
	}
	else
	if((mr_comm_msg.state1 & 0x30) == 0x20)
	{
		ant2_rf_state = 2;			//�澯
	}
	if(ant2_rf_state != old_ant2_rf_state) 	//״̬�仯
	{
		if(ant2_rf_state)
			ant2_alm_cnt++;
		if(ant2_alm_cnt > 999)
			ant2_alm_cnt = 0;
		alarm_data_in(WARN_SATA);
		old_ant2_rf_state = ant2_rf_state;
	}	
	//ͨ��1
	if((mr_comm_msg.state1 & 0x08) == 0x00)
	{
		ch1_rf_state = 0;			//����
	}
	else
	if((mr_comm_msg.state1 & 0x08) == 0x08)
	{
		ch1_rf_state = 1;			//�澯
	}
	if(ch1_rf_state != old_ch1_rf_state) 	//״̬�仯
	{
		if(ch1_rf_state)
			ch1_alm_cnt++;
		if(ch1_alm_cnt > 999)
			ch1_alm_cnt = 0;
		alarm_data_in(WARN_RCH1);
		old_ch1_rf_state = ch1_rf_state;
	}	
	//ͨ��2
	if((mr_comm_msg.state1 & 0x04) == 0x00)
	{
		ch2_rf_state = 0;			//����
	}
	else
	if((mr_comm_msg.state1 & 0x04) == 0x04)
	{
		ch2_rf_state = 1;			//�澯
	}
	if(ch2_rf_state != old_ch2_rf_state) 	//״̬�仯
	{
		if(ch2_rf_state)
			ch2_alm_cnt++;
		if(ch2_alm_cnt > 999)
			ch2_alm_cnt = 0;
		alarm_data_in(WARN_RCH2);
		old_ch2_rf_state = ch2_rf_state;
	}	
	//RF�̵���1
	if((mr_comm_msg.state1 & 0x02) == 0x00)
	{
		relay1_alm_state_rf = 0;			//����
	}
	else
	if((mr_comm_msg.state1 & 0x02) == 0x02)
	{
		relay1_alm_state_rf = 1;			//�澯
	}
	if(relay1_alm_state_rf != old_relay1_alm_state_rf) 	//״̬�仯
	{
		rf_pd8_alm = 0;
		alarm_data_in(WARN_RLY1);
		old_relay1_alm_state_rf = relay1_alm_state_rf;
		if(relay1_alm_state_rf)
			beep_alarm_flag = 1;
	}	
	//�̵���2
	if((mr_comm_msg.state1 & 0x01) == 0x00)
	{
		relay2_alm_state = 0;			//����
	}
	else
	if((mr_comm_msg.state1 & 0x01) == 0x01)
	{
		relay2_alm_state = 1;			//�澯
	}
	if(relay2_alm_state != old_relay2_alm_state) 	//״̬�仯
	{
		alarm_data_in(WARN_RLY2);
		old_relay2_alm_state = relay2_alm_state;
		if(relay2_alm_state)
			beep_alarm_flag = 1;
	}	
	//��Դ1
	if((mp_mpd8_comm_msg.mpd8_pwr_state & 0x08) == 0x00)
	{
		pwr1_pd8_state = 0;				//����
	}
	else
	if((mp_mpd8_comm_msg.mpd8_pwr_state & 0x08) == 0x08)
	{
		pwr1_pd8_state = 1;				//�澯
	}		
	if(pwr1_pd8_state != old_pwr1_pd8_state) 	//״̬�仯
	{
		if(pwr1_pd8_state)
			pwr1_alm_cnt++;
		if(pwr1_alm_cnt > 999)
			pwr1_alm_cnt = 0;
		alarm_data_in(WARN_MAPW);
		old_pwr1_pd8_state = pwr1_pd8_state;
	}	
	//��Դ2
	if((mp_mpd8_comm_msg.mpd8_pwr_state & 0x04) == 0x00)
	{
		pwr2_pd8_state = 0;				//����
	}
	else
	if((mp_mpd8_comm_msg.mpd8_pwr_state & 0x04) == 0x04)
	{
		pwr2_pd8_state = 1;				//�澯
	}		
	if(pwr2_pd8_state != old_pwr2_pd8_state) 	//״̬�仯
	{
		if(pwr2_pd8_state)
			pwr2_alm_cnt++;
		if(pwr2_alm_cnt > 999)
			pwr2_alm_cnt = 0;
		alarm_data_in(WARN_SAPW);
		old_pwr2_pd8_state = pwr2_pd8_state;
	}	
	//RF�̵���3
	if((mp_mpd8_comm_msg.mpd8_pwr_state & 0x02) == 0x00)
	{
		relay3_alm_state = 0;				//����
	}
	else
	if((mp_mpd8_comm_msg.mpd8_pwr_state & 0x02) == 0x02)
	{
		relay3_alm_state = 1;				//�澯
	}		
	if(relay3_alm_state != old_relay3_alm_state) 	//״̬�仯
	{
		alarm_data_in(WARN_RLY3);
		old_relay3_alm_state = relay3_alm_state;
		if(relay3_alm_state)
			beep_alarm_flag = 1;
	}	
	//�̵���4
	if((mp_mpd8_comm_msg.mpd8_pwr_state & 0x01) == 0x00)
	{
		relay4_alm_state = 0;				//����
	}
	else
	if((mp_mpd8_comm_msg.mpd8_pwr_state & 0x01) == 0x01)
	{
		relay4_alm_state = 1;				//�澯
	}		
	if(relay4_alm_state != old_relay4_alm_state) 	//״̬�仯
	{
		alarm_data_in(WARN_RLY4);
		old_relay4_alm_state = relay4_alm_state;
		if(relay4_alm_state)
			beep_alarm_flag = 1;
	}	
	//MPD8�̵���1
	if((mp_mpd8_comm_msg.mpd8_alm_state & 0xFF) == 0x00)
	{
		relay1_alm_state_mpd8 = 0;			//����
	}
	else
	if((mp_mpd8_comm_msg.mpd8_alm_state & 0xFF) != 0x00)
	{
		relay1_alm_state_mpd8 = 1;			//�澯
	}
	if(relay1_alm_state_mpd8 != old_relay1_alm_state_mpd8) 	//״̬�仯
	{
		rf_pd8_alm = 1;
		alarm_data_in(WARN_RLY1);
		old_relay1_alm_state_mpd8 = relay1_alm_state_mpd8;
		if(relay1_alm_state_mpd8)
			beep_alarm_flag = 1;
	}	
	//MPD8���״̬
	if((mp_mpd8_comm_msg.mpd8_out_state & 0xFF) == 0x00)
	{
		mpd8_out_state = 0;			//����
	}
	else
	if((mp_mpd8_comm_msg.mpd8_out_state & 0xFF) != 0x00)
	{
		mpd8_out_state = 1;			//�澯
	}	
	
	for(i = 0; i < mcb_cur_pd8_slave_num; i++)
	{
		
		if((mcb_cur_valid_slave_addr[i] > 0) && (mcb_cur_valid_slave_addr[i] < 9))
		{
			//SPD8�̵���1
			if((mp_spd8_comm_msg[mcb_cur_valid_slave_addr[i] - 1].spd8_alm_state & 0xFF) == 0x00)
			{
				relay1_alm_state_spd8[mcb_cur_valid_slave_addr[i] - 1] = 0;			//����
			}
			else
			if((mp_spd8_comm_msg[mcb_cur_valid_slave_addr[i] - 1].spd8_alm_state & 0xFF) != 0x00)
			{
				relay1_alm_state_spd8[mcb_cur_valid_slave_addr[i] - 1] = 1;			//�澯
			}
			if(relay1_alm_state_spd8[mcb_cur_valid_slave_addr[i] - 1] != old_relay1_alm_state_spd8[mcb_cur_valid_slave_addr[i] - 1]) 	//״̬�仯
			{
				rf_pd8_alm = 0x80 | mcb_cur_valid_slave_addr[i];
				alarm_data_in(WARN_RLY1);
				old_relay1_alm_state_spd8[mcb_cur_valid_slave_addr[i] - 1] = relay1_alm_state_spd8[mcb_cur_valid_slave_addr[i] - 1];
				if(relay1_alm_state_spd8[mcb_cur_valid_slave_addr[i] - 1])
					beep_alarm_flag = 1;
			}	
			
			//SPD8���״̬
			if((mp_spd8_comm_msg[mcb_cur_valid_slave_addr[i] - 1].spd8_out_state & 0xFF) == 0x00)
			{
				spd8_out_state[mcb_cur_valid_slave_addr[i] - 1] = 0;			//����
			}
			else
			if((mp_spd8_comm_msg[mcb_cur_valid_slave_addr[i] - 1].spd8_out_state & 0xFF) != 0x00)
			{
				spd8_out_state[mcb_cur_valid_slave_addr[i] - 1] = 1;			//�澯
			}						
		}
	}
	/* LED RELAY JUDGE*/
	//LED1 - ANT1/CH1/MPD8/SPD8/RFRLY1/MPD8RLY1/SPD8RLY1
	local_flag = 0;
	for(i = 0; i < mcb_cur_pd8_slave_num; i++)
	{		
		if(relay1_alm_state_spd8[mcb_cur_valid_slave_addr[i] - 1] || spd8_out_state[mcb_cur_valid_slave_addr[i] - 1])
		{
			local_flag = 1;
			break;
		}	
	}
	if(ant1_rf_state | ch1_rf_state | relay1_alm_state_rf | relay1_alm_state_mpd8 | mpd8_out_state | local_flag) 
		LED_1_ALARM();
	else
		LED_1_NORMAL();
	//LED2 - ANT2/CH2/RLY2
	if(ant2_rf_state | ch2_rf_state | relay2_alm_state) 
		LED_2_ALARM();
	else
		LED_2_NORMAL();
	//LED3 - PWR1/PWR2/RLY3/RLY4
	if(pwr1_pd8_state | pwr2_pd8_state | relay3_alm_state | relay4_alm_state) 
		LED_3_ALARM();
	else
		LED_3_NORMAL();
		
	//RLY1 - NODE1/
	local_flag = 0;
	for(i = 0; i < mcb_cur_pd8_slave_num; i++)
	{		
		if(relay1_alm_state_spd8[mcb_cur_valid_slave_addr[i] - 1])
		{
			local_flag = 1;
			break;
		}	
	}
	if(relay1_alm_state_rf | relay1_alm_state_mpd8 | local_flag)
		RELAY_1_ALARM();
	else
		RELAY_1_NORMAL();
	//RLY2 - NODE2
	if(relay2_alm_state)
		RELAY_2_ALARM();
	else
		RELAY_2_NORMAL();
	//RLY3 - PWR1
	if(relay3_alm_state)
		RELAY_3_ALARM();
	else
		RELAY_3_NORMAL();
	//RLY4 - PWR2
	if(relay4_alm_state)
		RELAY_4_ALARM();
	else
		RELAY_4_NORMAL();	
	
	//���ϴ���
	j++;
	if(sys_fault_word & 0x01)
	{
		//����Ƶ��/Э���ͨ�Ź���,���ڿ����ϵ���������������
        #ifndef __TEST_HMU__
            if(mcb_hmu_disp_pages == PWR_ON_PAGE)
            {
            	cnt++;
            	if(cnt > 300)	//200*300=60000ms //900-3min 1500-5min
            	{
            		cnt = 0;	
            		mcb_hmu_disp_pages = MAIN_PAGE;	
            	}
            }
            else
            	cnt = 0;                
        #endif
		#ifdef __DAN_SE_LED__
			if(j & 0x04)	
				LED_1_ALARM();
			else
				LED_1_NORMAL();		
		#else
			LED_1_FAULT();		//˫ɫLED�ã�����ɫ����
		#endif
		//WARN UP WIFI
		if(!flg1)
		{
			flg1 = 1;
			alarm_data_in(WARN_PTBD);	
		}
	}
	else
	{
		if(flg1)
			alarm_data_in(WARN_PTBD);	
		flg1 = 0;	
	}
	//�������˹��ְ�ͨ�Ź���
	if(sys_fault_word & 0x02)
	{
		#ifdef __DAN_SE_LED__
			if(j & 0x04)	
				LED_2_ALARM();
			else
				LED_2_NORMAL();		
		#else
			LED_2_FAULT();		//˫ɫLED�ã�����ɫ����
		#endif	
		//WARN UP WIFI
		if(!flg2)
		{
			flg2 = 1;
			alarm_data_in(WARN_OPPT);	
		}
	}	
	else
	{
		if(flg2)
			alarm_data_in(WARN_OPPT);	
		flg2 = 0;		
	}
		
	LED_IO_UPDATE();			//LED״̬����
	RELAY_IO_UPDATE();			//RELAY״̬����
}

/*
********************************************************************************
�������ƣ�alarm_data_in
����������
����������
����ֵ	��
********************************************************************************
*/
void alarm_data_in(uint8_t alm_type)
{
	uint8_t flag;
	
	flag = 0x00;
	if(mcb_alm_cur_num < MCB_ALM_UP_ARRAY_NUM)
	{
		switch(alm_type)
		{
			case WARN_MATA:	warn_mata_deal();	break;
			case WARN_SATA:	warn_sata_deal();	break;
			case WARN_RCH1:	warn_rch1_deal();	break;
			case WARN_RCH2:	warn_rch2_deal();	break;
			case WARN_MAPW:	warn_mapw_deal();	break;
			case WARN_SAPW:	warn_sapw_deal();	break;
			case WARN_RLY1:	warn_rly1_deal(rf_pd8_alm);	break;
			case WARN_RLY2:	warn_rly2_deal();	break;
			case WARN_RLY3:	warn_rly3_deal();	break;
			case WARN_RLY4:	warn_rly4_deal();	break;
			case WARN_PTBD:	warn_ptbd_deal();	break;
			case WARN_OPPT:	warn_oppt_deal();	break;
			case WARN_CTBD:	warn_ctbd_deal();	break;
			default: flag = 0x01; break;
		}
		//
		if(!flag)
		{
			mcb_alm_in_index++;
			if(mcb_alm_in_index >= MCB_ALM_UP_ARRAY_NUM)
				mcb_alm_in_index = 0;			
			mcb_alm_cur_num++;					
		}
	}
}

/*
********************************************************************************
�������ƣ�alarm_data_out
����������
����������
����ֵ	��
********************************************************************************
*/
void alarm_data_out(void)
{
	if(mcb_alm_cur_num > 0)
	{
		mcb_alm_cur_index = mcb_alm_out_index;
		mcb_alm_out_index++;
		if(mcb_alm_out_index >= MCB_ALM_UP_ARRAY_NUM)
			mcb_alm_out_index = 0;	
		mcb_alm_cur_num--;			
	}
}

/*
********************************************************************************
�������ƣ�warn_mata_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_mata_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'M';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'A';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'T';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'A';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(ant1_rf_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	rfb_date_time_to_str(mr_comm_bt_time);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = rf_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(old_ant1_rf_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';
}

/*
********************************************************************************
�������ƣ�warn_sata_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_sata_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'S';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'A';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'T';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'A';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(ant2_rf_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	rfb_date_time_to_str(mr_comm_bt_time);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = rf_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(old_ant2_rf_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';	
}

/*
********************************************************************************
�������ƣ�warn_rch1_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_rch1_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'R';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'C';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'H';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(ch1_rf_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	rfb_date_time_to_str(mr_comm_bt_time);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = rf_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(old_ch1_rf_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';		
}

/*
********************************************************************************
�������ƣ�warn_rch2_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_rch2_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'R';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'C';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'H';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '2';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(ch2_rf_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	rfb_date_time_to_str(mr_comm_bt_time);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = rf_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(old_ch2_rf_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';			
}

/*
********************************************************************************
�������ƣ�warn_mapw_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_mapw_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'M';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'A';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'P';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'W';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(pwr1_pd8_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	pd8_date_time_to_str(mp_mpd8_comm_msg.year, mp_mpd8_comm_msg.month, mp_mpd8_comm_msg.day, 	\
						 mp_mpd8_comm_msg.hour, mp_mpd8_comm_msg.min, mp_mpd8_comm_msg.sec);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = mp_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(old_pwr1_pd8_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';			
}

/*
********************************************************************************
�������ƣ�warn_sapw_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_sapw_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'S';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'A';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'P';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'W';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(pwr2_pd8_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	pd8_date_time_to_str(mp_mpd8_comm_msg.year, mp_mpd8_comm_msg.month, mp_mpd8_comm_msg.day, 	\
						 mp_mpd8_comm_msg.hour, mp_mpd8_comm_msg.min, mp_mpd8_comm_msg.sec);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = mp_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(old_pwr2_pd8_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';				
}

/*
********************************************************************************
�������ƣ�warn_rly1_deal
����������
����������rf_pd8: 0 - rf, 1 - pd8, 0x81~0x88
����ֵ	��
********************************************************************************
*/
void warn_rly1_deal(uint8_t rf_pd8)
{
	uint8_t i, j, temp;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'R';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'L';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'Y';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';

	j = 0;
	if(rf_pd8 == 0)  //rfb relay 1
	{
		//����/��ʧ��־' '
		if(relay1_alm_state_rf)
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
		else
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
		mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';	
		//�澯ʱ��' '	
		rfb_date_time_to_str(mr_comm_bt_time);
		while(rf_date_time_str[j] != '\0')
		{
			mcb_alm_up_buf[mcb_alm_in_index][i++] = rf_date_time_str[j++];
		}	
		mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
		//�澯��ֵ	
		if(old_relay1_alm_state_rf)
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
		else
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';					
	}
	else
	if(rf_pd8 == 1) //mpd8 relay 1
	{
		//����/��ʧ��־' '
		if(relay1_alm_state_mpd8)
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
		else
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
		mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';	
		//�澯ʱ��' '	
		pd8_date_time_to_str(mp_mpd8_comm_msg.year, mp_mpd8_comm_msg.month, mp_mpd8_comm_msg.day, 	\
						     mp_mpd8_comm_msg.hour, mp_mpd8_comm_msg.min, mp_mpd8_comm_msg.sec);
		while(mp_date_time_str[j] != '\0')
		{
			mcb_alm_up_buf[mcb_alm_in_index][i++] = mp_date_time_str[j++];
		}	
		mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
		//�澯��ֵ	
		if(old_relay1_alm_state_mpd8)
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
		else
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';			
	}
	else
	if(rf_pd8 & 0x80) //spd8 relay 1
	{
		temp = rf_pd8 & 0x0F;
		temp -= 1;
		if(temp > 8)
		{
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';	
			return ;	
		}
		//����/��ʧ��־' '
		if(relay1_alm_state_spd8[temp])
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
		else
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
		mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';	
		//�澯ʱ��' '	
		pd8_date_time_to_str(mp_spd8_comm_msg[temp].year, mp_spd8_comm_msg[temp].month, mp_spd8_comm_msg[temp].day, 	\
							 mp_spd8_comm_msg[temp].hour, mp_spd8_comm_msg[temp].min, mp_spd8_comm_msg[temp].sec);
		while(mp_date_time_str[j] != '\0')
		{
			mcb_alm_up_buf[mcb_alm_in_index][i++] = mp_date_time_str[j++];
		}				
		mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
		//�澯��ֵ	
		if(old_relay1_alm_state_spd8[temp])
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
		else
			mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	}
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';				
}

/*
********************************************************************************
�������ƣ�warn_rly2_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_rly2_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'R';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'L';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'Y';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '2';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(relay2_alm_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	rfb_date_time_to_str(mr_comm_bt_time);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = rf_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(old_relay2_alm_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';				
}

/*
********************************************************************************
�������ƣ�warn_rly3_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_rly3_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'R';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'L';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'Y';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '3';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(relay3_alm_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	pd8_date_time_to_str(mp_mpd8_comm_msg.year, mp_mpd8_comm_msg.month, mp_mpd8_comm_msg.day, 	\
						 mp_mpd8_comm_msg.hour, mp_mpd8_comm_msg.min, mp_mpd8_comm_msg.sec);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = mp_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(old_relay3_alm_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';			
}

/*
********************************************************************************
�������ƣ�warn_rly4_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_rly4_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'R';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'L';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'Y';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '4';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(relay4_alm_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	pd8_date_time_to_str(mp_mpd8_comm_msg.year, mp_mpd8_comm_msg.month, mp_mpd8_comm_msg.day, 	\
						 mp_mpd8_comm_msg.hour, mp_mpd8_comm_msg.min, mp_mpd8_comm_msg.sec);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = mp_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(old_relay4_alm_state)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';			
}

/*
********************************************************************************
�������ƣ�warn_ptbd_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_ptbd_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'P';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'T';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'B';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'D';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(sys_fault_word & 0x01)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	rfb_date_time_to_str(mr_comm_bt_time);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = rf_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(sys_fault_word & 0x01)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';				
}

/*
********************************************************************************
�������ƣ�warn_oppt_deal
����������
����������
����ֵ	��
********************************************************************************
*/
void warn_oppt_deal(void)
{
	uint8_t i, j;
		
	i = 0;
	//�澯���ͱ���' '
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'O';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'P';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'P';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = 'T';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//����/��ʧ��־' '
	if(sys_fault_word & 0x02)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯ʱ��' '
	j = 0;
	pd8_date_time_to_str(mp_mpd8_comm_msg.year, mp_mpd8_comm_msg.month, mp_mpd8_comm_msg.day, 	\
						 mp_mpd8_comm_msg.hour, mp_mpd8_comm_msg.min, mp_mpd8_comm_msg.sec);
	while(rf_date_time_str[j] != '\0')
	{
		mcb_alm_up_buf[mcb_alm_in_index][i++] = mp_date_time_str[j++];
	}
	mcb_alm_up_buf[mcb_alm_in_index][i++] = ' ';
	//�澯��ֵ	
	if(sys_fault_word & 0x02)
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '1';
	else
		mcb_alm_up_buf[mcb_alm_in_index][i++] = '0';	
	//str end
	mcb_alm_up_buf[mcb_alm_in_index][i++] = '\0';				
}

/*
********************************************************************************
�������ƣ�warn_ctbd_deal
�������������ذ�澯��Ϣ
����������
����ֵ	��
********************************************************************************
*/
void warn_ctbd_deal(void)
{
	
}

/*
********************************************************************************
�������ƣ�beep_en_on_off
����������
����������
����ֵ	��
********************************************************************************
*/
void beep_en_on_off(void)
{
	if(beep_ctrl_en)
		beep_ctrl_en = 0;
	else
	if(!beep_ctrl_en)	
		beep_ctrl_en = 1;
	beep_alarm_cnt = 0;
	beep_alarm_cnt_en = 0;	
	//beep_alarm_flag = 0;		//ע�͵�60s�ڻ����������Ͻ����죬ȥ��ע�Ͳ��죬����60S��������������
	#ifndef __OLD_MCB_VER__
		TIM1->CR1 &= ~TIM_CR1_CEN; 
		TIM1->CNT = 0;	
	#endif
}

/*
********************************************************************************
�������ƣ�rfb_date_time_to_str
����������
����������
����ֵ	��
********************************************************************************
*/
void rfb_date_time_to_str(MR_COMM_BT_TIME_DEF bt_time)	
{
	uint8_t i, j;
	char str[5];
	
	i = 0;
	sprintf(str, "%4d", bt_time.year);	
	for(j = 0; j < 4; j++)
		rf_date_time_str[i++] = str[j];
	rf_date_time_str[i++] = '-';
	sprintf(str, "%02d", bt_time.month);	
	for(j = 0; j < 2; j++)
		rf_date_time_str[i++] = str[j];
	rf_date_time_str[i++] = '-';	
	sprintf(str, "%02d", bt_time.day);	
	for(j = 0; j < 2; j++)
		rf_date_time_str[i++] = str[j];
	rf_date_time_str[i++] = '_';	
	sprintf(str, "%02d", bt_time.hour);	
	for(j = 0; j < 2; j++)
		rf_date_time_str[i++] = str[j];
	rf_date_time_str[i++] = ':';
	sprintf(str, "%02d", bt_time.min);	
	for(j = 0; j < 2; j++)
		rf_date_time_str[i++] = str[j];
	rf_date_time_str[i++] = ':';
	sprintf(str, "%02d", bt_time.sec);	
	for(j = 0; j < 2; j++)
		rf_date_time_str[i++] = str[j];
}

/*
********************************************************************************
�������ƣ�pd8_date_time_to_str
���������������ӻ��˹��ְ�UTCʱ�����룬ת����ʱ�䣬���ַ���
����������
����ֵ	��
********************************************************************************
*/
void pd8_date_time_to_str(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	uint8_t i, j;
	char str[5];
	
	MP_UTCToBeijing(year, month, day, hour, minute, second);
	
	i = 0;
	sprintf(str, "%4d", mp_comm_bt_time.year);	
	for(j = 0; j < 4; j++)
		mp_date_time_str[i++] = str[j];
	mp_date_time_str[i++] = '-';
	sprintf(str, "%02d", mp_comm_bt_time.month);	
	for(j = 0; j < 2; j++)
		mp_date_time_str[i++] = str[j];
	mp_date_time_str[i++] = '-';	
	sprintf(str, "%02d", mp_comm_bt_time.day);	
	for(j = 0; j < 2; j++)
		mp_date_time_str[i++] = str[j];
	mp_date_time_str[i++] = '_';	
	sprintf(str, "%02d", mp_comm_bt_time.hour);	
	for(j = 0; j < 2; j++)
		mp_date_time_str[i++] = str[j];
	mp_date_time_str[i++] = ':';
	sprintf(str, "%02d", mp_comm_bt_time.min);	
	for(j = 0; j < 2; j++)
		mp_date_time_str[i++] = str[j];
	mp_date_time_str[i++] = ':';
	sprintf(str, "%02d", mp_comm_bt_time.sec);	
	for(j = 0; j < 2; j++)
		mp_date_time_str[i++] = str[j];	
}

/*
********************************************************************************
�������ƣ�BEEP_init
������������������ʼ��
����������
����ֵ	��
********************************************************************************
*/	
void BEEP_init(void)
{
	TIM1_Config();										//TIM1ʱ��IO����
	TIM1_PWM_init(TIM1_PERIOD_BEEP, TIM1_PULSE_BEEP);	//TIM1 PWM��ʼ��
}	

/*
********************************************************************************
�������ƣ�ALL_LED_OFF
�������������еƺ졢�̵ƾ���д����
����������
����ֵ	��
********************************************************************************
*/
void ALL_LED_OFF(void)
{
	port1_buf &= ~(RLED1_CTRL | GLED1_CTRL | RLED2_CTRL | GLED2_CTRL | RLED3_CTRL | GLED3_CTRL);
}

/*
********************************************************************************
�������ƣ�LED_1_NORMAL
����������LED1����д����
����������
����ֵ	��
********************************************************************************
*/
void LED_1_NORMAL(void)
{
	port1_buf |= GLED1_CTRL;
	port1_buf &= ~RLED1_CTRL;
}

/*
********************************************************************************
�������ƣ�LED_2_NORMAL
����������LED2����д����
����������
����ֵ	��
********************************************************************************
*/
void LED_2_NORMAL(void)
{
	port1_buf |= GLED2_CTRL;
	port1_buf &= ~RLED2_CTRL;
}

/*
********************************************************************************
�������ƣ�LED_3_NORMAL
����������LED3����д����
����������
����ֵ	��
********************************************************************************
*/
void LED_3_NORMAL(void)
{
	port1_buf |= GLED3_CTRL;
	port1_buf &= ~RLED3_CTRL;
}

/*
********************************************************************************
�������ƣ�LED_1_ALARM
����������LED1�澯д����
����������
����ֵ	��
********************************************************************************
*/
void LED_1_ALARM(void)
{
	port1_buf |= RLED1_CTRL;
	port1_buf &= ~GLED1_CTRL;
}

/*
********************************************************************************
�������ƣ�LED_2_ALARM
����������LED2�澯д����
����������
����ֵ	��
********************************************************************************
*/
void LED_2_ALARM(void)
{
	port1_buf |= RLED2_CTRL;
	port1_buf &= ~GLED2_CTRL;
}

/*
********************************************************************************
�������ƣ�LED_3_ALARM
����������LED3�澯д����
����������
����ֵ	��
********************************************************************************
*/
void LED_3_ALARM(void)
{
	port1_buf |= RLED3_CTRL;
	port1_buf &= ~GLED3_CTRL;
}

/*
********************************************************************************
�������ƣ�LED_1_FAULT
����������LED1����д����
����������
����ֵ	��
********************************************************************************
*/
void LED_1_FAULT(void)
{
	port1_buf |= (RLED1_CTRL | GLED1_CTRL);
}

/*
********************************************************************************
�������ƣ�LED_2_FAULT
����������LED2����д����
����������
����ֵ	��
********************************************************************************
*/
void LED_2_FAULT(void)
{
	port1_buf |= (RLED2_CTRL | GLED2_CTRL);
}

/*
********************************************************************************
�������ƣ�LED_IO_UPDATE
��������������LED IO ״̬��PCA9555
����������
����ֵ	��
********************************************************************************
*/
void LED_IO_UPDATE(void)
{
	pca9555_wr_port(PCA9555_I2C_ADDR_1, PCA9555_PORT1, port1_buf);	
}

/*
********************************************************************************
�������ƣ�ALL_RELAY_NORMAL
��������������RELAY NORMAL״̬д����
����������
����ֵ	��
********************************************************************************
*/
void ALL_RELAY_NORMAL(void)
{
	port0_buf |= ALM4_CTRL | ALM3_CTRL | ALM2_CTRL | ALM1_CTRL;
}

/*
********************************************************************************
�������ƣ�RELAY_1_NORMAL
����������RELAY1����״̬д����
����������
����ֵ	��
********************************************************************************
*/
void RELAY_1_NORMAL(void)
{
	port0_buf |= ALM1_CTRL;
}	

/*
********************************************************************************
�������ƣ�RELAY_2_NORMAL
����������RELAY2����״̬д����
����������
����ֵ	��
********************************************************************************
*/				
void RELAY_2_NORMAL(void)
{
	port0_buf |= ALM2_CTRL;
}

/*
********************************************************************************
�������ƣ�RELAY_3_NORMAL
����������RELAY3����״̬д����
����������
����ֵ	��
********************************************************************************
*/
void RELAY_3_NORMAL(void)
{
	port0_buf |= ALM3_CTRL;
}

/*
********************************************************************************
�������ƣ�RELAY_4_NORMAL
����������RELAY4����״̬д����
����������
����ֵ	��
********************************************************************************
*/
void RELAY_4_NORMAL(void)
{
	port0_buf |= ALM4_CTRL;
}

/*
********************************************************************************
�������ƣ�RELAY_1_ALARM
����������RELAY1����״̬д����
����������
����ֵ	��
********************************************************************************
*/
void RELAY_1_ALARM(void)
{
	port0_buf &= ~ALM1_CTRL;
}

/*
********************************************************************************
�������ƣ�RELAY_2_ALARM
����������RELAY2����״̬д����
����������
����ֵ	��
********************************************************************************
*/
void RELAY_2_ALARM(void)
{
	port0_buf &= ~ALM2_CTRL;
}

/*
********************************************************************************
�������ƣ�RELAY_3_ALARM
����������RELAY3����״̬д����
����������
����ֵ	��
********************************************************************************
*/
void RELAY_3_ALARM(void)
{
	port0_buf &= ~ALM3_CTRL;
}

/*
********************************************************************************
�������ƣ�RELAY_4_ALARM
����������RELAY4����״̬д����
����������
����ֵ	��
********************************************************************************
*/
void RELAY_4_ALARM(void)
{
	port0_buf &= ~ALM4_CTRL;
}

/*
********************************************************************************
�������ƣ�RELAY_IO_UPDATE
��������������RELAY IO ״̬��PCA9555
����������
����ֵ	��
********************************************************************************
*/
void RELAY_IO_UPDATE(void)
{
	pca9555_wr_port(PCA9555_I2C_ADDR_1, PCA9555_PORT0, port0_buf);	
}		

/*
********************************************************************************
�������ƣTTIM1_Config
����������TIM1ʱ��IO����
����������
����ֵ	��
********************************************************************************
*/
void TIM1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef __OLD_MCB_VER__
	/* ʹ��GPIOʱ�� */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE);
	
	/* ����GPIO�ܽŸ��� PC10 BEEP OUT*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
#else
	/* ʹ��GPIOʱ�� */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	
	/* ����GPIO�ܽŸ��� PA8 TIM1_CH1 PWM OUT*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);
#endif
}

/*
********************************************************************************
�������ƣ�TIM1_PWM_init
����������TIM1 PWM��ʼ��
����������
����ֵ	��
********************************************************************************
*/
void TIM1_PWM_init(uint16_t period, uint16_t pulse)
{
#ifndef __OLD_MCB_VER__
    
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* TIM1 ʱ��ʹ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM1->CR1 &= ~TIM_CR1_CEN; 
	
	/* Time ��ʱ��������*/
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //Time ��ʱ����Ϊ�����ؼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	/* ch1 PWM ģʽ���� */ 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	//TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	
	TIM_OCInitStructure.TIM_Pulse = pulse;						//ʹ��Ƶ��1����
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	/* TIM1 ������ʹ��*/
	//TIM_Cmd(TIM1, ENABLE);
    //TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	/* TIM1 �����ʹ�� */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);	
#endif
}
