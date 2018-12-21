/*
********************************************************************************
�ļ���	��mcb_pd8.c
�ļ����������ذ���˹��ְ�ͨ��ģ��
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __MCB_PD8_GLOBAL
#include "mcb_pd8.h" 
#include "public.h"                


/*
********************************************************************************
�������ƣ�mcb_pd8_init
�������������ذ���˹��ְ�ͨ�ų�ʼ��
����������
����ֵ	��
********************************************************************************
*/
void mcb_pd8_init(void) 
{
	uint8_t *pt; 
	uint32_t i;
	
	mcb_pd8_task_flag = 0;
	
	mp_comm_recv_end_flag = 0;
    mp_comm_link_tx_flag = 0;
    mp_comm_data_deal_flag = 0; 
    mp_comm_sys_tx_flag = 0;
    mp_comm_sys_tx_to_flag = 0;
    mp_comm_time_out_cnt = 0;
    mp_comm_time_out_en = 0;
    mp_comm_sys_tx_cmd = 0;
    mp_comm_sys_tx_cmd1 = 0;
    mp_comm_sys_tx_cmd2 = 0;
    mp_comm_send_recv_end = 1;
    mp_comm_sys_tx_end = 1;
    
    mp_to_cnt1 = 0;
    mp_to_cnt2 = 0;
    
    mp_mpd8_or_spd8 = 0;
    mp_gps_or_bd = 0;
    mp_ch_att_sel = 0;    
    mp_det_att = 0;	
    mp_out_att = 0;	
    mp_pd8_sel = 0;
    mp_pd8_in_out_ru_flag = 0;
    mp_pd8_ru_succ_flag = 0;
    
    mcb_cur_rw_slave_index = 0;
    mcb_cur_pd8_slave_num = 0;
	
	pt = (uint8_t *)(&mp_comm_data.rx_cnt);
	for(i = 0; i < sizeof(MP_COMM_TYPE_DEF); i++)
		*pt = 0;
		
	pt = (uint8_t *)(&mp_mpd8_comm_msg.mpd8_gps_kjx);
	for(i = 0; i < sizeof(MPD8_COMM_MSG_DEF); i++)
		*pt = 0;	
		
	pt = (uint8_t *)(&mpd8_kyx_cn_array.GPS_KYX_CN[0][0]);
	for(i = 0; i < sizeof(MP_COMM_KYX_CN_DEF); i++)
		*pt = 0;		
	for(i = 0; i < MP_SPD8_MAX_NUM; i++)
	{
		spd8_kyx_cn_array[i] = mpd8_kyx_cn_array;
		mcb_pd8_slave_addr[i] = 0;	
		mcb_cur_valid_slave_addr[i] = 0;	
	}
	
	pt = (uint8_t *)(&mp_spd8_comm_msg[0].spd8_addr);
	for(i = 0; i < sizeof(SPD8_COMM_MSG_DEF); i++)
		*pt = 0;			
	for(i = 1; i < MP_SPD8_MAX_NUM; i++)
	{
		mp_spd8_comm_msg[i] = mp_spd8_comm_msg[0];
	}	
	
	pt = (uint8_t *)(&mp_comm_bt_time.year);
    for(i = 0; i < sizeof(MP_COMM_BT_TIME_DEF); i++)
        *pt = 0;   
        
	UART5_init(MCB_PD8_NW_BR);
}

/*
********************************************************************************
�������ƣ�mcb_pd8_task
�������������ذ���˹��ְ�ͨ������
����������
����ֵ	��
********************************************************************************
*/
void mcb_pd8_task(void) 
{
    mcb_pd8_task_flag = 0;
    //ϵͳ��������
    if((mp_comm_sys_tx_flag) && (mp_comm_sys_tx_end))
    {
        mp_comm_sys_tx_flag = 0;
        if(mp_comm_send_recv_end)                           //һ��ͨ��������ɣ��˴��ṹ�����Ż�������д���ڵ���
        {
            if(mp_comm_sys_tx_cmd2 != 0)
            {
                mp_comm_sys_tx_cmd = mp_comm_sys_tx_cmd2;   //����ѯ����
                //mp_comm_sys_tx_cmd2 = 0;
            }
            else
            if(mp_comm_sys_tx_cmd1 != 0)
            {
                mp_comm_sys_tx_cmd = mp_comm_sys_tx_cmd1;   //��ѯ����
                mp_comm_sys_tx_cmd1 = 0;
            }
            mp_comm_sys_send(mp_comm_sys_tx_cmd);           //����������֡����
            mp_comm_send_recv_end = 0;
            mp_comm_time_out_cnt = 0;                       //��ʱ��ʼ��
            mp_comm_time_out_en = 1;    
            mp_comm_sys_tx_end = 0;
            mp_comm_link_tx_flag = 1;   
        }
        /*
        else                                                //һ��ͨ������δ���
        {
            if(mp_comm_sys_tx_cmd != MP_QQCXBXXZT_CMD_88)
            {
                mp_comm_sys_tx_flag = 1;
                mcb_pd8_task_flag = 1;                      
            }
        }
        */
    }
    //else
    //ϵͳ���ͳ�ʱ����
    if((mp_comm_sys_tx_to_flag) && (mp_comm_sys_tx_end))
    {
    	mp_comm_sys_tx_to_flag = 0;		
		mp_to_cnt1++;
		if(mp_to_cnt1 < MP_RECV_TO_ALLOW_NUM)					//��ʱ����ش�3��
		{
			mp_comm_data.rx_cnt = 0;
			mp_comm_data.comm_state |= MP_RECV_TIME_OUT;	
			if(mp_comm_sys_tx_cmd != MP_QQCXBXXZT_CMD_88)
			{
				//mp_comm_sys_tx_end = 1;
				mp_comm_sys_tx_flag = 1;
				mp_comm_send_recv_end = 1;	
                mcb_pd8_task_flag = 1;    
			}
			else
			{
				mp_to_cnt1 = 0;
				mp_comm_time_out_en = 0;
				mp_comm_send_recv_end = 1;	
			}									
		}
		else
		{
			mp_to_cnt1 = 0;
			mp_comm_time_out_en = 0;
			mp_comm_sys_tx_cmd2 = 0;
			mp_comm_send_recv_end = 1;	
		}
    }
    //�������ݴ���
    //else
    if(mp_comm_recv_end_flag)
    {
        mp_comm_recv_end_flag = 0;              
        mp_comm_data_deal();                                //���ݽ���
        mp_comm_data_deal_flag = 0;
        /*
        if(mp_comm_sys_tx_cmd2 != 0) 						//��ֹ©��ϵͳ����ѯ����
        {
            mp_comm_sys_tx_flag = 1;
            mcb_pd8_task_flag = 1;                  
        }*/
    }
    //��·���ʹ���
    if(mp_comm_link_tx_flag)
    {
        mp_comm_link_tx_flag = 0;   
        mp_comm_data_send();                                //���ݷ���
    }
}

/*
********************************************************************************
�������ƣ�mp_comm_sys_send
�����������������ݺ���
����������
����ֵ  ��
********************************************************************************
*/
void mp_comm_sys_send(uint8_t cmd)
{ 
	uint8_t i, chksum;
    
    i = 0;
    //֡ͷ
    mp_comm_data.tx[i++] = MP_COMM_SYN_CHAR_1;
    mp_comm_data.tx[i++] = MP_COMM_SYN_CHAR_2;
	//��֡
    switch(cmd)
    {
        case MP_QQCXBXXZT_CMD_88:  mp_cmd_88_deal(); break;
        case MP_QQKYXZZB_CMD_87:   mp_cmd_87_deal(); break;
        case MP_QQSZSBLSJZ_CMD_85: mp_cmd_85_deal(); break;
        case MP_QQDQSBLSJZ_CMD_84: mp_cmd_84_deal(); break;
        case MP_QQSBSCGLSZ_CMD_83: mp_cmd_83_deal(); break;
        case MP_QQDQSBWJM_CMD_FE:  mp_cmd_FE_deal(); break;
        case MP_QQJRYCSJ_CMD_FF:   mp_cmd_FF_deal(); break;                                           
        default: mp_cmd_88_deal(); break;
    }
    //����У���
    chksum = 0;             
    for(i = 2; i < mp_comm_data.tx_len; i++)
        chksum ^= mp_comm_data.tx[i];       
    mp_comm_data.tx[mp_comm_data.tx_len++] = chksum; 
}

/*
********************************************************************************
�������ƣ�mp_comm_data_deal
�������������ݽ���������
����������
����ֵ	��
********************************************************************************
*/
void mp_comm_data_deal(void)		//�ṹ���Ż�
{
    uint8_t i, chksum;  
    
    chksum = 0;             //����У���
    for(i = 2; i < mp_comm_data.rx_len; i++)
        chksum ^= mp_comm_data.rx[i];

    //mp_comm_data.comm_state &= ~MP_RECV_TIME_OUT;
    //sys_fault_word &= ~0x02;

    if(chksum)              //У��ʹ���
    {
        mp_comm_data.comm_state |= MP_RECV_CHKSUM_ERR;
        //��Э��Լ��У��ʹ�������Ӧ��������𷽸���Ӧ��ʱ�ط�����
    }
    else                    //У�����ȷ
    {
        mp_comm_data.comm_state &= ~MP_RECV_CHKSUM_ERR;
        switch(mp_comm_data.rx[2])  
        {
            case MP_HFZCJBXX_CMD_01:	mp_cmd_01_deal(); break;
            case MP_HFQR_CMD_02:       	mp_cmd_02_deal(); break;
            case MP_HFLGSJ_CMD_03:     	mp_cmd_03_deal(); break;
            case MP_HFKYXZZB_CMD_05:   	mp_cmd_05_deal(); break;
            case MP_ZDTZSJCG_CMD_7C:   	mp_cmd_7C_deal(); break;
            case MP_HFJRSJQR_CMD_7D:   	mp_cmd_7D_deal(); break;
            case MP_ZDTZTCYCSJ_CMD_7E: 	mp_cmd_7E_deal(); break;   
            case MP_HFSBWJM_CMD_7F:    	mp_cmd_7F_deal(); break;                           
            default: break; 
        }
    }
    //���豸�ظ�֡�������豸����֡�������´��� 
    if((mp_comm_data.rx[2] != MP_ZDTZSJCG_CMD_7C) || (mp_comm_data.rx[2] != MP_ZDTZTCYCSJ_CMD_7E))
    {
	    mp_comm_time_out_en = 0;					//��ʱ����ʹ�ܱ�־��0
	    mp_comm_send_recv_end = 1;  				//һ�η���������ɱ�־
	    mp_comm_sys_tx_cmd2 = 0;
	    mp_comm_data.comm_state &= ~MP_RECV_TIME_OUT;    
	    sys_fault_word &= ~0x02;  	
	    mp_to_cnt1 = 0;
    	mp_to_cnt2 = 0;	
    }   	
}

/*
********************************************************************************
�������ƣ�mp_comm_data_send
�����������������ݺ���
����������
����ֵ	��
********************************************************************************
*/
void mp_comm_data_send(void)
{
	mp_comm_data.tx_cnt = 0;
	USART5->ICR |= 0x40;
	USART5->CR1 |= 0x40;
	USART5->TDR = mp_comm_data.tx[mp_comm_data.tx_cnt++];
}

/*
********************************************************************************
�������ƣ�mp_cmd_88_deal
������������ȡ����Ϣ��״̬
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_88_deal(void)
{
    uint8_t i;
    
    i = 2;
    mp_comm_data.tx[i++] = MP_QQCXBXXZT_CMD_88;
    mp_comm_data.tx[i++] = 0x01;
    mp_comm_data.tx[i++] = 0x04;
    mp_comm_data.tx_len = i;
}

/*
********************************************************************************
�������ƣ�mp_cmd_87_deal
������������ȡ���豸�������Ǻż��������
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_87_deal(void)
{
    uint8_t i;
    
    i = 2;
    mp_comm_data.tx[i++] = MP_QQKYXZZB_CMD_87;
    mp_comm_data.tx[i++] = 0x03;
    mp_comm_data.tx[i++] = mp_mpd8_or_spd8;		//�����ӻ�ѡ��
    if(mp_mpd8_or_spd8)							//�ӻ�������ӻ���
    	mp_comm_data.tx[i++] = mcb_cur_rw_slave_index + 1;
    else  	
    	mp_comm_data.tx[i++] = 0;				//������Ϊ0
    mp_comm_data.tx[i++] = mp_gps_or_bd;
    mp_comm_data.tx_len = i;
}

/*
********************************************************************************
�������ƣ�mp_cmd_85_deal
����������������豸����2��˥��ֵ
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_85_deal(void)
{
   	uint8_t i, n;
    
    i = 2;
    mp_comm_data.tx[i++] = MP_QQSZSBLSJZ_CMD_85;
    // mr_comm_data.tx[3] = len;
    
    n = 0;
    i = 4;
    
    if(mp_ch_att_sel & 0x01)		//�������ͨ�� att
    {
	    mp_comm_data.tx[i++] = 0x01;
	    mp_comm_data.tx[i++] = mp_out_att;
	    n += 2;
    }
    if(mp_ch_att_sel & 0x02)		//���ü��ͨ�� att
    {
	    mp_comm_data.tx[i++] = 0x02;
	    mp_comm_data.tx[i++] = mp_det_att;
	    n += 2;
    }
    
    mp_comm_data.tx[3] = n;
    
    mp_comm_data.tx_len = i;    
}

/*
********************************************************************************
�������ƣ�mp_cmd_84_deal
����������������豸��ȡ2��˥��ֵ
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_84_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mp_comm_data.tx[i++] = MP_QQDQSBLSJZ_CMD_84;
    mp_comm_data.tx[i++] = 0x01;
    mp_comm_data.tx[i++] = 0x02;
    mp_comm_data.tx_len = i;    
}

/*
********************************************************************************
�������ƣ�mp_cmd_83_deal
����������������豸����������ã�������
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_83_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mp_comm_data.tx[i++] = MP_QQSBSCGLSZ_CMD_83;
    mp_comm_data.tx[i++] = 0x01;
    mp_comm_data.tx[i++] = 0x01;
    mp_comm_data.tx_len = i;        
}

/*
********************************************************************************
�������ƣ�mp_cmd_80_deal
����������ȷ�ϴ��豸���͵�����
����������1��ok��0��fail
����ֵ  ��
********************************************************************************
*/
void mp_cmd_80_deal(uint8_t type, uint8_t flag)
{
    uint8_t i, chksum;
    
    i = 0;
    while(!mp_comm_sys_tx_end)		//ȷ��������·�����
    {
    	delay_xms(1);
    	i++;
    	if(i > 4)	
    		break;    	
    }   
    mp_comm_sys_tx_end = 0;
    
    i = 0;
    //֡ͷ
    mp_comm_data.tx[i++] = MP_COMM_SYN_CHAR_1;
    mp_comm_data.tx[i++] = MP_COMM_SYN_CHAR_2;
    mp_comm_data.tx[i++] = MP_QRSBFSSS_CMD_80;
    mp_comm_data.tx[i++] = 0x02;
    mp_comm_data.tx[i++] = type;
    if(flag)
    	mp_comm_data.tx[i++] = MP_COMM_OK;
    else
    	mp_comm_data.tx[i++] = MP_COMM_FAIL;
    mp_comm_data.tx_len = i;     
    //����У���
    chksum = 0;             
    for(i = 2; i < mp_comm_data.tx_len; i++)
        chksum ^= mp_comm_data.tx[i];       
    mp_comm_data.tx[mp_comm_data.tx_len++] = chksum; 
    
	mp_comm_link_tx_flag = 1;
}

/*
********************************************************************************
�������ƣ�mp_cmd_FE_deal
�������������豸�ļ���
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_FE_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mp_comm_data.tx[i++] = MP_QQDQSBWJM_CMD_FE;
    mp_comm_data.tx[i++] = 0x01;
    mp_comm_data.tx[i++] = mp_pd8_sel;
    mp_comm_data.tx_len = i;      
}

/*
********************************************************************************
�������ƣ�mp_cmd_FF_deal
����������֪ͨ���豸����Զ������ģʽ
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_FF_deal(void)
{
   	uint8_t i;
    
    i = 2;
    mp_comm_data.tx[i++] = MP_QQJRYCSJ_CMD_FF;
    mp_comm_data.tx[i++] = 0x01;
    if(mp_pd8_in_out_ru_flag & 0x01)
    	mp_comm_data.tx[i++] = 0x04;
    else
    if(mp_pd8_in_out_ru_flag & 0x02)
    	mp_comm_data.tx[i++] = 0x05;
    else
    if(mp_pd8_in_out_ru_flag & 0x04)
    	mp_comm_data.tx[i++] = 0x06;
    mp_comm_data.tx_len = i;         
}

/*
********************************************************************************
�������ƣ�mp_cmd_01_deal
�����������ظ���Ϣ��״̬
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_01_deal(void)
{
	uint8_t i, j, z, flag;
	
	if(mp_comm_data.rx[3] >= 16)
	{
		mcb_cur_pd8_slave_num = (mp_comm_data.rx[3] - 16) / 15; 	//�ӻ�����			
		//������Ϣ��״̬
		j = 4;
		mp_mpd8_comm_msg.mpd8_gps_kjx = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.mpd8_bd_kjx = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.mpd8_gps_kyx = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.mpd8_bd_kyx = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.mpd8_port_state1 = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.mpd8_port_state2 = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.year = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.month = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.day = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.hour = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.min = mp_comm_data.rx[j++];
		mp_mpd8_comm_msg.sec = mp_comm_data.rx[j++];	
		mp_mpd8_comm_msg.mpd8_pwr_state	= mp_comm_data.rx[j++];	
		mp_mpd8_comm_msg.mpd8_out_state = mp_comm_data.rx[j++];	
		mp_mpd8_comm_msg.mpd8_alm_state = mp_comm_data.rx[j++];	
		mp_mpd8_comm_msg.mpd8_dev_state = mp_comm_data.rx[j++];	
		if(mp_mpd8_comm_msg.mpd8_dev_state & 0x80)
        	sys_fault_word |= 0x40;
        else
        	sys_fault_word &= ~0x40;
		//�ӻ�   
		for(i = 0; i < MP_SPD8_MAX_NUM; i++) 	
		{
			mcb_pd8_slave_addr[i] = 0;								//�ӻ���ַ��0
			mcb_cur_valid_slave_addr[i] = 0;						//��ǰ��Ч�ӻ���ַ
		}		
		flag = 0;
		for(z = 0; z < mcb_cur_pd8_slave_num; z++)
		{
			if(mp_comm_data.rx[j] > 0)
				mcb_cur_rw_slave_index = mp_comm_data.rx[j++] - 1;	
			else
				return ;		
			//�ӻ���Ϣ��״̬	
			mcb_pd8_slave_addr[mcb_cur_rw_slave_index] = mcb_cur_rw_slave_index + 1;	//�ӻ���ַ
			mcb_cur_valid_slave_addr[z] = mcb_pd8_slave_addr[mcb_cur_rw_slave_index];	//��ǰ��Ч�ӻ���ַ
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_addr = mcb_pd8_slave_addr[mcb_cur_rw_slave_index];	
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_gps_kjx = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_bd_kjx = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_gps_kyx = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_bd_kyx = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_port_state = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].year = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].month = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].day = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].hour = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].min = mp_comm_data.rx[j++];
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].sec = mp_comm_data.rx[j++];	
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_out_state = mp_comm_data.rx[j++];	
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_alm_state = mp_comm_data.rx[j++];	
			mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_dev_state = mp_comm_data.rx[j++];		
			if(mp_spd8_comm_msg[mcb_cur_rw_slave_index].spd8_dev_state & 0x80)
        		flag = 1;
		}
		if(flag)
			sys_fault_word |= 0x80;
		else
			sys_fault_word &= ~0x80;
	}
}

/*
********************************************************************************
�������ƣ�mp_cmd_02_deal
����������ȷ�����豸���͵���������
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_02_deal(void)
{
    if(mp_comm_data.rx[3] == 0x02)
    {
    	if(mp_comm_data.rx[5] == MP_COMM_OK)
    	{
  			;		//ȷ��ok�����账��
    	}	
    	else
    	if(mp_comm_data.rx[5] == MP_COMM_FAIL)
    	{
    		if(mp_comm_data.rx[4] == 0x21)
    		{
				;			//�����÷����˻���wifi������״̬��  
  				
    		}
    		else
    		if(mp_comm_data.rx[4] == 0x41)
    		{
				;			//�����÷����˻���wifi������״̬��  
  				
    		}
    		else
    		if(mp_comm_data.rx[4] == 0x42)
    		{
				;			//�����÷����˻���wifi������״̬��  
  				
    		}
    	}
    }
    else
    if(mp_comm_data.rx[3] == 0x04)
    {
    	if((mp_comm_data.rx[5] == MP_COMM_OK) && (mp_comm_data.rx[7] == MP_COMM_OK))
    	{
  			;		//ȷ��ok�����账��
    	}	
    	else
    	if(mp_comm_data.rx[5] == MP_COMM_FAIL)
    	{
    		if(mp_comm_data.rx[4] == 0x41)
    		{
				;		//�����÷����˻���wifi������״̬��  
  				
    		}
    		else
    		if(mp_comm_data.rx[4] == 0x42)
    		{
				;		//�����÷����˻���wifi������״̬��  
  				
    		}
    	} 
    	else
    	if(mp_comm_data.rx[7] == MP_COMM_FAIL)
    	{
    		if(mp_comm_data.rx[6] == 0x41)
    		{
				;		//�����÷����˻���wifi������״̬��  
  				
    		}
    		else
    		if(mp_comm_data.rx[6] == 0x42)
    		{
				;		//�����÷����˻���wifi������״̬��  
  				
    		}
    	}     	   		
    }
}

/*
********************************************************************************
�������ƣ�mp_cmd_03_deal
��������������2��˥��ֵ
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_03_deal(void)
{
    if(mp_comm_data.rx[3] == 0x02)
    {
    	mp_out_att = mp_comm_data.rx[4];
		mp_det_att = mp_comm_data.rx[5];
    }  
}

/*
********************************************************************************
�������ƣ�mp_cmd_05_deal
�������������ش��豸�Ŀ������Ǻż��������
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_05_deal(void)
{
	uint8_t i , n;
	uint8_t *pt = 0;

    n = mp_comm_data.rx[3] - 3;				//�ɼ�������*2
    
    if(mp_comm_data.rx[4] == 0)				//����
    {
    	//if(mp_comm_data.rx[5] == 0)		//�̶�Ϊ0
	    if(mp_comm_data.rx[6] == 0)			//GPS
	    {
	    	pt = (uint8_t *)&mpd8_kyx_cn_array.GPS_KYX_CN[0][0];
	    }	
	    else
		if(mp_comm_data.rx[6] == 1)			//BD
		{
	    	pt = (uint8_t *)&mpd8_kyx_cn_array.BDS_KYX_CN[0][0];
		}
    }
    else
    if(mp_comm_data.rx[4] == 1)				//�ӻ�	
    {
    	if((mp_comm_data.rx[5] > 0) && (mp_comm_data.rx[5] < 9))
        {
            mcb_cur_rw_slave_index = mp_comm_data.rx[5] - 1;	//�ӻ�������
            if(mp_comm_data.rx[6] == 0)			//GPS
            {
                pt = (uint8_t *)&spd8_kyx_cn_array[mcb_cur_rw_slave_index].GPS_KYX_CN[0][0];
            }	
            else
            if(mp_comm_data.rx[6] == 1)			//BD
            {
                pt = (uint8_t *)&spd8_kyx_cn_array[mcb_cur_rw_slave_index].BDS_KYX_CN[0][0];
            }    
        }            
    }
    
    if(pt != 0)
    {
        if(n <= (MP_KYX_MAX_NUM << 1))
        {
            for(i = 0; i < n; i++)
                *pt++ = mp_comm_data.rx[i + 7];	//��ȡ�����Ǽ������
        }
        n = MP_KYX_MAX_NUM << 1;
        for(; i < n; i++)
            *pt++ = 0;						//����12�ŵĿռ���0
    }
}

/*
********************************************************************************
�������ƣ�mp_cmd_7C_deal
����������֪ͨ���豸Զ�������ɹ�
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_7C_deal(void)
{
    if(mp_comm_data.rx[3] == 0x01)
    {
		if(mp_comm_data.rx[4] == 0x04)
		{
			mp_pd8_ru_succ_flag = 0x01;
			mcb_wifi_ru_xxb = 0x08;		
		}
		else
		if(mp_comm_data.rx[4] == 0x05)
		{
			mp_pd8_ru_succ_flag = 0x02;
			mcb_wifi_ru_xxb = 0x10;		
		}
		else	
		if(mp_comm_data.rx[4] == 0x06)
		{
			mp_pd8_ru_succ_flag = 0x04;
			mcb_wifi_ru_xxb = 0x20;		
		}
		mp_cmd_80_deal(mp_comm_data.rx[4] | 0x80, 1); 	
		
		//֪ͨwifi�ӿ����񣬽�һ�����ϻ㱨����״̬
		mcb_wifi_sys_tx_cmd = WIFI_ZDTZSJCG_CMD_7C;
		wifi_comm_sys_tx_flag = 1;
		mcb_wifi_task_flag = 1;
		
    }  
    else
   		mp_cmd_80_deal(mp_comm_data.rx[4] | 0x80, 0);        
}

/*
********************************************************************************
�������ƣ�mp_cmd_7D_deal
�������������豸����Զ������ģʽȷ��֡
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_7D_deal(void)
{
    if(mp_comm_data.rx[3] == 0x01)
    {
		if(mp_comm_data.rx[4] == mp_pd8_sel)
		{
			//֪ͨwifiģ�飬�ɹ�����
			//����󣬴˴��ڲ�������Ҫ���ߴ���
			if(mr_comm_data.rx[4] == 4)
				mcb_wifi_ru_state = 0x08;
			else
			if(mr_comm_data.rx[4] == 5)
				mcb_wifi_ru_state = 0x10;	
			else
			if(mr_comm_data.rx[4] == 6)
				mcb_wifi_ru_state = 0x20;			
			
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
�������ƣ�mp_cmd_7E_deal
����������֪ͨ���豸�˳�Զ������ģʽ
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_7E_deal(void)
{
    if(mp_comm_data.rx[3] == 0x01)
    {
		if(mp_comm_data.rx[4] == 0x04) 
		{
			mcb_wifi_ru_xxb = 0x08;	
		}
		else		
		if(mp_comm_data.rx[4] == 0x05) 
		{
			mcb_wifi_ru_xxb = 0x10;	
		}			
		else
		if(mp_comm_data.rx[4] == 0x06)
		{
			mcb_wifi_ru_xxb = 0x20;		
		}
		mp_pd8_in_out_ru_flag = 0x00;
		mp_cmd_80_deal(mp_comm_data.rx[4], 1);   
		
		//֪ͨwifi�˳�����ģʽ
		mcb_wifi_sys_tx_cmd = WIFI_ZDTZTCYCSJ_CMD_7E;
		wifi_comm_sys_tx_flag = 1;
		mcb_wifi_task_flag = 1;
		
		//�˳��󣬲�������Ҫ��������
		
    }  
    else
    	mp_cmd_80_deal(mp_comm_data.rx[4], 0);            
}

/*
********************************************************************************
�������ƣ�mp_cmd_7F_deal
�������������ش��豸�ļ���
����������
����ֵ  ��
********************************************************************************
*/
void mp_cmd_7F_deal(void)
{
	uint8_t i, j, z, addr;
	
	if (mp_comm_data.rx[4] == 0x04)						//����
	{
		if(mp_comm_data.rx[3] == 0x0D)
		{
			for(i = 0; i < (13 - 1); i++)
				mpd8_file[i] = mp_comm_data.rx[i + 5];
			mpd8_file[12]	= '\0';				
		}
	}
	else
	if (mp_comm_data.rx[4] == 0x05)						//�ӻ�
	{
		if(mp_comm_data.rx[3] == 0x07)					//�޴ӻ�
		{
			;//�޴ӻ����账��;	
		}	
		else
		if(mp_comm_data.rx[3] > 13)						//�ӻ�
		{
			j = (mp_comm_data.rx[3] - 1) / 13;
		 
		 	for(z = 0; z < j; z++)	
		 	{
				addr = mp_comm_data.rx[z * 13 + 5];
				for(i = 0; i < (13 - 1); i++)
					spd8_file[addr - 1][i]	= mp_comm_data.rx[z * 13 + i + 6];
				spd8_file[addr - 1][12]	= '\0';		 		
		 	}
		}			
	}
	else
	if (mp_comm_data.rx[4] == 0x06)						//�����ӻ�
	{
		if(mp_comm_data.rx[3] == 19)					//�������޴ӻ�
		{
			for(i = 0; i < (13 - 1); i++)
				mpd8_file[i] = mp_comm_data.rx[i + 5];
			mpd8_file[12]	= '\0';	
			//�޴ӻ����账��;			
		}	
		else
		if(mp_comm_data.rx[3] >= 26)					//�����ӻ�
		{
			j = (mp_comm_data.rx[3] - 13) / 13;
			
			for(i = 0; i < (13 - 1); i++)				
				mpd8_file[i] = mp_comm_data.rx[i + 5];
			mpd8_file[12]	= '\0';		
			
		 	for(z = 1; z < (j + 1); z++)	
		 	{
				addr = mp_comm_data.rx[z * 13 + 4];
				for(i = 0; i < (13 - 1); i++)
					spd8_file[addr - 1][i]	= mp_comm_data.rx[z * 13 + i + 5];
				spd8_file[addr - 1][12]	= '\0';		 		
		 	}			
		}	
	}
}

/*
********************************************************************************
�������ƣ�UART5_init
����������UART5��ʼ��
����������
����ֵ	��
********************************************************************************
*/
void UART5_init(uint32_t br)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);  		//ʹ��GPIOB��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART5, ENABLE ); 	//ʹ��USART��ʱ��
	/* USART5�Ķ˿����� */        
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_4);  		//����PB3�ɵڶ���������TX
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_4);  		//����PB4�ɵڶ���������RX	      

	//USART5_TX -> PB3 , USART5_RX ->  PB4                            
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);        
	
	/* USART5�Ļ������� */
	USART_InitStructure.USART_BaudRate = br;					//���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//��������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����Ч��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ù���ģʽ
	USART_Init(USART5, &USART_InitStructure); 					//������ṹ��

	USART_ITConfig(USART5,USART_IT_RXNE,ENABLE);           		
	USART_Cmd(USART5, ENABLE);									

	NVIC_InitStruct.NVIC_IRQChannel = USART3_6_IRQn;   
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;    
	NVIC_Init(&NVIC_InitStruct);
}			

/*
********************************************************************************
�������ƣ�MP_UTCToBeijing
����������UTCʱ��ת��Ϊ����ʱ��
����������
����ֵ  ��
********************************************************************************
*/
void MP_UTCToBeijing(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
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
	mp_comm_bt_time.year = year + 2000;
	mp_comm_bt_time.month = month; 
	mp_comm_bt_time.day = day; 
	mp_comm_bt_time.hour = hour; 
	mp_comm_bt_time.min = minute; 
	mp_comm_bt_time.sec = second; 
}

