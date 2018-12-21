/*
********************************************************************************
�ļ���	��mcb_wifi.c
�ļ����������ذ���WIFIͨ��ģ�飬֧������ͨ��Э�飬��������(BINARY)�����ݽ������֣�ASCII��
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __MCB_WIFI_GLOBAL
#include "mcb_wifi.h"  
#include "public.h"        
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
********************************************************************************
�������ƣ�mcb_wifi_init
�������������ذ���WIFIͨ��ģ���ʼ��
����������
����ֵ	��
********************************************************************************
*/
void mcb_wifi_init(void) 
{
	uint8_t *pt; 
	uint32_t i;
	
	mcb_wifi_task_flag = 0;
	mcb_wifi_recv_cp_type = 0;
	mcb_wifi_ru_state = 0;
	mcb_wifi_ru_send_flag = 0;
	mcb_wifi_info_send_flag = 0;
	mcb_wifi_alm_send_flag = 0;
	mcb_wifi_ru_deal_flag = 0;
	mcb_wifi_info_deal_flag = 0;
	mcb_wifi_ru_xxb = 0;
	mcb_wifi_sys_tx_cmd = 0;
	
	wifi_comm_recv_end_flag = 0;		
	wifi_comm_link_tx_flag = 0;		
	wifi_comm_sys_tx_flag = 0;			
	wifi_comm_sys_tx_to_flag = 0;	
	wifi_comm_data_deal_flag = 0;	
	wifi_comm_time_out_cnt = 0;		
	wifi_comm_time_out_en = 0;		
	wifi_comm_send_recv_end = 1;	
	wifi_comm_sys_tx_end = 1;
	
	mcb_wifi_recv_cp_cnt = 0;
	mcb_wifi_recv_cp_arr[0]	= 0;	
	mcb_wifi_recv_cp_arr[1]	= 0;	
	mcb_wifi_recv_cp_arr[2]	= 0;	
	mcb_wifi_set_succ = 0;
    
    wifi_to_cnt1 = 0;

	
	pt = (uint8_t *)(&wifi_comm_ru_data.rx_cnt);
	for(i = 0; i < sizeof(WIFI_COMM_TYPE_DEF); i++)
		*pt = 0;
	pt = (uint8_t *)(&wifi_comm_info_data.rx_cnt);
	for(i = 0; i < sizeof(WIFI_COMM_TYPE_DEF); i++)
		*pt = 0;	
	pt = (uint8_t *)(&wifi_comm_alm_data.rx_cnt);
	for(i = 0; i < sizeof(WIFI_COMM_TYPE_DEF); i++)
		*pt = 0;	
	
	usart1_init(MCB_WIFI_NW_BR);
}

/*
********************************************************************************
�������ƣ�mcb_wifi_task
�������������ذ���WIFIͨ��ģ������
����������
����ֵ	��
********************************************************************************
*/
void mcb_wifi_task(void) 
{   
    mcb_wifi_task_flag = 0;
    
    //if(mcb_wifi_ru_state)								//����Զ�����������ݴ�������ƣ�����͸�������н�����ת����wifi����Y-MODEMЭ��
   	//{
    //		
    //}
	
    if((mcb_alm_upwifi_flag) && (!mcb_wifi_ru_state) && (wifi_comm_sys_tx_end))	//ASCII WARN INFO������Ҫ����ʱ����
    {													//�и澯�¼��Ҳ���Զ�������У���Ҫ���ڽ��շ��ʹ����вſ��ϱ�����ͨ����·����ʱ�ſ����Զ��ϱ�							
    	mcb_alm_upwifi_flag = 0;
    	wifi_comm_sys_warn_send();
		wifi_comm_sys_tx_end = 0;
		mcb_wifi_alm_send_flag = 1;
	   	wifi_comm_link_tx_flag = 1;  
    }
    else
    if((wifi_comm_sys_tx_flag) && (wifi_comm_sys_tx_end))	//ϵͳ������������,���ڷ������ݴ����вſ�
    {
        wifi_comm_sys_tx_flag = 0;
        if(wifi_comm_send_recv_end)                  	//һ��ͨ��������ɣ��˴��ṹ�����Ż�������д���ڵ���
        {
	    	wifi_comm_sys_ru_send();        			//����������֡����
	     	wifi_comm_send_recv_end = 0;
	        wifi_comm_time_out_cnt = 0;              	//��ʱ��ʼ��
	        wifi_comm_time_out_en = 1;    
	        wifi_comm_sys_tx_end = 0;
	        mcb_wifi_ru_send_flag = 1;
	        wifi_comm_link_tx_flag = 1;           	
        }
        /*
        else                                         	//һ��ͨ������δ���
        {
            wifi_comm_sys_tx_flag = 1;
            mcb_wifi_task_flag = 1;                      
        }
        */
    }
    //else
    if((wifi_comm_sys_tx_to_flag) && (wifi_comm_sys_tx_end))			//ϵͳ���ͳ�ʱ����		
    {
    	wifi_comm_sys_tx_to_flag = 0;		
		wifi_to_cnt1++;
		if(mr_to_cnt1 < WIFI_RECV_TO_ALLOW_NUM)				//��ʱ����ش�3��
		{
			wifi_comm_ru_data.rx_cnt = 0;
			wifi_comm_ru_data.comm_state |= WIFI_RECV_TIME_OUT;	
			//wifi_comm_sys_tx_end = 1;
			wifi_comm_send_recv_end = 1;
			wifi_comm_sys_tx_flag = 1;
            mcb_wifi_task_flag = 1;    							
		}
		else
		{
			wifi_to_cnt1 = 0;
			wifi_comm_time_out_en = 0;
			wifi_comm_send_recv_end = 1;
            //mcb_wifi_sys_tx_cmd = 0;           
		}
		
    }
    //�������ݴ���
    //else
    if(wifi_comm_recv_end_flag)
    {
        wifi_comm_recv_end_flag = 0;        
        if(mcb_wifi_info_deal_flag)  
        {
			mcb_wifi_info_deal_flag = 0;
			wifi_comm_info_data_deal();                  	//��Ϣ���ݽ���        		
        }    
       	else
       	if(mcb_wifi_ru_deal_flag)
       	{
			mcb_wifi_ru_deal_flag = 0;
			wifi_comm_ru_data_deal();						//�������ݽ���       			
       	}
        wifi_comm_data_deal_flag = 0;
    }
    //��·���ʹ���
    if(wifi_comm_link_tx_flag)
    {
        wifi_comm_link_tx_flag = 0;   
        wifi_comm_data_send();                           	//���ݷ���
    }	
    
    if(mcb_alm_upwifi_flag | wifi_comm_sys_tx_flag)			//��©
    	mcb_wifi_task_flag = 1;
}

/*
********************************************************************************
�������ƣ�wifi_comm_sys_ru_send
����������ϵͳ�����������ݺ���
����������
����ֵ  ��
********************************************************************************
*/
void wifi_comm_sys_ru_send(void)
{ 
	uint8_t i, chksum;
	
    i = 0;
    //֡ͷ
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//��֡

   	if(mcb_wifi_sys_tx_cmd == WIFI_ZDTZSJCG_CMD_7C)
   		wifi_cmd_7C_deal();		//�������ж�mcb_wifi_ru_xxb������
   	else
   	if(mcb_wifi_sys_tx_cmd == WIFI_ZDTZTCYCSJ_CMD_7E)
		wifi_cmd_7E_deal(); 	//�������ж�mcb_wifi_ru_xxb������
	else
   	if(mcb_wifi_sys_tx_cmd == WIFI_HFJRSJQR_CMD_7D)
		wifi_cmd_7D_deal_reply(); 	//�������ж�mcb_wifi_ru_xxb������	
	else
		return ;                               

    //����У���
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 
}

/*
********************************************************************************
�������ƣ�wifi_comm_sys_warn_send
����������ϵͳ���͸澯���ݺ���
����������
����ֵ  ��
********************************************************************************
*/
void wifi_comm_sys_warn_send(void)
{ 	
	char *str;
	
	//�澯����ͷ
	wifi_comm_alm_data.tx_len = 0;
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = 'W';
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = 'A';
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = 'R';
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = 'N';
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = ' ';
	//�ѱ�������������copy���������ݷ��ͻ�����
	alarm_data_out();		//���ñ������ݺ���	
	str = &mcb_alm_up_buf[mcb_alm_cur_index][0];	
	while(*str != '\0')
	{
		wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = *str++;
	}
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = '*';
}

/*
********************************************************************************
�������ƣ�wifi_comm_info_data_deal
������������Ϣ���ݽ���������,ASCII�벻��У���
����������
����ֵ	��
********************************************************************************
*/
void wifi_comm_info_data_deal(void)		//�ṹ���Ż�
{
	uint8_t i, num, index;
	
	if((mcb_wifi_recv_cp_arr[1] == 'E') && (mcb_wifi_recv_cp_arr[2] == 'T'))
	{
		//��ѯ�������ֶ�����
		if((wifi_comm_info_data.rx[3] == WIFI_MCU_SPACE_CHAR) && (wifi_comm_info_data.rx[5] == WIFI_MCU_SPACE_CHAR))
		{
			if((wifi_comm_info_data.rx[4] > '0') && (wifi_comm_info_data.rx[4] <= '9'))
				num = wifi_comm_info_data.rx[4] - '0';
			else
				return ;					
		}
		/*
		else //�ݶ��ֶ���<10
		if((wifi_comm_info_data.rx[3] == WIFI_MCU_SPACE_CHAR) && (wifi_comm_info_data.rx[6] == WIFI_MCU_SPACE_CHAR))
		{
			if((wifi_comm_info_data.rx[4] >= '0') && (wifi_comm_info_data.rx[4] <= '9'))
				num = wifi_comm_info_data.rx[4] - '0';
			else
				return ;
			if((wifi_comm_info_data.rx[5] >= '0') && (wifi_comm_info_data.rx[5] <= '9'))	
				num = num * 10 + (wifi_comm_info_data.rx[5] - '0');
			else
				return ;					
		}	
		*/

		//���ݴ���
		if(mcb_wifi_recv_cp_arr[0] == 'G')		//��ѯ
		{
			//֡ͷ
			wifi_comm_info_data.tx_len = 0;
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'G';
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'E';
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'T';		
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = wifi_comm_info_data.rx[3];
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = wifi_comm_info_data.rx[4];
			
			for(i = 0; i < num; i++)
			{
				index = find_get_cmd_index(i + 1);	
				switch(index)
				{
					case GET_LGTD: get_lgtd();	break;
					case GET_LTTD: get_lttd();	break;
					case GET_STCT: get_stct();	break;
					case GET_MNGN: get_mngn();	break;
					case GET_IPSN: get_ipsn();	break;
					case GET_IPCN: get_ipcn();	break;
					case GET_IPTP: get_iptp();	break;
					case GET_IPBP: get_ipbp();	break;
					case GET_FDDV: get_fddv();	break;	
					case GET_L1OP: get_l1op();	break;
					case GET_L2OP: get_l2op();	break;
					case GET_R1GN: get_r1gn();	break;
					case GET_R2GN: get_r2gn();	break;
					case GET_RDGN: get_rdgn();	break;	
					case GET_PMGN: get_pmgn();	break;
					case GET_PSGN: get_psgn();	break;
					default: break;
				}	
			}
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '*';
			
			i = 0;
		    while(!wifi_comm_sys_tx_end)		//ȷ��������·�����
		    {
		    	delay_xms(1);
		    	i++;
		    	if(i > 19)	
		    		break;    	
    		}  
			wifi_comm_sys_tx_end = 0;
			mcb_wifi_info_send_flag = 1;
			wifi_comm_link_tx_flag = 1;
		}
		else
		if(mcb_wifi_recv_cp_arr[0] == 'S')		//����
		{
			//֡ͷ
			wifi_comm_info_data.tx_len = 0;
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'S';
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'E';
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'T';		
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = wifi_comm_info_data.rx[3];
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = wifi_comm_info_data.rx[4];
			
			for(i = 0; i < num; i++)
			{
				index = find_set_cmd_index(i + 1);		
				switch(index)
				{
					case SET_ATTE: set_atte(i + 1);	break;
					case SET_MNGN: set_mngn(i + 1);	break;
					case SET_IPCN: set_ipcn(i + 1);	break;
					case SET_IPTP: set_iptp(i + 1);	break;
					case SET_IPBP: set_ipbp(i + 1);	break;
					case SET_FDDV: set_fddv(i + 1);	break;
					case SET_R1GN: set_r1gn(i + 1);	break;
					case SET_R2GN: set_r2gn(i + 1);	break;
					case SET_RDGN: set_rdgn(i + 1);	break;	
					case SET_PMGN: set_pmgn(i + 1);	break;
					case SET_PSGN: set_psgn(i + 1);	break;
					case SET_YZMD: set_yzmd(i + 1);	break;
					default: break;	
				}	
			}	
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
			if(mcb_wifi_set_succ)
			{
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'O';
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'K';				
			}
			else
			{
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'F';
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'A';
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'I';
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'L';						
			}
			//֡β
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '*';
			
			i = 0;
		    while(!wifi_comm_sys_tx_end)		//ȷ��������·�����
		    {
		    	delay_xms(1);
		    	i++;
		    	if(i > 19)	
		    		break;    	
    		}  
			wifi_comm_sys_tx_end = 0;
			mcb_wifi_info_send_flag = 1;
			wifi_comm_link_tx_flag = 1;					
		}			
	}
}

/*
********************************************************************************
�������ƣ�wifi_comm_ru_data_deal
�����������������ݽ���������
����������
����ֵ	��
********************************************************************************
*/
void wifi_comm_ru_data_deal(void)		//�ṹ���Ż�
{
    uint8_t i, chksum, flag;  
    
    chksum = 0;             //����У���
    flag = 0;
    for(i = 2; i < wifi_comm_ru_data.rx_len; i++)
        chksum ^= wifi_comm_ru_data.rx[i];

    //wifi_comm_ru_data.comm_state &= ~WIFI_RECV_TIME_OUT;

    if(chksum)              //У��ʹ���
    {
        wifi_comm_ru_data.comm_state |= WIFI_RECV_CHKSUM_ERR;
        //��Э��Լ��У��ʹ�������Ӧ��������𷽸���Ӧ��ʱ�ط�����
    }
    else                    //У�����ȷ
    {
        wifi_comm_ru_data.comm_state &= ~WIFI_RECV_CHKSUM_ERR;
        switch(wifi_comm_ru_data.rx[2])  
        {
            case WIFI_QRSBFSSS_CMD_80:		wifi_cmd_80_deal(); break;
            case WIFI_QQSPD8SLDZ_CMD_FC:    wifi_cmd_FC_deal(); break;
            case WIFI_QQRFBHXYB_CMD_FD:    	wifi_cmd_FD_deal(); break;
            case WIFI_QQDQSBWJM_CMD_FE:   	wifi_cmd_FE_deal(); break;
            case WIFI_QQJRYCSJ_CMD_FF:   	wifi_cmd_FF_deal(); break;                  
            default: flag = 1; break; 
        }
    }
    //���豸�ظ�֡�������豸����֡�������´���
	if(wifi_comm_ru_data.rx[2] == WIFI_QRSBFSSS_CMD_80)
	{
		wifi_comm_time_out_en = 0;					//��ʱ����ʹ�ܱ�־��0
		wifi_comm_send_recv_end = 1;  				//һ�η���������ɱ�־
		wifi_comm_ru_data.comm_state &= ~WIFI_RECV_TIME_OUT;  
        wifi_to_cnt1 = 0;     
        //mcb_wifi_sys_tx_cmd = 0;    
	}
	else
    if(!flag)
	{
		i = 0;
		while(!wifi_comm_sys_tx_end)		//ȷ��������·�����
		{
			delay_xms(1);
		    i++;
		    if(i > 19)	
		    break;    	
    	}  
		wifi_comm_sys_tx_end = 0;
		mcb_wifi_ru_send_flag = 1;
	    wifi_comm_link_tx_flag = 1;    
	}	
}

/*
********************************************************************************
�������ƣ�wifi_comm_data_send
������������·�������ݺ���
����������
����ֵ	��
********************************************************************************
*/
void wifi_comm_data_send(void)
{	
	USART1->ICR |= 0x40;
	USART1->CR1 |= 0x40;
	if(mcb_wifi_info_send_flag)
	{
		wifi_comm_info_data.tx_cnt = 0;
		USART1->TDR = wifi_comm_info_data.tx[wifi_comm_info_data.tx_cnt++];			
	}
	else
	if(mcb_wifi_ru_send_flag)
	{
		wifi_comm_ru_data.tx_cnt = 0;
		USART1->TDR = wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_cnt++];			
	}
	else
	if(mcb_wifi_alm_send_flag)
	{
		wifi_comm_alm_data.tx_cnt = 0;
		USART1->TDR = wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_cnt++];				
	}
}

/*
********************************************************************************
�������ƣ�find_get_cmd_index
��������������GET����ţ�û��ʹ�ÿ��ļ�,strcmp,sscanf
����������
����ֵ	��
********************************************************************************
*/
uint8_t find_get_cmd_index(uint8_t num)
{
	uint8_t i, index;
	
	index = 0x00;
	i = 6 + (num - 1) * 5;		//ÿ���ֶ����̶�4���ַ�����ʽ���һ���ո񣬼򵥸�Ч

	if(wifi_comm_info_data.rx[i] == 'L')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'G') && (wifi_comm_info_data.rx[i + 2] == 'T') && (wifi_comm_info_data.rx[i + 3] == 'D'))
		{
			index = GET_LGTD;		
		}	
		else
		if((wifi_comm_info_data.rx[i + 1] == 'T') && (wifi_comm_info_data.rx[i + 2] == 'T') && (wifi_comm_info_data.rx[i + 3] == 'D'))
		{
			index = GET_LTTD;	
		}	
		else
		if((wifi_comm_info_data.rx[i + 1] == '1') && (wifi_comm_info_data.rx[i + 2] == 'O') && (wifi_comm_info_data.rx[i + 3] == 'P'))
		{
			index = GET_L1OP;	
		}	
		else
		if((wifi_comm_info_data.rx[i + 1] == '2') && (wifi_comm_info_data.rx[i + 2] == 'O') && (wifi_comm_info_data.rx[i + 3] == 'P'))	
		{
			index = GET_L2OP;	
		}	
	}
	else
	if(wifi_comm_info_data.rx[i] == 'S')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'T') && (wifi_comm_info_data.rx[i + 2] == 'C') && (wifi_comm_info_data.rx[i + 3] == 'T'))	
		{
			index = GET_STCT;			
		}	
	}	
	else
	if(wifi_comm_info_data.rx[i] == 'M')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'N') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N'))		
		{
			index = GET_MNGN;			
		}				
	}	
	else
	if(wifi_comm_info_data.rx[i] == 'I')
	{
		if(wifi_comm_info_data.rx[i + 1] == 'P')
		{
			if((wifi_comm_info_data.rx[i + 2] == 'S') && (wifi_comm_info_data.rx[i + 3] == 'N'))
			{
				index = GET_IPSN;		
			}
			else
			if((wifi_comm_info_data.rx[i + 2] == 'C') && (wifi_comm_info_data.rx[i + 3] == 'N'))
			{
				index = GET_IPCN;		
			}
			else
			if((wifi_comm_info_data.rx[i + 2] == 'T') && (wifi_comm_info_data.rx[i + 3] == 'P'))	
			{
				index = GET_IPTP;	
			}
			else
			if((wifi_comm_info_data.rx[i + 2] == 'B') && (wifi_comm_info_data.rx[i + 3] == 'P'))
			{
				index = GET_IPBP;	
			}				
		}	
	}	
	else
	if(wifi_comm_info_data.rx[i] == 'F')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'D') && (wifi_comm_info_data.rx[i + 2] == 'D') && (wifi_comm_info_data.rx[i + 3] == 'V'))	
		{
			index = GET_FDDV;		
		}				
	}
	else
	if(wifi_comm_info_data.rx[i] == 'R')
	{
		if((wifi_comm_info_data.rx[i + 1] == '1') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N'))	
		{
			index = GET_R1GN;		
		}	
		else
		if((wifi_comm_info_data.rx[i + 1] == '2') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N'))	
		{
			index = GET_R2GN;		
		}	
		else
		if((wifi_comm_info_data.rx[i + 1] == 'D') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N'))	
		{
			index = GET_RDGN;		
		}				
	}	
	else
	if(wifi_comm_info_data.rx[i] == 'P')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'M') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N'))
		{
			index = GET_PMGN;		
		}	
		else
		if((wifi_comm_info_data.rx[i + 1] == 'S') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N'))
		{
			index = GET_PSGN;		
		}							
	}				
	
	return index;					
}   
/*
********************************************************************************
�������ƣ�find_set_cmd_index
��������������SET����ţ�û��ʹ�ÿ��ļ�,strcmp,sscanf
����������
����ֵ	��
********************************************************************************
*/
uint8_t find_set_cmd_index(uint8_t num)
{
	uint8_t i, j, z, index;
	
	index = 0x00;
	
	z = 0;	
	for(j = 5; j < wifi_comm_info_data.rx_len; j++)
	{
		if(wifi_comm_info_data.rx[j] == ' ')
		{
			z++;
			if(z == (2 * num - 1))	
				break;
		}	
		else
		if(wifi_comm_info_data.rx[j] == '*')
		{
			return index;	
		}		
	}
	i = j + 1;			//��λ�ֶ�λ��

	if(wifi_comm_info_data.rx[i] == 'A')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'T') && (wifi_comm_info_data.rx[i + 2] == 'T') && (wifi_comm_info_data.rx[i + 3] == 'E'))
		{
			index = SET_ATTE;		
		}	
	}	
	else
	if(wifi_comm_info_data.rx[i] == 'M')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'N') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N')) 
		{
			index = SET_MNGN;			
		}	
	}	
	else
	if(wifi_comm_info_data.rx[i] == 'I')
	{
		if(wifi_comm_info_data.rx[i + 1] == 'P')
		{
			if((wifi_comm_info_data.rx[i + 2] == 'C') && (wifi_comm_info_data.rx[i + 3] == 'N'))
			{
				index = SET_IPCN;			
			}	
			else
			if((wifi_comm_info_data.rx[i + 2] == 'T') && (wifi_comm_info_data.rx[i + 3] == 'P')) 
			{
				index = SET_IPTP;			
			}			
			else
			if((wifi_comm_info_data.rx[i + 2] == 'B') && (wifi_comm_info_data.rx[i + 3] == 'P')) 
			{
				index = SET_IPBP;			
			}										
		}	
	}		
	else
	if(wifi_comm_info_data.rx[i] == 'F')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'D') && (wifi_comm_info_data.rx[i + 2] == 'D') && (wifi_comm_info_data.rx[i + 3] == 'V')) 
		{
			index = SET_FDDV;		
		}
	}	
	else
	if(wifi_comm_info_data.rx[i] == 'R')
	{
		if((wifi_comm_info_data.rx[i + 1] == '1') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N')) 
		{
			index = SET_R1GN;		
		}	
		else
		if((wifi_comm_info_data.rx[i + 1] == '2') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N')) 
		{
			index = SET_R2GN;		
		}	
		else
		if((wifi_comm_info_data.rx[i + 1] == 'D') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N')) 
		{
			index = SET_RDGN;		
		}				
	}	
	else
	if(wifi_comm_info_data.rx[i] == 'P')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'M') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N')) 
		{
			index = SET_PMGN;		
		}	
		else
		if((wifi_comm_info_data.rx[i + 1] == 'S') && (wifi_comm_info_data.rx[i + 2] == 'G') && (wifi_comm_info_data.rx[i + 3] == 'N')) 
		{
			index = SET_PSGN;		
		}							
	}	
	else
	if(wifi_comm_info_data.rx[i] == 'Y')
	{
		if((wifi_comm_info_data.rx[i + 1] == 'Z') && (wifi_comm_info_data.rx[i + 2] == 'M') && (wifi_comm_info_data.rx[i + 3] == 'D')) 
		{
			index = SET_YZMD;		
		}				
	}			
	
	return index;						
}

/*
********************************************************************************
�������ƣ�get_lgtd
������������ȡ����
����������
����ֵ	��
********************************************************************************
*/
void get_lgtd(void)
{
	uint8_t i;
	char str[11];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'L';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'G';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'T';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'D';
	
	sprintf(str, "%03.6f", mr_comm_msg.longitude);
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];
}

/*
********************************************************************************
�������ƣ�get_lttd
������������ȡγ��
����������
����ֵ	��
********************************************************************************
*/
void get_lttd(void)
{
	uint8_t i;
	char str[10];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'L';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'T';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'T';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'D';
	
	sprintf(str, "%02.6f", mr_comm_msg.latitude);
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];	
}

/*
********************************************************************************
�������ƣ�get_stct
������������ȡ����-GPSͨ��1��ͨ��2��������
����������
����ֵ	��
********************************************************************************
*/
void get_stct(void)
{
	uint8_t i;
	char str[3];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'S';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'T';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'C';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'T';
	
	if(mr_comm_msg.cur_chno == 0)
		sprintf(str, "%02d", mr_comm_msg.ch1_gps_kyx);
	else
	if(mr_comm_msg.cur_chno == 1)
		sprintf(str, "%02d", mr_comm_msg.ch2_gps_kyx);
	else
		sprintf(str, "%02d", 0x00);		//��·ģʽ��֪������Ĭ�ϸ�00
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}

/*
********************************************************************************
�������ƣ�get_mngn
������������ȡ������-����ȷ���ĸ����棬Ĭ��Ϊ0
����������
����ֵ	��
********************************************************************************
*/
void get_mngn(void)
{
	uint8_t i;
	char str[3];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'M';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'N';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'G';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'N';
	
	sprintf(str, "%02d", 0x00);		//������Ĭ�ϸ�00
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}

/*
********************************************************************************
�������ƣ�get_ipsn
������������ȡ�����ź�ͨ��1��ͨ��2��GPS�����AVG(û��ȥ�������Сֵ)
����������ע�⣺������ȡ��Ƶ�����Ǹ���������������Ȼ���1-3���ٶ�������Ч���ݣ���̨����������
����ֵ	��
********************************************************************************
*/
void get_ipsn(void)
{
	uint8_t i;
	char str[3];
	uint16_t sn;
	
	//read star and cn   
	mr_comm_sys_tx_cmd2 = QQKYXZZB_CMD_87;
	mr_gps_or_bd = 0;						//gps
	mr_channel_no = mr_comm_msg.cur_chno;	//cur_ch
	mr_comm_sys_tx_flag = 1;   
	mcb_rfb_task_flag = 1;			
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'I';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'S';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'N';
	
	sn = 0x00;
	if(mr_comm_msg.cur_chno == 0)
	{
		for(i = 0; i < MR_KYX_MAX_NUM; i++)
		{
			if(mr_kyx_cn_array.CH1_GPS_KYX_CN[i][1] != 0)
			{
				sn += mr_kyx_cn_array.CH1_GPS_KYX_CN[i][1];
			}
			else
				break;
		}
		if(i > 0)
			sn = sn / i;
	}	
	else
	if(mr_comm_msg.cur_chno == 1)
	{
		for(i = 0; i < MR_KYX_MAX_NUM; i++)
		{
			if(mr_kyx_cn_array.CH2_GPS_KYX_CN[i][1] != 0)
			{
				sn += mr_kyx_cn_array.CH2_GPS_KYX_CN[i][1];
			}
			else
				break;
		}
		if(i > 0)
			sn = sn / i;		
	}
	else
		sn = 0x00;		//��·ģʽĬ�ϸ�0
	
	sprintf(str, "%02d", sn);	
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];			
}

/*
********************************************************************************
�������ƣ�get_ipcn
������������ȡ�����ź�ͨ����0-ͨ��1��1-ͨ��2
����������
����ֵ	��
********************************************************************************
*/
void get_ipcn(void)
{
	uint8_t i;
	char str[2];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'I';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'C';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'N';
	
	sprintf(str, "%1d", mr_comm_msg.cur_chno);	
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];			
}

/*
********************************************************************************
�������ƣ�get_iptp
������������ȡ�������ͣ�0-������1-�ֶ���2-�Զ���4ͨ���Լ�
����������
����ֵ	��
********************************************************************************
*/
void get_iptp(void)
{
	uint8_t i;
	char str[2];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'I';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'T';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';
	
	sprintf(str, "%1d", mr_comm_msg.cur_mode);			
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];	
}

/*
********************************************************************************
�������ƣ�get_ipbp
������������ȡ��·��0-����·��1-��·			//����·��0-ͨ��1��1-ͨ��2����·��2-��·1��3-��·2
����������
����ֵ	��
********************************************************************************
*/
void get_ipbp(void)
{
	uint8_t i;
	char str[2];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'I';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'B';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';
	
	//sprintf(str, "%1d", mr_comm_msg.cur_chno);		//����·��0-ͨ��1��1-ͨ��2����·��2-��·1��3-��·2
	
	if(mr_comm_msg.cur_chno > 1)
		sprintf(str, "%1d", 1);
	else
		sprintf(str, "%1d", 0);
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}

/*
********************************************************************************
�������ƣ�get_fddv
������������ȡ�豸�� 0-δ���֣�1-�����豸
����������
����ֵ	��
********************************************************************************
*/
void get_fddv(void)
{
	uint8_t i;
	char str[2];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'F';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'D';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'D';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'V';
	
	sprintf(str, "%1d", 1); 	//Ĭ��1
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];			
}

/*
********************************************************************************
�������ƣ�get_l1op
������������ȡ�����˹��ְ�����˿�״̬
����������
����ֵ	��
********************************************************************************
*/
void get_l1op(void)
{
	uint8_t i, j, z, temp;
	char str[2];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'L';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '1';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'O';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';

	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	//port1-port4
	for(j = 0; j < 4; j++)
	{
		z = j << 1;
        temp = (mp_mpd8_comm_msg.mpd8_port_state2 & (0x03 << z));
		temp = temp >> z;
		if(temp == 0x00)
			i = 0;			//NULL
		else
		if(temp == 0x01)
			i = 4;			//SLAVE
		else
		if(temp == 0x02)	
			i = 1;			//BBU
			
		if((mp_mpd8_comm_msg.mpd8_out_state & (0x01 << j)) || (mp_mpd8_comm_msg.mpd8_alm_state & (0x01 << j)))
			i = 2;			//ALM
		
		sprintf(str, "%1d", i);	
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[0];			
	}
	//port5-port8
	for(j = 0; j < 4; j++)
	{
		z = j << 1;
        temp = (mp_mpd8_comm_msg.mpd8_port_state1 & (0x03 << z));
		temp = temp >> z;
		if(temp == 0x00)
			i = 0;
		else
		if(temp == 0x01)
			i = 4;
		else
		if(temp == 0x02)	
			i = 1;
			
		if((mp_mpd8_comm_msg.mpd8_out_state & (0x10 << j)) || (mp_mpd8_comm_msg.mpd8_alm_state & (0x10 << j)))
			i = 2;
		
		sprintf(str, "%1d", i);	
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[0];			
	}
}

/*
********************************************************************************
�������ƣ�get_l2op
������������ȡ�ӻ����˹��ְ�����˿�״̬
����������
����ֵ	��
********************************************************************************
*/
void get_l2op(void)
{
	uint8_t i, j, temp;
	char str[9];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'L';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '2';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'O';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';

	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	//port1-port4
	for(j = 0; j < 4; j++)
	{
		temp = (mp_mpd8_comm_msg.mpd8_port_state2 & (0x03 << (j << 1)));
		temp = temp >> (j << 1);
		
		if((temp == 0x00) || (temp == 0x02))	//�������ػ���BBU��û�нӴӻ�����ôȫΪ5
		{
			for(i = 0; i < 8; i++)
			{
				//str[i] = '5';
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '5'; //str[i];	
			}
		}
		else
		if(temp == 0x01)						//�ӻ�1-4�˿�״̬
		{
			for(i = 0; i < 8; i++)
			{
				if(mp_spd8_comm_msg[j].spd8_port_state & (0x01 << i))
					str[i] = '1';				//BBU
				else
					str[i] = '0';				//NULL
							
				if((mp_spd8_comm_msg[j].spd8_out_state & (0x01 << i)) || (mp_spd8_comm_msg[j].spd8_alm_state & (0x01 << i)))
					str[i] = '2';				//ALM
					
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];	
			}
		}
	}
	//port5-port8
	for(j = 0; j < 4; j++)
	{
		temp = (mp_mpd8_comm_msg.mpd8_port_state1 & (0x03 << (j << 1)));
		temp = temp >> (j << 1);
		
		if((temp == 0x00) || (temp == 0x02))	//�������ػ���BBU��û�нӴӻ�����ôȫΪ5
		{
			for(i = 0; i < 8; i++)
			{
				//str[i] = '5';
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '5'; //str[i];	
			}
		}
		else
		if(temp == 0x01)						//�ӻ�5-8�˿�״̬
		{
			for(i = 0; i < 8; i++)
			{
				if(mp_spd8_comm_msg[j + 4].spd8_port_state & (0x01 << i))
					str[i] = '1';				//BBU
				else
					str[i] = '0';				//NULL
							
				if((mp_spd8_comm_msg[j + 4].spd8_out_state & (0x01 << i)) || (mp_spd8_comm_msg[j + 4].spd8_alm_state & (0x01 << i)))
					str[i] = '2';				//ALM
					
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];	
			}
		}
	}
}

/*
********************************************************************************
�������ƣ�get_r1gn
������������ȡ��Ƶ��ͨ��1����-˥��ֵ
����������
����ֵ	��
********************************************************************************
*/
void get_r1gn(void)
{
	uint8_t i;
	char str[3];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'R';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '1';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'G';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'N';
	
	sprintf(str, "%02d", mr_ch1_att); 
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}

/*
********************************************************************************
�������ƣ�get_r2gn
������������ȡ��Ƶ��ͨ��2����-˥��ֵ
����������
����ֵ	��
********************************************************************************
*/
void get_r2gn(void)
{
	uint8_t i;
	char str[3];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'R';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '2';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'G';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'N';
	
	sprintf(str, "%02d", mr_ch2_att); 
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}

/*
********************************************************************************
�������ƣ�get_rdgn
������������ȡ��Ƶ����ͨ������-˥��ֵ
����������
����ֵ	��
********************************************************************************
*/
void get_rdgn(void)
{
	uint8_t i;
	char str[3];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'R';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'D';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'G';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'N';
	
	sprintf(str, "%02d", mr_com_att); 
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}    

/*
********************************************************************************
�������ƣ�get_pmgn
������������ȡ�����˹��ְ��������-˥��ֵ
����������
����ֵ	��
********************************************************************************
*/    
void get_pmgn(void)
{
	uint8_t i;
	char str[3];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'M';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'G';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'N';
	
	sprintf(str, "%02d", mp_out_att); 
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}    

/*
********************************************************************************
�������ƣ�get_psgn
������������ȡ�ӻ��˹��ְ��������-˥��ֵ,��ʱû�ã�Ĭ��Ϊ0
����������
����ֵ	��
********************************************************************************
*/           
void get_psgn(void)
{
	uint8_t i;
	char str[3];
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'P';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'S';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'G';
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = 'N';
	
	sprintf(str, "%02d", 0x00); 
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}  

/*
********************************************************************************
�������ƣ�set_atte
��������������˥��������ȷ��
����������
����ֵ	��
********************************************************************************
*/  
void set_atte(uint8_t num)
{
	
	mcb_wifi_set_succ = 1;
}

/*
********************************************************************************
�������ƣ�set_mngn
�������������������棬����ȷ��
����������
����ֵ	��
********************************************************************************
*/  
void set_mngn(uint8_t num)
{
	
	mcb_wifi_set_succ = 1;
}

/*
********************************************************************************
�������ƣ�set_ipcn
������������������ͨ����0-�˳��ֶ���1-����ͨ��1��2-����ͨ��2��3-��·1��4-��·2
����������
����ֵ	��
********************************************************************************
*/
void set_ipcn(uint8_t num)
{
	uint8_t i, j, z;
	char str[5];
	
	z = 0;	
	for(j = 5; j < wifi_comm_info_data.rx_len; j++)
	{
		if(wifi_comm_info_data.rx[j] == ' ')
		{
			z++;
			if(z == (2 * num))	
				break;
		}
		else
		if(wifi_comm_info_data.rx[j] == '*')
		{
			return ;	
		}
	}
	i = j + 1;			//��λ�ֶ�λ��		

	
	//��λ����ֵ��Χ
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//��ȡ����ֵ
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//ת��
	z = atoi(str);
	if(z < 5)
	{
		mcb_wifi_set_succ = 1;	
		mr_in_out_manu_mode = z;
		mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;
	}
	else
		mcb_wifi_set_succ = 0;	
}

/*
********************************************************************************
�������ƣ�set_iptp
���������������������ͣ�0-�˳��Զ���1-�����Զ�ģʽ
����������
����ֵ	��
********************************************************************************
*/
void set_iptp(uint8_t num)
{
	uint8_t i, j, z;
	char str[5];
	
	z = 0;	
	for(j = 5; j < wifi_comm_info_data.rx_len; j++)
	{
		if(wifi_comm_info_data.rx[j] == ' ')
		{
			z++;
			if(z == (2 * num))	
				break;
		}
		else
		if(wifi_comm_info_data.rx[j] == '*')
		{
			return ;	
		}
	}
	i = j + 1;			//��λ�ֶ�λ��		

	
	//��λ����ֵ��Χ
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//��ȡ����ֵ
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//ת��
	z = atoi(str);
	if(z < 2)
	{
		mcb_wifi_set_succ = 1;	
		mr_in_out_auto_mode = z;
		mr_comm_sys_tx_cmd2 = QQJRTCZDMS_CMD_82;
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;
	}
	else
		mcb_wifi_set_succ = 0;		
}

/*
********************************************************************************
�������ƣ�set_ipbp
��������������������·��0-�˳��ֶ���1-����ͨ����2-����ͨ��2��3-��·1��4-��·2
����������
����ֵ	��
********************************************************************************
*/
void set_ipbp(uint8_t num)
{
	uint8_t i, j, z;
	char str[5];
	
	z = 0;	
	for(j = 5; j < wifi_comm_info_data.rx_len; j++)
	{
		if(wifi_comm_info_data.rx[j] == ' ')
		{
			z++;
			if(z == (2 * num))	
				break;
		}
		else
		if(wifi_comm_info_data.rx[j] == '*')
		{
			return ;	
		}
	}
	i = j + 1;			//��λ�ֶ�λ��		

	
	//��λ����ֵ��Χ
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//��ȡ����ֵ
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//ת��
	z = atoi(str);
	if(z < 5)
	{
		mcb_wifi_set_succ = 1;	
		mr_in_out_manu_mode = z;
		mr_comm_sys_tx_cmd2 = QQJRTCSDMS_CMD_81;
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;
	}
	else
		mcb_wifi_set_succ = 0;	
}

/*
********************************************************************************
�������ƣ�set_fddv
���������������豸��Ŀǰ����ȷ��
����������
����ֵ	��
********************************************************************************
*/
void set_fddv(uint8_t num)
{
	
	mcb_wifi_set_succ = 1;
}

/*
********************************************************************************
�������ƣ�set_r1gn
����������������Ƶ��ͨ��1����-˥��
����������
����ֵ	��
********************************************************************************
*/
void set_r1gn(uint8_t num)
{
	uint8_t i, j, z;
	char str[5];
	
	z = 0;	
	for(j = 5; j < wifi_comm_info_data.rx_len; j++)
	{
		if(wifi_comm_info_data.rx[j] == ' ')
		{
			z++;
			if(z == (2 * num))	
				break;
		}
		else
		if(wifi_comm_info_data.rx[j] == '*')
		{
			return ;	
		}
	}
	i = j + 1;			//��λ�ֶ�λ��		

	
	//��λ����ֵ��Χ
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//��ȡ����ֵ
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//ת��
	z = atoi(str);
	if(z < 31)
	{
		mcb_wifi_set_succ = 1;	
		mr_ch_att_sel |= 0x01;
		mr_ch1_att = z;
		mr_comm_sys_tx_cmd2 = QQSZSBSSJZ_CMD_85;
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;
	}
	else
	{
		mcb_wifi_set_succ = 0;
		mr_ch_att_sel &= ~0x01;		
	}
}

/*
********************************************************************************
�������ƣ�set_r2gn
����������������Ƶ��ͨ��2����-˥��
����������
����ֵ	��
********************************************************************************
*/
void set_r2gn(uint8_t num)
{
	uint8_t i, j, z;
	char str[5];
	
	z = 0;	
	for(j = 5; j < wifi_comm_info_data.rx_len; j++)
	{
		if(wifi_comm_info_data.rx[j] == ' ')
		{
			z++;
			if(z == (2 * num))	
				break;
		}
		else
		if(wifi_comm_info_data.rx[j] == '*')
		{
			return ;	
		}
	}
	i = j + 1;			//��λ�ֶ�λ��		

	
	//��λ����ֵ��Χ
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//��ȡ����ֵ
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//ת��
	z = atoi(str);
	if(z < 31)
	{
		mcb_wifi_set_succ = 1;	
		mr_ch_att_sel |= 0x02;
		mr_ch2_att = z;
		mr_comm_sys_tx_cmd2 = QQSZSBSSJZ_CMD_85;
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;
	}
	else
	{
		mcb_wifi_set_succ = 0;
		mr_ch_att_sel &= ~0x02;		
	}
}

/*
********************************************************************************
�������ƣ�set_rdgn
����������������Ƶ�幫������-˥��
����������
����ֵ	��
********************************************************************************
*/
void set_rdgn(uint8_t num)
{
	uint8_t i, j, z;
	char str[5];
	
	z = 0;	
	for(j = 5; j < wifi_comm_info_data.rx_len; j++)
	{
		if(wifi_comm_info_data.rx[j] == ' ')
		{
			z++;
			if(z == (2 * num))	
				break;
		}
		else
		if(wifi_comm_info_data.rx[j] == '*')
		{
			return ;	
		}
	}
	i = j + 1;			//��λ�ֶ�λ��		

	
	//��λ����ֵ��Χ
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//��ȡ����ֵ
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//ת��
	z = atoi(str);
	if(z < 61)
	{
		mcb_wifi_set_succ = 1;	
		mr_ch_att_sel |= 0x04;
		mr_com_att = z;
		mr_comm_sys_tx_cmd2 = QQSZSBSSJZ_CMD_85;
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;
	}
	else
	{
		mcb_wifi_set_succ = 0;
		mr_ch_att_sel &= ~0x04;		
	}
}        

/*
********************************************************************************
�������ƣ�set_pmgn
�������������������˹��ְ�����
����������
����ֵ	��
********************************************************************************
*/
void set_pmgn(uint8_t num)
{
	uint8_t i, j, z;
	char str[5];
	
	z = 0;	
	for(j = 5; j < wifi_comm_info_data.rx_len; j++)
	{
		if(wifi_comm_info_data.rx[j] == ' ')
		{
			z++;
			if(z == (2 * num))	
				break;
		}
		else
		if(wifi_comm_info_data.rx[j] == '*')
		{
			return ;	
		}
	}
	i = j + 1;			//��λ�ֶ�λ��		

	
	//��λ����ֵ��Χ
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//��ȡ����ֵ
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//ת��
	z = atoi(str);
	if(z < 31)
	{
		mcb_wifi_set_succ = 1;	
		mp_ch_att_sel |= 0x01;
		mp_out_att = z;
		mp_comm_sys_tx_cmd2 = MP_QQSZSBLSJZ_CMD_85;
		mp_comm_sys_tx_flag = 1;
		mcb_pd8_task_flag = 1;
	}
	else
		mcb_wifi_set_succ = 0;		
}            

/*
********************************************************************************
�������ƣ�set_psgn
�������������ôӻ��˹��ְ����棬��ʱû��
����������
����ֵ	��
********************************************************************************
*/
void set_psgn(uint8_t num)
{
	
	mcb_wifi_set_succ = 1;
}           

/*
********************************************************************************
�������ƣ�set_yzmd
����������������ʾģʽ���ã���ʱû��
����������
����ֵ	��
********************************************************************************
*/
void set_yzmd(uint8_t num)
{
	mcb_wifi_set_succ = 1;	
}

/*
********************************************************************************
�������ƣ�wifi_cmd_80_deal
�������������豸������ȷ��
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_80_deal(void)
{
    if(wifi_comm_ru_data.rx[3] == 0x02)
    {
    	if(wifi_comm_ru_data.rx[5] == WIFI_COMM_OK)
    	{
  			if(wifi_comm_ru_data.rx[4] < 0x07)
  			{
  				//�˳�Զ������ģʽ
  				//ȷ���˳��󣬲����ʽ��������˳�����ģʽ��
  				
  			}	
  			else
  			if((wifi_comm_ru_data.rx[4] >= 0x80) && (wifi_comm_ru_data.rx[4] <= 0x86))
  			{
  				//Զ������ȷ�ϳɹ�	
  			}
    	}	
    	else
    	if(wifi_comm_ru_data.rx[5] == WIFI_COMM_FAIL)
    	{
    		switch(wifi_comm_ru_data.rx[4])			//ʧ�ܴ���������
    		{
    			//�˳�Զ������ȷ��ʧ��
    			case 0x00: break;
    			case 0x01: break;
    			case 0x02: break;
    			case 0x03: break;
    			case 0x04: break;
    			case 0x05: break;    	
    			case 0x06: break; 
    			//Զ�������ɹ�ȷ��ʧ��
    			case 0x80: break;
    			case 0x81: break;
    			case 0x82: break;
    			case 0x83: break;
    			case 0x84: break;
    			case 0x85: break;    	
    			case 0x86: break;  		 				    			    			
    			default: break;
    		}
    		//������Ƶ��Э��塢���Ӱ˹��ְ巴��״̬���м��豸�࣬������·�ѿظ����ˣ�
    	}
    }		
}

/*
********************************************************************************
�������ƣ�wifi_cmd_FC_deal
�������������豸���˹��ְ弰���ַ
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_FC_deal(void)
{
	if(wifi_comm_ru_data.rx[3] == 0x01)	
	{
		if(wifi_comm_ru_data.rx[4] == 0x01)	
		{
	 		wifi_cmd_7A_deal();
		}
	}
}

/*
********************************************************************************
�������ƣ�wifi_cmd_FD_deal
�������������豸����Ƶ�廹��Э���
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_FD_deal(void)
{
	if(wifi_comm_ru_data.rx[3] == 0x01)	
	{
		if(wifi_comm_ru_data.rx[4] == 0x01)	
		{
			wifi_cmd_7B_deal();
		}
	}		
}

/*
********************************************************************************
�������ƣ�wifi_cmd_FE_deal
�������������豸��ȡ��Ӧ���ļ���
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_FE_deal(void)
{
	if(wifi_comm_ru_data.rx[3] == 0x01)	
	{
		wifi_cmd_7F_deal();
	}		
}

/*
********************************************************************************
�������ƣ�wifi_cmd_FF_deal
����������֪ͨ���豸����Զ������ģʽ
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_FF_deal(void)
{	
	//����֪ͨ���豸�����ذ���Ҫ��һ��֪ͨ�¼��壨������Ӧ����Ƶ/Э����������Ӱ˹��ְ�����
	if(wifi_comm_ru_data.rx[3] == 0x01)	
	{
		if(wifi_comm_ru_data.rx[4] == 0x00)
			mcb_wifi_ru_xxb = 0x40;
		else
		if(wifi_comm_ru_data.rx[4] == 0x01)
			mcb_wifi_ru_xxb = 0x01;
		else
		if(wifi_comm_ru_data.rx[4] == 0x02)
			mcb_wifi_ru_xxb = 0x02;
		else
		if(wifi_comm_ru_data.rx[4] == 0x03)
			mcb_wifi_ru_xxb = 0x04;
		else
		if(wifi_comm_ru_data.rx[4] == 0x04)
			mcb_wifi_ru_xxb = 0x08;
		else
		if(wifi_comm_ru_data.rx[4] == 0x05)
			mcb_wifi_ru_xxb = 0x10;			
		else
		if(wifi_comm_ru_data.rx[4] == 0x06)
			mcb_wifi_ru_xxb = 0x20;
					
		wifi_cmd_7D_deal();
	}		
}

/*
********************************************************************************
�������ƣ�wifi_cmd_7A_deal
�������������������˹��ְ�������ӻ������ַ��Ϣ
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_7A_deal(void)
{
	uint8_t i, j, chksum;
	
    i = 0;
    //֡ͷ
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//��֡
 	wifi_comm_ru_data.tx[i++] = WIFI_HFSPD8SLDZ_CMD_7A;
 	wifi_comm_ru_data.tx[i++] = mcb_cur_pd8_slave_num + 1;
 	
 	if(mcb_cur_pd8_slave_num)
 	{
	 	wifi_comm_ru_data.tx[i++] = mcb_cur_pd8_slave_num;
	 	for(j  = 0; j < mcb_cur_pd8_slave_num; j++)
	 	{
	 		wifi_comm_ru_data.tx[i++] = mcb_cur_valid_slave_addr[j];
	 	}			
 	}
 	else
 		wifi_comm_ru_data.tx[i++] = 0x00;

    wifi_comm_ru_data.tx_len = i;                        

    //����У���
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 		
}

/*
********************************************************************************
�������ƣ�wifi_cmd_7B_deal
������������������Ƶ�廹��Э���
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_7B_deal(void)
{
	uint8_t i, chksum;
	
    i = 0;
    //֡ͷ
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//��֡
 	wifi_comm_ru_data.tx[i++] = WIFI_HFRFBHXYB_CMD_7B;
 	wifi_comm_ru_data.tx[i++] = 0x01;
 	
 	wifi_comm_ru_data.tx[i++] = rfb_or_xyb;		//1:��Ƶ��, 2:Э���

    wifi_comm_ru_data.tx_len = i;                        

    //����У���
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 				
}

/*
********************************************************************************
�������ƣ�wifi_cmd_7C_deal
����������֪ͨ���豸Զ�������ɹ�
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_7C_deal(void)
{
	uint8_t i;
	
    i = 2;
	//��֡
 	wifi_comm_ru_data.tx[i++] = WIFI_ZDTZSJCG_CMD_7C;
 	wifi_comm_ru_data.tx[i++] = 0x01;
 	
 	if(mcb_wifi_ru_xxb & 0x40)
 		wifi_comm_ru_data.tx[i++] = 0x00;
 	else
 	if(mcb_wifi_ru_xxb & 0x01)
 		wifi_comm_ru_data.tx[i++] = 0x01;
 	else
 	if(mcb_wifi_ru_xxb & 0x02)
 		wifi_comm_ru_data.tx[i++] = 0x02;
 	else
 	if(mcb_wifi_ru_xxb & 0x04)
 		wifi_comm_ru_data.tx[i++] = 0x03;
 	else
 	if(mcb_wifi_ru_xxb & 0x08)
 		wifi_comm_ru_data.tx[i++] = 0x04;	
  	else
 	if(mcb_wifi_ru_xxb & 0x10)
 		wifi_comm_ru_data.tx[i++] = 0x05;	
  	else
 	if(mcb_wifi_ru_xxb & 0x20)
 		wifi_comm_ru_data.tx[i++] = 0x06;	 		
 		
    wifi_comm_ru_data.tx_len = i;                        	
}

/*
********************************************************************************
�������ƣ�wifi_cmd_7D_deal
�������������豸����Զ������ģʽȷ��֡
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_7D_deal(void)
{
	uint8_t i, chksum;
	
    i = 0;
    //֡ͷ
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//��֡
 	wifi_comm_ru_data.tx[i++] = WIFI_HFJRSJQR_CMD_7D;
 	wifi_comm_ru_data.tx[i++] = 0x02;
 	
 	mcb_wifi_ru_state = 0x00;
 	
 	if(mcb_wifi_ru_xxb & 0x40)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x00;
		wifi_comm_ru_data.tx[i++] = WIFI_COMM_OK;
		mcb_wifi_ru_state = 0x40;
		//���ذ����Զ������
		//����󣬴˴��ڲ�������Ҫ���ߴ���
		
 	}
 	else
 	if(mcb_wifi_ru_xxb & 0x01)
 	{
		wifi_comm_ru_data.tx[i++] = 0x01; 	
		wifi_comm_ru_data.tx[i++] = 0x55;		//�ȴ����豸��Ӧ����ͬ
		
		rfb_xyb_in_out_ru_flag = 0x01;
		mr_comm_sys_tx_cmd2 = QQJRYCSJ_CMD_FF;  
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;   
 	}
 	else
 	if(mcb_wifi_ru_xxb & 0x02)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x02;
 		wifi_comm_ru_data.tx[i++] = 0x55;
 		
 		rfb_xyb_in_out_ru_flag = 0x02;
		mr_comm_sys_tx_cmd2 = QQJRYCSJ_CMD_FF;  
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;   
 	}
 	else
 	if(mcb_wifi_ru_xxb & 0x04)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x03;	
 		wifi_comm_ru_data.tx[i++] = 0x55;
 		
 		rfb_xyb_in_out_ru_flag = 0x04;
		mr_comm_sys_tx_cmd2 = QQJRYCSJ_CMD_FF;  
		mr_comm_sys_tx_flag = 1;
		mcb_rfb_task_flag = 1;   
 	}
 	else
 	if(mcb_wifi_ru_xxb & 0x08)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x04;	
 		wifi_comm_ru_data.tx[i++] = 0x55;
 		
 		mp_pd8_in_out_ru_flag = 0x01;
		mp_comm_sys_tx_cmd2 = MP_QQJRYCSJ_CMD_FF;  
		mp_comm_sys_tx_flag = 1;
		mcb_pd8_task_flag = 1;   
 	}	
  	else
 	if(mcb_wifi_ru_xxb & 0x10)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x05;	
 		wifi_comm_ru_data.tx[i++] = 0x55;	
 		
 		mp_pd8_in_out_ru_flag = 0x02;
		mp_comm_sys_tx_cmd2 = MP_QQJRYCSJ_CMD_FF;  
		mp_comm_sys_tx_flag = 1;
		mcb_pd8_task_flag = 1;   
 	}
  	else
 	if(mcb_wifi_ru_xxb & 0x20)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x06;
 		wifi_comm_ru_data.tx[i++] = 0x55;		
 		
 		mp_pd8_in_out_ru_flag = 0x04;
		mp_comm_sys_tx_cmd2 = MP_QQJRYCSJ_CMD_FF;  
		mp_comm_sys_tx_flag = 1;
		mcb_pd8_task_flag = 1;   
 	}
 			 	
    wifi_comm_ru_data.tx_len = i;    
    
    //����У���
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 	     
    
}

/*
********************************************************************************
�������ƣ�wifi_cmd_7D_deal_reply
�������������豸����Զ������ģʽȷ��֡�������ذ���
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_7D_deal_reply(void)
{
	uint8_t i;
	
    i = 2;
	//��֡
 	wifi_comm_ru_data.tx[i++] = WIFI_HFJRSJQR_CMD_7D;
 	wifi_comm_ru_data.tx[i++] = 0x02;
 	//
 	if(mcb_wifi_ru_xxb & 0x01)
 	{
		wifi_comm_ru_data.tx[i++] = 0x01; 	
 	}
 	else
 	if(mcb_wifi_ru_xxb & 0x02)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x02;
 	}
 	else
 	if(mcb_wifi_ru_xxb & 0x04)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x03;	
 	}
 	else
 	if(mcb_wifi_ru_xxb & 0x08)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x04;	
    }
  	else
 	if(mcb_wifi_ru_xxb & 0x10)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x05;	
 	}
  	else
 	if(mcb_wifi_ru_xxb & 0x20)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x06;
 	}
 	else
 		wifi_comm_ru_data.tx[i++] = 0x00;
 	//state
 	if(!mcb_wifi_ru_state)
 		wifi_comm_ru_data.tx[i++] = WIFI_COMM_OK;
 	else
 		wifi_comm_ru_data.tx[i++] = WIFI_COMM_FAIL;			
 			 	
    wifi_comm_ru_data.tx_len = i;                		
}

/*
********************************************************************************
�������ƣ�wifi_cmd_7E_deal
����������֪ͨ���豸�˳�Զ������ģʽ
����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_7E_deal(void)
{
	uint8_t i;
	
    i = 2;
	//��֡
 	wifi_comm_ru_data.tx[i++] = WIFI_ZDTZTCYCSJ_CMD_7E;
 	wifi_comm_ru_data.tx[i++] = 0x01;
 	
 	if(mcb_wifi_ru_xxb & 0x40)
 		wifi_comm_ru_data.tx[i++] = 0x00;
 	else
 	if(mcb_wifi_ru_xxb & 0x01)
 		wifi_comm_ru_data.tx[i++] = 0x01;
 	else
 	if(mcb_wifi_ru_xxb & 0x02)
 		wifi_comm_ru_data.tx[i++] = 0x02;
 	else
 	if(mcb_wifi_ru_xxb & 0x04)
 		wifi_comm_ru_data.tx[i++] = 0x03;
 	else
 	if(mcb_wifi_ru_xxb & 0x08)
 		wifi_comm_ru_data.tx[i++] = 0x04;	
  	else
 	if(mcb_wifi_ru_xxb & 0x10)
 		wifi_comm_ru_data.tx[i++] = 0x05;	
  	else
 	if(mcb_wifi_ru_xxb & 0x20)
 		wifi_comm_ru_data.tx[i++] = 0x06;	 		

    wifi_comm_ru_data.tx_len = i;     
    
    //�˳��󣬲�������Ҫ��������           
            
}

/*
********************************************************************************
�������ƣ�wifi_cmd_7F_deal
�������������ش��豸�ļ���
������������ȡ�Ǹ���ģ������Ǹ���Ķ�ȡ���Ȼ���1-3����ٶ�������Ч���ݣ�����̨����������
����ֵ	��
********************************************************************************
*/
void wifi_cmd_7F_deal(void)
{
	uint8_t i, j, chksum;
	
    i = 0;
    //֡ͷ
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//��֡
 	wifi_comm_ru_data.tx[i++] = WIFI_HFJRSJQR_CMD_7D;
 	
 	if(wifi_comm_ru_data.rx[4] == 0x00)
 	{
		wifi_comm_ru_data.tx[i++] = 12;
		wifi_comm_ru_data.tx[i++] = 0x00;
		for(j = 0; j < 11; j++)
			wifi_comm_ru_data.tx[i++] = mcb_file[j];
 	}
 	else
 	if(wifi_comm_ru_data.rx[4] == 0x01)
 	{
		if(rfb_or_xyb == RFB)
		{
			wifi_comm_ru_data.tx[i++] = 12;
			wifi_comm_ru_data.tx[i++] = 0x01;
			for(j = 0; j < 11; j++)
				wifi_comm_ru_data.tx[i++] = rfb_xybm_file[j];	
			//	
			rfb_xybm_xybf_sel = 1;
			mr_comm_sys_tx_cmd2 = QQDQSBWJM_CMD_FE;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;				
		}
		else
		if(rfb_or_xyb == XYB)
		{
			wifi_comm_ru_data.tx[i++] = 6;
			wifi_comm_ru_data.tx[i++] = 0x01;
			wifi_comm_ru_data.tx[i++] = 'N';	
			wifi_comm_ru_data.tx[i++] = 'O';
			wifi_comm_ru_data.tx[i++] = 'R';
			wifi_comm_ru_data.tx[i++] = 'F';
			wifi_comm_ru_data.tx[i++] = 'B';					
		}
 	} 	
  	else
 	if(wifi_comm_ru_data.rx[4] == 0x02)
 	{
		if(rfb_or_xyb == XYB)
		{
			wifi_comm_ru_data.tx[i++] = 13;
			wifi_comm_ru_data.tx[i++] = 0x02;
			for(j = 0; j < 12; j++)
				wifi_comm_ru_data.tx[i++] = rfb_xybm_file[j];	
			//	
			rfb_xybm_xybf_sel = 2;
			mr_comm_sys_tx_cmd2 = QQDQSBWJM_CMD_FE;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;							
		}
		else
		if(rfb_or_xyb == RFB)
		{
			wifi_comm_ru_data.tx[i++] = 6;
			wifi_comm_ru_data.tx[i++] = 0x02;
			wifi_comm_ru_data.tx[i++] = 'N';	
			wifi_comm_ru_data.tx[i++] = 'O';
			wifi_comm_ru_data.tx[i++] = 'X';
			wifi_comm_ru_data.tx[i++] = 'Y';
			wifi_comm_ru_data.tx[i++] = 'B';					
		}
 	} 	
 	else
 	if(wifi_comm_ru_data.rx[4] == 0x03)
 	{
		if(rfb_or_xyb == XYB)
		{
			wifi_comm_ru_data.tx[i++] = 13;
			wifi_comm_ru_data.tx[i++] = 0x03;
			for(j = 0; j < 12; j++)
				wifi_comm_ru_data.tx[i++] = rfb_xybf_file[j];	
			//	
			rfb_xybm_xybf_sel = 3;
			mr_comm_sys_tx_cmd2 = QQDQSBWJM_CMD_FE;
			mr_comm_sys_tx_flag = 1;
			mcb_rfb_task_flag = 1;							
		}
		else
		if(rfb_or_xyb == RFB)
		{
			wifi_comm_ru_data.tx[i++] = 6;
			wifi_comm_ru_data.tx[i++] = 0x03;
			wifi_comm_ru_data.tx[i++] = 'N';	
			wifi_comm_ru_data.tx[i++] = 'O';
			wifi_comm_ru_data.tx[i++] = 'X';
			wifi_comm_ru_data.tx[i++] = 'Y';
			wifi_comm_ru_data.tx[i++] = 'B';					
		}
 	} 	
 	else
 	if(wifi_comm_ru_data.rx[4] == 0x04)
 	{
		wifi_comm_ru_data.tx[i++] = 13;
		wifi_comm_ru_data.tx[i++] = 0x04;
		for(j = 0; j < 12; j++)
			wifi_comm_ru_data.tx[i++] = mpd8_file[j];	
		//	
		mp_pd8_sel = 0x04;
		mp_comm_sys_tx_cmd2 = MP_QQDQSBWJM_CMD_FE;
		mp_comm_sys_tx_flag = 1;
		mcb_pd8_task_flag = 1;		
 	} 	 	 
  	else
 	if(wifi_comm_ru_data.rx[4] == 0x05)
 	{
 		if(mcb_cur_pd8_slave_num == 0)
 		{
			wifi_comm_ru_data.tx[i++] = 7;
			wifi_comm_ru_data.tx[i++] = 0x05;
			wifi_comm_ru_data.tx[i++] = 'N';	
			wifi_comm_ru_data.tx[i++] = 'O';
			wifi_comm_ru_data.tx[i++] = 'P';
			wifi_comm_ru_data.tx[i++] = 'D';
			wifi_comm_ru_data.tx[i++] = '8';	
			wifi_comm_ru_data.tx[i++] = 'S';					
 		}
 		else
 		{
			wifi_comm_ru_data.tx[i++] = 13 * mcb_cur_pd8_slave_num;
			for(chksum = 0; chksum < mcb_cur_pd8_slave_num; chksum++)
			{
				wifi_comm_ru_data.tx[i++] = mcb_cur_valid_slave_addr[chksum];
				for(j = 0; j < 12; j++)
					wifi_comm_ru_data.tx[i++] = spd8_file[mcb_cur_valid_slave_addr[chksum] - 1][j];	 						
			}
			//	
			mp_pd8_sel = 0x05;
			mp_comm_sys_tx_cmd2 = MP_QQDQSBWJM_CMD_FE;
			mp_comm_sys_tx_flag = 1;
			mcb_pd8_task_flag = 1;		
 		} 		
 	} 	
 	else
 	if(wifi_comm_ru_data.rx[4] == 0x06)
 	{
 		if(mcb_cur_pd8_slave_num == 0)
 		{
			wifi_comm_ru_data.tx[i++] = 19;
			wifi_comm_ru_data.tx[i++] = 0x06;
			for(j = 0; j < 12; j++)
				wifi_comm_ru_data.tx[i++] = mpd8_file[j];				
			wifi_comm_ru_data.tx[i++] = 'N';	
			wifi_comm_ru_data.tx[i++] = 'O';
			wifi_comm_ru_data.tx[i++] = 'P';
			wifi_comm_ru_data.tx[i++] = 'D';
			wifi_comm_ru_data.tx[i++] = '8';	
			wifi_comm_ru_data.tx[i++] = 'S';					
 		}
 		else
 		{
			wifi_comm_ru_data.tx[i++] = 13 * (mcb_cur_pd8_slave_num + 1);
			wifi_comm_ru_data.tx[i++] = 0x06;
			for(j = 0; j < 12; j++)
				wifi_comm_ru_data.tx[i++] = mpd8_file[j];					
			for(chksum = 0; chksum < mcb_cur_pd8_slave_num; chksum++)
			{
				wifi_comm_ru_data.tx[i++] = mcb_cur_valid_slave_addr[chksum];
				for(j = 0; j < 12; j++)
					wifi_comm_ru_data.tx[i++] = spd8_file[mcb_cur_valid_slave_addr[chksum] - 1][j];	 						
			}
			//
			mp_pd8_sel = 0x06;
			mp_comm_sys_tx_cmd2 = MP_QQDQSBWJM_CMD_FE;
			mp_comm_sys_tx_flag = 1;
			mcb_pd8_task_flag = 1;			
 		}
 	} 	 		
 			 	
    wifi_comm_ru_data.tx_len = i;    
    
    //����У���
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 	     		
}

/*
********************************************************************************
�������ƣ�usart1_init
����������usart1��ʼ��
����������
����ֵ	��
********************************************************************************
*/
void usart1_init(uint32_t br)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);  		//ʹ��GPIOA��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );  	//ʹ��USART��ʱ��
	/* USART1�Ķ˿����� */        
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);  		//����PA9�ɵڶ���������	TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1); 		//����PA10�ɵڶ���������  RX	      
	/*
	*  USART1_TX -> PA9 , USART1_RX ->  PA10
	*/                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);        
	
	/* USART1�Ļ������� */
	USART_InitStructure.USART_BaudRate = br;					//���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//��������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����Ч��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//����������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ù���ģʽ
	USART_Init(USART1, &USART_InitStructure); 					//������ṹ��
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);          		//ʹ�ܽ����ж�
	USART_Cmd(USART1, ENABLE);									//ʹ�ܴ���1
	
	/* USART1��NVIC�ж����� */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;   			//����1�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;    			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStruct);
}			
