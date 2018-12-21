/*
********************************************************************************
�ļ���	��mcb_hmu.c
�ļ����������ذ��˻�ģ�飬ע�⣺��Ƶ���Э��忪�����治ͬ,���Ͳ�ͬ��ʾ������ͬ
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __MCB_HMU_GLOBAL
#include "mcb_hmu.h"      
#include "lcd_if.h" 
#include "lcd_drv.h"  
#include "lcd_api.h"          
#include "key.h"
#include "ziku.h"  
#include "public.h"
#include <stdio.h>
#include <string.h>

/*
********************************************************************************
�������ƣ�mcb_hmu_init
�����������˻�ģ���ʼ��
����������
����ֵ	��
********************************************************************************
*/
void mcb_hmu_init(void) 
{
	mcb_hmu_task_flag = 0;
	mcb_hmu_refresh_flag = 0;
	mcb_hmu_bl_cnt = 0;
	mcb_hmu_auto_return_cnt = 0;
	mcb_hmu_disp_pages = PWR_ON_PAGE;	
	mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
	mcb_hmu_bl_close_flag = 0;
	mcb_hmu_0hour_clr_flag = 0;
	
	mcb_hmu_modify_mode = 0;
	mcb_hmu_para_index = 0;
	mcb_hmu_para_bit = 0;
	mcb_hmu_para_bak = 0;
	ant_used_flag = 11;
	ant_used_gain = 38;
	wire_used_att = 22;
	resv_para_def = 0;
	read_att_flag = 0;
	
	//lcd��ʼ��
	key_init();
	lcd_api_init();
	mcb_hmu_bl_state = 0;
	//����
	lcd_clear();
	lcd_display();	
	mcb_hmu_refresh_flag = 1;
	mcb_hmu_task_flag = 1;
}

/*
********************************************************************************
�������ƣ�mcb_hmu_task
�����������˻�ģ������ע�⣺��Ƶ���Э���,���Ͳ�ͬ��ʾ������ͬ�Ĵ��봦��
����������
����ֵ	��
********************************************************************************
*/
void mcb_hmu_task(void) 
{
	mcb_hmu_task_flag = 0;
	
	if(key_press_flag)								//����ɨ��
	{
		mcb_hmu_task_flag = 1;
		key_scan();		
		if(!key_press_flag)		
			mcb_hmu_task_flag = 0;
	}
	
	if(mcb_hmu_bl_close_flag)						//�ر��⴦��
	{
		mcb_hmu_bl_close_flag = 0;
		mcb_hmu_bl_state = 1;
		LCD_BL_OFF;	
	}
	
	//ÿ��0���LCD������ʾ��ͨ�������ߺ͵�Դ�ĸ澯�����Լ�����/GPS�����С�������㣬�˻�����Ҫר�Ŵ���ˢ�����ɣ��ɱ��������������
	//if(mcb_hmu_0hour_clr_flag)

	if((key_value != NO_KEY) || (mcb_hmu_refresh_flag)) 
	{
		mcb_hmu_refresh_flag = 0;
		//����ʾ����
		lcd_clear();
		//��ʾ����
		switch(mcb_hmu_disp_pages)
		{
			case PWR_ON_PAGE: 	hmu_pwr_on_page(); 		break;
			case MAIN_PAGE: 	hmu_main_page(); 		break;
			case SUB_1_PAGE: 	hmu_sub1_page(); 		break;
			case SUB_2_PAGE:	hmu_sub2_page(); 		break;
			case SUB_3_PAGE: 	hmu_sub3_page(); 		break;
			case CH1_TEST_PAGE: hmu_ch1_test_page(); 	break;
			case CH2_TEST_PAGE:	hmu_ch2_test_page(); 	break;
			case AUTO_TEST_PAGE:hmu_auto_test_page(); 	break;
			case DEV_FAULT_PAGE:hmu_dev_fault_page();	break;
			case DEV_PARA_PAGE:	hmu_dev_para_page();	break;
			default: 			hmu_main_page();		break;
		}
		//��ʾ��������ȿ����޸ĺʹ���ֿ�ִ�У���ʱһ��ִ��
        #ifdef __LCD_TOL_ERR__
        //�ݴ���Ԥ��Һ����ѹ����ʱ���ֵ��쳣��ʾ
            lcd_re_init();   
        #endif
		lcd_display();
	}
}

/*
********************************************************************************
�������ƣ�hmu_pwr_on_page
��������������ҳ��
����������
����ֵ	��
********************************************************************************
*/
void hmu_pwr_on_page(void)
{
	if(rfb_or_xyb == RFB)
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			pwr_on_page_rfb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			pwr_on_page_rfb_jx2_disp();
		//�ж��Ƿ������������
		if(!mr_comm_msg.cur_mode)
		{
			key_value = NO_KEY;						//��ʱ���԰���
			mcb_hmu_disp_pages_bak = MAIN_PAGE;		//���������棬���ؽ���Ĭ�ϻ���������
			mcb_hmu_disp_pages = MAIN_PAGE;			
		}
	}
	else
	if(rfb_or_xyb == XYB) 
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
		{
			pwr_on_page_xyb_jx1_disp();
		}
		else
		if(dev_type == DEV_TYPE_2) 
		{
			pwr_on_page_xyb_jx2_disp();
		}			
	}
	//ҳ�水������
	pwr_on_page_key_deal();
}

/*
********************************************************************************
�������ƣ�pwr_on_page_rfb_jx1_disp
��������������ҳ����Ƶ�����1
����������
����ֵ	��
********************************************************************************
*/
void pwr_on_page_rfb_jx1_disp(void)
{
	static uint8_t cnt = 0;
	const uint8_t **pt = NULL;
	
	lcd_draw_graph(0, 0, 128, 56, ZK_GPS1);
	//��̬����......
	cnt++;
	if(cnt == 1)
		pt = zyd_1dot;
	else
	if(cnt == 2)
		pt = zyd_2dot;
	else
	if(cnt == 3)
		pt = zyd_3dot;
	else
	if(cnt == 4)
		pt = zyd_4dot;
	else
	if(cnt == 5)
		pt = zyd_5dot;
	else
	if(cnt == 6)
		pt = zyd_6dot;
	else
	if(cnt >= 7)
	{
		cnt = 0;
		pt = zyd_null;	
	}
	lcd_draw_char_5x7(54, 40, 3, pt);			
}

/*
********************************************************************************
�������ƣ�pwr_on_page_rfb_jx2_disp
��������������ҳ����Ƶ�����2
����������
����ֵ	��
********************************************************************************
*/
void pwr_on_page_rfb_jx2_disp(void)
{
	static uint8_t cnt = 0, cnt1 = 0;
	const uint8_t **pt = NULL;
    
    //���ο�
    //lcd_draw_rect(0, 0, 128, 64);
    
	lcd_draw_graph(0, 14, 100, 24, ZK_SZTB);
	lcd_draw_graph(24, 0, 128, 24, ZK_SXZNFLXT);						
	//��̬����>>>>>>
	cnt++;
	if(cnt == 1)
		pt = yjt_1dot;
	else
	if(cnt == 2)
		pt = yjt_2dot;
	else
	if(cnt == 3)
		pt = yjt_3dot;
	else
	if(cnt == 4)
		pt = yjt_4dot;
	else
	if(cnt == 5)
		pt = yjt_5dot;
	else
	if(cnt == 6)
		pt = yjt_6dot;
	else
	if(cnt >= 7)
	{
		cnt = 0;
		pt = yjt_null;	
	}
	lcd_draw_char_5x7(53, 40, 3, pt);
	//lcd_draw_hline(26, 24, 78); //LINE
	lcd_draw_hline(27, 24, 78);
	//BTS ICON
	lcd_draw_graph(2, 2, 16, 16, bts_icon16x16);
	//WEI XING ICON
	cnt1++;
	if(cnt1 < 3)
		lcd_draw_graph(2, 110, 16, 16, weixing_icon16x16);
	else
	if(cnt1 < 5)	
		lcd_draw_graph(2, 110, 16, 16, weixing_icon16x16 + 32);
	else
	if(cnt1 < 7)	
		lcd_draw_graph(2, 110, 16, 16, weixing_icon16x16 + 64);
	else
	if(cnt1 < 9)	
	{
		lcd_draw_graph(2, 110, 16, 16, weixing_icon16x16 + 96);
		if(cnt1 == 8)
			cnt1 = 0;	
	}
}	

/*
********************************************************************************
�������ƣ�pwr_on_page_xyb_jx1_disp
��������������ҳ��Э������1
����������
����ֵ	��
********************************************************************************
*/
void pwr_on_page_xyb_jx1_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	pwr_on_page_rfb_jx1_disp();
	lcd_draw_hline(62, 126, 1);
}	

/*
********************************************************************************
�������ƣ�pwr_on_page_xyb_jx2_disp
��������������ҳ��Э������2
����������
����ֵ	��
********************************************************************************
*/
void pwr_on_page_xyb_jx2_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	pwr_on_page_rfb_jx2_disp();
	lcd_draw_hline(62, 126, 1);
}

/*
********************************************************************************
�������ƣ�pwr_on_page_key_deal
��������������ҳ�水��������
����������
����ֵ	��
********************************************************************************
*/
void pwr_on_page_key_deal(void)
{
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/
	switch(key_value)
	{
		case UP_L_KEY: 
			mr_in_out_manu_mode = 1;
			mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;	
			//ͨ����·״̬����δ�жϣ���ͬ
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;	//����ͨ��1����
			mcb_hmu_disp_pages = CH1_TEST_PAGE;					
			break;
		case DOWN_L_KEY: 
			mr_in_out_manu_mode	= 2;
			mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;	
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;	//����ͨ��2����
			mcb_hmu_disp_pages = CH2_TEST_PAGE;		
			break;	
		case LEFT_L_KEY: 
			mr_in_out_auto_mode = 1;
			mr_comm_sys_tx_cmd2 = QQJRTCZDMS_CMD_82;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;	//�����Զ�ͨ������
			mcb_hmu_disp_pages = AUTO_TEST_PAGE;					
			break;		
		case RIGHT_S_KEY: 
			beep_en_on_off();								//BEEPʹ��/�ر�
			break;	
		case UD_L_KEY: 
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
			mcb_hmu_disp_pages = DEV_FAULT_PAGE;			//�����豸״̬ҳ��										
			break;	
		case OK_L_KEY: 
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
			mcb_hmu_disp_pages = MAIN_PAGE;					//����ok����Ҳ�ɷ��������棬����
		default: break;
	}
	key_value = NO_KEY;			
}			

/*
********************************************************************************
�������ƣ�hmu_main_page
������������ҳ�棬����ṹ���ٴη�װ�������Ż�
����������
����ֵ	��
********************************************************************************
*/
void hmu_main_page(void)
{
	if(rfb_or_xyb == RFB)
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			main_page_rfb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			main_page_rfb_jx2_disp();
	}
	else
	if(rfb_or_xyb == XYB) 
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			main_page_xyb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			main_page_xyb_jx2_disp();		
	}
	//ҳ�水������
	main_page_key_deal();
}

/*
********************************************************************************
�������ƣ�main_page_rfb_jx1_disp
������������ҳ����Ƶ�����1
����������
����ֵ	��
********************************************************************************
*/
void main_page_rfb_jx1_disp(void)
{
	uint8_t i;
	const uint8_t *pt[11];
	char str[11];
	
	//1��ͨ����
	lcd_draw_char_8x16(3, 14, 0, char1_8x16_xiao5);
	lcd_draw_hz_nx16_1(4, 24, 2, haotongdao1);
	lcd_draw_char_8x16(2, 72, 0, charmh_8x16);
	//�������澯��δ��
	if((!ant1_rf_state) && (!ch1_rf_state))
		lcd_draw_hz_nx16_1(4, 82, 2, zhengchang1);
	else
	if(ant1_rf_state == 1)
		lcd_draw_hz_nx16_1(4, 82, 2, weijie1);
	else
	if((ant1_rf_state == 2) || (ch1_rf_state))
		lcd_draw_hz_nx16_1(4, 82, 2, gaojing1);					
	//2��ͨ����
	lcd_draw_char_8x16(20, 14, 0, char2_8x16_xiao5);
	lcd_draw_hz_nx16_1(21, 24, 2, haotongdao1);
	lcd_draw_char_8x16(19, 72, 0, charmh_8x16);
	//�������澯��δ��
	if((!ant2_rf_state) && (!ch2_rf_state))
		lcd_draw_hz_nx16_1(21, 82, 2, zhengchang1);
	else
	if(ant2_rf_state == 1)
		lcd_draw_hz_nx16_1(21, 82, 2, weijie1);
	else
	if((ant2_rf_state == 2) || (ch2_rf_state))
		lcd_draw_hz_nx16_1(21, 82, 2, gaojing1);	
		
	//���ȣ�γ�ȣ�������ͼ��
	//
	sprintf(str, "%03.6f", mr_comm_msg.longitude);
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] != '.')
			pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
		else
			pt[i] = &ascii_5x7[CHAR_DIAN_INDEX][0];
	}
	pt[i] = NULL;
	if(i == 8)
		lcd_draw_char_5x7(42, 43, 1, pt);	
	else
	if(i == 9)	
		lcd_draw_char_5x7(42, 37, 1, pt);	
	else	
	if(i == 10)
		lcd_draw_char_5x7(42, 31, 1, pt);	
	//
	if((mr_comm_msg.e_or_w == 'E') || (mr_comm_msg.e_or_w == 'W'))
	{
		pt[0] = &ascii_5x7[CHAR_A_INDEX + mr_comm_msg.e_or_w - 'A'][0];
	}
	else
	{
		pt[0] = &ascii_5x7[CHAR_A_INDEX + 'E' - 'A'][0];
	}
	pt[1] = NULL;	
	lcd_draw_char_5x7(42, 10, 1, pt);	
	//	
	sprintf(str, "%02.6f", mr_comm_msg.latitude);
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] != '.')
			pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
		else
			pt[i] = &ascii_5x7[CHAR_DIAN_INDEX][0];
	}
	pt[i] = NULL;	
	if(i == 8)
		lcd_draw_char_5x7(53, 43, 1, pt);	
	else
	if(i == 9)	
		lcd_draw_char_5x7(53, 37, 1, pt);				
	//
	if((mr_comm_msg.s_or_n == 'N') || (mr_comm_msg.s_or_n == 'S'))
	{
		pt[0] = &ascii_5x7[CHAR_A_INDEX + mr_comm_msg.s_or_n - 'A'][0];
	}
	else
	{
		pt[0] = &ascii_5x7[CHAR_A_INDEX + 'N' - 'A'][0];
			
	}
	pt[1] = NULL;	
	lcd_draw_char_5x7(53, 10, 1, pt);	
	
	if(beep_ctrl_en)
		lcd_draw_graph(43, 105, 16, 16, BEEP_on_icon_16x16);
	else
		lcd_draw_graph(43, 105, 16, 16, beep_off_icon_16x16);			
}

/*
********************************************************************************
�������ƣ�main_page_rfb_jx2_disp
������������ҳ����Ƶ�����2
����������
����ֵ	��
********************************************************************************
*/
void main_page_rfb_jx2_disp(void)
{
    //���ο�
    lcd_draw_rect(0, 0, 128, 64);    
    
	//ͨ��1��
	lcd_draw_hz_nx16_2(7, 2, 0, tongdao2);
	lcd_draw_char_8x16(6, 26, 0, char1_8x16);
	lcd_draw_char_5x7(11, 34, 0, charmh_5x7);
	//�ж�״̬
	if(ch1_rf_state)
		lcd_draw_hz_nx16_2(7, 39, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(7, 39, 0, zhengchang2);
	
	//ͨ��2��
	lcd_draw_hz_nx16_2(7, 66, 0, tongdao2);
	lcd_draw_char_8x16(6, 90, 0, char2_8x16);
	lcd_draw_char_5x7(11, 98, 0, charmh_5x7);
	//�ж�״̬
	if(ch2_rf_state)
		lcd_draw_hz_nx16_2(7, 103, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(7, 103, 0, zhengchang2);
	
	//����1��
	lcd_draw_hz_nx16_2(26, 2, 0, tianxian2);
	lcd_draw_char_8x16(25, 26, 0, char1_8x16);
	lcd_draw_char_5x7(30, 34, 0, charmh_5x7);
	//�ж�״̬
	if(ant1_rf_state)
		lcd_draw_hz_nx16_2(26, 39, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(26, 39, 0, zhengchang2);
	
	//����2��
	lcd_draw_hz_nx16_2(26, 66, 0, tianxian2);
	lcd_draw_char_8x16(25, 90, 0, char2_8x16);
	lcd_draw_char_5x7(30, 98, 0, charmh_5x7);
	//�ж�״̬
	if(ant2_rf_state)
		lcd_draw_hz_nx16_2(26, 103, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(26, 103, 0, zhengchang2);
	
	//��Դ1��
	lcd_draw_hz_nx16_2(45, 2, 0, dianyuan2);
	lcd_draw_char_8x16(44, 26, 0, char1_8x16);
	lcd_draw_char_5x7(49, 34, 0, charmh_5x7);
	//
	if(pwr1_pd8_state)
		lcd_draw_hz_nx16_2(45, 39, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(45, 39, 0, zhengchang2);
	
	//��Դ2��
	lcd_draw_hz_nx16_2(45, 66, 0, dianyuan2);
	lcd_draw_char_8x16(44, 90, 0, char2_8x16);
	lcd_draw_char_5x7(49, 98, 0, charmh_5x7);
	//�ж�״̬
	if(pwr2_pd8_state)
		lcd_draw_hz_nx16_2(45, 103, 0, gaojing2);	
	else
		lcd_draw_hz_nx16_2(45, 103, 0, zhengchang2);		
}

/*
********************************************************************************
�������ƣ�main_page_rfb_jx1_disp
������������ҳ��Э������1
����������
����ֵ	��
********************************************************************************
*/
void main_page_xyb_jx1_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	main_page_rfb_jx1_disp();
	lcd_draw_hline(62, 126, 1);
}

/*
********************************************************************************
�������ƣ�main_page_rfb_jx2_disp
������������ҳ��Э������2
����������
����ֵ	��
********************************************************************************
*/
void main_page_xyb_jx2_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	main_page_rfb_jx2_disp();
	lcd_draw_hline(62, 126, 1);
}

/*
********************************************************************************
�������ƣ�main_page_key_deal
������������ҳ�水������
����������
����ֵ	��
********************************************************************************
*/
void main_page_key_deal(void)
{
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/
	switch(key_value)
	{
		case UP_L_KEY: 
			mr_in_out_manu_mode = 1;
			mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;	
			//ͨ����·״̬����δ�жϣ���ͬ
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;	//����ͨ��1����
			mcb_hmu_disp_pages = CH1_TEST_PAGE;					
			break;
		case DOWN_L_KEY: 
			mr_in_out_manu_mode	= 2;
			mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;	
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;	//����ͨ��2����
			mcb_hmu_disp_pages = CH2_TEST_PAGE;		
			break;	
		case LEFT_L_KEY: 
			mr_in_out_auto_mode = 1;
			mr_comm_sys_tx_cmd2 = QQJRTCZDMS_CMD_82;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;	//�����Զ�ͨ������
			mcb_hmu_disp_pages = AUTO_TEST_PAGE;					
			break;		
		case RIGHT_S_KEY: 
			beep_en_on_off();								//BEEPʹ��/�ر�
			break;		
		case OK_S_KEY: 										//ҳ���л�ģʽ������1 3��ҳ���л�������2 4��ҳ���л�
			if(mcb_hmu_disp_pages == MAIN_PAGE)
			{
				mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
				mcb_hmu_disp_pages = SUB_1_PAGE;		
			}
			else
			if(mcb_hmu_disp_pages == SUB_1_PAGE)
			{
				mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
				mcb_hmu_disp_pages = SUB_2_PAGE;		
			}
			else
			if(mcb_hmu_disp_pages == SUB_2_PAGE)
			{
				if(dev_type == DEV_TYPE_1)
				{
					mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
					mcb_hmu_disp_pages = MAIN_PAGE;
				}		
				else
				if(dev_type == DEV_TYPE_2)
				{
					mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
					mcb_hmu_disp_pages = SUB_3_PAGE;
				}	
			}	
			else
			if(mcb_hmu_disp_pages == SUB_3_PAGE)
			{
				mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
				mcb_hmu_disp_pages = MAIN_PAGE;		
			}		
			break;	
		case RETURN_S_KEY: 
			if(dev_type == DEV_TYPE_2) 						//return ���������棬����2
			{
				mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
				mcb_hmu_disp_pages = MAIN_PAGE;	
			}
			break;	
		case UD_L_KEY: 
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
			mcb_hmu_disp_pages = DEV_FAULT_PAGE;			//�����豸״̬ҳ��										
			break;	
		default: break;
	}
	key_value = NO_KEY;		
}

/*
********************************************************************************
�������ƣ�hmu_sub1_page
������������ҳ��1
����������
����ֵ	��
********************************************************************************
*/
void hmu_sub1_page(void)
{
	if(rfb_or_xyb == RFB)
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			sub1_page_rfb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			sub1_page_rfb_jx2_disp();
	}
	else
	if(rfb_or_xyb == XYB) 
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			sub1_page_xyb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			sub1_page_xyb_jx2_disp();		
	}
	//ҳ�水������
	sub1_page_key_deal();
}

/*
********************************************************************************
�������ƣ�sub1_page_rfb_jx1_disp
����������SUB1ҳ����Ƶ�����1
����������
����ֵ	��
********************************************************************************
*/
void sub1_page_rfb_jx1_disp(void)
{
	uint8_t i;
	const uint8_t *pt[4];
	char str[4];
	
	//����1��	�˴�����1ʹ�û���2���壬����һ�зŲ���
	lcd_draw_hz_nx16_2(5, 1, 0, tianxian2);
	lcd_draw_char_8x16(4, 25, 0, char1_8x16);
	lcd_draw_char_5x7(9, 33, 0, charmh_5x7);
	//�ж�״̬
	if(ant1_rf_state)
		lcd_draw_hz_nx16_2(5, 38, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(5, 38, 0, zhengchang2);
	
	//����2��
	lcd_draw_hz_nx16_2(5, 67, 0, tianxian2);
	lcd_draw_char_8x16(4, 91, 0, char2_8x16);
	lcd_draw_char_5x7(9, 99, 0, charmh_5x7);
	//�ж�״̬
	if(ant2_rf_state)
		lcd_draw_hz_nx16_2(5, 104, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(5, 104, 0, zhengchang2);
	
	//��Դ1��
	lcd_draw_hz_nx16_2(22, 1, 0, dianyuan2);
	lcd_draw_char_8x16(21, 25, 0, char1_8x16);
	lcd_draw_char_5x7(26, 33, 0, charmh_5x7);
	//
	if(pwr1_pd8_state)
		lcd_draw_hz_nx16_2(22, 38, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(22, 38, 0, zhengchang2);
	
	//��Դ2��
	lcd_draw_hz_nx16_2(22, 67, 0, dianyuan2);
	lcd_draw_char_8x16(21, 91, 0, char2_8x16);
	lcd_draw_char_5x7(26, 99, 0, charmh_5x7);
	//�ж�״̬
	if(pwr2_pd8_state)
		lcd_draw_hz_nx16_2(22, 104, 0, gaojing2);	
	else
		lcd_draw_hz_nx16_2(22, 104, 0, zhengchang2);		
		
	//BD GPS ��ǰ ��� ��С�� ��ʾ
	lcd_draw_char_5x7(40, 4, 1, charBD_5x7);
	sprintf(str, "%02d", cur_bd_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(40, 28, 1, pt);	
	lcd_draw_char_5x7(40, 46, 1, charMaxmh_5x7);
	sprintf(str, "%02d", cur_bd_max_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(40, 70, 1, pt);	
	lcd_draw_char_5x7(40, 88, 1, charMinmh_5x7);
	sprintf(str, "%02d", cur_bd_min_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(40, 112, 1, pt);				
	lcd_draw_char_5x7(52, 4, 1, charGPS_5x7);
	sprintf(str, "%02d", cur_gps_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(52, 28, 1, pt);		
	lcd_draw_char_5x7(52, 46, 1, charMaxmh_5x7);
	sprintf(str, "%02d", cur_gps_max_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(52, 70, 1, pt);	
	lcd_draw_char_5x7(52, 88, 1, charMinmh_5x7);
	sprintf(str, "%02d", cur_gps_min_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(52, 112, 1, pt);					
}

/*
********************************************************************************
�������ƣ�sub1_page_rfb_jx2_disp
����������SUB1ҳ����Ƶ�����2
����������
����ֵ	��
********************************************************************************
*/
void sub1_page_rfb_jx2_disp(void)
{
	uint8_t i;
	const uint8_t *pt[4];
	char str[4];

    //���ο�
    lcd_draw_rect(0, 0, 128, 64);
	
	//ͨ��1��
	lcd_draw_hz_nx16_2(7, 2, 0, tongdao2);
	lcd_draw_char_8x16(6, 26, 0, char1_8x16);
	lcd_draw_char_5x7(11, 34, 0, charmh_5x7);
	//��������
	sprintf(str, "%03d", ch1_alm_cnt);
	for(i = 0; i < 3; i++)
	{
		pt[i] = &ascii_8x16[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[3] = NULL;
	lcd_draw_char_8x16(6, 39, 0, pt);
	
	//ͨ��2��
	lcd_draw_hz_nx16_2(7, 66, 0, tongdao2);
	lcd_draw_char_8x16(6, 90, 0, char2_8x16);
	lcd_draw_char_5x7(11, 98, 0, charmh_5x7);
	//��������
	sprintf(str, "%03d", ch2_alm_cnt);
	for(i = 0; i < 3; i++)
	{
		pt[i] = &ascii_8x16[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[3] = NULL;
	lcd_draw_char_8x16(6, 103, 0, pt);
	
	//����1��
	lcd_draw_hz_nx16_2(26, 2, 0, tianxian2);
	lcd_draw_char_8x16(25, 26, 0, char1_8x16);
	lcd_draw_char_5x7(30, 34, 0, charmh_5x7);
	//��������
	sprintf(str, "%03d", ant1_alm_cnt);
	for(i = 0; i < 3; i++)
	{
		pt[i] = &ascii_8x16[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[3] = NULL;
	lcd_draw_char_8x16(25, 39, 0, pt);
	
	//����2��
	lcd_draw_hz_nx16_2(26, 66, 0, tianxian2);
	lcd_draw_char_8x16(25, 90, 0, char2_8x16);
	lcd_draw_char_5x7(30, 98, 0, charmh_5x7);
	//��������
	sprintf(str, "%03d", ant2_alm_cnt);
	for(i = 0; i < 3; i++)
	{
		pt[i] = &ascii_8x16[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[3] = NULL;
	lcd_draw_char_8x16(25, 103, 0, pt);
	
	//��Դ1��
	lcd_draw_hz_nx16_2(45, 2, 0, dianyuan2);
	lcd_draw_char_8x16(44, 26, 0, char1_8x16);
	lcd_draw_char_5x7(49, 34, 0, charmh_5x7);
	//��������
	sprintf(str, "%03d", pwr1_alm_cnt);
	for(i = 0; i < 3; i++)
	{
		pt[i] = &ascii_8x16[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[3] = NULL;
	lcd_draw_char_8x16(44, 39, 0, pt);
	
	//��Դ2��
	lcd_draw_hz_nx16_2(45, 66, 0, dianyuan2);
	lcd_draw_char_8x16(44, 90, 0, char2_8x16);
	lcd_draw_char_5x7(49, 98, 0, charmh_5x7);
	//��������
	sprintf(str, "%03d", pwr2_alm_cnt);
	for(i = 0; i < 3; i++)
	{
		pt[i] = &ascii_8x16[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[3] = NULL;
	lcd_draw_char_8x16(44, 103, 0, pt);			
}

/*
********************************************************************************
�������ƣ�sub1_page_xyb_jx1_disp
����������SUB1ҳ��Э������1
����������
����ֵ	��
********************************************************************************
*/
void sub1_page_xyb_jx1_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	sub1_page_rfb_jx1_disp();	
	lcd_draw_hline(62, 126, 1);
}

/*
********************************************************************************
�������ƣ�sub1_page_xyb_jx2_disp
����������SUB1ҳ��Э������2
����������
����ֵ	��
********************************************************************************
*/
void sub1_page_xyb_jx2_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	sub1_page_rfb_jx2_disp();	
	lcd_draw_hline(62, 126, 1);
}

/*
********************************************************************************
�������ƣ�sub1_page_key_deal
����������SUB1ҳ�水������
����������
����ֵ	��
********************************************************************************
*/
void sub1_page_key_deal(void)
{
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/
	switch(key_value)
	{
		case UP_L_KEY: 
			mr_in_out_manu_mode = 1;
			mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;	
			//ͨ����·״̬����δ�жϣ���ͬ
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;	//����ͨ��1����
			mcb_hmu_disp_pages = CH1_TEST_PAGE;					
			break;
		case DOWN_L_KEY: 
			mr_in_out_manu_mode	= 2;
			mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;	
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;	//����ͨ��2����
			mcb_hmu_disp_pages = CH2_TEST_PAGE;		
			break;	
		case LEFT_L_KEY: 
			mr_in_out_auto_mode = 1;
			mr_comm_sys_tx_cmd2 = QQJRTCZDMS_CMD_82;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;	//�����Զ�ͨ������
			mcb_hmu_disp_pages = AUTO_TEST_PAGE;					
			break;		
		case RIGHT_S_KEY: 
			beep_en_on_off();								//BEEPʹ��/�ر�
			break;		
		case OK_S_KEY: 										//ҳ���л�ģʽ������1 3��ҳ���л�������2 4��ҳ���л�
			if(mcb_hmu_disp_pages == MAIN_PAGE)
			{
				mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
				mcb_hmu_disp_pages = SUB_1_PAGE;		
			}
			else
			if(mcb_hmu_disp_pages == SUB_1_PAGE)
			{
				mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
				mcb_hmu_disp_pages = SUB_2_PAGE;		
			}
			else
			if(mcb_hmu_disp_pages == SUB_2_PAGE)
			{
				if(dev_type == DEV_TYPE_1)
				{
					mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
					mcb_hmu_disp_pages = MAIN_PAGE;
				}		
				else
				if(dev_type == DEV_TYPE_2)
				{
					mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
					mcb_hmu_disp_pages = SUB_3_PAGE;
				}	
			}	
			else
			if(mcb_hmu_disp_pages == SUB_3_PAGE)
			{
				mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
				mcb_hmu_disp_pages = MAIN_PAGE;		
			}		
			break;	
		case RETURN_S_KEY: 
			if(dev_type == DEV_TYPE_2) 						//return ���������棬����2
			{
				mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
				mcb_hmu_disp_pages = MAIN_PAGE;	
			}
			break;	
		case UD_L_KEY: 
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
			mcb_hmu_disp_pages = DEV_FAULT_PAGE;			//�����豸״̬ҳ��										
			break;	
		case OK_L_KEY: 
			mcb_hmu_disp_pages_bak = mcb_hmu_disp_pages;
			mcb_hmu_disp_pages = MAIN_PAGE;					//����ok����Ҳ�ɷ���������
		default: break;
	}
	key_value = NO_KEY;				
}


/*
********************************************************************************
�������ƣ�hmu_sub2_page
������������ҳ��2
����������
����ֵ	��
********************************************************************************
*/
void hmu_sub2_page(void)
{
	if(rfb_or_xyb == RFB)
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			sub2_page_rfb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			sub2_page_rfb_jx2_disp();
	}
	else
	if(rfb_or_xyb == XYB) 
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			sub2_page_xyb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			sub2_page_xyb_jx2_disp();		
	}
	//ҳ�水������
	sub2_page_key_deal();	
}

/*
********************************************************************************
�������ƣ�sub2_page_rfb_jx1_disp
������������ҳ��2��Ƶ�����1
����������
����ֵ	��
********************************************************************************
*/
void sub2_page_rfb_jx1_disp(void)
{
	uint8_t i, flag, cnt1, cnt2;
	const uint8_t *pt[9];
	char str[9], str1[9];
	
	//ͨ��1��
	lcd_draw_hz_nx16_2(7, 1, 0, tongdao2);
	lcd_draw_char_8x16(6, 25, 0, char1_8x16);
	lcd_draw_char_5x7(11, 33, 0, charmh_5x7);
	//��������
	sprintf(str, "%03d", ch1_alm_cnt);
	for(i = 0; i < 3; i++)
	{
		pt[i] = &ascii_8x16[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[3] = NULL;
	lcd_draw_char_8x16(6, 38, 0, pt);
	
	//ͨ��2��
	lcd_draw_hz_nx16_2(7, 67, 0, tongdao2);
	lcd_draw_char_8x16(6, 91, 0, char2_8x16);
	lcd_draw_char_5x7(11, 99, 0, charmh_5x7);
	//��������
	sprintf(str, "%03d", ch2_alm_cnt);
	for(i = 0; i < 3; i++)
	{
		pt[i] = &ascii_8x16[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[3] = NULL;
	lcd_draw_char_8x16(6, 104, 0, pt);
	
	//�������������/�澯
	lcd_draw_hz_nx16_2(26, 1, 0, zhujishuchu2);
	lcd_draw_char_5x7(30, 49, 0, charmh_5x7);
	if(mpd8_out_state)
		lcd_draw_hz_nx16_2(26, 55, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(26, 55, 0, zhengchang2);	
	//�ӻ�������޴ӻ�/����/�澯 P(8*16)12345678(5*7)
	lcd_draw_hz_nx16_2(45, 1, 0, congjishuchu2);
	lcd_draw_char_5x7(49, 49, 0, charmh_5x7);
#ifdef __TEST_HMU__	
	lcd_draw_hz_nx16_2(45, 55, 0, zhengchang2);
	lcd_draw_char_5x7(47, 79, 0, charP_5x7);	
	lcd_draw_char_5x7(49, 85, 0, char1_8_5x7);
#else	
	if(!mcb_cur_pd8_slave_num)
		lcd_draw_hz_nx16_2(45, 55, 0, wucongji2);	
	else
	if(mcb_cur_pd8_slave_num < 9)
	{
		flag = 0;
		cnt1 = 0;
		cnt2 = 0;
		for(i = 0; i < mcb_cur_pd8_slave_num; i++)
		{
			if(spd8_out_state[mcb_cur_valid_slave_addr[i] - 1])
			{
				if(mcb_cur_valid_slave_addr[i])
				{
					flag = 1;
					str[cnt1++] = mcb_cur_valid_slave_addr[i];							
				}
			}
			else
			{
				if(mcb_cur_valid_slave_addr[i])
					str1[cnt2++] = mcb_cur_valid_slave_addr[i];	
			}	
		}
		str[cnt1] = NULL;
		str1[cnt2] = NULL;
			
		if(flag)
		{
			lcd_draw_hz_nx16_2(45, 55, 0, gaojing2);
			lcd_draw_char_5x7(47, 79, 0, charP_5x7);
			for(i = 0; i < cnt1; i++)
			{
				pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i]][0];
			}
			pt[cnt1] = NULL;						
			lcd_draw_char_5x7(49, 85, 0, pt);	
		}
		else
		if(cnt2)
		{
			lcd_draw_hz_nx16_2(45, 55, 0, zhengchang2);	
			lcd_draw_char_5x7(47, 79, 0, charP_5x7);
			for(i = 0; i < cnt2; i++)
			{
				pt[i] = &ascii_5x7[CHAR_0_INDEX + str1[i]][0];
			}
			pt[cnt2] = NULL;								
			lcd_draw_char_5x7(49, 85, 0, pt);	
		}			
	}
#endif
}

/*
********************************************************************************
�������ƣ�sub2_page_rfb_jx2_disp
������������ҳ��2��Ƶ�����2
����������
����ֵ	��
********************************************************************************
*/
void sub2_page_rfb_jx2_disp(void)
{
	uint8_t i;
	const uint8_t *pt[11];
	char str[11];

    //���ο�
    lcd_draw_rect(0, 0, 128, 64);
	
	//���ȣ�γ�ȣ�������ͼ��
	//
	sprintf(str, "%03.6f", mr_comm_msg.longitude);
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] != '.')
			pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
		else
			pt[i] = &ascii_5x7[CHAR_DIAN_INDEX][0];
	}
	pt[i] = NULL;
	if(i == 8)
		lcd_draw_char_5x7(6, 31, 1, pt);	
	else
	if(i == 9)	
		lcd_draw_char_5x7(6, 25, 1, pt);	
	else	
	if(i == 10)
		lcd_draw_char_5x7(6, 19, 1, pt);	
	//
	if((mr_comm_msg.e_or_w == 'E') || (mr_comm_msg.e_or_w == 'W'))
	{
		pt[0] = &ascii_5x7[CHAR_A_INDEX + mr_comm_msg.e_or_w - 'A'][0];
	}
	else
	{
		pt[0] = &ascii_5x7[CHAR_A_INDEX + 'E' - 'A'][0];
	}
	pt[1] = NULL;	
	lcd_draw_char_5x7(6, 88, 1, pt);	
	//	
	sprintf(str, "%02.6f", mr_comm_msg.latitude);
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] != '.')
			pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
		else
			pt[i] = &ascii_5x7[CHAR_DIAN_INDEX][0];
	}
	pt[i] = NULL;	
	if(i == 8)
		lcd_draw_char_5x7(20, 31, 1, pt);	
	else
	if(i == 9)	
		lcd_draw_char_5x7(20, 25, 1, pt);				
	//
	if((mr_comm_msg.s_or_n == 'N') || (mr_comm_msg.s_or_n == 'S'))
	{
		pt[0] = &ascii_5x7[CHAR_A_INDEX + mr_comm_msg.s_or_n - 'A'][0];
	}
	else
	{
		pt[0] = &ascii_5x7[CHAR_A_INDEX + 'N' - 'A'][0];
			
	}
	pt[1] = NULL;	
	lcd_draw_char_5x7(20, 88, 1, pt);	
	
	if(beep_ctrl_en)
		lcd_draw_graph(13, 105, 12, 8, BEEP_on_icon_12x8);
	else
		lcd_draw_graph(13, 105, 12, 8, beep_off_icon_12x8);	
	
	//BD GPS ��ǰ ��� ��С�� ��ʾ
	lcd_draw_char_5x7(34, 5, 1, charBD_5x7);
	sprintf(str, "%02d", cur_bd_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(34, 29, 1, pt);	
	lcd_draw_char_5x7(34, 47, 1, charMaxmh_5x7);
	sprintf(str, "%02d", cur_bd_max_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(34, 71, 1, pt);	
	lcd_draw_char_5x7(34, 89, 1, charMinmh_5x7);
	sprintf(str, "%02d", cur_bd_min_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(34, 113, 1, pt);				
	lcd_draw_char_5x7(48, 5, 1, charGPS_5x7);
	sprintf(str, "%02d", cur_gps_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(48, 29, 1, pt);		
	lcd_draw_char_5x7(48, 47, 1, charMaxmh_5x7);
	sprintf(str, "%02d", cur_gps_max_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(48, 71, 1, pt);	
	lcd_draw_char_5x7(48, 89, 1, charMinmh_5x7);
	sprintf(str, "%02d", cur_gps_min_star);
	for(i = 0; i < 2; i++)
	{
		pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i] - '0'][0];
	}
	pt[2] = NULL;
	lcd_draw_char_5x7(48, 113, 1, pt);							
}

/*
********************************************************************************
�������ƣ�sub2_page_xyb_jx1_disp
������������ҳ��2Э������2
����������
����ֵ	��
********************************************************************************
*/
void sub2_page_xyb_jx1_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	sub2_page_rfb_jx1_disp();	
	lcd_draw_hline(62, 126, 1);
}

/*
********************************************************************************
�������ƣ�sub2_page_xyb_jx2_disp
������������ҳ��2Э������2
����������
����ֵ	��
********************************************************************************
*/
void sub2_page_xyb_jx2_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	sub2_page_rfb_jx2_disp();	
	lcd_draw_hline(62, 126, 1);
}

/*
********************************************************************************
�������ƣ�sub2_page_key_deal
������������ҳ��2��������
����������
����ֵ	��
********************************************************************************
*/
void sub2_page_key_deal(void)
{
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/
	sub1_page_key_deal();
}


/*
********************************************************************************
�������ƣ�hmu_sub3_page
������������ҳ��3������1û�У�����2��
����������
����ֵ	��
********************************************************************************
*/
void hmu_sub3_page(void)
{
	if(rfb_or_xyb == RFB)
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			sub3_page_rfb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			sub3_page_rfb_jx2_disp();
	}
	else
	if(rfb_or_xyb == XYB) 
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			sub3_page_xyb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			sub3_page_xyb_jx2_disp();		
	}
	//ҳ�水������
	sub3_page_key_deal();		
}

/*
********************************************************************************
�������ƣ�sub3_page_rfb_jx1_disp
������������ҳ��3��Ƶ�����1����
����������
����ֵ	��
********************************************************************************
*/
void sub3_page_rfb_jx1_disp(void)
{
	//exception deal
	mcb_hmu_disp_pages_bak = MAIN_PAGE;		//���������棬���ؽ���Ĭ�ϻ���������
	mcb_hmu_disp_pages = MAIN_PAGE;		
}

/*
********************************************************************************
�������ƣ�sub3_page_rfb_jx2_disp
������������ҳ��3��Ƶ�����2
����������
����ֵ	��
********************************************************************************
*/
void sub3_page_rfb_jx2_disp(void)
{
	uint8_t i, flag, cnt1, cnt2;
	const uint8_t *pt[9];
	char str[9], str1[9];

    //���ο�
    lcd_draw_rect(0, 0, 128, 64);
	
	//�������������/�澯
	lcd_draw_hz_nx16_2(7, 3, 0, zhujishuchu2);
	lcd_draw_char_5x7(11, 51, 0, charmh_5x7);
	if(mpd8_out_state)
		lcd_draw_hz_nx16_2(7, 56, 0, gaojing2);
	else
		lcd_draw_hz_nx16_2(7, 56, 0, zhengchang2);	
	//�ӻ�������޴ӻ�/����/�澯 P(8*16)12345678(5*7)
	lcd_draw_hz_nx16_2(26, 3, 0, congjishuchu2);
	lcd_draw_char_5x7(30, 51, 0, charmh_5x7);	
#ifdef __TEST_HMU__	
	lcd_draw_hz_nx16_2(26, 56, 0, gaojing2);
	lcd_draw_char_5x7(28, 80, 0, charP_5x7);	
	//lcd_draw_graph(26, 80, 5, 12, charP_5x12);//P
	lcd_draw_char_5x7(30, 86, 0, char1_8_5x7);
#else	
	if(!mcb_cur_pd8_slave_num)
		lcd_draw_hz_nx16_2(26, 56, 0, wucongji2);	
	else
	if(mcb_cur_pd8_slave_num < 9)
	{
		flag = 0;
		cnt1 = 0;
		cnt2 = 0;
		for(i = 0; i < mcb_cur_pd8_slave_num; i++)
		{
			if(spd8_out_state[mcb_cur_valid_slave_addr[i] - 1])
			{
				if(mcb_cur_valid_slave_addr[i])
				{
					flag = 1;
					str[cnt1++] = mcb_cur_valid_slave_addr[i];							
				}
			}
			else
			{
				if(mcb_cur_valid_slave_addr[i])
					str1[cnt2++] = mcb_cur_valid_slave_addr[i];	
			}	
		}
		str[cnt1] = NULL;
		str1[cnt2] = NULL;
			
		if(flag)
		{
			lcd_draw_hz_nx16_2(26, 56, 0, gaojing2);
			lcd_draw_char_5x7(28, 80, 0, charP_5x7);	
			for(i = 0; i < cnt1; i++)
			{
				pt[i] = &ascii_5x7[CHAR_0_INDEX + str[i]][0];
			}
			pt[cnt1] = NULL;						
			lcd_draw_char_5x7(30, 86, 0, pt);	
		}
		else
		if(cnt2)
		{
			lcd_draw_hz_nx16_2(26, 56, 0, zhengchang2);	
			lcd_draw_char_5x7(28, 80, 0, charP_5x7);
			for(i = 0; i < cnt2; i++)
			{
				pt[i] = &ascii_5x7[CHAR_0_INDEX + str1[i]][0];
			}
			pt[cnt2] = NULL;								
			lcd_draw_char_5x7(30, 86, 0, pt);	
		}			
	}	
#endif	
	//������·����/��
	lcd_draw_hz_nx16_2(45, 3, 0, zhujipanglu2);	
	lcd_draw_char_5x7(49, 51, 0, charmh_5x7);	
	if(mr_comm_msg.cur_chno > 1)
		lcd_draw_hz_nx16_2(45, 56, 0, shi2);	
	else
		lcd_draw_hz_nx16_2(45, 56, 0, fou2);		
}

/*
********************************************************************************
�������ƣ�sub3_page_xyb_jx1_disp
������������ҳ��3Э������1����
����������
����ֵ	��
********************************************************************************
*/
void sub3_page_xyb_jx1_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	sub3_page_rfb_jx1_disp();
	lcd_draw_hline(62, 126, 1);
}

/*
********************************************************************************
�������ƣ�sub3_page_xyb_jx2_disp
������������ҳ��3Э������2
����������
����ֵ	��
********************************************************************************
*/
void sub3_page_xyb_jx2_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	sub3_page_rfb_jx2_disp();	
	lcd_draw_hline(62, 126, 1);
}

/*
********************************************************************************
�������ƣ�sub3_page_key_deal
������������ҳ��3��������
����������
����ֵ	��
********************************************************************************
*/
void sub3_page_key_deal(void)
{
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/	
	sub1_page_key_deal();
}


/*
********************************************************************************
�������ƣ�hmu_ch1_test_page
�����������ֶ�ͨ��1���Խ���
����������
����ֵ	
********************************************************************************
*/
void hmu_ch1_test_page(void)
{
	if(rfb_or_xyb == RFB)
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			ch1t_page_rfb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			ch1t_page_rfb_jx2_disp();
	}
	else
	if(rfb_or_xyb == XYB) 
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			ch1t_page_xyb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			ch1t_page_xyb_jx2_disp();		
	}
	//ҳ�水������
	ch1t_page_key_deal();			
}

/*
********************************************************************************
�������ƣ�ch1t_page_rfb_jx1_disp
�����������ֶ�ͨ��1��Ƶ�����1
����������
����ֵ	
********************************************************************************
*/
void ch1t_page_rfb_jx1_disp(void)
{
	static uint8_t cnt = 0;
	const uint8_t **pt = NULL;

	//Test Mode (RF1)
	lcd_draw_char_5x7(6, 11, 2, charTestModeRF1_5x7);	
	//1��ͨ����
	lcd_draw_char_8x16(19, 20, 0, char1_8x16_xiao5);	
	lcd_draw_hz_nx16_1(20, 29, 0, haotongdao1);	
	lcd_draw_char_8x16(18, 71, 0, charmh_8x16);	
	//Ŭ��������......
	lcd_draw_hz_nx16_1(41, 14, 0, nvlisousuozhong1);		
	//��̬����......
	cnt++;
	if(cnt == 1)
		pt = zyd_1dot;
	else
	if(cnt == 2)
		pt = zyd_2dot;
	else
	if(cnt == 3)
		pt = zyd_3dot;
	else
	if(cnt == 4)
		pt = zyd_4dot;
	else
	if(cnt == 5)
		pt = zyd_5dot;
	else
	if(cnt == 6)
		pt = zyd_6dot;
	else
	if(cnt >= 7)
	{
		cnt = 0;
		pt = zyd_null;	
	}	
	lcd_draw_char_5x7(45, 84, 0, pt);	
	//���
	if(mr_comm_msg.cur_mode == 1)							//�ֶ�ģʽ
	{
		if(!mr_comm_msg.cur_chno)							//ͨ��1
		{
			if((!ant1_rf_state) && (!ch1_rf_state))
				lcd_draw_hz_nx16_1(20, 79, 0, zhengchang1);	
			else
				lcd_draw_hz_nx16_1(20, 79, 0, gaojing1);				
		}
	}	
}

/*
********************************************************************************
�������ƣ�ch1t_page_rfb_jx2_disp
�����������ֶ�ͨ��1��Ƶ�����2
����������
����ֵ	
********************************************************************************
*/
void ch1t_page_rfb_jx2_disp(void)
{
	static uint8_t cnt = 0;
	const uint8_t **pt = NULL;
	
	//���ο�
    lcd_draw_rect(0, 0, 128, 64);
    lcd_draw_hline(19, 0, 128);

	//<Hand Test 1>
	lcd_draw_char_5x7(6, 3, 1, charHandTest1_5x7);	
	//ͨ��1��
	lcd_draw_hz_nx16_2(26, 34, 0, tongdao2);	
	lcd_draw_char_8x16(25, 58, 0, char1_8x16);	
	lcd_draw_char_5x7(30, 66, 0, charmh_5x7);	
	//��������......
	lcd_draw_hz_nx16_2(45, 25, 0, zhengzaisouxing2);	
	//��̬����......
	cnt++;
	if(cnt == 1)
		pt = zyd_1dot;
	else
	if(cnt == 2)
		pt = zyd_2dot;
	else
	if(cnt == 3)
		pt = zyd_3dot;
	else
	if(cnt == 4)
		pt = zyd_4dot;
	else
	if(cnt == 5)
		pt = zyd_5dot;
	else
	if(cnt == 6)
		pt = zyd_6dot;
	else
	if(cnt >= 7)
	{
		cnt = 0;
		pt = zyd_null;	
	}	
	lcd_draw_char_5x7(48, 74, 0, pt);	
	//���
	if(mr_comm_msg.cur_mode == 1)							//�ֶ�ģʽ
	{
		if(!mr_comm_msg.cur_chno)							//ͨ��1
		{
			if((!ant1_rf_state) && (!ch1_rf_state))
				lcd_draw_hz_nx16_2(26, 72, 0, zhengchang2);	
			else
				lcd_draw_hz_nx16_2(26, 72, 0, gaojing2);				
		}
	}	
}

/*
********************************************************************************
�������ƣ�ch1t_page_xyb_jx1_disp
�����������ֶ�ͨ��1Э������1
����������
����ֵ	
********************************************************************************
*/
void ch1t_page_xyb_jx1_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	ch1t_page_rfb_jx1_disp();	
	lcd_draw_hline(62, 126, 1);	
}

/*
********************************************************************************
�������ƣ�ch1t_page_xyb_jx2_disp
�����������ֶ�ͨ��1Э������1
����������
����ֵ	
********************************************************************************
*/
void ch1t_page_xyb_jx2_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	ch1t_page_rfb_jx2_disp();	
	lcd_draw_hline(62, 126, 1);	
}

/*
********************************************************************************
�������ƣ�ch1t_page_key_deal
�����������ֶ�ͨ��1��������
����������
����ֵ	
********************************************************************************
*/
void ch1t_page_key_deal(void)
{
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/	
	
	switch(key_value)
	{
		case RIGHT_S_KEY: 
			beep_en_on_off();								//BEEPʹ��/�ر�
			break;		
		case OK_S_KEY: 										//����ҳ�� �˳��ֶ�ͨ��1/2���Զ�����ģʽ
			if(dev_type == DEV_TYPE_1)
			{
				if(mr_in_out_manu_mode)
				{
					mr_in_out_manu_mode = 0;
					mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
					mr_comm_sys_tx_flag = 1;
					mcb_rfb_task_flag = 1;
				}
				else
				if(mr_in_out_auto_mode)
				{
					mr_in_out_auto_mode = 0;	
					mr_comm_sys_tx_cmd2 = QQJRTCZDMS_CMD_82;
					mr_comm_sys_tx_flag = 1;
					mcb_rfb_task_flag = 1;		
				}
				mcb_hmu_disp_pages = mcb_hmu_disp_pages_bak;//����֮ǰ���棬����������				
			}	
			break;	
		case RETURN_S_KEY: 
			if(dev_type == DEV_TYPE_2) 						//return ���ؽ�����棬����2
			{
				if(mr_in_out_manu_mode)
				{
					mr_in_out_manu_mode = 0;
					mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
					mr_comm_sys_tx_flag = 1;
					mcb_rfb_task_flag = 1;
				}
				else
				if(mr_in_out_auto_mode)
				{
					mr_in_out_auto_mode = 0;	
					mr_comm_sys_tx_cmd2 = QQJRTCZDMS_CMD_82;
					mr_comm_sys_tx_flag = 1;
					mcb_rfb_task_flag = 1;		
				}				
				mcb_hmu_disp_pages = mcb_hmu_disp_pages_bak;	
			}
			break;	
		default: break;
	}
	key_value = NO_KEY;				
}

/*
********************************************************************************
�������ƣ�hmu_ch2_test_page
�����������ֶ�ͨ��2���Խ���
����������
����ֵ	��
********************************************************************************
*/
void hmu_ch2_test_page(void)
{
	if(rfb_or_xyb == RFB)
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			ch2t_page_rfb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			ch2t_page_rfb_jx2_disp();
	}
	else
	if(rfb_or_xyb == XYB) 
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			ch2t_page_xyb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			ch2t_page_xyb_jx2_disp();		
	}
	//ҳ�水������
	ch2t_page_key_deal();				
}

/*
********************************************************************************
�������ƣ�ch2t_page_rfb_jx1_disp
�����������ֶ�ͨ��2��Ƶ�����1
����������
����ֵ	��
********************************************************************************
*/
void ch2t_page_rfb_jx1_disp(void)
{
	static uint8_t cnt = 0;
	const uint8_t **pt = NULL;

	//Test Mode (RF2)
	lcd_draw_char_5x7(6, 11, 2, charTestModeRF2_5x7);	
	//2��ͨ����
	lcd_draw_char_8x16(19, 20, 0, char2_8x16_xiao5);	
	lcd_draw_hz_nx16_1(20, 29, 0, haotongdao1);	
	lcd_draw_char_8x16(18, 71, 0, charmh_8x16);	
	//Ŭ��������......
	lcd_draw_hz_nx16_1(41, 14, 0, nvlisousuozhong1);		
	//��̬����......
	cnt++;
	if(cnt == 1)
		pt = zyd_1dot;
	else
	if(cnt == 2)
		pt = zyd_2dot;
	else
	if(cnt == 3)
		pt = zyd_3dot;
	else
	if(cnt == 4)
		pt = zyd_4dot;
	else
	if(cnt == 5)
		pt = zyd_5dot;
	else
	if(cnt == 6)
		pt = zyd_6dot;
	else
	if(cnt >= 7)
	{
		cnt = 0;
		pt = zyd_null;	
	}	
	lcd_draw_char_5x7(45, 84, 0, pt);	
	//���
	if(mr_comm_msg.cur_mode == 1)							//�ֶ�ģʽ
	{
		if(mr_comm_msg.cur_chno == 1)						//ͨ��2
		{
			if((!ant2_rf_state) && (!ch2_rf_state))
				lcd_draw_hz_nx16_1(20, 79, 0, zhengchang1);	
			else
				lcd_draw_hz_nx16_1(20, 79, 0, gaojing1);				
		}
	}	
}

/*
********************************************************************************
�������ƣ�ch2t_page_rfb_jx2_disp
�����������ֶ�ͨ��2��Ƶ�����2
����������
����ֵ	��
********************************************************************************
*/
void ch2t_page_rfb_jx2_disp(void)
{
	static uint8_t cnt = 0;
	const uint8_t **pt = NULL;
	
	//���ο�
    lcd_draw_rect(0, 0, 128, 64);
    lcd_draw_hline(19, 0, 128);

	//<Hand Test 2>
	lcd_draw_char_5x7(6, 3, 1, charHandTest2_5x7);	
	//ͨ��2��
	lcd_draw_hz_nx16_2(26, 34, 0, tongdao2);	
	lcd_draw_char_8x16(25, 58, 0, char2_8x16);	
	lcd_draw_char_5x7(30, 66, 0, charmh_5x7);	
	//��������......
	lcd_draw_hz_nx16_2(45, 25, 0, zhengzaisouxing2);	
	//��̬����......
	cnt++;
	if(cnt == 1)
		pt = zyd_1dot;
	else
	if(cnt == 2)
		pt = zyd_2dot;
	else
	if(cnt == 3)
		pt = zyd_3dot;
	else
	if(cnt == 4)
		pt = zyd_4dot;
	else
	if(cnt == 5)
		pt = zyd_5dot;
	else
	if(cnt == 6)
		pt = zyd_6dot;
	else
	if(cnt >= 7)
	{
		cnt = 0;
		pt = zyd_null;	
	}	
	lcd_draw_char_5x7(48, 74, 0, pt);	
	//���
	if(mr_comm_msg.cur_mode == 1)							//�ֶ�ģʽ
	{
		if(mr_comm_msg.cur_chno == 1)						//ͨ��2
		{
			if((!ant2_rf_state) && (!ch2_rf_state))
				lcd_draw_hz_nx16_2(26, 72, 0, zhengchang2);	
			else
				lcd_draw_hz_nx16_2(26, 72, 0, gaojing2);				
		}
	}	
}

/*
********************************************************************************
�������ƣ�ch2t_page_xyb_jx1_disp
�����������ֶ�ͨ��2Э������1
����������
����ֵ	��
********************************************************************************
*/
void ch2t_page_xyb_jx1_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	ch2t_page_rfb_jx1_disp();	
	lcd_draw_hline(62, 126, 1);		
}

/*
********************************************************************************
�������ƣ�ch2t_page_xyb_jx2_disp
�����������ֶ�ͨ��2Э������2
����������
����ֵ	��
********************************************************************************
*/
void ch2t_page_xyb_jx2_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	ch2t_page_rfb_jx2_disp();	
	lcd_draw_hline(62, 126, 1);		
}

/*
********************************************************************************
�������ƣ�ch2t_page_key_deal
�����������ֶ�ͨ��2��������
����������
����ֵ	��
********************************************************************************
*/
void ch2t_page_key_deal(void)
{
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/	
	ch1t_page_key_deal();
}

/*
********************************************************************************
�������ƣ�hmu_auto_test_page
�����������Զ�ͨ�����Խ���
����������
����ֵ	��
********************************************************************************
*/
void hmu_auto_test_page(void)
{
	if(rfb_or_xyb == RFB)
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			auto_page_rfb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			auto_page_rfb_jx2_disp();
	}
	else
	if(rfb_or_xyb == XYB) 
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			auto_page_xyb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			auto_page_xyb_jx2_disp();		
	}
	//ҳ�水������
	auto_page_key_deal();					
}

/*
********************************************************************************
�������ƣ�auto_page_rfb_jx1_disp
�����������Զ�ͨ��������Ƶ�����1
����������
����ֵ	��
********************************************************************************
*/
void auto_page_rfb_jx1_disp(void)
{
	//Test Mode (Two)
	lcd_draw_char_5x7(6, 11, 2, charTestModeTwo_5x7);	
	//1��ͨ����
	lcd_draw_char_8x16(19, 20, 0, char1_8x16_xiao5);	
	lcd_draw_hz_nx16_1(20, 29, 0, haotongdao1);	
	lcd_draw_char_8x16(18, 71, 0, charmh_8x16);	
	//2��ͨ����
	lcd_draw_char_8x16(40, 20, 0, char2_8x16_xiao5);	
	lcd_draw_hz_nx16_1(41, 29, 0, haotongdao1);	
	lcd_draw_char_8x16(39, 71, 0, charmh_8x16);	
	//���
	if(mr_comm_msg.cur_mode == 2)							//�Զ�ģʽ
	{
		//if(mr_comm_msg.cur_chno == 0)						//ͨ��1
		{
			if((!ant1_rf_state) && (!ch1_rf_state))
				lcd_draw_hz_nx16_1(20, 79, 0, zhengchang1);	
			else
			if(ant1_rf_state == 1)
				lcd_draw_hz_nx16_1(20, 79, 0, weijie1);	
			else
            if((ant1_rf_state == 2) || (ch1_rf_state == 1))
				lcd_draw_hz_nx16_1(20, 79, 0, gaojing1);				
		}		
		//if(mr_comm_msg.cur_chno == 1)						//ͨ��2
		{
			if((!ant2_rf_state) && (!ch2_rf_state))
				lcd_draw_hz_nx16_1(41, 79, 0, zhengchang1);	
			else
			if(ant2_rf_state == 1)
				lcd_draw_hz_nx16_1(41, 79, 0, weijie1);	
			else
            if((ant2_rf_state == 2) || (ch2_rf_state == 1))
				lcd_draw_hz_nx16_1(41, 79, 0, gaojing1);				
		}
	}		
}

/*
********************************************************************************
�������ƣ�auto_page_rfb_jx2_disp
�����������Զ�ͨ��������Ƶ�����2
����������
����ֵ	��
********************************************************************************
*/
void auto_page_rfb_jx2_disp(void)
{
	//���ο�
    lcd_draw_rect(0, 0, 128, 64);
    lcd_draw_hline(19, 0, 128);

	//<Auto Test>
	lcd_draw_char_5x7(6, 3, 1, charAutoTest_5x7);		
	//ͨ��1��
	lcd_draw_hz_nx16_2(26, 34, 0, tongdao2);	
	lcd_draw_char_8x16(25, 58, 0, char1_8x16);	
	lcd_draw_char_5x7(30, 66, 0, charmh_5x7);	
	lcd_draw_hz_nx16_2(45, 34, 0, tongdao2);	
	lcd_draw_char_8x16(44, 58, 0, char2_8x16);	
	lcd_draw_char_5x7(49, 66, 0, charmh_5x7);	
	//���
	if(mr_comm_msg.cur_mode == 2)							//�Զ�ģʽ
	{
		//if(!mr_comm_msg.cur_chno)							//ͨ��1
		{
			if((!ant1_rf_state) && (!ch1_rf_state))
				lcd_draw_hz_nx16_2(26, 72, 0, zhengchang2);	
			else
            if((ant1_rf_state < 3) || (ch1_rf_state))
				lcd_draw_hz_nx16_2(26, 72, 0, gaojing2);				
		}
		//if(mr_comm_msg.cur_chno == 1)						//ͨ��2
		{
			if((!ant2_rf_state) && (!ch2_rf_state))
				lcd_draw_hz_nx16_2(45, 72, 0, zhengchang2);	
			else
            if((ant2_rf_state < 3) || (ch1_rf_state == 1))
				lcd_draw_hz_nx16_2(45, 72, 0, gaojing2);				
		}		
	}		
}

/*
********************************************************************************
�������ƣ�auto_page_xyb_jx1_disp
�����������Զ�ͨ������Э�����1
����������
����ֵ	��
********************************************************************************
*/
void auto_page_xyb_jx1_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	auto_page_rfb_jx1_disp();	
	lcd_draw_hline(62, 126, 1);		
}

/*
********************************************************************************
�������ƣ�auto_page_xyb_jx2_disp
�����������Զ�ͨ������Э�����2
����������
����ֵ	��
********************************************************************************
*/
void auto_page_xyb_jx2_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	auto_page_rfb_jx2_disp();	
	lcd_draw_hline(62, 126, 1);		
}

/*
********************************************************************************
�������ƣ�auto_page_key_deal
�����������Զ�ͨ�����԰�������
����������
����ֵ	��
********************************************************************************
*/
void auto_page_key_deal(void)
{
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/
	ch1t_page_key_deal();
}

/*
********************************************************************************
�������ƣ�hmu_dev_fault_page
�����������豸����ά��ҳ��
����������
����ֵ	��
********************************************************************************
*/
void hmu_dev_fault_page(void)
{
	if(rfb_or_xyb == RFB)
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			fault_page_rfb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			fault_page_rfb_jx2_disp();
	}
	else
	if(rfb_or_xyb == XYB) 
	{
		//���Ͳ�һ������ʾ���治ͬ
		if(dev_type == DEV_TYPE_1)
			fault_page_xyb_jx1_disp();
		else
		if(dev_type == DEV_TYPE_2) 
			fault_page_xyb_jx2_disp();		
	}
	//ҳ�水������
		fault_page_key_deal();	
}

/*
********************************************************************************
�������ƣ�fault_page_rfb_jx1_disp
�����������豸����ά��ҳ����Ƶ�����1
����������
����ֵ	��
********************************************************************************
*/
void fault_page_rfb_jx1_disp(void)
{
	uint8_t i;
	const uint8_t *pt[3];
	const uint8_t *pt1[5];
	
	pt[0] =  &ascii_5x7[CHAR_0_INDEX][0];
	pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
	pt[2] = NULL;
	//�豸״̬��Ϣ
	lcd_draw_hz_nx16_1(2, 22, 0, shebeiztxx1);		
	//MCB ��
	lcd_draw_char_5x7(21, 3, 1, charMCBkmh_5x7);	
	if(sys_fault_word)
	{
		if(sys_fault_word & 0x03)
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + 3][0];
		else
		if(sys_fault_word & 0x02)
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + 2][0];
		else
		if(sys_fault_word & 0x01)
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + 1][0];
		//
		lcd_draw_char_5x7(21, 33, 1, pt);		
	}
	else
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
		lcd_draw_char_5x7(21, 33, 1, pt);		
	}
	//HW��
	lcd_draw_char_5x7(21, 80, 1, charHWmh_5x7);	
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX + 'V' - 'A'][0];
	pt1[1] =  &ascii_5x7[CHAR_0_INDEX + mcb_file[4] - '0'][0];
	pt1[2] =  &ascii_5x7[CHAR_DIAN_INDEX][0];
	pt1[3] =  &ascii_5x7[CHAR_0_INDEX + mcb_file[6] - '0'][0];
	pt1[4] =  NULL;
	lcd_draw_char_5x7(21, 98, 1, pt1);	
	//RFXY:
	lcd_draw_char_5x7(32, 3, 1, charRFXYmh_5x7);	
	if(mr_comm_msg.state2)
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + 1][0];
		//
		lcd_draw_char_5x7(32, 33, 1, pt);		
	}
	else
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
		lcd_draw_char_5x7(32, 33, 1, pt);		
	}
	//SW��
	lcd_draw_char_5x7(32, 80, 1, charSWmh_5x7);	
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX + 'V' - 'A'][0];
	pt1[1] =  &ascii_5x7[CHAR_0_INDEX + mcb_file[8] - '0'][0];
	pt1[2] =  &ascii_5x7[CHAR_DIAN_INDEX][0];
	pt1[3] =  &ascii_5x7[CHAR_0_INDEX + mcb_file[10] - '0'][0];
	pt1[4] =  NULL;	
	lcd_draw_char_5x7(32, 98, 1, pt1);	
	//PD8M:
	lcd_draw_char_5x7(43, 3, 1, charPD8Mmh_5x7);	
	if(mp_mpd8_comm_msg.mpd8_dev_state)
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + 1][0];
		//
		lcd_draw_char_5x7(43, 33, 1, pt);	
	}	
	else
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
		lcd_draw_char_5x7(43, 33, 1, pt);	
	}
	//PD8S:
	if(!mcb_cur_pd8_slave_num)
	{
		pt[0] =  &ascii_5x7[CHAR_HLINE_INDEX][0];
		pt[1] =  &ascii_5x7[CHAR_HLINE_INDEX][0];
		lcd_draw_char_5x7(54, 33, 1, pt);
	}
	lcd_draw_char_5x7(54, 3, 1, charPD8Smh_5x7);	
	for(i = 0; i < mcb_cur_pd8_slave_num; i++)		//00����λ����ӻ��ţ�1-8������λ������Ϻţ�0-9�����գ���ʾ�޴ӻ�
	{
		if(mcb_cur_valid_slave_addr[i] > 0)
		{
			if(mp_spd8_comm_msg[mcb_cur_valid_slave_addr[i] - 1].spd8_dev_state)
			{
				
				pt[0] =  &ascii_5x7[CHAR_0_INDEX + mcb_cur_valid_slave_addr[i]][0];
				pt[1] =  &ascii_5x7[CHAR_0_INDEX + 1][0];
				//
				lcd_draw_char_5x7(54, 33 + i * 12, 0, pt);	
			}	
			else
			{
				pt[0] =  &ascii_5x7[CHAR_0_INDEX + mcb_cur_valid_slave_addr[i]][0];
				pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
				lcd_draw_char_5x7(54, 33 + i * 12, 0, pt);		
			}				
		}
	}
}

/*
********************************************************************************
�������ƣ�fault_page_rfb_jx2_disp
�����������豸����ά��ҳ����Ƶ�����2
����������
����ֵ	��
********************************************************************************
*/
void fault_page_rfb_jx2_disp(void)
{
	uint8_t i;
	const uint8_t *pt[3];
	const uint8_t *pt1[6];
	
    lcd_draw_rect(0, 0, 128, 64);
    lcd_draw_hline(16, 0, 128);
	
	pt[0] =  &ascii_5x7[CHAR_0_INDEX][0];
	pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
	pt[2] = NULL;
	//�豸״̬
	lcd_draw_hz_nx16_2(2, 38, 1, shebeizt2);		
	//���أ�
	lcd_draw_hz_nx16_2(19, 2, 0, zhukong2);	
	lcd_draw_char_5x7(23, 26, 0, charmh_5x7);	
	if(sys_fault_word)
	{
		if(sys_fault_word & 0x03)
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + 3][0];
		else
		if(sys_fault_word & 0x02)
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + 2][0];
		else
		if(sys_fault_word & 0x01)
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + 1][0];
			
		lcd_draw_char_5x7(23, 32, 1, pt);		
	}
	else
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
		lcd_draw_char_5x7(23, 32, 1, pt);		
	}
	//����:
	lcd_draw_hz_nx16_2(19, 64, 0, shuru2);	
	lcd_draw_char_5x7(23, 88, 0, charmh_5x7);	
	if(mr_comm_msg.state2)
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + 1][0];
		//
		lcd_draw_char_5x7(23, 94, 1, pt);		
	}
	else
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
		lcd_draw_char_5x7(23, 94, 1, pt);		
	}
	//����:
	lcd_draw_hz_nx16_2(34, 2, 0, zhuchu2);
	lcd_draw_char_5x7(38, 26, 0, charmh_5x7);		
	if(mp_mpd8_comm_msg.mpd8_dev_state)
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + 1][0];
		//
		lcd_draw_char_5x7(38, 32, 1, pt);	
	}	
	else
	{
		pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
		lcd_draw_char_5x7(38, 32, 1, pt);	
	}
	//�汾:
	lcd_draw_hz_nx16_2(34, 64, 0, banben2);	
	lcd_draw_char_5x7(38, 88, 0, charmh_5x7);	
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX + 'V' - 'A'][0];
	pt1[1] =  &ascii_5x7[CHAR_0_INDEX + mcb_file[4] - '0'][0];
	pt1[2] =  &ascii_5x7[CHAR_0_INDEX + mcb_file[6] - '0'][0];
	//pt1[3] =  &ascii_5x7[CHAR_A_INDEX + 'S' - 'A'][0];
	pt1[3] =  &ascii_5x7[CHAR_0_INDEX + mcb_file[8] - '0'][0];
	pt1[4] =  &ascii_5x7[CHAR_0_INDEX + mcb_file[10] - '0'][0];
	pt1[5] =  NULL;
	lcd_draw_char_5x7(38, 94, 1, pt1);		
	//�ӳ�:
	lcd_draw_hz_nx16_2(49, 2, 0, congchu2);	
	lcd_draw_char_5x7(53, 26, 0, charmh_5x7);	
	if(!mcb_cur_pd8_slave_num)
	{
		pt[0] =  &ascii_5x7[CHAR_HLINE_INDEX][0];
		pt[1] =  &ascii_5x7[CHAR_HLINE_INDEX][0];
		lcd_draw_char_5x7(53, 32, 1, pt);
	}
	for(i = 0; i < mcb_cur_pd8_slave_num; i++)		//00����λ����ӻ��ţ�1-8������λ������Ϻţ�0-9����--����ʾ�޴ӻ�
	{
		if(mcb_cur_valid_slave_addr[i] > 0)
		{
			if(mp_spd8_comm_msg[mcb_cur_valid_slave_addr[i] - 1].spd8_dev_state)
			{
				
				pt[0] =  &ascii_5x7[CHAR_0_INDEX + mcb_cur_valid_slave_addr[i]][0];
				pt[1] =  &ascii_5x7[CHAR_0_INDEX + 1][0];
				//
				lcd_draw_char_5x7(53, 32 + i * 12, 0, pt);	
			}	
			else
			{
				pt[0] =  &ascii_5x7[CHAR_0_INDEX + mcb_cur_valid_slave_addr[i]][0];
				pt[1] =  &ascii_5x7[CHAR_0_INDEX][0];
				lcd_draw_char_5x7(53, 32 + i * 12, 0, pt);		
			}				
		}
	}	
}

/*
********************************************************************************
�������ƣ�fault_page_xyb_jx1_disp
�����������豸����ά��ҳ��Э������1
����������
����ֵ	��
********************************************************************************
*/
void fault_page_xyb_jx1_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	fault_page_rfb_jx1_disp();	
	lcd_draw_hline(62, 126, 1);		
}

/*
********************************************************************************
�������ƣ�fault_page_xyb_jx2_disp
�����������豸����ά��ҳ��Э������2
����������
����ֵ	��
********************************************************************************
*/
void fault_page_xyb_jx2_disp(void)
{
	//��ʱ������Ƶ�壬�����������޸�
	fault_page_rfb_jx2_disp();	
	lcd_draw_hline(62, 126, 1);		
}

/*
********************************************************************************
�������ƣ�fault_page_key_deal
�����������豸����ά��ҳ�水������
����������
����ֵ	��
********************************************************************************
*/
void fault_page_key_deal(void)
{
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/
	switch(key_value)
	{
		case LR_L_KEY: 
			read_att_flag = 1;
			mcb_hmu_disp_pages = DEV_PARA_PAGE;
			break;		
		case RIGHT_S_KEY: 
			beep_en_on_off();								//BEEPʹ��/�ر�
			break;		
		case OK_S_KEY: 										//�˳�
			//if(dev_type == DEV_TYPE_1)
			{
				mcb_hmu_disp_pages = mcb_hmu_disp_pages_bak;//����֮ǰ���棬����������				
			}	
			break;	
		case RETURN_S_KEY: 
			if(dev_type == DEV_TYPE_2) 						//return ���ؽ���ǰ���棬����2
			{
				mcb_hmu_disp_pages = mcb_hmu_disp_pages_bak;	
			}
			break;	
		default: break;
	}
	key_value = NO_KEY;					
}

/*
********************************************************************************
�������ƣ�hmu_dev_para_page
����������dev para page, just for debug......
����������
����ֵ	��
********************************************************************************
*/
void hmu_dev_para_page(void)
{
	uint8_t i, j;
	const uint8_t *pt[3];
	const uint8_t *pt1[9];
	static uint8_t cnt = 0;
	
	pt[0] =  NULL;
	pt1[0] =  NULL;
	//
	lcd_draw_rect(0, 0, 128, 64);
	if(read_att_flag == 1)
	{
		read_att_flag = 0;
		//read rf att
		mr_comm_sys_tx_cmd2 = QQDQSBSSJZ_CMD_84;
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;	
		//read pd8 att
		mp_comm_sys_tx_cmd2 = MP_QQDQSBLSJZ_CMD_84;
		mp_comm_sys_tx_flag = 1;
		mcb_pd8_task_flag = 1;				
	}

	//ANTCH12:
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX][0];
	pt1[1] =  &ascii_5x7[CHAR_A_INDEX + 'N' - 'A'][0];
	pt1[2] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[3] =  &ascii_5x7[CHAR_A_INDEX + 'C' - 'A'][0];
	pt1[4] =  &ascii_5x7[CHAR_A_INDEX + 'H' - 'A'][0];
	pt1[5] =  &ascii_5x7[CHAR_0_INDEX + '1' - '0'][0];
	pt1[6] =  &ascii_5x7[CHAR_0_INDEX + '2' - '0'][0];
	pt1[7] =  &ascii_5x7[CHAR_MAOHAO_INDEX][0];
	pt1[8] =  NULL;
	lcd_draw_char_5x7(7, 2, 1, pt1);		
	//value: xx
	//ant_used_flag?

	cnt++;
	if(mcb_hmu_para_index == 0)	
	{
		if(mcb_hmu_modify_mode)
		{
			i = mcb_hmu_para_bak / 10;
			j = mcb_hmu_para_bak % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;			
			lcd_draw_char_5x7(7, 50, 1, pt);	
			if(mcb_hmu_para_bit)
			{
				if(cnt & 0x01)
					lcd_draw_hline(15, 50, 5);	
				else
					lcd_clr_hline(15, 50, 5);		
			}				
			else
			{
				if(cnt & 0x01)
					lcd_draw_hline(15, 56, 5);	
				else
					lcd_clr_hline(15, 56, 5);							
			}
		}
		else
		{
			i = ant_used_flag / 10;
			j = ant_used_flag % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;	
			lcd_draw_char_5x7(7, 50, 1, pt);
		}
	}
	else
	{
		i = ant_used_flag / 10;
		j = ant_used_flag % 10;			
		pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
		pt[2] =  NULL;	
		lcd_draw_char_5x7(7, 50, 1, pt);
	}
	//CH1ATTE:
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX + 'C' - 'A'][0];
	pt1[1] =  &ascii_5x7[CHAR_A_INDEX + 'H' - 'A'][0];
	pt1[2] =  &ascii_5x7[CHAR_0_INDEX + '1' - '0'][0];
	pt1[3] =  &ascii_5x7[CHAR_A_INDEX + 'A' - 'A'][0];
	pt1[4] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[5] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[6] =  &ascii_5x7[CHAR_A_INDEX + 'E' - 'A'][0];
	pt1[7] =  &ascii_5x7[CHAR_MAOHAO_INDEX][0];
	pt1[8] =  NULL;
	lcd_draw_char_5x7(7, 67, 1, pt1);		
	//value: xx
	if(mcb_hmu_para_index == 1)	
	{
		if(mcb_hmu_modify_mode)
		{
			i = mcb_hmu_para_bak / 10;
			j = mcb_hmu_para_bak % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;			
			lcd_draw_char_5x7(7, 115, 1, pt);	
			if(mcb_hmu_para_bit)
			{
				if(cnt & 0x01)
					lcd_draw_hline(15, 115, 5);	
				else
					lcd_clr_hline(15, 115, 5);		
			}				
			else
			{
				if(cnt & 0x01)
					lcd_draw_hline(15, 121, 5);	
				else
					lcd_clr_hline(15, 121, 5);							
			}			
		}
		else
		{
			i = mr_ch1_att / 10;
			j = mr_ch1_att % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;	
			lcd_draw_char_5x7(7, 115, 1, pt);
		}	
	}
	else
	{
		i = mr_ch1_att / 10;
		j = mr_ch1_att % 10;			
		pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
		pt[2] =  NULL;	
		lcd_draw_char_5x7(7, 115, 1, pt);
	}	
	//ANTGAIN:
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX][0];
	pt1[1] =  &ascii_5x7[CHAR_A_INDEX + 'N' - 'A'][0];
	pt1[2] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[3] =  &ascii_5x7[CHAR_A_INDEX + 'G' - 'A'][0];
	pt1[4] =  &ascii_5x7[CHAR_A_INDEX + 'A' - 'A'][0];
	pt1[5] =  &ascii_5x7[CHAR_A_INDEX + 'I' - 'A'][0];
	pt1[6] =  &ascii_5x7[CHAR_A_INDEX + 'N' - 'A'][0];
	pt1[7] =  &ascii_5x7[CHAR_MAOHAO_INDEX][0];
	pt1[8] =  NULL;
	lcd_draw_char_5x7(21, 2, 1, pt1);		
	//value: xx
	//ant_used_gain?	

	if(mcb_hmu_para_index == 2)	
	{
		if(mcb_hmu_modify_mode)
		{
			i = mcb_hmu_para_bak / 10;
			j = mcb_hmu_para_bak % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;			
			lcd_draw_char_5x7(21, 50, 1, pt);	
			if(mcb_hmu_para_bit)
			{
				if(cnt & 0x01)
					lcd_draw_hline(29, 50, 5);	
				else
					lcd_clr_hline(29, 50, 5);		
			}				
			else
			{
				if(cnt & 0x01)
					lcd_draw_hline(29, 56, 5);	
				else
					lcd_clr_hline(29, 56, 5);							
			}						
		}
		else
		{
			i = ant_used_gain / 10;
			j = ant_used_gain % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;	
			lcd_draw_char_5x7(21, 50, 1, pt);
		}
	}
	else
	{
		i = ant_used_gain / 10;
		j = ant_used_gain % 10;			
		pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
		pt[2] =  NULL;	
		lcd_draw_char_5x7(21, 50, 1, pt);
	}
	//CH2ATTE:
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX + 'C' - 'A'][0];
	pt1[1] =  &ascii_5x7[CHAR_A_INDEX + 'H' - 'A'][0];
	pt1[2] =  &ascii_5x7[CHAR_0_INDEX + '2' - '0'][0];
	pt1[3] =  &ascii_5x7[CHAR_A_INDEX + 'A' - 'A'][0];
	pt1[4] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[5] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[6] =  &ascii_5x7[CHAR_A_INDEX + 'E' - 'A'][0];
	pt1[7] =  &ascii_5x7[CHAR_MAOHAO_INDEX][0];
	pt1[8] =  NULL;
	lcd_draw_char_5x7(21, 67, 1, pt1);		
	//value: xx

	if(mcb_hmu_para_index == 3)	
	{
		if(mcb_hmu_modify_mode)
		{
			i = mcb_hmu_para_bak / 10;
			j = mcb_hmu_para_bak % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;			
			lcd_draw_char_5x7(21, 115, 1, pt);	
			if(mcb_hmu_para_bit)
			{
				if(cnt & 0x01)
					lcd_draw_hline(29, 115, 5);	
				else
					lcd_clr_hline(29, 115, 5);		
			}				
			else
			{
				if(cnt & 0x01)
					lcd_draw_hline(29, 121, 5);	
				else
					lcd_clr_hline(29, 121, 5);							
			}						
		}
		else
		{
			i = mr_ch2_att / 10;
			j = mr_ch2_att % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;	
			lcd_draw_char_5x7(21, 115, 1, pt);
		}	
	}
	else
	{
		i = mr_ch2_att / 10;
		j = mr_ch2_att % 10;			
		pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
		pt[2] =  NULL;	
		lcd_draw_char_5x7(21, 115, 1, pt);
	}			
	//WIREATT:
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX + 'W' - 'A'][0];
	pt1[1] =  &ascii_5x7[CHAR_A_INDEX + 'I' - 'A'][0];
	pt1[2] =  &ascii_5x7[CHAR_A_INDEX + 'R' - 'A'][0];
	pt1[3] =  &ascii_5x7[CHAR_A_INDEX + 'E' - 'A'][0];
	pt1[4] =  &ascii_5x7[CHAR_A_INDEX + 'A' - 'A'][0];
	pt1[5] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[6] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[7] =  &ascii_5x7[CHAR_MAOHAO_INDEX][0];
	pt1[8] =  NULL;
	lcd_draw_char_5x7(35, 2, 1, pt1);		
	//value: xx
	//wire_used_att?
	if(mcb_hmu_para_index == 4)	
	{
		if(mcb_hmu_modify_mode)
		{
			i = mcb_hmu_para_bak / 10;
			j = mcb_hmu_para_bak % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;			
			lcd_draw_char_5x7(35, 50, 1, pt);	
			if(mcb_hmu_para_bit)
			{
				if(cnt & 0x01)
					lcd_draw_hline(43, 50, 5);	
				else
					lcd_clr_hline(43, 50, 5);		
			}				
			else
			{
				if(cnt & 0x01)
					lcd_draw_hline(43, 56, 5);	
				else
					lcd_clr_hline(43, 56, 5);							
			}						
		}
		else
		{
			i = wire_used_att / 10;
			j = wire_used_att % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;	
			lcd_draw_char_5x7(35, 50, 1, pt);
		}		
	}
	else
	{
		i = wire_used_att / 10;
		j = wire_used_att % 10;			
		pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
		pt[2] =  NULL;	
		lcd_draw_char_5x7(35, 50, 1, pt);
	}
	//COMATTE:
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX + 'C' - 'A'][0];
	pt1[1] =  &ascii_5x7[CHAR_A_INDEX + 'O' - 'A'][0];
	pt1[2] =  &ascii_5x7[CHAR_A_INDEX + 'M' - 'A'][0];
	pt1[3] =  &ascii_5x7[CHAR_A_INDEX + 'A' - 'A'][0];
	pt1[4] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[5] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[6] =  &ascii_5x7[CHAR_A_INDEX + 'E' - 'A'][0];
	pt1[7] =  &ascii_5x7[CHAR_MAOHAO_INDEX][0];
	pt1[8] =  NULL;
	lcd_draw_char_5x7(35, 67, 1, pt1);		
	//value: xx
	if(mcb_hmu_para_index == 5)	
	{
		if(mcb_hmu_modify_mode)
		{
			i = mcb_hmu_para_bak / 10;
			j = mcb_hmu_para_bak % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;			
			lcd_draw_char_5x7(35, 115, 1, pt);	
			if(mcb_hmu_para_bit)
			{
				if(cnt & 0x01)
					lcd_draw_hline(43, 115, 5);	
				else
					lcd_clr_hline(43, 115, 5);		
			}				
			else
			{
				if(cnt & 0x01)
					lcd_draw_hline(43, 121, 5);	
				else
					lcd_clr_hline(43, 121, 5);							
			}						
		}
		else
		{
			i = mr_com_att / 10;
			j = mr_com_att % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;	
			lcd_draw_char_5x7(35, 115, 1, pt);
		}			
	}
	else
	{
		i = mr_com_att / 10;
		j = mr_com_att % 10;			
		pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
		pt[2] =  NULL;	
		lcd_draw_char_5x7(35, 115, 1, pt);
	}				
	//RESVPAR:
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX + 'R' - 'A'][0];
	pt1[1] =  &ascii_5x7[CHAR_A_INDEX + 'E' - 'A'][0];
	pt1[2] =  &ascii_5x7[CHAR_A_INDEX + 'S' - 'A'][0];
	pt1[3] =  &ascii_5x7[CHAR_A_INDEX + 'V' - 'A'][0];
	pt1[4] =  &ascii_5x7[CHAR_A_INDEX + 'P' - 'A'][0];
	pt1[5] =  &ascii_5x7[CHAR_A_INDEX + 'A' - 'A'][0];
	pt1[6] =  &ascii_5x7[CHAR_A_INDEX + 'R' - 'A'][0];
	pt1[7] =  &ascii_5x7[CHAR_MAOHAO_INDEX][0];
	pt1[8] =  NULL;
	lcd_draw_char_5x7(49, 2, 1, pt1);		
	//value: xx
	//resv_para_def?
	if(mcb_hmu_para_index == 6)	
	{
		if(mcb_hmu_modify_mode)
		{
			i = mcb_hmu_para_bak / 10;
			j = mcb_hmu_para_bak % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;			
			lcd_draw_char_5x7(49, 50, 1, pt);	
			if(mcb_hmu_para_bit)
			{
				if(cnt & 0x01)
					lcd_draw_hline(57, 50, 5);	
				else
					lcd_clr_hline(57, 50, 5);		
			}				
			else
			{
				if(cnt & 0x01)
					lcd_draw_hline(57, 56, 5);	
				else
					lcd_clr_hline(57, 56, 5);							
			}						
		}
		else
		{
			i = resv_para_def / 10;
			j = resv_para_def % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;	
			lcd_draw_char_5x7(49, 50, 1, pt);
		}		
	}
	else
	{
		i = resv_para_def / 10;
		j = resv_para_def % 10;			
		pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
		pt[2] =  NULL;	
		lcd_draw_char_5x7(49, 50, 1, pt);
	}	
	//POUTATT:
	pt1[0] =  &ascii_5x7[CHAR_A_INDEX + 'P' - 'A'][0];
	pt1[1] =  &ascii_5x7[CHAR_A_INDEX + 'O' - 'A'][0];
	pt1[2] =  &ascii_5x7[CHAR_A_INDEX + 'U' - 'A'][0];
	pt1[3] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[4] =  &ascii_5x7[CHAR_A_INDEX + 'A' - 'A'][0];
	pt1[5] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[6] =  &ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0];
	pt1[7] =  &ascii_5x7[CHAR_MAOHAO_INDEX][0];
	pt1[8] =  NULL;
	lcd_draw_char_5x7(49, 67, 1, pt1);		
	//value: xx
	//mp_out_att

	if(mcb_hmu_para_index == 7)	
	{
		if(mcb_hmu_modify_mode)
		{
			i = mcb_hmu_para_bak / 10;
			j = mcb_hmu_para_bak % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;			
			lcd_draw_char_5x7(49, 115, 1, pt);	
			if(mcb_hmu_para_bit)
			{
				if(cnt & 0x01)
					lcd_draw_hline(57, 115, 5);	
				else
					lcd_clr_hline(57, 115, 5);		
			}				
			else
			{
				if(cnt & 0x01)
					lcd_draw_hline(57, 121, 5);	
				else
					lcd_clr_hline(57, 121, 5);							
			}						
		}
		else
		{
			i = mp_out_att / 10;
			j = mp_out_att % 10;			
			pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
			pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
			pt[2] =  NULL;	
			lcd_draw_char_5x7(49, 115, 1, pt);
		}			
	}
	else
	{
		i = mp_out_att / 10;
		j = mp_out_att % 10;			
		pt[0] =  &ascii_5x7[CHAR_0_INDEX + i][0];
		pt[1] =  &ascii_5x7[CHAR_0_INDEX + j][0];
		pt[2] =  NULL;	
		lcd_draw_char_5x7(49, 115, 1, pt);
	}			
	/*
	if(rfb_or_xyb == RFB)
		
	else
	if(rfb_or_xyb == XYB) 	
	
	*/
	switch(key_value)
	{
		case RIGHT_S_KEY: 
			beep_en_on_off();								//BEEPʹ��/�ر�
			break;		
		case OK_L_KEY: 										//�˳�
			//if(dev_type == DEV_TYPE_1)
			{
				mcb_hmu_disp_pages = mcb_hmu_disp_pages_bak;//����֮ǰ���棬����������				
			}	
			break;	
		case RETURN_S_KEY: 
			if(dev_type == DEV_TYPE_2) 						//return ���ؽ���ǰ���棬����2
			{
				mcb_hmu_disp_pages = mcb_hmu_disp_pages_bak;	
			}
			break;	
		case UP_S_KEY: 										//+
			if(mcb_hmu_modify_mode)
			{		
				if(mcb_hmu_para_bit)	
					mcb_hmu_para_bak += 10;
				else
					mcb_hmu_para_bak += 1;		

				if((mcb_hmu_para_index == 1) || (mcb_hmu_para_index == 3) || (mcb_hmu_para_index == 7))
				{
					if(mcb_hmu_para_bak > 30)		
						mcb_hmu_para_bak = 0;								
				}
				else
				if(mcb_hmu_para_index == 5)	
				{
					if(mcb_hmu_para_bak > 60)		
						mcb_hmu_para_bak = 0;			
				}
				else
				{
					if(mcb_hmu_para_bak > 99)		
						mcb_hmu_para_bak = 0;			
				}
			}
			break;	
		case DOWN_S_KEY: 									//item shift
			if(mcb_hmu_modify_mode)
			{			
				mcb_hmu_para_bit = 0;
				mcb_hmu_para_index++;	
				if(mcb_hmu_para_index > 7)
					mcb_hmu_para_index = 0;		
				//	
				if(mcb_hmu_para_index == 0)	
					mcb_hmu_para_bak = ant_used_flag;
				else
				if(mcb_hmu_para_index == 1)	
					mcb_hmu_para_bak = mr_ch1_att;
				else	
				if(mcb_hmu_para_index == 2)	
					mcb_hmu_para_bak = ant_used_gain;
				else
				if(mcb_hmu_para_index == 3)	
					mcb_hmu_para_bak = mr_ch2_att;
				else
				if(mcb_hmu_para_index == 4)	
					mcb_hmu_para_bak = wire_used_att;
				else
				if(mcb_hmu_para_index == 5)	
					mcb_hmu_para_bak = mr_com_att;
				else	
				if(mcb_hmu_para_index == 6)	
					mcb_hmu_para_bak = resv_para_def;
				else
				if(mcb_hmu_para_index == 7)	
					mcb_hmu_para_bak = mp_out_att;										
			}					
			break;	
		case LEFT_S_KEY: 									//bit shift
			if(mcb_hmu_modify_mode)
			{
				mcb_hmu_para_bit++;
				if(mcb_hmu_para_bit > 1)
					mcb_hmu_para_bit = 0;							
			}
			break;	
		case OK_S_KEY: 										//save
			if(mcb_hmu_modify_mode)
			{			
				mcb_hmu_modify_mode = 0;
				if(mcb_hmu_para_index == 0)	
					ant_used_flag = mcb_hmu_para_bak;
				else
				if(mcb_hmu_para_index == 1)	
				{
					if(mr_ch1_att != mcb_hmu_para_bak)
					{
						mr_ch1_att = mcb_hmu_para_bak;
						mr_ch_att_sel = 0x01;
						mr_comm_sys_tx_cmd2 = QQSZSBSSJZ_CMD_85;
						mr_comm_sys_tx_flag = 1;
						mcb_rfb_task_flag = 1;								
					}
				}
				else	
				if(mcb_hmu_para_index == 2)	
					ant_used_gain = mcb_hmu_para_bak;
				else
				if(mcb_hmu_para_index == 3)	
				{
					if(mr_ch2_att != mcb_hmu_para_bak)
					{
						mr_ch2_att = mcb_hmu_para_bak;
						mr_ch_att_sel = 0x02;
						mr_comm_sys_tx_cmd2 = QQSZSBSSJZ_CMD_85;
						mr_comm_sys_tx_flag = 1;
						mcb_rfb_task_flag = 1;								
					}
				}
				else
				if(mcb_hmu_para_index == 4)	
					wire_used_att = mcb_hmu_para_bak;
				else
				if(mcb_hmu_para_index == 5)	
				{
					if(mr_com_att != mcb_hmu_para_bak)
					{
						mr_com_att = mcb_hmu_para_bak;
						mr_ch_att_sel = 0x04;
						mr_comm_sys_tx_cmd2 = QQSZSBSSJZ_CMD_85;
						mr_comm_sys_tx_flag = 1;
						mcb_rfb_task_flag = 1;								
					}
				}
				else	
				if(mcb_hmu_para_index == 6)	
				{
					if(resv_para_def != mcb_hmu_para_bak)
					{
						resv_para_def = mcb_hmu_para_bak;	
						//for test
						if(resv_para_def == 11)
						{
							mr_comm_sys_tx_cmd2 = QQDQSBBLX_CMD_86;
							mr_comm_sys_tx_flag = 1;
							mcb_rfb_task_flag = 1;			
						}	
						else
						if(resv_para_def == 12)
						{
							mr_gps_or_bd = 0;
							mr_channel_no = 1;
							mr_comm_sys_tx_cmd2 = QQKYXZZB_CMD_87;
							mr_comm_sys_tx_flag = 1;
							mcb_rfb_task_flag = 1;								
						}	
						else
						if(resv_para_def == 13)
						{
							mr_gps_or_bd = 0;
							mr_channel_no = 2;							
							mr_comm_sys_tx_cmd2 = QQKYXZZB_CMD_87;
							mr_comm_sys_tx_flag = 1;
							mcb_rfb_task_flag = 1;								
						}	
						else
						if(resv_para_def == 14)
						{
							mr_gps_or_bd = 1;
							mr_channel_no = 1;							
							mr_comm_sys_tx_cmd2 = QQKYXZZB_CMD_87;
							mr_comm_sys_tx_flag = 1;
							mcb_rfb_task_flag = 1;								
						}		
						else
						if(resv_para_def == 15)
						{
							mr_gps_or_bd = 1;
							mr_channel_no = 2;							
							mr_comm_sys_tx_cmd2 = QQKYXZZB_CMD_87;
							mr_comm_sys_tx_flag = 1;
							mcb_rfb_task_flag = 1;								
						}		
						else
						if(resv_para_def == 16)
						{
							rfb_xybm_xybf_sel = 1;
							mr_comm_sys_tx_cmd2 = QQDQSBWJM_CMD_FE;
							mr_comm_sys_tx_flag = 1;
							mcb_rfb_task_flag = 1;								
						}		
						else
						if(resv_para_def == 17)
						{
							rfb_xybm_xybf_sel = 2;
							mr_comm_sys_tx_cmd2 = QQDQSBWJM_CMD_FE;
							mr_comm_sys_tx_flag = 1;
							mcb_rfb_task_flag = 1;								
						}	
						else
						if(resv_para_def == 18)
						{
							rfb_xybm_xybf_sel = 3;
							mr_comm_sys_tx_cmd2 = QQDQSBWJM_CMD_FE;
							mr_comm_sys_tx_flag = 1;
							mcb_rfb_task_flag = 1;								
						}	
						else
						if(resv_para_def == 51)
						{
							mp_mpd8_or_spd8 = 0x00;
							mp_gps_or_bd = 0;
							mp_comm_sys_tx_cmd2 = MP_QQKYXZZB_CMD_87;
							mp_comm_sys_tx_flag = 1;
							mcb_pd8_task_flag = 1;									
						}	
						else
						if(resv_para_def == 52)
						{
							mp_mpd8_or_spd8 = 0x00;
							mp_gps_or_bd = 1;
							mp_comm_sys_tx_cmd2 = MP_QQKYXZZB_CMD_87;
							mp_comm_sys_tx_flag = 1;
							mcb_pd8_task_flag = 1;									
						}	
						else
						if(resv_para_def == 53)
						{
							mp_mpd8_or_spd8 = 0x01;
							mcb_cur_rw_slave_index = 0;
							mp_gps_or_bd = 0;
							mp_comm_sys_tx_cmd2 = MP_QQKYXZZB_CMD_87;
							mp_comm_sys_tx_flag = 1;
							mcb_pd8_task_flag = 1;									
						}		
						else
						if(resv_para_def == 54)
						{
							mp_mpd8_or_spd8 = 0x01;
							mcb_cur_rw_slave_index = 0;
							mp_gps_or_bd = 1;
							mp_comm_sys_tx_cmd2 = MP_QQKYXZZB_CMD_87;
							mp_comm_sys_tx_flag = 1;
							mcb_pd8_task_flag = 1;									
						}		
						else
						if(resv_para_def == 61)
						{
							mp_pd8_sel = 0x04;
							mp_comm_sys_tx_cmd2 = MP_QQDQSBWJM_CMD_FE;
							mp_comm_sys_tx_flag = 1;
							mcb_pd8_task_flag = 1;								
						}		
						else
						if(resv_para_def == 62)
						{
							mp_pd8_sel = 0x05;
							mp_comm_sys_tx_cmd2 = MP_QQDQSBWJM_CMD_FE;
							mp_comm_sys_tx_flag = 1;
							mcb_pd8_task_flag = 1;								
						}	
						else
						if(resv_para_def == 63)
						{
							mp_pd8_sel = 0x06;
							mp_comm_sys_tx_cmd2 = MP_QQDQSBWJM_CMD_FE;
							mp_comm_sys_tx_flag = 1;
							mcb_pd8_task_flag = 1;								
						}										
					}
				}
				else
				if(mcb_hmu_para_index == 7)	
				{
					if(mp_out_att != mcb_hmu_para_bak)
					{
						mp_out_att = mcb_hmu_para_bak;
						mp_ch_att_sel = 0x01;
						mp_comm_sys_tx_cmd2 = MP_QQSZSBLSJZ_CMD_85;
						mp_comm_sys_tx_flag = 1;
						mcb_pd8_task_flag = 1;								
					}
				}						
			}	
			else
			{
				mcb_hmu_modify_mode = 1;
				if(mcb_hmu_para_index == 0)	
					mcb_hmu_para_bak = ant_used_flag;
				else
				if(mcb_hmu_para_index == 1)	
					mcb_hmu_para_bak = mr_ch1_att;
				else	
				if(mcb_hmu_para_index == 2)	
					mcb_hmu_para_bak = ant_used_gain;
				else
				if(mcb_hmu_para_index == 3)	
					mcb_hmu_para_bak = mr_ch2_att;
				else
				if(mcb_hmu_para_index == 4)	
					mcb_hmu_para_bak = wire_used_att;
				else
				if(mcb_hmu_para_index == 5)	
					mcb_hmu_para_bak = mr_com_att;
				else	
				if(mcb_hmu_para_index == 6)	
					mcb_hmu_para_bak = resv_para_def;
				else
				if(mcb_hmu_para_index == 7)	
					mcb_hmu_para_bak = mp_out_att;							
			}
			break;			
		default: break;
	}
	key_value = NO_KEY;					
}
