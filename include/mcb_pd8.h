/*
********************************************************************************
文件名	：mcb_pd8.h
文件描述：主控板与八功分板通信头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __MCB_PD8_H
#define __MCB_PD8_H

#ifdef __MCB_PD8_GLOBAL
	#define __MCB_PD8_EXE 
#else
	#define __MCB_PD8_EXE extern
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
#define MCB_PD8_NW_BR			115200		//正常工作波特率
#define MCB_PD8_RU_BR			1000000		//远程升级波特率
#define MP_COMM_RX_BUF_LEN		235			//数量可根据协议最长帧优化，RAM紧张时
#define MP_COMM_TX_BUF_LEN		235			//数量可根据协议最长帧优化，RAM紧张时
#define MP_BUF_MAX_LEN			230			//数据域最大长度230

#define MCB_MP_POLL_TIME		110			//110*10ms=1100ms

#define MP_COMM_SYN_CHAR_1		0xAA		//同步符12定义
#define MP_COMM_SYN_CHAR_2		0x55
#define MP_COMM_OK				0xA5
#define MP_COMM_FAIL			0x5A

#define MP_KYX_MAX_NUM			12			//可用星最大数量
#define MP_SPD8_MAX_NUM			8			//一个主机最多带8个从机

#define	MP_RECV_TIME_OUT		0x01		//接收超时位定义
#define	MP_RECV_CHKSUM_ERR		0x02		//校验和错误位定义
#define MP_RECV_TIME_OUT_CNT	50			//50*10ms=500ms
#define MP_RECV_TO_ALLOW_NUM	3			//接收超时允许重发次数
#define PD8_LINK_FAULT_NUM		9			//链路故障判断次数

//接收命令定义
#define MP_HFZCJBXX_CMD_01		0x01		//回复主、从机板信息及状态
#define MP_HFQR_CMD_02			0x02		//回复确认主设备的命令
#define MP_HFLGSJ_CMD_03		0x03		//回复2个衰减值
#define MP_HFKYXZZB_CMD_05		0x05		//回复可用星及其载噪比
#define MP_ZDTZSJCG_CMD_7C		0x7C		//主动通知升级成功
#define MP_HFJRSJQR_CMD_7D		0x7D		//回复进入升级确认
#define MP_ZDTZTCYCSJ_CMD_7E	0x7E		//主动通知退出远程升级
#define MP_HFSBWJM_CMD_7F		0x7F		//回复设备文件名
//发送命令定义                          	
#define MP_QRSBFSSS_CMD_80		0x80		//确认设备发送的数据命令
#define MP_QQSBSCGLSZ_CMD_83	0x83		//请求设备输出功率设制
#define MP_QQDQSBLSJZ_CMD_84	0x84		//请求读取设备2个衰减值
#define MP_QQSZSBLSJZ_CMD_85	0x85		//请求设置设备2个衰减值
#define MP_QQKYXZZB_CMD_87		0x87		//请求可用星及其载噪比
#define MP_QQCXBXXZT_CMD_88		0x88		//请求查询板信息状态
#define MP_QQDQSBWJM_CMD_FE		0xFE		//请求读取设备文件名
#define MP_QQJRYCSJ_CMD_FF		0xFF		//请求进入远程升级

//通信结构体定义
typedef struct
{
	uint8_t	rx_cnt;
	uint8_t	rx_len;
	uint8_t	tx_cnt;
	uint8_t	tx_len;	
	uint8_t	rx[MP_COMM_RX_BUF_LEN];
	uint8_t	tx[MP_COMM_TX_BUF_LEN];
	uint8_t comm_state;
}MP_COMM_TYPE_DEF;
//主机八功分板信息状态结构体定义
typedef struct
{
	uint8_t	mpd8_gps_kjx;		//GPS可见星
	uint8_t	mpd8_bd_kjx;		//BD可见星
	uint8_t	mpd8_gps_kyx;		//GPS可用星
	uint8_t	mpd8_bd_kyx;		//BD可用星
	uint8_t	mpd8_port_state1;	//端口状态1, port8~port5，两位一个状态，00：悬空，01：从机，10：BBU，11：保留
	uint8_t	mpd8_port_state2;	//端口状态2, port4~port1，两位一个状态，00：悬空，01：从机，10：BBU，11：保留
	uint8_t	year;				//utc date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;				//utc time...
	uint8_t	min;
	uint8_t	sec;	
	//信息及状态：
	uint8_t	mpd8_pwr_state;		//主从电源状态，bit7~bit4：保留，bit3 		bit2 		bit1 			bit0
								//0：正常，1：告警				 pwr1状态	pwr2状态	pwr1干接点3    	pwr2干接点4
	//设备状态信息：
	uint8_t	mpd8_out_state;		//主机八功分板输出状态，port8 ~ port1, bit7 ~ bit0, 0：正常，1：告警	
	uint8_t	mpd8_alm_state;		//主机八功分板干节点1告警状态，port8 ~ port1 ： bit7 ~ bit0	0：正常，1：干接点1告警	
	uint8_t	mpd8_dev_state;		//bit7 板上GPS模块通信不上 0：OK, 1：故障；bi6~bit0 保留
}MPD8_COMM_MSG_DEF;
//从机八功分板信息状态结构体定义
typedef struct
{
	uint8_t	spd8_addr;			//从机地址
	uint8_t	spd8_gps_kjx;		//GPS可见星
	uint8_t	spd8_bd_kjx;		//BD可见星
	uint8_t	spd8_gps_kyx;		//GPS可用星
	uint8_t	spd8_bd_kyx;		//BD可用星
	uint8_t	spd8_port_state;	//端口状态, port8~port1，bit7~bit0，0：悬空，1：BBU
	uint8_t	year;				//utc date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;				//utc time...
	uint8_t	min;
	uint8_t	sec;	
	//设备状态信息：
	uint8_t	spd8_out_state;		//从机八功分板输出状态，port8 ~ port1, bit7 ~ bit0,	 0：正常，1：告警	
	uint8_t	spd8_alm_state;		//从机八功分板干节点1告警状态，port8 ~ port1, bit7 ~ bit0, 0：正常，1：干接点1告警	
	uint8_t	spd8_dev_state;		//bit7 板上GPS模块通信不上 0：OK, 1：故障；bi6~bit0 保留
}SPD8_COMM_MSG_DEF;
//北京时间
typedef struct
{
	uint16_t year;				//BEIJING date...
	uint8_t	month;
	uint8_t	day;
	uint8_t	hour;				//BEIJING time...
	uint8_t	min;
	uint8_t	sec;	
}MP_COMM_BT_TIME_DEF;
//可用星星号及其载噪比
typedef struct
{
	uint8_t GPS_KYX_CN[MP_KYX_MAX_NUM][2];		//[X][0]: 可用星星号 ，[X][1]: 可用星载噪比
	uint8_t BDS_KYX_CN[MP_KYX_MAX_NUM][2];		//同上...
}MP_COMM_KYX_CN_DEF;

/*
********************************************************************************
公共变量定义
********************************************************************************
*/
__MCB_PD8_EXE uint32_t mcb_pd8_task_flag;			//主控板与八功分板通信任务标志
__MCB_PD8_EXE MPD8_COMM_MSG_DEF mp_mpd8_comm_msg;	//主机信息及状态结构体变量
__MCB_PD8_EXE MP_COMM_BT_TIME_DEF mp_comm_bt_time;	//北京时间结构体变量

__MCB_PD8_EXE uint8_t mcb_cur_rw_slave_index;		//当前读写的从机索引号 = （从机地址 - 1），从机地址必须在1~8之间
__MCB_PD8_EXE uint8_t mcb_cur_valid_slave_addr[MP_SPD8_MAX_NUM];	//当前有效从机地址列表，过程需要动态检查更新，便于快速查找调用信息，和从机数量配合使用
__MCB_PD8_EXE uint8_t mcb_cur_pd8_slave_num;		//当前统计的从机八功分板数量0~8，只是个数量，不参与实际操作，//实际操作总是在8个从机地址数组中查询有效地址
//注意第N个从机的信息就对应以下从机相关数组的元素[N-1]，有效从机地址1~8，0无效，只要知道从机号N就可以在以下数组里直接取出相应信息
//比如主机的第2个和第5个口分别接了2个从机，那么主机分配给两个从机的地址分别是2,5，从机的信息就分别存储在以下数组变量的[2-1]和[5-1]元素里
//其他不用的数组变量无效，增加一个从机，一组数据变量有效，去掉一个从机，相应的数据变量无效
//主要是从机数量变量值的增减和从机地址变量值的变化（要么从机地址，要么0无效地址），实际程序需要查询 从机八功分板地址数组[] 才知道哪些有效,这是通用做法
__MCB_PD8_EXE uint8_t mcb_pd8_slave_addr[MP_SPD8_MAX_NUM];	//从机八功分板地址数组，有效从机地址1~8，0无效地址
__MCB_PD8_EXE SPD8_COMM_MSG_DEF mp_spd8_comm_msg[MP_SPD8_MAX_NUM];	//从机信息及状态结构体数组变量，[0]~[7]八个从机信息对应从机地址1~8
__MCB_PD8_EXE MP_COMM_KYX_CN_DEF spd8_kyx_cn_array[MP_SPD8_MAX_NUM];//从机可用星星号及其载噪比结构体数组，[0]~[7]八个从机可用星及其载噪比对应从机地址1~8
#ifdef __MCB_PD8_GLOBAL
	char spd8_file[MP_SPD8_MAX_NUM][13] =			//从机八功分板文件名
	{
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
		 "PD8S_1.0_1.0",
	};			
#else
	extern char spd8_file[MP_SPD8_MAX_NUM][13];
#endif

#ifdef __MCB_PD8_GLOBAL
	char mpd8_file[13] = "PD8M_1.0_1.0";			//主机八功分板文件名
#else
	extern char mpd8_file[13];
#endif

__MCB_PD8_EXE uint8_t mp_mpd8_or_spd8;				//读取主机还是从机，0-主机，1-从机
__MCB_PD8_EXE uint8_t mp_gps_or_bd;					//读取GPS还是bd卫星可用星号及载噪比变量 0 - gps，1 - bd
__MCB_PD8_EXE MP_COMM_KYX_CN_DEF mpd8_kyx_cn_array;	//主机可用星星号及其载噪比结构体

__MCB_PD8_EXE uint8_t mp_ch_att_sel;				//设置通道选择, 			bit7~bit2，		bit1  			bit0
													//为1表示设置此通道的衰减值	保留	   		1：检测通道		1：输出通道
__MCB_PD8_EXE uint8_t mp_det_att;					//检测通道衰减值0~30dB
__MCB_PD8_EXE uint8_t mp_out_att;					//输出通道衰减值0~30dB

__MCB_PD8_EXE uint8_t mp_pd8_sel;					//4-主机八功分板，5-从机八功分板, 6-主、从机八功分板
__MCB_PD8_EXE uint8_t mp_pd8_in_out_ru_flag;		//进入、0退出远程升级标志：0x01-主机八功分板，0x02-从机八功分板, 0x04-主、从机八功分板
__MCB_PD8_EXE uint8_t mp_pd8_ru_succ_flag;			//远程升级成功标志：0x01-主机八功分板，0x02-从机八功分板, 0x04-主、从机八功分板

/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void mcb_pd8_init(void);							//主控板与八功分板通信初始化
void mcb_pd8_task(void);							//主控板与八功分板通信任务

/*
********************************************************************************
私有变量定义
********************************************************************************
*/
__MCB_PD8_EXE MP_COMM_TYPE_DEF mp_comm_data;		//通信结构体变量
__MCB_PD8_EXE uint8_t mp_comm_recv_end_flag;		//接收完成标志
__MCB_PD8_EXE uint8_t mp_comm_link_tx_flag;			//链路发送标志
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_flag;			//系统发送标志
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_to_flag;		//系统发送超时标志
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_cmd;			//系统发送命令公共变量
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_cmd1;			//系统发送读取设备信息轮询命令
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_cmd2;			//系统发送其他设置、读取命令
__MCB_PD8_EXE uint8_t mp_comm_data_deal_flag;		//数据处理中标志
__MCB_PD8_EXE uint8_t mp_comm_time_out_cnt;			//系统发送超时计数器
__MCB_PD8_EXE uint8_t mp_comm_time_out_en;			//系统发送超时计允许标志
__MCB_PD8_EXE uint8_t mp_comm_send_recv_end;		//一次发收事务结束标志，允许其他帧发送
__MCB_PD8_EXE uint8_t mp_comm_sys_tx_end;			//链路发送完成
__MCB_PD8_EXE uint8_t mp_to_cnt1;					//超时计数器1
__MCB_PD8_EXE uint8_t mp_to_cnt2;					//超时计数器2

/*
********************************************************************************
私有函数定义
********************************************************************************
*/
void UART5_init(uint32_t br);						//串口5初始化函数
void mp_comm_data_deal(void);						//数据解析处理函数
void mp_comm_data_send(void);						//发送数据函数
void mp_comm_sys_send(uint8_t cmd);					//系统发送数据函数

//下行
void mp_cmd_88_deal(void);							//下行命令组帧函数
void mp_cmd_87_deal(void);							//同上...
void mp_cmd_85_deal(void);
void mp_cmd_84_deal(void);
void mp_cmd_83_deal(void);
void mp_cmd_80_deal(uint8_t type, uint8_t flag);
void mp_cmd_FE_deal(void);
void mp_cmd_FF_deal(void);

//上行
void mp_cmd_01_deal(void);							//上行命令解帧函数
void mp_cmd_02_deal(void);							//同上...
void mp_cmd_03_deal(void);
void mp_cmd_05_deal(void);
void mp_cmd_7C_deal(void);
void mp_cmd_7D_deal(void);
void mp_cmd_7E_deal(void);
void mp_cmd_7F_deal(void);

void MP_UTCToBeijing(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

#ifdef __cplusplus
	}
#endif

#endif
