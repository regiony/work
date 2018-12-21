/*
********************************************************************************
�ļ���  ��mcb_rfb.c
�ļ����������ذ�����Ƶ��ͨ��ģ�飬��Ƶ���Э��尴�����������������ĵط����ֿ�
������  �������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���  ��
�޸����ڣ�
********************************************************************************
*/

#define __MCB_RFB_GLOBAL
#include "mcb_rfb.h"                 
#include "public.h"

/*
********************************************************************************
�������ƣ�mcb_rfb_init
�������������ذ�����Ƶ��ͨ�ų�ʼ��
����������
����ֵ  ��
********************************************************************************
*/
void mcb_rfb_init(void) 
{
    uint8_t *pt; 
    uint32_t i;
    
    mcb_rfb_task_flag = 0;
    
    mr_comm_recv_end_flag = 0;
    mr_comm_link_tx_flag = 0;
    mr_comm_data_deal_flag = 0; 
    mr_comm_sys_tx_flag = 0;
    mr_comm_sys_tx_to_flag = 0;
    mr_comm_time_out_cnt = 0;
    mr_comm_time_out_en = 0;
    mr_comm_sys_tx_cmd = 0;
    mr_comm_sys_tx_cmd1 = 0;
    mr_comm_sys_tx_cmd2 = 0;
    mr_comm_send_recv_end = 1;
    mr_comm_sys_tx_end = 1;
    mr_to_cnt1 = 0;	
    mr_to_cnt2 = 0;	
    
    mr_gps_or_bd = 0;
    mr_channel_no = 1;   
    mr_ch_att_sel = 0;    
    mr_ch1_att = 0;	
    mr_ch2_att = 0;	
    mr_com_att = 0;	
    
    mr_in_out_auto_mode = 0;
    mr_in_out_manu_mode = 0;
    rfb_xybm_xybf_sel = 0;
    rfb_xyb_in_out_ru_flag = 0;
    rfb_xyb_ru_succ_flag = 0;
    
    pt = (uint8_t *)(&mr_comm_data.rx_cnt);
    for(i = 0; i < sizeof(MR_COMM_MSG_DEF); i++)
        *pt = 0;

    pt = (uint8_t *)(&mr_comm_msg.cur_mode);
    for(i = 0; i < sizeof(MR_COMM_TYPE_DEF); i++)
        *pt = 0;
    mr_comm_msg.cur_mode = 4;		//Ĭ��ͨ�����ģʽ
        
    pt = (uint8_t *)(&mr_comm_bt_time.year);
    for(i = 0; i < sizeof(MR_COMM_BT_TIME_DEF); i++)
        *pt = 0;    
    mr_comm_bt_time.year = 2000;    
 
    pt = (uint8_t *)(&mr_kyx_cn_array.CH1_GPS_KYX_CN[0][0]);
    for(i = 0; i < sizeof(MR_COMM_KYX_CN_DEF); i++)
        *pt = 0;         
        
    UART6_init(MCB_RFB_NW_BR);
}

/*
********************************************************************************
�������ƣ�mcb_rfb_task
�������������ذ�����Ƶ��ͨ������
����������
����ֵ  ��
********************************************************************************
*/
void mcb_rfb_task(void) 
{
    mcb_rfb_task_flag = 0;
    //ϵͳ��������
    if((mr_comm_sys_tx_flag) && (mr_comm_sys_tx_end))
    {
        mr_comm_sys_tx_flag = 0;
        if(mr_comm_send_recv_end)                           //һ��ͨ��������ɣ��˴��ṹ�����Ż�������д���ڵ���
        {
            if(mr_comm_sys_tx_cmd2 != 0)
            {
                mr_comm_sys_tx_cmd = mr_comm_sys_tx_cmd2;   //����ѯ����
                //mr_comm_sys_tx_cmd2 = 0;
            }
            else
            if(mr_comm_sys_tx_cmd1 != 0)
            {
                mr_comm_sys_tx_cmd = mr_comm_sys_tx_cmd1;   //��ѯ����
                mr_comm_sys_tx_cmd1 = 0;
            }
            mr_comm_sys_send(mr_comm_sys_tx_cmd);           //����������֡����
            mr_comm_send_recv_end = 0;
            mr_comm_time_out_cnt = 0;                       //��ʱ��ʼ��
            mr_comm_time_out_en = 1;    
            mr_comm_sys_tx_end = 0;
            mr_comm_link_tx_flag = 1;   
        }
        /*
        else                                                //һ��ͨ������δ���
        {
            if(mr_comm_sys_tx_cmd != QQCXBXXZT_CMD_88)
            {
                mr_comm_sys_tx_flag = 1;
                mcb_rfb_task_flag = 1;                      
            }
        }
        */
    }
    //else
    //ϵͳ���ͳ�ʱ����
    if((mr_comm_sys_tx_to_flag) && (mr_comm_sys_tx_end))
    {
    	mr_comm_sys_tx_to_flag = 0;		
		mr_to_cnt1++;
		if(mr_to_cnt1 < RECV_TO_ALLOW_NUM)						//��ʱ����ش�3��
		{
			mr_comm_data.rx_cnt = 0;
			mr_comm_data.comm_state |= RECV_TIME_OUT;	
			if(mr_comm_sys_tx_cmd != QQCXBXXZT_CMD_88)
			{
				//mr_comm_sys_tx_end = 1;
				mr_comm_sys_tx_flag = 1;
				mr_comm_send_recv_end = 1;	
                mcb_rfb_task_flag = 1;    
			}
			else
			{
				mr_to_cnt1 = 0;
				mr_comm_time_out_en = 0;
				mr_comm_send_recv_end = 1;	
			}									
		}
		else
		{
			mr_to_cnt1 = 0;
			mr_comm_time_out_en = 0;
            mr_comm_sys_tx_cmd2 = 0;
			mr_comm_send_recv_end = 1;	
		}
    }
    //�������ݴ���
    //else
    if(mr_comm_recv_end_flag)
    {
        mr_comm_recv_end_flag = 0;              
        mr_comm_data_deal();                                //���ݽ���
        mr_comm_data_deal_flag = 0;
        /*
        if(mr_comm_sys_tx_cmd2 != 0) 						//��ֹ©��ϵͳ����ѯ����
        {
            mr_comm_sys_tx_flag = 1;
            mcb_rfb_task_flag = 1;                  
        }*/
    }
    //��·���ʹ���
    if(mr_comm_link_tx_flag)
    {
        mr_comm_link_tx_flag = 0;   
        mr_comm_data_send();                                //���ݷ���
    }
}

/*
********************************************************************************
�������ƣ�mr_comm_sys_send
�����������������ݺ���
����������
����ֵ  ��
********************************************************************************
*/
void mr_comm_sys_send(uint8_t cmd)
{ 
	uint8_t i, chksum;
    
    i = 0;
    //֡ͷ
    mr_comm_data.tx[i++] = COMM_SYN_CHAR_1;
    mr_comm_data.tx[i++] = COMM_SYN_CHAR_2;
	//��֡
    switch(cmd)
    {
        case QQCXBXXZT_CMD_88:  cmd_88_deal(); break;
        case QQKYXZZB_CMD_87:   cmd_87_deal(); break;
        case QQDQSBBLX_CMD_86:  cmd_86_deal(); break;
        case QQSZSBSSJZ_CMD_85: cmd_85_deal(); break;
        case QQDQSBSSJZ_CMD_84: cmd_84_deal(); break;
        case QQSBSCGLSZ_CMD_83: cmd_83_deal(); break;
        case QQJRTCZDMS_CMD_82: cmd_82_deal(); break;
        case QQJRTCSDMS_CMD_81: cmd_81_deal(); break;   
        case QQDQSBWJM_CMD_FE:  cmd_FE_deal(); break;
        case QQJRYCSJ_CMD_FF:   cmd_FF_deal(); break;                                           
        default: cmd_88_deal(); break;
    }
    //����У���
    chksum = 0;             
    for(i = 2; i < mr_comm_data.tx_len; i++)
        chksum ^= mr_comm_data.tx[i];       
    mr_comm_data.tx[mr_comm_data.tx_len++] = chksum; 
}

/*
********************************************************************************
�������ƣ�mr_comm_data_deal
�������������ݽ���������
����������
����ֵ  ��
********************************************************************************
*/
void mr_comm_data_deal(void)
{
    uint8_t i, chksum;  
    
    chksum = 0;             //����У���
    for(i = 2; i < mr_comm_data.rx_len; i++)
        chksum ^= mr_comm_data.rx[i];

    //mr_comm_data.comm_state &= ~RECV_TIME_OUT;
    //sys_fault_word &= ~0x01;	

    if(chksum)              //У��ʹ���
    {
        mr_comm_data.comm_state |= RECV_CHKSUM_ERR;
        //��Э��Լ��У��ʹ�������Ӧ��������𷽸���Ӧ��ʱ�ط�����
    }
    else                    //У�����ȷ
    {
        mr_comm_data.comm_state &= ~RECV_CHKSUM_ERR;
        switch(mr_comm_data.rx[2])  
        {
            case HFBXX_CMD_01:      cmd_01_deal(); break;
            case HFQR_CMD_02:       cmd_02_deal(); break;
            case HFSGSJ_CMD_03:     cmd_03_deal(); break;
            case HFBLX_CMD_04:      cmd_04_deal(); break;   
            case HFKYXZZB_CMD_05:   cmd_05_deal(); break;
            case ZDTZSJCG_CMD_7C:   cmd_7C_deal(); break;
            case HFJRSJQR_CMD_7D:   cmd_7D_deal(); break;
            case ZDTZTCYCSJ_CMD_7E: cmd_7E_deal(); break;   
            case HFSBWJM_CMD_7F:    cmd_7F_deal(); break;                           
            default: break; 
        }
    }
    //���豸�ظ�֡�������豸����֡�������´��� 
    if((mr_comm_data.rx[2] != ZDTZSJCG_CMD_7C) || (mr_comm_data.rx[2] != ZDTZTCYCSJ_CMD_7E))
    {
	    mr_comm_time_out_en = 0;					//��ʱ����ʹ�ܱ�־��0
	    mr_comm_send_recv_end = 1;  				//һ�η���������ɱ�־
        mr_comm_sys_tx_cmd2 = 0;
	    mr_comm_data.comm_state &= ~RECV_TIME_OUT;      
	    sys_fault_word &= ~0x01;	
	    mr_to_cnt1 = 0;	
	    mr_to_cnt2 = 0;	
    }   	
}

/*
********************************************************************************
�������ƣ�mr_comm_data_send
�����������������ݺ���
����������
����ֵ  ��
********************************************************************************
*/
void mr_comm_data_send(void)
{
    mr_comm_data.tx_cnt = 0;
    USART6->ICR |= 0x40;
    USART6->CR1 |= 0x40;
    USART6->TDR = mr_comm_data.tx[mr_comm_data.tx_cnt++];
}

/*
********************************************************************************
�������ƣ�cmd_88_deal
������������ȡ����Ϣ��״̬
����������
����ֵ  ��
********************************************************************************
*/
void cmd_88_deal(void)
{
    uint8_t i;
    
    i = 2;
    mr_comm_data.tx[i++] = QQCXBXXZT_CMD_88;
    mr_comm_data.tx[i++] = 0x01;
    if(rfb_or_xyb == RFB)
        mr_comm_data.tx[i++] = 0x01;
    else
     if(rfb_or_xyb == XYB)	
        mr_comm_data.tx[i++] = 0x02;
    mr_comm_data.tx_len = i;
}

/*
********************************************************************************
�������ƣ�cmd_87_deal
������������ȡ���豸�������Ǻż��������
����������
����ֵ  ��
********************************************************************************
*/
void cmd_87_deal(void)
{
    uint8_t i;
    
    i = 2;
    mr_comm_data.tx[i++] = QQKYXZZB_CMD_87;
    mr_comm_data.tx[i++] = 0x02;
    mr_comm_data.tx[i++] = mr_gps_or_bd;
    mr_comm_data.tx[i++] = mr_channel_no;
    mr_comm_data.tx_len = i;
}

/*
********************************************************************************
�������ƣ�cmd_86_deal
������������ȡ���豸������
����������
����ֵ  ��
********************************************************************************
*/
void cmd_86_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mr_comm_data.tx[i++] = QQDQSBBLX_CMD_86;
    mr_comm_data.tx[i++] = 0x01;
    mr_comm_data.tx[i++] = 0x01;
    mr_comm_data.tx_len = i;
}

/*
********************************************************************************
�������ƣ�cmd_85_deal
����������������豸����3��˥��ֵ mr_ch_att_sel
����������
����ֵ  ��
********************************************************************************
*/
void cmd_85_deal(void)
{
   	uint8_t i, n;
    
    i = 2;
    mr_comm_data.tx[i++] = QQSZSBSSJZ_CMD_85;
    // mr_comm_data.tx[3] = len;
    
    n = 0;
    i = 4;
    
    if(mr_ch_att_sel & 0x01)		//����ͨ��1 att
    {
	    mr_comm_data.tx[i++] = 0x01;
	    mr_comm_data.tx[i++] = mr_ch1_att;
	    n += 2;
    }
    if(mr_ch_att_sel & 0x02)		//����ͨ��2 att
    {
	    mr_comm_data.tx[i++] = 0x02;
	    mr_comm_data.tx[i++] = mr_ch2_att;
	    n += 2;
    }
    if(mr_ch_att_sel & 0x04)		//���ù���ͨ�� att
    {
	    mr_comm_data.tx[i++] = 0x03;
	    mr_comm_data.tx[i++] = mr_com_att;
	    n += 2;
    }
    mr_comm_data.tx[3] = n;
    
    mr_comm_data.tx_len = i;    
}

/*
********************************************************************************
�������ƣ�cmd_84_deal
����������������豸��ȡ3��˥��ֵ
����������
����ֵ  ��
********************************************************************************
*/
void cmd_84_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mr_comm_data.tx[i++] = QQDQSBSSJZ_CMD_84;
    mr_comm_data.tx[i++] = 0x01;
    mr_comm_data.tx[i++] = 0x03;
    mr_comm_data.tx_len = i;    
}

/*
********************************************************************************
�������ƣ�cmd_83_deal
����������������豸����������ã�������
����������
����ֵ  ��
********************************************************************************
*/
void cmd_83_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mr_comm_data.tx[i++] = QQSBSCGLSZ_CMD_83;
    mr_comm_data.tx[i++] = 0x01;
    mr_comm_data.tx[i++] = mr_channel_no;
    mr_comm_data.tx_len = i;        
}

/*
********************************************************************************
�������ƣ�cmd_82_deal
����������������豸����\�˳��Զ�ģʽ
����������
����ֵ  ��
********************************************************************************
*/
void cmd_82_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mr_comm_data.tx[i++] = QQJRTCZDMS_CMD_82;
    mr_comm_data.tx[i++] = 0x01;
    mr_comm_data.tx[i++] = mr_in_out_auto_mode;
    mr_comm_data.tx_len = i;     
}

/*
********************************************************************************
�������ƣ�cmd_81_deal
����������������豸����\�˳��ֶ�ģʽ
����������
����ֵ  ��
********************************************************************************
*/
void cmd_81_deal(void) 
{
   	uint8_t i;
    
    i = 2;
    mr_comm_data.tx[i++] = QQJRTCSDMS_CMD_81;
    mr_comm_data.tx[i++] = 0x01;
    mr_comm_data.tx[i++] = mr_in_out_manu_mode;
    mr_comm_data.tx_len = i;        
}

/*
********************************************************************************
�������ƣ�cmd_80_deal
����������ȷ�ϴ��豸���͵�����
����������1��ok��0��fail
����ֵ  ��
********************************************************************************
*/
void cmd_80_deal(uint8_t type, uint8_t flag)
{
    uint8_t i, chksum;
    
    i = 0;
    while(!mr_comm_sys_tx_end)		//ȷ��������·����ܣ��������⿪���»���
    {
    	delay_xms(1);
    	i++;
    	if(i > 4)	
    		break;    	
    }   
    mr_comm_sys_tx_end = 0;
    
    i = 0;
    //֡ͷ
    mr_comm_data.tx[i++] = COMM_SYN_CHAR_1;
    mr_comm_data.tx[i++] = COMM_SYN_CHAR_2;
    mr_comm_data.tx[i++] = QRSBFSSS_CMD_80;
    mr_comm_data.tx[i++] = 0x02;
    mr_comm_data.tx[i++] = type;
    if(flag)
    	mr_comm_data.tx[i++] = COMM_OK;
    else
    	mr_comm_data.tx[i++] = COMM_FAIL;
    mr_comm_data.tx_len = i;     
    //����У���
    chksum = 0;             
    for(i = 2; i < mr_comm_data.tx_len; i++)
        chksum ^= mr_comm_data.tx[i];       
    mr_comm_data.tx[mr_comm_data.tx_len++] = chksum; 
    
	mr_comm_link_tx_flag = 1;
}

/*
********************************************************************************
�������ƣ�cmd_FE_deal
�������������豸�ļ���
����������
����ֵ  ��
********************************************************************************
*/
void cmd_FE_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mr_comm_data.tx[i++] = QQDQSBWJM_CMD_FE;
    mr_comm_data.tx[i++] = 0x01;
    mr_comm_data.tx[i++] = rfb_xybm_xybf_sel;
    mr_comm_data.tx_len = i;      
}

/*
********************************************************************************
�������ƣ�cmd_FF_deal
����������֪ͨ���豸����Զ������ģʽ
����������
����ֵ  ��
********************************************************************************
*/
void cmd_FF_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mr_comm_data.tx[i++] = QQJRYCSJ_CMD_FF;
    mr_comm_data.tx[i++] = 0x01;
    if(rfb_xyb_in_out_ru_flag & 0x01)
    	mr_comm_data.tx[i++] = 0x01;
    else
    if(rfb_xyb_in_out_ru_flag & 0x02)
    	mr_comm_data.tx[i++] = 0x02;
    else
    if(rfb_xyb_in_out_ru_flag & 0x04)
    	mr_comm_data.tx[i++] = 0x03;
    mr_comm_data.tx_len = i;         
}

/*
********************************************************************************
�������ƣ�cmd_01_deal
�����������ظ���Ϣ��״̬
����������
����ֵ  ��
********************************************************************************
*/
void cmd_01_deal(void)
{
	uint8_t i, *pt;

    if(mr_comm_data.rx[3] == 0x20)      //ȷ�ϳ���
    {
		i = 4;
		//ģʽ��ͨ��
        mr_comm_msg.cur_mode = mr_comm_data.rx[i++];
        mr_comm_msg.cur_chno = mr_comm_data.rx[i++];
        //������
        mr_comm_msg.e_or_w = mr_comm_data.rx[i++];
        //����
        pt = (uint8_t *)&mr_comm_msg.longitude;
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        //�ϱ�γ
        mr_comm_msg.s_or_n = mr_comm_data.rx[i++];
        //γ��
        pt = (uint8_t *)&mr_comm_msg.latitude;
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        //����
        pt = (uint8_t *)&mr_comm_msg.altitude;
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];    
        //ͨ��1��2�ɼ�������
        mr_comm_msg.ch1_gps_kjx = mr_comm_data.rx[i++];
        mr_comm_msg.ch1_bd_kjx = mr_comm_data.rx[i++];
        mr_comm_msg.ch1_gps_kyx = mr_comm_data.rx[i++];
        mr_comm_msg.ch1_bd_kyx = mr_comm_data.rx[i++];
        mr_comm_msg.ch2_gps_kjx = mr_comm_data.rx[i++];
        mr_comm_msg.ch2_bd_kjx = mr_comm_data.rx[i++];
        mr_comm_msg.ch2_gps_kyx = mr_comm_data.rx[i++];    
        mr_comm_msg.ch2_bd_kyx = mr_comm_data.rx[i++];  
        //UTC TIME
        mr_comm_msg.year = mr_comm_data.rx[i++];
        mr_comm_msg.month = mr_comm_data.rx[i++];
        mr_comm_msg.day = mr_comm_data.rx[i++];
        mr_comm_msg.hour = mr_comm_data.rx[i++];
        mr_comm_msg.min = mr_comm_data.rx[i++];
        mr_comm_msg.sec = mr_comm_data.rx[i++];
        //״̬
        mr_comm_msg.state1 = mr_comm_data.rx[i++];    
        mr_comm_msg.state2 = mr_comm_data.rx[i++];    
        if(mr_comm_msg.state2 & 0x80)
        	sys_fault_word |= 0x20;
        else
        	sys_fault_word &= ~0x20;
           
      	//�õ�����ʱ��Ϊ������ȡʱ���
        UTCToBeijing(mr_comm_msg.year, mr_comm_msg.month, mr_comm_msg.day, mr_comm_msg.hour, mr_comm_msg.min, mr_comm_msg.sec);
        //
        //ÿ��0���LCD������ʾ��ͨ�������ߺ͵�Դ�ĸ澯�����Լ�����/GPS�����С��������
        if((!mr_comm_bt_time.hour) && (!mr_comm_bt_time.min) && (mr_comm_bt_time.sec < 	HOUR00_MIN00_SECX) && (!mcb_alm_0hour_clr))		//Ϊ�˱���10s�ھ���Ч
        {
        	//֪ͨ����ģ�鴦��
        	mcb_alm_0hour_clr = 1;	
        }
    }
}

/*
********************************************************************************
�������ƣ�cmd_02_deal
����������ȷ�����豸���͵���������
����������
����ֵ  ��
********************************************************************************
*/
void cmd_02_deal(void)
{
    if(mr_comm_data.rx[3] == 0x02)
    {
    	if(mr_comm_data.rx[5] == COMM_OK)
    	{
  			;		//ȷ��ok�����账��
    	}	
    	else
    	if(mr_comm_data.rx[5] == COMM_FAIL)
    	{
    		switch(mr_comm_data.rx[4])			//�����÷����˻���wifi��������
    		{
    			//���롢�˳��ֶ�ģʽʧ��
    			case 0x00: 	break;
    			case 0x01: 	break;
    			case 0x02: 	break;
    			case 0x03: 	break;
    			case 0x04: 	break;
    			//���롢�˳��Զ�ģʽʧ��
    			case 0x10: 	break;	
    			case 0x11: 	break;		
    			//ͨ�������������ʧ��
    			case 0x21: 	break;	
    			case 0x22: 	break;	
    			//ͨ����������ʧ��
     			case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;
    		}
    		//��������ʧ�ܱ�־;		
    	}
    }
    else
    if(mr_comm_data.rx[3] == 0x04)
    {
    	if((mr_comm_data.rx[5] == COMM_OK) && (mr_comm_data.rx[7] == COMM_OK))
    	{
  			;		//ȷ��ok�����账��
    	}	
    	else
    	if(mr_comm_data.rx[5] == COMM_FAIL)		//�����÷����˻���wifi��������
    	{
    		switch(mr_comm_data.rx[4])	
    		{
     			//ͨ����������ʧ��
     			case 0x41:	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;	    			
    		}
    	}
    	else
    	if(mr_comm_data.rx[7] == COMM_FAIL)		//�����÷����˻���wifi��������
    	{
     		switch(mr_comm_data.rx[6])	
			{
				//ͨ����������ʧ��
				case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;		
			}	
    	}
    	//��������ʧ�ܱ�־;	
    }
    else
    if(mr_comm_data.rx[3] == 0x06)
    {
    	if((mr_comm_data.rx[5] == COMM_OK) && (mr_comm_data.rx[7] == COMM_OK) && (mr_comm_data.rx[9] == COMM_OK))
    	{
  			;		//ȷ��ok�����账��
    	}	
    	else
    	if(mr_comm_data.rx[5] == COMM_FAIL)		//�����÷����˻���wifi��������
    	{
     		switch(mr_comm_data.rx[4])	
     		{
     			//ͨ����������ʧ��
     			case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;       			
     		}
    	}
    	else
    	if(mr_comm_data.rx[7] == COMM_FAIL)		//�����÷����˻���wifi��������
    	{
     		switch(mr_comm_data.rx[6])	
     		{
     			//ͨ����������ʧ��
     			case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;       			
     		}	
    	}
    	else
    	if(mr_comm_data.rx[9] == COMM_FAIL)		//�����÷����˻���wifi��������
    	{
     		switch(mr_comm_data.rx[8])	
     		{
     			//ͨ����������ʧ��
     			case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;       			
     		}	
    	}
    	//��������ʧ�ܱ�־;	
    }
}

/*
********************************************************************************
�������ƣ�cmd_03_deal
��������������3��˥��ֵ
����������
����ֵ  ��
********************************************************************************
*/
void cmd_03_deal(void)
{
    if(mr_comm_data.rx[3] == 0x03)
    {
    	mr_ch1_att = mr_comm_data.rx[4];
		mr_ch2_att = mr_comm_data.rx[5];
		mr_com_att = mr_comm_data.rx[6];
    }  
}

/*
********************************************************************************
�������ƣ�cmd_04_deal
�������������ش��豸�İ�����
����������
����ֵ  ��
********************************************************************************
*/
void cmd_04_deal(void)
{
    if(mr_comm_data.rx[3] == 0x01)
    {
    	//rfb_or_xyb = mr_comm_data.rx[4];		//��Ƶ���Э���Ŀǰ��Ӳ��GPIO�ж�
    	if(rfb_or_xyb == mr_comm_data.rx[4])	//���û�л���û��������
    		;
    }
}

/*
********************************************************************************
�������ƣ�cmd_05_deal
�������������ش��豸�Ŀ������Ǻż��������
����������
����ֵ  ��
********************************************************************************
*/
void cmd_05_deal(void)
{
	uint8_t i , n;
	uint8_t *pt = 0;

    n = mr_comm_data.rx[3] - 2;			//�ɼ�������*2
    
    if(mr_comm_data.rx[4] == 0)			//GPS
    {
    	if(mr_comm_data.rx[5] == 1)		//ͨ��1
    		pt = (uint8_t *)&mr_kyx_cn_array.CH1_GPS_KYX_CN[0][0];
    	else
    	if(mr_comm_data.rx[5] == 2)		//ͨ��2
    		pt = (uint8_t *)&mr_kyx_cn_array.CH2_GPS_KYX_CN[0][0];
    }	
    else
	if(mr_comm_data.rx[4] == 1)			//BD
	{
    	if(mr_comm_data.rx[5] == 1)		//ͨ��1
    		pt = (uint8_t *)&mr_kyx_cn_array.CH1_BDS_KYX_CN[0][0];
    	else
    	if(mr_comm_data.rx[5] == 2)		//ͨ��2	
    		pt = (uint8_t *)&mr_kyx_cn_array.CH2_BDS_KYX_CN[0][0];	
	}
    
    if(pt != 0)
    {
		if(n <= (MR_KYX_MAX_NUM << 1))
        {
            for(i = 0; i < n; i++)
                *pt++ = mr_comm_data.rx[i + 6];	//��ȡ�����Ǽ������
        }
		n = MR_KYX_MAX_NUM << 1;
		for(; i < n; i++)
			*pt++ = 0;					//����12�ŵĿռ���0
    }
}

/*
********************************************************************************
�������ƣ�cmd_7C_deal
����������֪ͨ���豸Զ�������ɹ�
����������
����ֵ  ��
********************************************************************************
*/
void cmd_7C_deal(void)
{
    if(mr_comm_data.rx[3] == 0x01)
    {
		if(mr_comm_data.rx[4] == 0x01)
		{
			rfb_xyb_ru_succ_flag = 0x01;
			mcb_wifi_ru_xxb = 0x01;
		}
		else
		if(mr_comm_data.rx[4] == 0x02)
		{
			rfb_xyb_ru_succ_flag = 0x02;
			mcb_wifi_ru_xxb = 0x02;			
		}	
		else	
		if(mr_comm_data.rx[4] == 0x03)
		{
			rfb_xyb_ru_succ_flag = 0x04;
			mcb_wifi_ru_xxb = 0x04;			
		}
		cmd_80_deal(mr_comm_data.rx[4] | 0x80, 1); 	
		
		//֪ͨwifi�ӿ����񣬽�һ�����ϻ㱨����״̬
		mcb_wifi_sys_tx_cmd = WIFI_ZDTZSJCG_CMD_7C;
		wifi_comm_sys_tx_flag = 1;
		mcb_wifi_task_flag = 1;
    }  
    else
   		cmd_80_deal(mr_comm_data.rx[4] | 0x80, 0);        
}

/*
********************************************************************************
�������ƣ�cmd_7D_deal
�������������豸����Զ������ģʽȷ��֡
����������
����ֵ  ��
********************************************************************************
*/
void cmd_7D_deal(void)
{
    if(mr_comm_data.rx[3] == 0x01)
    {
		if(mr_comm_data.rx[4] == rfb_xybm_xybf_sel)
		{
			//֪ͨwifiģ�飬�ɹ�����
			//����󣬴˴��ڲ�������Ҫ���ߴ���
			if(mr_comm_data.rx[4] <= 2)
				mcb_wifi_ru_state = mr_comm_data.rx[4];
			else
			if(mr_comm_data.rx[4] == 3)
				mcb_wifi_ru_state = 0x04;
				
			mcb_wifi_sys_tx_cmd = WIFI_HFJRSJQR_CMD_7D;
			wifi_comm_sys_tx_flag = 1;
			mcb_wifi_task_flag = 1;
			
		}
		else
		{
			//֪ͨwifiģ�飬����ʧ��
			mcb_wifi_ru_state = 0x00;			
			mcb_wifi_sys_tx_cmd = WIFI_HFJRSJQR_CMD_7D;
			wifi_comm_sys_tx_flag = 1;
			mcb_wifi_task_flag = 1;
		}
    }      
}

/*
********************************************************************************
�������ƣ�cmd_7E_deal
����������֪ͨ���豸�˳�Զ������ģʽ
����������
����ֵ  ��
********************************************************************************
*/
void cmd_7E_deal(void)
{
    if(mr_comm_data.rx[3] == 0x01)
    {
		if((mr_comm_data.rx[4] == 0x01) || (mr_comm_data.rx[4] == 0x02))
			mcb_wifi_ru_xxb = mr_comm_data.rx[4];
		else	
		if(mr_comm_data.rx[4] == 0x03)
			mcb_wifi_ru_xxb = 0x04;
			
		rfb_xyb_in_out_ru_flag = 0x00;
		cmd_80_deal(mr_comm_data.rx[4], 1);   
		
		//֪ͨwifi�ӿ����񣬽�һ�����ϻ㱨�˳�����ģʽ
		mcb_wifi_sys_tx_cmd = WIFI_ZDTZTCYCSJ_CMD_7E;
		wifi_comm_sys_tx_flag = 1;
		mcb_wifi_task_flag = 1;
		
		//�˳��󣬲�������Ҫ��������
		
    }  
    else
    	cmd_80_deal(mr_comm_data.rx[4], 0);            
}

/*
********************************************************************************
�������ƣ�cmd_7F_deal
�������������ش��豸�ļ���
����������
����ֵ  ��
********************************************************************************
*/
void cmd_7F_deal(void)
{
	uint8_t i;
	
    if((mr_comm_data.rx[3] == 0x0B) || (mr_comm_data.rx[3] == 0x0C))
    {
		for(i = 0; i < mr_comm_data.rx[3]; i++)
		{
			if(rfb_xybm_xybf_sel < 3)
				rfb_xybm_file[i]	= mr_comm_data.rx[i + 4];	
			else
				rfb_xybf_file[i] = mr_comm_data.rx[i + 4];			
		}
		for(; i < 13; i++)
		{
			if(rfb_xybm_xybf_sel < 3)
				rfb_xybm_file[i]	= '\0';
			else
				rfb_xybf_file[i]	= '\0';			
		}
    }    
}

/*
********************************************************************************
�������ƣ�UART6_init
����������UART6��ʼ��
����������
����ֵ  ��
********************************************************************************
*/
void UART6_init(uint32_t br)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
            
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource0,GPIO_AF_2);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource1,GPIO_AF_2);        
    
    //USART6_TX -> PC0 , USART6_RX ->  PC1                              
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);        
            
    USART_InitStructure.USART_BaudRate = br;        //baud rate
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART6, &USART_InitStructure); 
    
    USART_ITConfig(USART6, USART_IT_RXNE,ENABLE);          
    USART_Cmd(USART6, ENABLE);
    
    NVIC_InitStruct.NVIC_IRQChannel = USART3_6_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

/*
********************************************************************************
�������ƣ�UTCToBeijing
����������UTCʱ��ת��Ϊ����ʱ��
����������
����ֵ  ��
********************************************************************************
*/
void UTCToBeijing(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	hour += 8;
	if(month==1 || month==3 || month==5 || month==7 || month==8 || month==10 || month==12)	//1,3,5,7,8,9,12��ÿ��Ϊ31��
	{
	    if(hour >= 24) 
	    {   
	        hour -= 24;
	        day += 1;			//�������24Сʱ����ȥ24Сʱ�����ټ���һ��
	        if(day > 31)
	        {
	        	day -= 31;
	        	month += 1;		//�������31һ�죬��ȥ31�죬�����һ����
	        	if(month > 12)
	        		year +=  1;	//����12���£����1
	        }	
	    }
	}
	else 
	if(month==4 || month==6 || month==9 || month==11)	//4��6��9��11��ÿ��Ϊ30��
	{
	    if(hour >= 24) 
	    {   
	        hour -= 24;
	        day += 1;			//�������24Сʱ����ȥ24Сʱ�����ټ���һ��
	        if(day > 30)
	        {
	        	day -= 30;
	        	month += 1;		//�������30һ�죬��ȥ30�죬�����һ����
	        }
	    }
	}
	else						//ʣ��Ϊ2�£�����Ϊ29�죬ƽ��Ϊ28��
	{
	    if(hour >= 24)
	    {
	        hour -= 24;
	        day += 1;
	        if((year%400 == 0) || (year%4 == 0  &&  year%100 != 0))	//�ж��Ƿ�Ϊ���꣬����ܱ�400����������ܱ�4�����������ܱ�100����Ϊ����
	        {
	        	if(day > 29)	//Ϊ����
	        	{
	        		day -= 29;
	        		month += 1;		
	        	}
	        }
	        else
	        {
	        	if(day > 28)	//Ϊƽ��
	        	{
	        		day -= 28;
	        		month += 1;
	        	}
	        }
		}
	}	
	//���±���ʱ��ṹ�����
	mr_comm_bt_time.year = year + 2000;
	mr_comm_bt_time.month = month; 
	mr_comm_bt_time.day = day; 
	mr_comm_bt_time.hour = hour; 
	mr_comm_bt_time.min = minute; 
	mr_comm_bt_time.sec = second; 
}
