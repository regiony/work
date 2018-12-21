/*
********************************************************************************
文件名  ：mcb_rfb.c
文件描述：主控板与射频板通信模块，射频板和协议板按兼容软件处理，有区别的地方区分开
建立人  ：杨瑞军
建立日期：2016.12.15
修改内容：
修改人  ：
修改日期：
********************************************************************************
*/

#define __MCB_RFB_GLOBAL
#include "mcb_rfb.h"                 
#include "public.h"

/*
********************************************************************************
函数名称：mcb_rfb_init
函数描述：主控板与射频板通信初始化
参数描述：
返回值  ：
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
    mr_comm_msg.cur_mode = 4;		//默认通道检测模式
        
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
函数名称：mcb_rfb_task
函数描述：主控板与射频板通信任务
参数描述：
返回值  ：
********************************************************************************
*/
void mcb_rfb_task(void) 
{
    mcb_rfb_task_flag = 0;
    //系统发送请求
    if((mr_comm_sys_tx_flag) && (mr_comm_sys_tx_end))
    {
        mr_comm_sys_tx_flag = 0;
        if(mr_comm_send_recv_end)                           //一次通信事务完成，此处结构可以优化，这样写便于调试
        {
            if(mr_comm_sys_tx_cmd2 != 0)
            {
                mr_comm_sys_tx_cmd = mr_comm_sys_tx_cmd2;   //非轮询命令
                //mr_comm_sys_tx_cmd2 = 0;
            }
            else
            if(mr_comm_sys_tx_cmd1 != 0)
            {
                mr_comm_sys_tx_cmd = mr_comm_sys_tx_cmd1;   //轮询命令
                mr_comm_sys_tx_cmd1 = 0;
            }
            mr_comm_sys_send(mr_comm_sys_tx_cmd);           //发送命令组帧处理
            mr_comm_send_recv_end = 0;
            mr_comm_time_out_cnt = 0;                       //超时初始化
            mr_comm_time_out_en = 1;    
            mr_comm_sys_tx_end = 0;
            mr_comm_link_tx_flag = 1;   
        }
        /*
        else                                                //一次通信事务未完成
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
    //系统发送超时处理
    if((mr_comm_sys_tx_to_flag) && (mr_comm_sys_tx_end))
    {
    	mr_comm_sys_tx_to_flag = 0;		
		mr_to_cnt1++;
		if(mr_to_cnt1 < RECV_TO_ALLOW_NUM)						//超时最大重传3次
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
    //接收数据处理
    //else
    if(mr_comm_recv_end_flag)
    {
        mr_comm_recv_end_flag = 0;              
        mr_comm_data_deal();                                //数据解析
        mr_comm_data_deal_flag = 0;
        /*
        if(mr_comm_sys_tx_cmd2 != 0) 						//防止漏掉系统非轮询命令
        {
            mr_comm_sys_tx_flag = 1;
            mcb_rfb_task_flag = 1;                  
        }*/
    }
    //链路发送处理
    if(mr_comm_link_tx_flag)
    {
        mr_comm_link_tx_flag = 0;   
        mr_comm_data_send();                                //数据发送
    }
}

/*
********************************************************************************
函数名称：mr_comm_sys_send
函数描述：发送数据函数
参数描述：
返回值  ：
********************************************************************************
*/
void mr_comm_sys_send(uint8_t cmd)
{ 
	uint8_t i, chksum;
    
    i = 0;
    //帧头
    mr_comm_data.tx[i++] = COMM_SYN_CHAR_1;
    mr_comm_data.tx[i++] = COMM_SYN_CHAR_2;
	//组帧
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
    //计算校验和
    chksum = 0;             
    for(i = 2; i < mr_comm_data.tx_len; i++)
        chksum ^= mr_comm_data.tx[i];       
    mr_comm_data.tx[mr_comm_data.tx_len++] = chksum; 
}

/*
********************************************************************************
函数名称：mr_comm_data_deal
函数描述：数据解析处理函数
参数描述：
返回值  ：
********************************************************************************
*/
void mr_comm_data_deal(void)
{
    uint8_t i, chksum;  
    
    chksum = 0;             //计算校验和
    for(i = 2; i < mr_comm_data.rx_len; i++)
        chksum ^= mr_comm_data.rx[i];

    //mr_comm_data.comm_state &= ~RECV_TIME_OUT;
    //sys_fault_word &= ~0x01;	

    if(chksum)              //校验和错误
    {
        mr_comm_data.comm_state |= RECV_CHKSUM_ERR;
        //本协议约定校验和错误不做响应，由命令发起方根据应答超时重发处理
    }
    else                    //校验和正确
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
    //从设备回复帧处理，从设备请求帧不需以下处理 
    if((mr_comm_data.rx[2] != ZDTZSJCG_CMD_7C) || (mr_comm_data.rx[2] != ZDTZTCYCSJ_CMD_7E))
    {
	    mr_comm_time_out_en = 0;					//超时计数使能标志清0
	    mr_comm_send_recv_end = 1;  				//一次发收事务完成标志
        mr_comm_sys_tx_cmd2 = 0;
	    mr_comm_data.comm_state &= ~RECV_TIME_OUT;      
	    sys_fault_word &= ~0x01;	
	    mr_to_cnt1 = 0;	
	    mr_to_cnt2 = 0;	
    }   	
}

/*
********************************************************************************
函数名称：mr_comm_data_send
函数描述：发送数据函数
参数描述：
返回值  ：
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
函数名称：cmd_88_deal
函数描述：读取板信息及状态
参数描述：
返回值  ：
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
函数名称：cmd_87_deal
函数描述：读取从设备可用星星号及其载噪比
参数描述：
返回值  ：
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
函数名称：cmd_86_deal
函数描述：读取从设备板类型
参数描述：
返回值  ：
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
函数名称：cmd_85_deal
函数描述：请求从设备设置3个衰减值 mr_ch_att_sel
参数描述：
返回值  ：
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
    
    if(mr_ch_att_sel & 0x01)		//设置通道1 att
    {
	    mr_comm_data.tx[i++] = 0x01;
	    mr_comm_data.tx[i++] = mr_ch1_att;
	    n += 2;
    }
    if(mr_ch_att_sel & 0x02)		//设置通道2 att
    {
	    mr_comm_data.tx[i++] = 0x02;
	    mr_comm_data.tx[i++] = mr_ch2_att;
	    n += 2;
    }
    if(mr_ch_att_sel & 0x04)		//设置公共通道 att
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
函数名称：cmd_84_deal
函数描述：请求从设备读取3个衰减值
参数描述：
返回值  ：
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
函数名称：cmd_83_deal
函数描述：请求从设备输出功率设置（保留）
参数描述：
返回值  ：
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
函数名称：cmd_82_deal
函数描述：请求从设备进入\退出自动模式
参数描述：
返回值  ：
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
函数名称：cmd_81_deal
函数描述：请求从设备进入\退出手动模式
参数描述：
返回值  ：
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
函数名称：cmd_80_deal
函数描述：确认从设备发送的数据
参数描述：1：ok，0：fail
返回值  ：
********************************************************************************
*/
void cmd_80_deal(uint8_t type, uint8_t flag)
{
    uint8_t i, chksum;
    
    i = 0;
    while(!mr_comm_sys_tx_end)		//确保发送链路不打架，若有问题开辟新缓冲
    {
    	delay_xms(1);
    	i++;
    	if(i > 4)	
    		break;    	
    }   
    mr_comm_sys_tx_end = 0;
    
    i = 0;
    //帧头
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
    //计算校验和
    chksum = 0;             
    for(i = 2; i < mr_comm_data.tx_len; i++)
        chksum ^= mr_comm_data.tx[i];       
    mr_comm_data.tx[mr_comm_data.tx_len++] = chksum; 
    
	mr_comm_link_tx_flag = 1;
}

/*
********************************************************************************
函数名称：cmd_FE_deal
函数描述：读设备文件名
参数描述：
返回值  ：
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
函数名称：cmd_FF_deal
函数描述：通知从设备进入远程升级模式
参数描述：
返回值  ：
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
函数名称：cmd_01_deal
函数描述：回复信息和状态
参数描述：
返回值  ：
********************************************************************************
*/
void cmd_01_deal(void)
{
	uint8_t i, *pt;

    if(mr_comm_data.rx[3] == 0x20)      //确认长度
    {
		i = 4;
		//模式、通道
        mr_comm_msg.cur_mode = mr_comm_data.rx[i++];
        mr_comm_msg.cur_chno = mr_comm_data.rx[i++];
        //东西经
        mr_comm_msg.e_or_w = mr_comm_data.rx[i++];
        //经度
        pt = (uint8_t *)&mr_comm_msg.longitude;
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        //南北纬
        mr_comm_msg.s_or_n = mr_comm_data.rx[i++];
        //纬度
        pt = (uint8_t *)&mr_comm_msg.latitude;
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        //海拔
        pt = (uint8_t *)&mr_comm_msg.altitude;
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];
        *pt++ = mr_comm_data.rx[i++];    
        //通道1、2可见可用星
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
        //状态
        mr_comm_msg.state1 = mr_comm_data.rx[i++];    
        mr_comm_msg.state2 = mr_comm_data.rx[i++];    
        if(mr_comm_msg.state2 & 0x80)
        	sys_fault_word |= 0x20;
        else
        	sys_fault_word &= ~0x20;
           
      	//得到北京时间为报警提取时间戳
        UTCToBeijing(mr_comm_msg.year, mr_comm_msg.month, mr_comm_msg.day, mr_comm_msg.hour, mr_comm_msg.min, mr_comm_msg.sec);
        //
        //每天0点把LCD屏上显示的通道、天线和电源的告警次数以及北斗/GPS最大最小星数清零
        if((!mr_comm_bt_time.hour) && (!mr_comm_bt_time.min) && (mr_comm_bt_time.sec < 	HOUR00_MIN00_SECX) && (!mcb_alm_0hour_clr))		//为了保险10s内均有效
        {
        	//通知报警模块处理
        	mcb_alm_0hour_clr = 1;	
        }
    }
}

/*
********************************************************************************
函数名称：cmd_02_deal
函数描述：确认主设备发送的命令请求
参数描述：
返回值  ：
********************************************************************************
*/
void cmd_02_deal(void)
{
    if(mr_comm_data.rx[3] == 0x02)
    {
    	if(mr_comm_data.rx[5] == COMM_OK)
    	{
  			;		//确认ok，无需处理
    	}	
    	else
    	if(mr_comm_data.rx[5] == COMM_FAIL)
    	{
    		switch(mr_comm_data.rx[4])			//向设置方（人机、wifi）反馈？
    		{
    			//进入、退出手动模式失败
    			case 0x00: 	break;
    			case 0x01: 	break;
    			case 0x02: 	break;
    			case 0x03: 	break;
    			case 0x04: 	break;
    			//进入、退出自动模式失败
    			case 0x10: 	break;	
    			case 0x11: 	break;		
    			//通道输出功率设置失败
    			case 0x21: 	break;	
    			case 0x22: 	break;	
    			//通道增益设置失败
     			case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;
    		}
    		//反馈操作失败标志;		
    	}
    }
    else
    if(mr_comm_data.rx[3] == 0x04)
    {
    	if((mr_comm_data.rx[5] == COMM_OK) && (mr_comm_data.rx[7] == COMM_OK))
    	{
  			;		//确认ok，无需处理
    	}	
    	else
    	if(mr_comm_data.rx[5] == COMM_FAIL)		//向设置方（人机、wifi）反馈？
    	{
    		switch(mr_comm_data.rx[4])	
    		{
     			//通道增益设置失败
     			case 0x41:	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;	    			
    		}
    	}
    	else
    	if(mr_comm_data.rx[7] == COMM_FAIL)		//向设置方（人机、wifi）反馈？
    	{
     		switch(mr_comm_data.rx[6])	
			{
				//通道增益设置失败
				case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;		
			}	
    	}
    	//反馈操作失败标志;	
    }
    else
    if(mr_comm_data.rx[3] == 0x06)
    {
    	if((mr_comm_data.rx[5] == COMM_OK) && (mr_comm_data.rx[7] == COMM_OK) && (mr_comm_data.rx[9] == COMM_OK))
    	{
  			;		//确认ok，无需处理
    	}	
    	else
    	if(mr_comm_data.rx[5] == COMM_FAIL)		//向设置方（人机、wifi）反馈？
    	{
     		switch(mr_comm_data.rx[4])	
     		{
     			//通道增益设置失败
     			case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;       			
     		}
    	}
    	else
    	if(mr_comm_data.rx[7] == COMM_FAIL)		//向设置方（人机、wifi）反馈？
    	{
     		switch(mr_comm_data.rx[6])	
     		{
     			//通道增益设置失败
     			case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;       			
     		}	
    	}
    	else
    	if(mr_comm_data.rx[9] == COMM_FAIL)		//向设置方（人机、wifi）反馈？
    	{
     		switch(mr_comm_data.rx[8])	
     		{
     			//通道增益设置失败
     			case 0x41: 	break;	
    			case 0x42: 	break;	  
    			case 0x43: 	break;	 			
    			default:break;       			
     		}	
    	}
    	//反馈操作失败标志;	
    }
}

/*
********************************************************************************
函数名称：cmd_03_deal
函数描述：返回3个衰减值
参数描述：
返回值  ：
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
函数名称：cmd_04_deal
函数描述：返回从设备的板类型
参数描述：
返回值  ：
********************************************************************************
*/
void cmd_04_deal(void)
{
    if(mr_comm_data.rx[3] == 0x01)
    {
    	//rfb_or_xyb = mr_comm_data.rx[4];		//射频板和协议板目前有硬件GPIO判断
    	if(rfb_or_xyb == mr_comm_data.rx[4])	//软件没有基本没有意义了
    		;
    }
}

/*
********************************************************************************
函数名称：cmd_05_deal
函数描述：返回从设备的可用星星号及其载噪比
参数描述：
返回值  ：
********************************************************************************
*/
void cmd_05_deal(void)
{
	uint8_t i , n;
	uint8_t *pt = 0;

    n = mr_comm_data.rx[3] - 2;			//可见星数量*2
    
    if(mr_comm_data.rx[4] == 0)			//GPS
    {
    	if(mr_comm_data.rx[5] == 1)		//通道1
    		pt = (uint8_t *)&mr_kyx_cn_array.CH1_GPS_KYX_CN[0][0];
    	else
    	if(mr_comm_data.rx[5] == 2)		//通道2
    		pt = (uint8_t *)&mr_kyx_cn_array.CH2_GPS_KYX_CN[0][0];
    }	
    else
	if(mr_comm_data.rx[4] == 1)			//BD
	{
    	if(mr_comm_data.rx[5] == 1)		//通道1
    		pt = (uint8_t *)&mr_kyx_cn_array.CH1_BDS_KYX_CN[0][0];
    	else
    	if(mr_comm_data.rx[5] == 2)		//通道2	
    		pt = (uint8_t *)&mr_kyx_cn_array.CH2_BDS_KYX_CN[0][0];	
	}
    
    if(pt != 0)
    {
		if(n <= (MR_KYX_MAX_NUM << 1))
        {
            for(i = 0; i < n; i++)
                *pt++ = mr_comm_data.rx[i + 6];	//获取可用星及载噪比
        }
		n = MR_KYX_MAX_NUM << 1;
		for(; i < n; i++)
			*pt++ = 0;					//不足12颗的空间清0
    }
}

/*
********************************************************************************
函数名称：cmd_7C_deal
函数描述：通知主设备远程升级成功
参数描述：
返回值  ：
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
		
		//通知wifi接口任务，进一步向上汇报升级状态
		mcb_wifi_sys_tx_cmd = WIFI_ZDTZSJCG_CMD_7C;
		wifi_comm_sys_tx_flag = 1;
		mcb_wifi_task_flag = 1;
    }  
    else
   		cmd_80_deal(mr_comm_data.rx[4] | 0x80, 0);        
}

/*
********************************************************************************
函数名称：cmd_7D_deal
函数描述：从设备进入远程升级模式确认帧
参数描述：
返回值  ：
********************************************************************************
*/
void cmd_7D_deal(void)
{
    if(mr_comm_data.rx[3] == 0x01)
    {
		if(mr_comm_data.rx[4] == rfb_xybm_xybf_sel)
		{
			//通知wifi模块，成功进入
			//进入后，此串口波特率需要升高处理？
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
函数名称：cmd_7E_deal
函数描述：通知主设备退出远程升级模式
参数描述：
返回值  ：
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
		
		//通知wifi接口任务，进一步向上汇报退出升级模式
		mcb_wifi_sys_tx_cmd = WIFI_ZDTZTCYCSJ_CMD_7E;
		wifi_comm_sys_tx_flag = 1;
		mcb_wifi_task_flag = 1;
		
		//退出后，波特率需要降下来？
		
    }  
    else
    	cmd_80_deal(mr_comm_data.rx[4], 0);            
}

/*
********************************************************************************
函数名称：cmd_7F_deal
函数描述：返回从设备文件名
参数描述：
返回值  ：
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
函数名称：UART6_init
函数描述：UART6初始化
参数描述：
返回值  ：
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
函数名称：UTCToBeijing
函数描述：UTC时间转换为北京时间
参数描述：
返回值  ：
********************************************************************************
*/
void UTCToBeijing(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
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
	mr_comm_bt_time.year = year + 2000;
	mr_comm_bt_time.month = month; 
	mr_comm_bt_time.day = day; 
	mr_comm_bt_time.hour = hour; 
	mr_comm_bt_time.min = minute; 
	mr_comm_bt_time.sec = second; 
}
