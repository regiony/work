/*
********************************************************************************
文件名	：mcb_pd8.c
文件描述：主控板与八功分板通信模块
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __MCB_PD8_GLOBAL
#include "mcb_pd8.h" 
#include "public.h"                


/*
********************************************************************************
函数名称：mcb_pd8_init
函数描述：主控板与八功分板通信初始化
参数描述：
返回值	：
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
函数名称：mcb_pd8_task
函数描述：主控板与八功分板通信任务
参数描述：
返回值	：
********************************************************************************
*/
void mcb_pd8_task(void) 
{
    mcb_pd8_task_flag = 0;
    //系统发送请求
    if((mp_comm_sys_tx_flag) && (mp_comm_sys_tx_end))
    {
        mp_comm_sys_tx_flag = 0;
        if(mp_comm_send_recv_end)                           //一次通信事务完成，此处结构可以优化，这样写便于调试
        {
            if(mp_comm_sys_tx_cmd2 != 0)
            {
                mp_comm_sys_tx_cmd = mp_comm_sys_tx_cmd2;   //非轮询命令
                //mp_comm_sys_tx_cmd2 = 0;
            }
            else
            if(mp_comm_sys_tx_cmd1 != 0)
            {
                mp_comm_sys_tx_cmd = mp_comm_sys_tx_cmd1;   //轮询命令
                mp_comm_sys_tx_cmd1 = 0;
            }
            mp_comm_sys_send(mp_comm_sys_tx_cmd);           //发送命令组帧处理
            mp_comm_send_recv_end = 0;
            mp_comm_time_out_cnt = 0;                       //超时初始化
            mp_comm_time_out_en = 1;    
            mp_comm_sys_tx_end = 0;
            mp_comm_link_tx_flag = 1;   
        }
        /*
        else                                                //一次通信事务未完成
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
    //系统发送超时处理
    if((mp_comm_sys_tx_to_flag) && (mp_comm_sys_tx_end))
    {
    	mp_comm_sys_tx_to_flag = 0;		
		mp_to_cnt1++;
		if(mp_to_cnt1 < MP_RECV_TO_ALLOW_NUM)					//超时最大重传3次
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
    //接收数据处理
    //else
    if(mp_comm_recv_end_flag)
    {
        mp_comm_recv_end_flag = 0;              
        mp_comm_data_deal();                                //数据解析
        mp_comm_data_deal_flag = 0;
        /*
        if(mp_comm_sys_tx_cmd2 != 0) 						//防止漏掉系统非轮询命令
        {
            mp_comm_sys_tx_flag = 1;
            mcb_pd8_task_flag = 1;                  
        }*/
    }
    //链路发送处理
    if(mp_comm_link_tx_flag)
    {
        mp_comm_link_tx_flag = 0;   
        mp_comm_data_send();                                //数据发送
    }
}

/*
********************************************************************************
函数名称：mp_comm_sys_send
函数描述：发送数据函数
参数描述：
返回值  ：
********************************************************************************
*/
void mp_comm_sys_send(uint8_t cmd)
{ 
	uint8_t i, chksum;
    
    i = 0;
    //帧头
    mp_comm_data.tx[i++] = MP_COMM_SYN_CHAR_1;
    mp_comm_data.tx[i++] = MP_COMM_SYN_CHAR_2;
	//组帧
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
    //计算校验和
    chksum = 0;             
    for(i = 2; i < mp_comm_data.tx_len; i++)
        chksum ^= mp_comm_data.tx[i];       
    mp_comm_data.tx[mp_comm_data.tx_len++] = chksum; 
}

/*
********************************************************************************
函数名称：mp_comm_data_deal
函数描述：数据解析处理函数
参数描述：
返回值	：
********************************************************************************
*/
void mp_comm_data_deal(void)		//结构待优化
{
    uint8_t i, chksum;  
    
    chksum = 0;             //计算校验和
    for(i = 2; i < mp_comm_data.rx_len; i++)
        chksum ^= mp_comm_data.rx[i];

    //mp_comm_data.comm_state &= ~MP_RECV_TIME_OUT;
    //sys_fault_word &= ~0x02;

    if(chksum)              //校验和错误
    {
        mp_comm_data.comm_state |= MP_RECV_CHKSUM_ERR;
        //本协议约定校验和错误不做响应，由命令发起方根据应答超时重发处理
    }
    else                    //校验和正确
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
    //从设备回复帧处理，从设备请求帧不需以下处理 
    if((mp_comm_data.rx[2] != MP_ZDTZSJCG_CMD_7C) || (mp_comm_data.rx[2] != MP_ZDTZTCYCSJ_CMD_7E))
    {
	    mp_comm_time_out_en = 0;					//超时计数使能标志清0
	    mp_comm_send_recv_end = 1;  				//一次发收事务完成标志
	    mp_comm_sys_tx_cmd2 = 0;
	    mp_comm_data.comm_state &= ~MP_RECV_TIME_OUT;    
	    sys_fault_word &= ~0x02;  	
	    mp_to_cnt1 = 0;
    	mp_to_cnt2 = 0;	
    }   	
}

/*
********************************************************************************
函数名称：mp_comm_data_send
函数描述：发送数据函数
参数描述：
返回值	：
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
函数名称：mp_cmd_88_deal
函数描述：读取板信息及状态
参数描述：
返回值  ：
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
函数名称：mp_cmd_87_deal
函数描述：读取从设备可用星星号及其载噪比
参数描述：
返回值  ：
********************************************************************************
*/
void mp_cmd_87_deal(void)
{
    uint8_t i;
    
    i = 2;
    mp_comm_data.tx[i++] = MP_QQKYXZZB_CMD_87;
    mp_comm_data.tx[i++] = 0x03;
    mp_comm_data.tx[i++] = mp_mpd8_or_spd8;		//主、从机选择
    if(mp_mpd8_or_spd8)							//从机则输入从机号
    	mp_comm_data.tx[i++] = mcb_cur_rw_slave_index + 1;
    else  	
    	mp_comm_data.tx[i++] = 0;				//主机恒为0
    mp_comm_data.tx[i++] = mp_gps_or_bd;
    mp_comm_data.tx_len = i;
}

/*
********************************************************************************
函数名称：mp_cmd_85_deal
函数描述：请求从设备设置2个衰减值
参数描述：
返回值  ：
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
    
    if(mp_ch_att_sel & 0x01)		//设置输出通道 att
    {
	    mp_comm_data.tx[i++] = 0x01;
	    mp_comm_data.tx[i++] = mp_out_att;
	    n += 2;
    }
    if(mp_ch_att_sel & 0x02)		//设置检测通道 att
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
函数名称：mp_cmd_84_deal
函数描述：请求从设备读取2个衰减值
参数描述：
返回值  ：
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
函数名称：mp_cmd_83_deal
函数描述：请求从设备输出功率设置（保留）
参数描述：
返回值  ：
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
函数名称：mp_cmd_80_deal
函数描述：确认从设备发送的数据
参数描述：1：ok，0：fail
返回值  ：
********************************************************************************
*/
void mp_cmd_80_deal(uint8_t type, uint8_t flag)
{
    uint8_t i, chksum;
    
    i = 0;
    while(!mp_comm_sys_tx_end)		//确保发送链路不打架
    {
    	delay_xms(1);
    	i++;
    	if(i > 4)	
    		break;    	
    }   
    mp_comm_sys_tx_end = 0;
    
    i = 0;
    //帧头
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
    //计算校验和
    chksum = 0;             
    for(i = 2; i < mp_comm_data.tx_len; i++)
        chksum ^= mp_comm_data.tx[i];       
    mp_comm_data.tx[mp_comm_data.tx_len++] = chksum; 
    
	mp_comm_link_tx_flag = 1;
}

/*
********************************************************************************
函数名称：mp_cmd_FE_deal
函数描述：读设备文件名
参数描述：
返回值  ：
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
函数名称：mp_cmd_FF_deal
函数描述：通知从设备进入远程升级模式
参数描述：
返回值  ：
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
函数名称：mp_cmd_01_deal
函数描述：回复信息和状态
参数描述：
返回值  ：
********************************************************************************
*/
void mp_cmd_01_deal(void)
{
	uint8_t i, j, z, flag;
	
	if(mp_comm_data.rx[3] >= 16)
	{
		mcb_cur_pd8_slave_num = (mp_comm_data.rx[3] - 16) / 15; 	//从机数量			
		//主机信息及状态
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
		//从机   
		for(i = 0; i < MP_SPD8_MAX_NUM; i++) 	
		{
			mcb_pd8_slave_addr[i] = 0;								//从机地址清0
			mcb_cur_valid_slave_addr[i] = 0;						//当前有效从机地址
		}		
		flag = 0;
		for(z = 0; z < mcb_cur_pd8_slave_num; z++)
		{
			if(mp_comm_data.rx[j] > 0)
				mcb_cur_rw_slave_index = mp_comm_data.rx[j++] - 1;	
			else
				return ;		
			//从机信息及状态	
			mcb_pd8_slave_addr[mcb_cur_rw_slave_index] = mcb_cur_rw_slave_index + 1;	//从机地址
			mcb_cur_valid_slave_addr[z] = mcb_pd8_slave_addr[mcb_cur_rw_slave_index];	//当前有效从机地址
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
函数名称：mp_cmd_02_deal
函数描述：确认主设备发送的命令请求
参数描述：
返回值  ：
********************************************************************************
*/
void mp_cmd_02_deal(void)
{
    if(mp_comm_data.rx[3] == 0x02)
    {
    	if(mp_comm_data.rx[5] == MP_COMM_OK)
    	{
  			;		//确认ok，无需处理
    	}	
    	else
    	if(mp_comm_data.rx[5] == MP_COMM_FAIL)
    	{
    		if(mp_comm_data.rx[4] == 0x21)
    		{
				;			//向设置方（人机、wifi）反馈状态？  
  				
    		}
    		else
    		if(mp_comm_data.rx[4] == 0x41)
    		{
				;			//向设置方（人机、wifi）反馈状态？  
  				
    		}
    		else
    		if(mp_comm_data.rx[4] == 0x42)
    		{
				;			//向设置方（人机、wifi）反馈状态？  
  				
    		}
    	}
    }
    else
    if(mp_comm_data.rx[3] == 0x04)
    {
    	if((mp_comm_data.rx[5] == MP_COMM_OK) && (mp_comm_data.rx[7] == MP_COMM_OK))
    	{
  			;		//确认ok，无需处理
    	}	
    	else
    	if(mp_comm_data.rx[5] == MP_COMM_FAIL)
    	{
    		if(mp_comm_data.rx[4] == 0x41)
    		{
				;		//向设置方（人机、wifi）反馈状态？  
  				
    		}
    		else
    		if(mp_comm_data.rx[4] == 0x42)
    		{
				;		//向设置方（人机、wifi）反馈状态？  
  				
    		}
    	} 
    	else
    	if(mp_comm_data.rx[7] == MP_COMM_FAIL)
    	{
    		if(mp_comm_data.rx[6] == 0x41)
    		{
				;		//向设置方（人机、wifi）反馈状态？  
  				
    		}
    		else
    		if(mp_comm_data.rx[6] == 0x42)
    		{
				;		//向设置方（人机、wifi）反馈状态？  
  				
    		}
    	}     	   		
    }
}

/*
********************************************************************************
函数名称：mp_cmd_03_deal
函数描述：返回2个衰减值
参数描述：
返回值  ：
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
函数名称：mp_cmd_05_deal
函数描述：返回从设备的可用星星号及其载噪比
参数描述：
返回值  ：
********************************************************************************
*/
void mp_cmd_05_deal(void)
{
	uint8_t i , n;
	uint8_t *pt = 0;

    n = mp_comm_data.rx[3] - 3;				//可见星数量*2
    
    if(mp_comm_data.rx[4] == 0)				//主机
    {
    	//if(mp_comm_data.rx[5] == 0)		//固定为0
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
    if(mp_comm_data.rx[4] == 1)				//从机	
    {
    	if((mp_comm_data.rx[5] > 0) && (mp_comm_data.rx[5] < 9))
        {
            mcb_cur_rw_slave_index = mp_comm_data.rx[5] - 1;	//从机号索引
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
                *pt++ = mp_comm_data.rx[i + 7];	//获取可用星及载噪比
        }
        n = MP_KYX_MAX_NUM << 1;
        for(; i < n; i++)
            *pt++ = 0;						//不足12颗的空间清0
    }
}

/*
********************************************************************************
函数名称：mp_cmd_7C_deal
函数描述：通知主设备远程升级成功
参数描述：
返回值  ：
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
		
		//通知wifi接口任务，进一步向上汇报升级状态
		mcb_wifi_sys_tx_cmd = WIFI_ZDTZSJCG_CMD_7C;
		wifi_comm_sys_tx_flag = 1;
		mcb_wifi_task_flag = 1;
		
    }  
    else
   		mp_cmd_80_deal(mp_comm_data.rx[4] | 0x80, 0);        
}

/*
********************************************************************************
函数名称：mp_cmd_7D_deal
函数描述：从设备进入远程升级模式确认帧
参数描述：
返回值  ：
********************************************************************************
*/
void mp_cmd_7D_deal(void)
{
    if(mp_comm_data.rx[3] == 0x01)
    {
		if(mp_comm_data.rx[4] == mp_pd8_sel)
		{
			//通知wifi模块，成功进入
			//进入后，此串口波特率需要升高处理？
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
			//通知wifi模块，进入失败
			mcb_wifi_ru_state = 0x00;
			mcb_wifi_sys_tx_cmd = WIFI_HFJRSJQR_CMD_7D;
			wifi_comm_sys_tx_flag = 1;
			mcb_wifi_task_flag = 1;
		}
    }      
}

/*
********************************************************************************
函数名称：mp_cmd_7E_deal
函数描述：通知主设备退出远程升级模式
参数描述：
返回值  ：
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
		
		//通知wifi退出升级模式
		mcb_wifi_sys_tx_cmd = WIFI_ZDTZTCYCSJ_CMD_7E;
		wifi_comm_sys_tx_flag = 1;
		mcb_wifi_task_flag = 1;
		
		//退出后，波特率需要降下来？
		
    }  
    else
    	mp_cmd_80_deal(mp_comm_data.rx[4], 0);            
}

/*
********************************************************************************
函数名称：mp_cmd_7F_deal
函数描述：返回从设备文件名
参数描述：
返回值  ：
********************************************************************************
*/
void mp_cmd_7F_deal(void)
{
	uint8_t i, j, z, addr;
	
	if (mp_comm_data.rx[4] == 0x04)						//主机
	{
		if(mp_comm_data.rx[3] == 0x0D)
		{
			for(i = 0; i < (13 - 1); i++)
				mpd8_file[i] = mp_comm_data.rx[i + 5];
			mpd8_file[12]	= '\0';				
		}
	}
	else
	if (mp_comm_data.rx[4] == 0x05)						//从机
	{
		if(mp_comm_data.rx[3] == 0x07)					//无从机
		{
			;//无从机不需处理;	
		}	
		else
		if(mp_comm_data.rx[3] > 13)						//从机
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
	if (mp_comm_data.rx[4] == 0x06)						//主、从机
	{
		if(mp_comm_data.rx[3] == 19)					//主机、无从机
		{
			for(i = 0; i < (13 - 1); i++)
				mpd8_file[i] = mp_comm_data.rx[i + 5];
			mpd8_file[12]	= '\0';	
			//无从机不需处理;			
		}	
		else
		if(mp_comm_data.rx[3] >= 26)					//主、从机
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
函数名称：UART5_init
函数描述：UART5初始化
参数描述：
返回值	：
********************************************************************************
*/
void UART5_init(uint32_t br)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);  		//使能GPIOB的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART5, ENABLE ); 	//使能USART的时钟
	/* USART5的端口配置 */        
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_4);  		//配置PB3成第二功能引脚TX
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_4);  		//配置PB4成第二功能引脚RX	      

	//USART5_TX -> PB3 , USART5_RX ->  PB4                            
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);        
	
	/* USART5的基本配置 */
	USART_InitStructure.USART_BaudRate = br;					//设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//设置数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//设置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//设置效验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
	USART_Init(USART5, &USART_InitStructure); 					//配置入结构体

	USART_ITConfig(USART5,USART_IT_RXNE,ENABLE);           		
	USART_Cmd(USART5, ENABLE);									

	NVIC_InitStruct.NVIC_IRQChannel = USART3_6_IRQn;   
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;    
	NVIC_Init(&NVIC_InitStruct);
}			

/*
********************************************************************************
函数名称：MP_UTCToBeijing
函数描述：UTC时间转换为北京时间
参数描述：
返回值  ：
********************************************************************************
*/
void MP_UTCToBeijing(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	hour += 8;
	if(month==1 || month==3 || month==5 || month==7 || month==8 || month==10 || month==12)	//1,3,5,7,8,9,12月每月为31天
	{
	    if(hour >= 24) 
	    {   
	        hour -= 24;
	        day += 1;			//如果超过24小时，减去24小时，后再加上一天
	        if(day > 31)
	        {
	        	day -= 31;
	        	month += 1;		//如果超过31一天，减去31天，后加上一个月
	        	if(month > 12)
	        		year +=  1;	//超过12个月，年加1
	        }	
	    }
	}
	else 
	if(month==4 || month==6 || month==9 || month==11)	//4，6，9，11月每月为30天
	{
	    if(hour >= 24) 
	    {   
	        hour -= 24;
	        day += 1;			//如果超过24小时，减去24小时，后再加上一天
	        if(day > 30)
	        {
	        	day -= 30;
	        	month += 1;		//如果超过30一天，减去30天，后加上一个月
	        }
	    }
	}
	else						//剩下为2月，闰年为29天，平年为28天
	{
	    if(hour >= 24)
	    {
	        hour -= 24;
	        day += 1;
	        if((year%400 == 0) || (year%4 == 0  &&  year%100 != 0))	//判断是否为闰年，年号能被400整除或年号能被4整除，而不能被100整除为闰年
	        {
	        	if(day > 29)	//为闰年
	        	{
	        		day -= 29;
	        		month += 1;		
	        	}
	        }
	        else
	        {
	        	if(day > 28)	//为平年
	        	{
	        		day -= 28;
	        		month += 1;
	        	}
	        }
		}
	}	
	//更新北京时间结构体变量
	mp_comm_bt_time.year = year + 2000;
	mp_comm_bt_time.month = month; 
	mp_comm_bt_time.day = day; 
	mp_comm_bt_time.hour = hour; 
	mp_comm_bt_time.min = minute; 
	mp_comm_bt_time.sec = second; 
}

