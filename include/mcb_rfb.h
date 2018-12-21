/*
********************************************************************************
文件名	：mcb_rfb.h
文件描述：主控板与射频板通信头文件，射频板和协议板按兼容软件处理，有区别的地方区分开
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __MCB_RFB_H
#define __MCB_RFB_H

#ifdef __MCB_RFB_GLOBAL
	#define __MCB_RFB_EXE 
#else
	#define __MCB_RFB_EXE extern
#endif

#ifdef __cplusplus
	extern "C" {
#endif 

/*
********************************************************************************
头文件
********************************************************************************
*/
#include "stm32f0xx.h"

/*
********************************************************************************
宏定义
********************************************************************************
*/
#define MCB_RFB_NW_BR		57600		//正常工作波特率
#define MCB_RFB_RU_BR		1000000		//远程升级波特率
#define MR_COMM_RX_BUF_LEN	235			//数量可根据协议最长帧优化，RAM紧张时
#define MR_COMM_TX_BUF_LEN	235			//数量可根据协议最长帧优化，RAM紧张时
#define MR_BUF_MAX_LEN		230			//数据域最大长度230 

#define MCB_RFB_POLL_TIME	100			//100*10ms=1000ms
#define	HOUR00_MIN00_SECX	10			//每天0点10s内判断有效

#define COMM_SYN_CHAR_1		0xAA		//同步符12定义
#define COMM_SYN_CHAR_2		0x55
#define COMM_OK				0xA5
#define COMM_FAIL			0x5A

#define MR_KYX_MAX_NUM		12			//可用星最大数量

#define	RECV_TIME_OUT		0x01		//接收超时位定义
#define	RECV_CHKSUM_ERR		0x02		//校验和错误位定义
#define RECV_TIME_OUT_CNT	50			//50*10ms=500ms
#define RECV_TO_ALLOW_NUM	3			//接收超时允许重发次数
#define RF_LINK_FAULT_NUM	9			//链路故障判断次数

//接收命令定义
#define HFBXX_CMD_01		0x01		//回复射频板信息及状态
#define HFQR_CMD_02			0x02		//回复确认主设备的命令
#define HFSGSJ_CMD_03		0x03		//回复三个衰减值
#define HFBLX_CMD_04		0x04		//回复板类型
#define HFKYXZZB_CMD_05		0x05		//回复可用星及其载噪比
#define ZDTZSJCG_CMD_7C		0x7C		//主动通知升级成功
#define HFJRSJQR_CMD_7D		0x7D		//回复进入升级确认
#define ZDTZTCYCSJ_CMD_7E	0x7E		//主动通知退出远程升级
#define HFSBWJM_CMD_7F		0x7F		//回复设备文件名
//发送命令定义
#define QRSBFSSS_CMD_80		0x80		//确认设备发送的数据命令
#define QQJRTCSDMS_CMD_81	0x81		//请求进入退出手动模式
#define QQJRTCZDMS_CMD_82	0x82		//请求进入退出自动模式
#define QQSBSCGLSZ_CMD_83	0x83		//请求设备输出功率设制
#define QQDQSBSSJZ_CMD_84	0x84		//请求读取设备三个衰减值
#define QQSZSBSSJZ_CMD_85	0x85		//请求设置设备三个衰减值
#define QQDQSBBLX_CMD_86	0x86		//请求读取设备板类型
#define QQKYXZZB_CMD_87		0x87		//请求可用星及其载噪比
#define QQCXBXXZT_CMD_88	0x88		//请求查询板信息状态
#define QQDQSBWJM_CMD_FE	0xFE		//请求读取设备文件名
#define QQJRYCSJ_CMD_FF		0xFF		//请求进入远程升级

//通信结构体定义
typedef struct
{
	uint8_t	rx_cnt;
	uint8_t	rx_len;
	uint8_t	tx_cnt;
	uint8_t	tx_len;	
	uint8_t	rx[MR_COMM_RX_BUF_LEN];
	uint8_t	tx[MR_COMM_TX_BUF_LEN];
	uint8_t comm_state;
}MR_COMM_TYPE_DEF;
//信息状态结构体定义
typedef struct
{
	uint8_t	cur_mode;		//当前模式 0-正常 1-手动 2-自动 4-通道自检
	uint8_t	cur_chno;		//当前通道 0-1通道 1-2通道 2-旁路1 3-旁路2
	char	e_or_w;			//东西经
	char	s_or_n;			//南北纬
	float	longitude;		//经度
	float	latitude;		//纬度
	float	altitude;		//海拔
	uint8_t	ch1_gps_kjx;	//通道1 GPS可见星
	uint8_t	ch1_bd_kjx;		//通道1 BD可见星
	uint8_t	ch1_gps_kyx;	//通道1 GPS可用星
	uint8_t	ch1_bd_kyx;		//通道1 BD可用星
	uint8_t	ch2_gps_kjx;	//同上...
	uint8_t	ch2_bd_kjx;
	uint8_t	ch2_gps_kyx;
	uint8_t	ch2_bd_kyx;
	uint8_t	year;			//utc date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;			//utc time...
	uint8_t	min;
	uint8_t	sec;	
	//信息及状态：
	uint8_t	state1;			
	//bit7~bit6 天线1    bit5~bit4 天线2   	bit3 通道1    bit2 通道2	bit2 干接点1 	bit1 干接点2
	//00：ok   01:未接   00：ok   01:未接   0：ok         0：ok         0：ok       	0：ok     
	//10：告警 11:保留   10：告警 11:保留   1：告警       1：告警       1：故障     	1：故障  
	//设备状态信息：
	uint8_t	state2;			
	//bit7 板上GPS模块通信不上	bi6~bit0 保留
	//0:ok		
	//1:故障
}MR_COMM_MSG_DEF;
//北京时间
typedef struct
{
	uint16_t year;			//BEIJING date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;			//BEIJING time...
	uint8_t	min;
	uint8_t	sec;	
}MR_COMM_BT_TIME_DEF;
//可用星星号及其载噪比
typedef struct
{
	uint8_t CH1_GPS_KYX_CN[MR_KYX_MAX_NUM][2];		//[X][0]: 可用星星号 ，[X][1]: 可用星载噪比
	uint8_t CH1_BDS_KYX_CN[MR_KYX_MAX_NUM][2];		//同上...
	uint8_t CH2_GPS_KYX_CN[MR_KYX_MAX_NUM][2];
	uint8_t CH2_BDS_KYX_CN[MR_KYX_MAX_NUM][2];
}MR_COMM_KYX_CN_DEF;

/*
********************************************************************************
公共变量定义
********************************************************************************
*/
__MCB_RFB_EXE uint32_t mcb_rfb_task_flag;		//主控板与射频板通信任务标志

__MCB_RFB_EXE MR_COMM_MSG_DEF mr_comm_msg;		//信息及状态结构体变量
__MCB_RFB_EXE MR_COMM_BT_TIME_DEF mr_comm_bt_time;	//北京时间结构体变量

__MCB_RFB_EXE uint8_t mr_gps_or_bd;				//读取GPS还是bd卫星可用星号及载噪比变量 0 - gps，1 - bd
__MCB_RFB_EXE uint8_t mr_channel_no;			//读取射频板哪个通道的卫星可用星号及载噪比变量 1 - 通道1， 2 - 通道2
__MCB_RFB_EXE MR_COMM_KYX_CN_DEF mr_kyx_cn_array;	//可用星星号及其载噪比

__MCB_RFB_EXE uint8_t mr_ch_att_sel;			//设置通道选择, 			bit7~bit3，	bit2 	 		bit1  			bit0
												//为1表示设置此通道的衰减值	保留		1：公共通道		1：通道2		1：通道1
__MCB_RFB_EXE uint8_t mr_ch1_att;				//通道1衰减值0~30dB
__MCB_RFB_EXE uint8_t mr_ch2_att;				//通道2衰减值0~30dB
__MCB_RFB_EXE uint8_t mr_com_att;				//公共通道衰减值0~60dB

__MCB_RFB_EXE uint8_t  mr_in_out_auto_mode;		//进入退出自动模式 0-退出，1-进入自动模式
__MCB_RFB_EXE uint8_t  mr_in_out_manu_mode;		//进入退出手动模式 0-退出，1-通道1进入手动模式，2-通道2进入手动模式，3-旁路1进入手动模式，4-旁路2进入手动模式
#ifdef __MCB_RFB_GLOBAL
	char rfb_xybm_file[13] = "RFB_1.0_1.0";		//射频板或协议板mcu文件名
	char rfb_xybf_file[13] = "XYBF_1.0_1.0";	//协议板fpga文件名
#else
	extern char rfb_xybm_file[13];
	extern char rfb_xybf_file[13];
	
#endif
__MCB_RFB_EXE uint8_t rfb_xybm_xybf_sel;		//1-射频板，2-协议板MCU, 3-协议板FPGA
__MCB_RFB_EXE uint8_t rfb_xyb_in_out_ru_flag;	//进入、退出远程升级标志：0x01-射频板，0x02-协议板MCU, 0x04-协议板FPGA
__MCB_RFB_EXE uint8_t rfb_xyb_ru_succ_flag;		//远程升级成功标志：0x01-射频板，0x02-协议板MCU, 0x04-协议板FPGA
/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void mcb_rfb_init(void);						//主控板与射频板通信初始化
void mcb_rfb_task(void);						//主控板与射频板通信任务

void UTCToBeijing(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second); //GPS UTC时间转北京时间
/*
********************************************************************************
私有变量定义
********************************************************************************
*/
__MCB_RFB_EXE MR_COMM_TYPE_DEF mr_comm_data;	//通信结构体变量
__MCB_RFB_EXE uint8_t mr_comm_recv_end_flag;	//接收完成标志
__MCB_RFB_EXE uint8_t mr_comm_link_tx_flag;		//链路发送标志
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_flag;		//系统发送标志
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_to_flag;	//系统发送超时标志
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_cmd;		//系统发送命令公共变量
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_cmd1;		//系统发送读取设备信息轮询命令
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_cmd2;		//系统发送其他设置、读取命令
__MCB_RFB_EXE uint8_t mr_comm_data_deal_flag;	//数据处理中标志
__MCB_RFB_EXE uint8_t mr_comm_time_out_cnt;		//系统发送超时计数器
__MCB_RFB_EXE uint8_t mr_comm_time_out_en;		//系统发送超时计允许标志
__MCB_RFB_EXE uint8_t mr_comm_send_recv_end;	//一次发收事务结束标志，允许其他帧发送
__MCB_RFB_EXE uint8_t mr_comm_sys_tx_end;		//链路发送完成
__MCB_RFB_EXE uint8_t mr_to_cnt1;				//超时计数器1
__MCB_RFB_EXE uint8_t mr_to_cnt2;				//超时计数器2

/*
********************************************************************************
私有函数定义
********************************************************************************
*/
void UART6_init(uint32_t br);					//串口6初始化函数
void mr_comm_data_deal(void);					//数据解析处理函数
void mr_comm_data_send(void);					//链路发送数据函数
void mr_comm_sys_send(uint8_t cmd);				//系统发送数据函数

//下行
void cmd_88_deal(void);							//下行命令组帧函数
void cmd_87_deal(void);							//同上...
void cmd_86_deal(void);
void cmd_85_deal(void);
void cmd_84_deal(void);
void cmd_83_deal(void);
void cmd_82_deal(void);
void cmd_81_deal(void);   
void cmd_80_deal(uint8_t type, uint8_t flag);
void cmd_FE_deal(void);
void cmd_FF_deal(void);

//上行
void cmd_01_deal(void);							//上行命令解帧函数
void cmd_02_deal(void);							//同上...
void cmd_03_deal(void);
void cmd_04_deal(void);
void cmd_05_deal(void);
void cmd_7C_deal(void);
void cmd_7D_deal(void);
void cmd_7E_deal(void);
void cmd_7F_deal(void);


#ifdef __cplusplus
	}
#endif

#endif
