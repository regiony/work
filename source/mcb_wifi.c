/*
********************************************************************************
文件名	：mcb_wifi.c
文件描述：主控板与WIFI通信模块，支持两种通信协议，升级部分(BINARY)和数据交互部分（ASCII）
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
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
函数名称：mcb_wifi_init
函数描述：主控板与WIFI通信模块初始化
参数描述：
返回值	：
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
函数名称：mcb_wifi_task
函数描述：主控板与WIFI通信模块任务
参数描述：
返回值	：
********************************************************************************
*/
void mcb_wifi_task(void) 
{   
    mcb_wifi_task_flag = 0;
    
    //if(mcb_wifi_ru_state)								//进入远程升级包数据处理待完善，下行透传，上行解析并转发给wifi，走Y-MODEM协议
   	//{
    //		
    //}
	
    if((mcb_alm_upwifi_flag) && (!mcb_wifi_ru_state) && (wifi_comm_sys_tx_end))	//ASCII WARN INFO，不需要做超时处理
    {													//有告警事件且不在远程升级中，还要不在接收发送处理中才可上报，即通信链路空闲时才可以自动上报							
    	mcb_alm_upwifi_flag = 0;
    	wifi_comm_sys_warn_send();
		wifi_comm_sys_tx_end = 0;
		mcb_wifi_alm_send_flag = 1;
	   	wifi_comm_link_tx_flag = 1;  
    }
    else
    if((wifi_comm_sys_tx_flag) && (wifi_comm_sys_tx_end))	//系统发送升级数据,不在发送数据处理中才可
    {
        wifi_comm_sys_tx_flag = 0;
        if(wifi_comm_send_recv_end)                  	//一次通信事务完成，此处结构可以优化，这样写便于调试
        {
	    	wifi_comm_sys_ru_send();        			//发送命令组帧处理
	     	wifi_comm_send_recv_end = 0;
	        wifi_comm_time_out_cnt = 0;              	//超时初始化
	        wifi_comm_time_out_en = 1;    
	        wifi_comm_sys_tx_end = 0;
	        mcb_wifi_ru_send_flag = 1;
	        wifi_comm_link_tx_flag = 1;           	
        }
        /*
        else                                         	//一次通信事务未完成
        {
            wifi_comm_sys_tx_flag = 1;
            mcb_wifi_task_flag = 1;                      
        }
        */
    }
    //else
    if((wifi_comm_sys_tx_to_flag) && (wifi_comm_sys_tx_end))			//系统发送超时处理		
    {
    	wifi_comm_sys_tx_to_flag = 0;		
		wifi_to_cnt1++;
		if(mr_to_cnt1 < WIFI_RECV_TO_ALLOW_NUM)				//超时最大重传3次
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
    //接收数据处理
    //else
    if(wifi_comm_recv_end_flag)
    {
        wifi_comm_recv_end_flag = 0;        
        if(mcb_wifi_info_deal_flag)  
        {
			mcb_wifi_info_deal_flag = 0;
			wifi_comm_info_data_deal();                  	//信息数据解析        		
        }    
       	else
       	if(mcb_wifi_ru_deal_flag)
       	{
			mcb_wifi_ru_deal_flag = 0;
			wifi_comm_ru_data_deal();						//升级数据解析       			
       	}
        wifi_comm_data_deal_flag = 0;
    }
    //链路发送处理
    if(wifi_comm_link_tx_flag)
    {
        wifi_comm_link_tx_flag = 0;   
        wifi_comm_data_send();                           	//数据发送
    }	
    
    if(mcb_alm_upwifi_flag | wifi_comm_sys_tx_flag)			//查漏
    	mcb_wifi_task_flag = 1;
}

/*
********************************************************************************
函数名称：wifi_comm_sys_ru_send
函数描述：系统发送升级数据函数
参数描述：
返回值  ：
********************************************************************************
*/
void wifi_comm_sys_ru_send(void)
{ 
	uint8_t i, chksum;
	
    i = 0;
    //帧头
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//组帧

   	if(mcb_wifi_sys_tx_cmd == WIFI_ZDTZSJCG_CMD_7C)
   		wifi_cmd_7C_deal();		//函数内判断mcb_wifi_ru_xxb板类型
   	else
   	if(mcb_wifi_sys_tx_cmd == WIFI_ZDTZTCYCSJ_CMD_7E)
		wifi_cmd_7E_deal(); 	//函数内判断mcb_wifi_ru_xxb板类型
	else
   	if(mcb_wifi_sys_tx_cmd == WIFI_HFJRSJQR_CMD_7D)
		wifi_cmd_7D_deal_reply(); 	//函数内判断mcb_wifi_ru_xxb板类型	
	else
		return ;                               

    //计算校验和
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 
}

/*
********************************************************************************
函数名称：wifi_comm_sys_warn_send
函数描述：系统发送告警数据函数
参数描述：
返回值  ：
********************************************************************************
*/
void wifi_comm_sys_warn_send(void)
{ 	
	char *str;
	
	//告警数据头
	wifi_comm_alm_data.tx_len = 0;
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = 'W';
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = 'A';
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = 'R';
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = 'N';
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = ' ';
	//把报警缓冲区数据copy到报警数据发送缓冲区
	alarm_data_out();		//调用报警数据函数	
	str = &mcb_alm_up_buf[mcb_alm_cur_index][0];	
	while(*str != '\0')
	{
		wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = *str++;
	}
	wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_len++] = '*';
}

/*
********************************************************************************
函数名称：wifi_comm_info_data_deal
函数描述：信息数据解析处理函数,ASCII码不带校验和
参数描述：
返回值	：
********************************************************************************
*/
void wifi_comm_info_data_deal(void)		//结构待优化
{
	uint8_t i, num, index;
	
	if((mcb_wifi_recv_cp_arr[1] == 'E') && (mcb_wifi_recv_cp_arr[2] == 'T'))
	{
		//查询或设置字段数量
		if((wifi_comm_info_data.rx[3] == WIFI_MCU_SPACE_CHAR) && (wifi_comm_info_data.rx[5] == WIFI_MCU_SPACE_CHAR))
		{
			if((wifi_comm_info_data.rx[4] > '0') && (wifi_comm_info_data.rx[4] <= '9'))
				num = wifi_comm_info_data.rx[4] - '0';
			else
				return ;					
		}
		/*
		else //暂定字段数<10
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

		//数据处理
		if(mcb_wifi_recv_cp_arr[0] == 'G')		//查询
		{
			//帧头
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
		    while(!wifi_comm_sys_tx_end)		//确保发送链路不打架
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
		if(mcb_wifi_recv_cp_arr[0] == 'S')		//设置
		{
			//帧头
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
			//帧尾
			wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '*';
			
			i = 0;
		    while(!wifi_comm_sys_tx_end)		//确保发送链路不打架
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
函数名称：wifi_comm_ru_data_deal
函数描述：升级数据解析处理函数
参数描述：
返回值	：
********************************************************************************
*/
void wifi_comm_ru_data_deal(void)		//结构待优化
{
    uint8_t i, chksum, flag;  
    
    chksum = 0;             //计算校验和
    flag = 0;
    for(i = 2; i < wifi_comm_ru_data.rx_len; i++)
        chksum ^= wifi_comm_ru_data.rx[i];

    //wifi_comm_ru_data.comm_state &= ~WIFI_RECV_TIME_OUT;

    if(chksum)              //校验和错误
    {
        wifi_comm_ru_data.comm_state |= WIFI_RECV_CHKSUM_ERR;
        //本协议约定校验和错误不做响应，由命令发起方根据应答超时重发处理
    }
    else                    //校验和正确
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
    //主设备回复帧处理，主设备请求帧不需以下处理
	if(wifi_comm_ru_data.rx[2] == WIFI_QRSBFSSS_CMD_80)
	{
		wifi_comm_time_out_en = 0;					//超时计数使能标志清0
		wifi_comm_send_recv_end = 1;  				//一次发收事务完成标志
		wifi_comm_ru_data.comm_state &= ~WIFI_RECV_TIME_OUT;  
        wifi_to_cnt1 = 0;     
        //mcb_wifi_sys_tx_cmd = 0;    
	}
	else
    if(!flag)
	{
		i = 0;
		while(!wifi_comm_sys_tx_end)		//确保发送链路不打架
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
函数名称：wifi_comm_data_send
函数描述：链路发送数据函数
参数描述：
返回值	：
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
函数名称：find_get_cmd_index
函数描述：查找GET命令号，没有使用库文件,strcmp,sscanf
参数描述：
返回值	：
********************************************************************************
*/
uint8_t find_get_cmd_index(uint8_t num)
{
	uint8_t i, index;
	
	index = 0x00;
	i = 6 + (num - 1) * 5;		//每个字段名固定4个字符，格式外带一个空格，简单高效

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
函数名称：find_set_cmd_index
函数描述：查找SET命令号，没有使用库文件,strcmp,sscanf
参数描述：
返回值	：
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
	i = j + 1;			//定位字段位置

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
函数名称：get_lgtd
函数描述：获取经度
参数描述：
返回值	：
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
函数名称：get_lttd
函数描述：获取纬度
参数描述：
返回值	：
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
函数名称：get_stct
函数描述：获取星数-GPS通道1或通道2可用星数
参数描述：
返回值	：
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
		sprintf(str, "%02d", 0x00);		//旁路模式不知道星数默认给00
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}

/*
********************************************************************************
函数名称：get_mngn
函数描述：获取主增益-不明确是哪个增益，默认为0
参数描述：
返回值	：
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
	
	sprintf(str, "%02d", 0x00);		//主增益默认给00
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];		
}

/*
********************************************************************************
函数名称：get_ipsn
函数描述：获取输入信号通道1或通道2的GPS载噪比AVG(没有去掉最大最小值)
参数描述：注意：触发读取射频板卫星个数及其载噪比命令，然后过1-3秒再读才是有效数据，后台间隔多读几次
返回值	：
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
		sn = 0x00;		//旁路模式默认给0
	
	sprintf(str, "%02d", sn);	
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];			
}

/*
********************************************************************************
函数名称：get_ipcn
函数描述：获取输入信号通道：0-通道1，1-通道2
参数描述：
返回值	：
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
函数名称：get_iptp
函数描述：获取输入类型：0-正常、1-手动、2-自动、4通道自检
参数描述：
返回值	：
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
函数名称：get_ipbp
函数描述：获取旁路？0-非旁路，1-旁路			//非旁路：0-通道1，1-通道2，旁路：2-旁路1，3-旁路2
参数描述：
返回值	：
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
	
	//sprintf(str, "%1d", mr_comm_msg.cur_chno);		//非旁路：0-通道1，1-通道2，旁路：2-旁路1，3-旁路2
	
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
函数名称：get_fddv
函数描述：获取设备？ 0-未发现，1-发现设备
参数描述：
返回值	：
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
	
	sprintf(str, "%1d", 1); 	//默认1
	
	wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = ' ';
	for(i = 0; i < strlen(str); i++)
		wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = str[i];			
}

/*
********************************************************************************
函数名称：get_l1op
函数描述：获取主机八功分板输出端口状态
参数描述：
返回值	：
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
函数名称：get_l2op
函数描述：获取从机机八功分板输出端口状态
参数描述：
返回值	：
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
		
		if((temp == 0x00) || (temp == 0x02))	//主机空载或者BBU，没有接从机，那么全为5
		{
			for(i = 0; i < 8; i++)
			{
				//str[i] = '5';
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '5'; //str[i];	
			}
		}
		else
		if(temp == 0x01)						//从机1-4端口状态
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
		
		if((temp == 0x00) || (temp == 0x02))	//主机空载或者BBU，没有接从机，那么全为5
		{
			for(i = 0; i < 8; i++)
			{
				//str[i] = '5';
				wifi_comm_info_data.tx[wifi_comm_info_data.tx_len++] = '5'; //str[i];	
			}
		}
		else
		if(temp == 0x01)						//从机5-8端口状态
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
函数名称：get_r1gn
函数描述：获取射频板通道1增益-衰减值
参数描述：
返回值	：
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
函数名称：get_r2gn
函数描述：获取射频板通道2增益-衰减值
参数描述：
返回值	：
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
函数名称：get_rdgn
函数描述：获取射频板检测通道增益-衰减值
参数描述：
返回值	：
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
函数名称：get_pmgn
函数描述：获取主机八功分板输出增益-衰减值
参数描述：
返回值	：
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
函数名称：get_psgn
函数描述：获取从机八功分板输出增益-衰减值,暂时没用，默认为0
参数描述：
返回值	：
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
函数名称：set_atte
函数描述：设置衰减，不明确？
参数描述：
返回值	：
********************************************************************************
*/  
void set_atte(uint8_t num)
{
	
	mcb_wifi_set_succ = 1;
}

/*
********************************************************************************
函数名称：set_mngn
函数描述：设置主增益，不明确？
参数描述：
返回值	：
********************************************************************************
*/  
void set_mngn(uint8_t num)
{
	
	mcb_wifi_set_succ = 1;
}

/*
********************************************************************************
函数名称：set_ipcn
函数描述：设置输入通道：0-退出手动，1-进入通道1，2-进入通道2，3-旁路1，4-旁路2
参数描述：
返回值	：
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
	i = j + 1;			//定位字段位置		

	
	//定位设置值范围
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//获取设置值
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//转换
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
函数名称：set_iptp
函数描述：设置输入类型，0-退出自动，1-进入自动模式
参数描述：
返回值	：
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
	i = j + 1;			//定位字段位置		

	
	//定位设置值范围
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//获取设置值
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//转换
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
函数名称：set_ipbp
函数描述：设置输入旁路，0-退出手动，1-进入通道，2-进入通道2，3-旁路1，4-旁路2
参数描述：
返回值	：
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
	i = j + 1;			//定位字段位置		

	
	//定位设置值范围
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//获取设置值
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//转换
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
函数名称：set_fddv
函数描述：发现设备，目前不明确？
参数描述：
返回值	：
********************************************************************************
*/
void set_fddv(uint8_t num)
{
	
	mcb_wifi_set_succ = 1;
}

/*
********************************************************************************
函数名称：set_r1gn
函数描述：设置射频板通道1增益-衰减
参数描述：
返回值	：
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
	i = j + 1;			//定位字段位置		

	
	//定位设置值范围
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//获取设置值
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//转换
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
函数名称：set_r2gn
函数描述：设置射频板通道2增益-衰减
参数描述：
返回值	：
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
	i = j + 1;			//定位字段位置		

	
	//定位设置值范围
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//获取设置值
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//转换
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
函数名称：set_rdgn
函数描述：设置射频板公共增益-衰减
参数描述：
返回值	：
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
	i = j + 1;			//定位字段位置		

	
	//定位设置值范围
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//获取设置值
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//转换
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
函数名称：set_pmgn
函数描述：设置主机八功分板增益
参数描述：
返回值	：
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
	i = j + 1;			//定位字段位置		

	
	//定位设置值范围
	for(j = i; j < wifi_comm_info_data.rx_len; j++)
	{
		if((wifi_comm_info_data.rx[j] == ' ') || (wifi_comm_info_data.rx[j] == '*'))
		{
				break;
		}
	}
	//获取设置值
	z = 0;
	while(i < j)
	{
		str[z++] = wifi_comm_info_data.rx[i++];
	}
	str[z] = '\0';
	//转换
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
函数名称：set_psgn
函数描述：设置从机八功分板增益，暂时没用
参数描述：
返回值	：
********************************************************************************
*/
void set_psgn(uint8_t num)
{
	
	mcb_wifi_set_succ = 1;
}           

/*
********************************************************************************
函数名称：set_yzmd
函数描述：样机演示模式设置，暂时没用
参数描述：
返回值	：
********************************************************************************
*/
void set_yzmd(uint8_t num)
{
	mcb_wifi_set_succ = 1;	
}

/*
********************************************************************************
函数名称：wifi_cmd_80_deal
函数描述：主设备发来的确认
参数描述：
返回值	：
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
  				//退出远程升级模式
  				//确认退出后，波特率降下来？退出升级模式？
  				
  			}	
  			else
  			if((wifi_comm_ru_data.rx[4] >= 0x80) && (wifi_comm_ru_data.rx[4] <= 0x86))
  			{
  				//远程升级确认成功	
  			}
    	}	
    	else
    	if(wifi_comm_ru_data.rx[5] == WIFI_COMM_FAIL)
    	{
    		switch(wifi_comm_ru_data.rx[4])			//失败处理，待处理？
    		{
    			//退出远程升级确认失败
    			case 0x00: break;
    			case 0x01: break;
    			case 0x02: break;
    			case 0x03: break;
    			case 0x04: break;
    			case 0x05: break;    	
    			case 0x06: break; 
    			//远程升级成功确认失败
    			case 0x80: break;
    			case 0x81: break;
    			case 0x82: break;
    			case 0x83: break;
    			case 0x84: break;
    			case 0x85: break;    	
    			case 0x86: break;  		 				    			    			
    			default: break;
    		}
    		//向本身、射频、协议板、主从八功分板反馈状态？中间设备多，整体链路把控复杂了？
    	}
    }		
}

/*
********************************************************************************
函数名称：wifi_cmd_FC_deal
函数描述：主设备读八功分板及其地址
参数描述：
返回值	：
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
函数名称：wifi_cmd_FD_deal
函数描述：主设备读射频板还是协议板
参数描述：
返回值	：
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
函数名称：wifi_cmd_FE_deal
函数描述：主设备读取相应板文件名
参数描述：
返回值	：
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
函数名称：wifi_cmd_FF_deal
函数描述：通知从设备进入远程升级模式
参数描述：
返回值	：
********************************************************************************
*/
void wifi_cmd_FF_deal(void)
{	
	//根据通知的设备，主控板需要进一步通知下级板（触发相应的射频/协议板任务、主从八功分板任务）
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
函数名称：wifi_cmd_7A_deal
函数描述：返回主机八功分板带几个从机及其地址信息
参数描述：
返回值	：
********************************************************************************
*/
void wifi_cmd_7A_deal(void)
{
	uint8_t i, j, chksum;
	
    i = 0;
    //帧头
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//组帧
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

    //计算校验和
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 		
}

/*
********************************************************************************
函数名称：wifi_cmd_7B_deal
函数描述：返回是射频板还是协议板
参数描述：
返回值	：
********************************************************************************
*/
void wifi_cmd_7B_deal(void)
{
	uint8_t i, chksum;
	
    i = 0;
    //帧头
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//组帧
 	wifi_comm_ru_data.tx[i++] = WIFI_HFRFBHXYB_CMD_7B;
 	wifi_comm_ru_data.tx[i++] = 0x01;
 	
 	wifi_comm_ru_data.tx[i++] = rfb_or_xyb;		//1:射频板, 2:协议板

    wifi_comm_ru_data.tx_len = i;                        

    //计算校验和
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 				
}

/*
********************************************************************************
函数名称：wifi_cmd_7C_deal
函数描述：通知主设备远程升级成功
参数描述：
返回值	：
********************************************************************************
*/
void wifi_cmd_7C_deal(void)
{
	uint8_t i;
	
    i = 2;
	//组帧
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
函数名称：wifi_cmd_7D_deal
函数描述：从设备进入远程升级模式确认帧
参数描述：
返回值	：
********************************************************************************
*/
void wifi_cmd_7D_deal(void)
{
	uint8_t i, chksum;
	
    i = 0;
    //帧头
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//组帧
 	wifi_comm_ru_data.tx[i++] = WIFI_HFJRSJQR_CMD_7D;
 	wifi_comm_ru_data.tx[i++] = 0x02;
 	
 	mcb_wifi_ru_state = 0x00;
 	
 	if(mcb_wifi_ru_xxb & 0x40)
 	{
 		wifi_comm_ru_data.tx[i++] = 0x00;
		wifi_comm_ru_data.tx[i++] = WIFI_COMM_OK;
		mcb_wifi_ru_state = 0x40;
		//主控板进入远程升级
		//进入后，此串口波特率需要升高处理？
		
 	}
 	else
 	if(mcb_wifi_ru_xxb & 0x01)
 	{
		wifi_comm_ru_data.tx[i++] = 0x01; 	
		wifi_comm_ru_data.tx[i++] = 0x55;		//等待从设备响应，下同
		
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
    
    //计算校验和
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 	     
    
}

/*
********************************************************************************
函数名称：wifi_cmd_7D_deal_reply
函数描述：从设备进入远程升级模式确认帧，除主控板外
参数描述：
返回值	：
********************************************************************************
*/
void wifi_cmd_7D_deal_reply(void)
{
	uint8_t i;
	
    i = 2;
	//组帧
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
函数名称：wifi_cmd_7E_deal
函数描述：通知主设备退出远程升级模式
参数描述：
返回值	：
********************************************************************************
*/
void wifi_cmd_7E_deal(void)
{
	uint8_t i;
	
    i = 2;
	//组帧
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
    
    //退出后，波特率需要降下来？           
            
}

/*
********************************************************************************
函数名称：wifi_cmd_7F_deal
函数描述：返回从设备文件名
参数描述：读取那个板的，触发那个板的读取命令，然后过1-3秒后再读才是有效数据！，后台间隔多读几次
返回值	：
********************************************************************************
*/
void wifi_cmd_7F_deal(void)
{
	uint8_t i, j, chksum;
	
    i = 0;
    //帧头
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_1;
    wifi_comm_ru_data.tx[i++] = WIFI_COMM_SYN_CHAR_2;
	//组帧
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
    
    //计算校验和
    chksum = 0;             
    for(i = 2; i < wifi_comm_ru_data.tx_len; i++)
        chksum ^= wifi_comm_ru_data.tx[i];       
    wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_len++] = chksum; 	     		
}

/*
********************************************************************************
函数名称：usart1_init
函数描述：usart1初始化
参数描述：
返回值	：
********************************************************************************
*/
void usart1_init(uint32_t br)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);  		//使能GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );  	//使能USART的时钟
	/* USART1的端口配置 */        
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);  		//配置PA9成第二功能引脚	TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1); 		//配置PA10成第二功能引脚  RX	      
	/*
	*  USART1_TX -> PA9 , USART1_RX ->  PA10
	*/                                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);        
	
	/* USART1的基本配置 */
	USART_InitStructure.USART_BaudRate = br;					//设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//设置数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//设置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//设置效验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//设置流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
	USART_Init(USART1, &USART_InitStructure); 					//配置入结构体
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);          		//使能接收中断
	USART_Cmd(USART1, ENABLE);									//使能串口1
	
	/* USART1的NVIC中断配置 */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;   			//串口1中断通道
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;    			//IRQ通道使能
	NVIC_Init(&NVIC_InitStruct);
}			
