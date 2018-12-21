/*
********************************************************************************
文件名	：mcb_wifi.h
文件描述：主控板与WIFI模块通信头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __MCB_WIFI_H
#define __MCB_WIFI_H

#ifdef __MCB_WIFI_GLOBAL
	#define __MCB_WIFI_EXE 
#else
	#define __MCB_WIFI_EXE extern
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
#define MCB_WIFI_NW_BR				57600		//正常工作波特率
#define MCB_WIFI_RU_BR				1000000		//远程升级波特率

#define WIFI_COMM_RX_BUF_LEN		235			//数量可根据协议最长帧优化，RAM紧张时
#define WIFI_COMM_TX_BUF_LEN		235			//数量可根据协议最长帧优化，RAM紧张时
#define WIFI_BUF_MAX_LEN			230			//数据域最大长度230

#define WIFI_COMM_SYN_CHAR_1		0xAA		//同步符12定义
#define WIFI_COMM_SYN_CHAR_2		0x55
#define WIFI_COMM_OK				0xA5
#define WIFI_COMM_FAIL				0x5A

//ascii信息数据协议
//wifi - mcu 查询命令的下行和上行：
//GET+空格+查询字段数+空格+查询字段的别名[+空格+字段别名+…]+*
//GET+空格+查询字段数+空格+查询字段的别名+空格+查询字段返回值[+空格+字段别名+空格+查询字段返回值…]+*
//wifi - mcu 设置命令的下行和上行：
//SET+空格+设置字段数+空格+设置字段的别名+空格+设置字段值[+空格+字段别名+设置字段值…]+*
//SET+空格+设置字段数+空格+设置成败的标志(OK/FAIL)+*
//mcu - wifi mcu主动上报告警信息到wifi的上行：
//WARN+空格+告警类型别名+空格+发生/消失标志+空格+时间+空格+数值+*
#define WIFI_MCU_GET_STR			"GET"		//同步符GET字符串定义
#define WIFI_MCU_SET_STR			"SET"		//同步符SET字符串定义
#define WIFI_MCU_SPACE_CHAR			' '			//空格字符定义
#define WIFI_MCU_STAR_CHAR			'*'			//星号字符定义
#define WIFI_MCU_POUND_CHAR			'#'			//井号字符定义
//char *GET_CMD_STR[GET_CMD_NUM] = {"LGTD","LTTD","STCT","MNGN","IPSN","IPCN","IPTP","IPBP","FDDV","L1OP","L2OP",};   	
//char *SET_CMD_STR[SET_CMD_NUM] = {"ATTE","MNGN","IPCN","IPTP","IPBP","FDDV","R1GN","R2GN","RDGN","PMGN","PSGN","YZMD",};   		
//char *WARN_DEF_STR[WARN_DEF_NUM] = {"MATA","SATA","MAPW","SAPW","PTBD","OPPT","CTBD",};   
//GET CMD
#define GET_CMD_NUM					16
#define GET_LGTD					1			//经度
#define GET_LTTD					2			//纬度
#define GET_STCT					3			//星数
#define GET_MNGN					4			//主增益
#define GET_IPSN					5			//输入信号?载噪比avg
#define GET_IPCN					6			//输入通道
#define GET_IPTP					7			//输入类型0-正常,1-手动,2-自动,4-通道自检
#define GET_IPBP					8			//输入旁路
#define GET_FDDV					9			//发现设备
#define GET_L1OP					10			//主机八功分板输出
#define GET_L2OP					11			//从机八功分板输出
#define GET_R1GN					12			//射频板通道1增益-衰减      											
#define GET_R2GN					13			//射频板通道2增益-衰减       											
#define GET_RDGN					14			//射频板检测通道增益-衰减      											
#define GET_PMGN					15			//主机八功分板输出增益-衰减，检测通道无增益-衰减									
#define GET_PSGN					16			//从机八功分板输出增益-衰减，检测通道无增益-衰减		
//SET CMD
#define SET_CMD_NUM					12
#define SET_ATTE					50			//衰减            											
#define SET_MNGN					51			//主增益           											
#define SET_IPCN					52			//输入通道            											
#define SET_IPTP					53			//输入自动模式        											
#define SET_IPBP					54			//输入旁路 											
#define SET_FDDV					55			//发现设备        											
#define SET_R1GN					56			//射频板通道1增益-衰减      											
#define SET_R2GN					57			//射频板通道2增益-衰减       											
#define SET_RDGN					58			//射频板检测通道增益-衰减      											
#define SET_PMGN					59			//主机八功分板增益-衰减											
#define SET_PSGN					60			//从机八功分板增益-衰减											
#define SET_YZMD					61	        //样机演示模式
//WARN
//#define WARN_DEF_NUM				7
//#define WARN_MATA					100			//主天线          
//#define WARN_SATA					101         //从天线        
//#define WARN_MAPW					102         //主电源           
//#define WARN_SAPW					103         //从电源         
//#define WARN_PTBD					104         //协议板
//#define WARN_OPPT					105         //输出端口    
//#define WARN_CTBD					106         //控制板      


#define	WIFI_RECV_TIME_OUT			0x01		//接收超时位定义
#define	WIFI_RECV_CHKSUM_ERR		0x02		//校验和错误位定义
#define WIFI_RECV_TIME_OUT_CNT		50			//50*10ms=500ms，请求WIFI回复超时时间，二进制协议用
#define WIFI_BD_RECV_TO_CNT			20			//30*10=300ms，被动接收WIFI数据超时时间，ASCII码和二进制都用
#define WIFI_RECV_TO_ALLOW_NUM		3			//接收超时允许重发次数

//接收命令定义
#define WIFI_HFSPD8SLDZ_CMD_7A		0x7A		//回复从机八功分板的数量及其地址
#define WIFI_HFRFBHXYB_CMD_7B		0x7B		//回复是射频板还是协议板
#define WIFI_ZDTZSJCG_CMD_7C		0x7C		//主动通知升级成功
#define WIFI_HFJRSJQR_CMD_7D		0x7D		//回复进入升级确认
#define WIFI_ZDTZTCYCSJ_CMD_7E		0x7E		//主动通知退出远程升级
#define WIFI_HFSBWJM_CMD_7F			0x7F		//回复设备文件名
//发送命令定义                          	
#define WIFI_QRSBFSSS_CMD_80		0x80		//确认设备发送的数据命令
#define WIFI_QQSPD8SLDZ_CMD_FC		0xFC		//请求读取从机八功分板的数量及其地址
#define WIFI_QQRFBHXYB_CMD_FD		0xFD		//请求读取是射频板还是协议板
#define WIFI_QQDQSBWJM_CMD_FE		0xFE		//请求读取设备文件名
#define WIFI_QQJRYCSJ_CMD_FF		0xFF		//请求进入远程升级

//通信结构体定义
typedef struct
{
	uint8_t	rx_cnt;
	uint8_t	rx_len;
	uint8_t	tx_cnt;
	uint8_t	tx_len;	
	uint8_t	rx[WIFI_COMM_RX_BUF_LEN];
	uint8_t	tx[WIFI_COMM_TX_BUF_LEN];
	uint8_t comm_state;
}WIFI_COMM_TYPE_DEF;


/*
********************************************************************************
公共变量定义
********************************************************************************
*/
__MCB_WIFI_EXE uint32_t mcb_wifi_task_flag;		//主控板与WIFI模块通信任务标志
__MCB_WIFI_EXE uint32_t mcb_wifi_ru_state;		//远程升级状态，=0不在升级中, bit6~bit0: 1：xxb升级中
						//0-不在升级状态，bit7-resv=0, bit6-mcb，bit5-pd8ms，bit4-pd8s，bit3-pd8m，bit2-xybf，bit1-xybm，bit0-rfb
__MCB_WIFI_EXE uint32_t mcb_wifi_ru_xxb;		//升级的板 bit7-resv=0, bit6-mcb，bit5-pd8ms，bit4-pd8s，bit3-pd8m，bit2-xybf，bit1-xybm，bit0-rfb
__MCB_WIFI_EXE uint32_t mcb_wifi_sys_tx_cmd;	//升级的上报命令 7C / 7E	

/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void mcb_wifi_init(void);						//主控板与WIFI模块通信初始化
void mcb_wifi_task(void);						//主控板与WIFI模块通信任务

/*
********************************************************************************
私有变量定义
********************************************************************************
*/
__MCB_WIFI_EXE WIFI_COMM_TYPE_DEF wifi_comm_ru_data;	//远程升级通信结构体变量
__MCB_WIFI_EXE WIFI_COMM_TYPE_DEF wifi_comm_info_data;	//信息数据通信结构体变量
__MCB_WIFI_EXE WIFI_COMM_TYPE_DEF wifi_comm_alm_data;	//报警数据通信结构体变量
__MCB_WIFI_EXE uint8_t wifi_comm_recv_end_flag;			//接收完成标志
__MCB_WIFI_EXE uint8_t wifi_comm_link_tx_flag;			//链路发送标志
__MCB_WIFI_EXE uint8_t wifi_comm_sys_tx_flag;			//系统发送标志
__MCB_WIFI_EXE uint8_t wifi_comm_sys_tx_to_flag;		//系统发送超时标志
__MCB_WIFI_EXE uint8_t wifi_comm_data_deal_flag;		//数据处理中标志
__MCB_WIFI_EXE uint8_t wifi_comm_time_out_cnt;			//系统发送超时计数器
__MCB_WIFI_EXE uint8_t wifi_comm_time_out_en;			//系统发送超时计允许标志
__MCB_WIFI_EXE uint8_t wifi_comm_send_recv_end;			//一次发收事务结束标志，允许其他帧发送
__MCB_WIFI_EXE uint8_t wifi_comm_sys_tx_end;			//信息数据发送标志
__MCB_WIFI_EXE uint8_t wifi_to_cnt1;                    //

__MCB_WIFI_EXE uint8_t mcb_wifi_recv_cp_cnt;			//通信协议判断计数器
__MCB_WIFI_EXE uint8_t mcb_wifi_recv_cp_arr[3];			//通信协议判断接收缓冲区
__MCB_WIFI_EXE uint8_t mcb_wifi_recv_cp_type;			//接收通信协议类型 1：ASCII信息数据协议，2：BINARY升级数据协议
__MCB_WIFI_EXE uint8_t mcb_wifi_ru_deal_flag;			//升级数据处理标志
__MCB_WIFI_EXE uint8_t mcb_wifi_info_deal_flag;			//信息数据处理标志
__MCB_WIFI_EXE uint8_t mcb_wifi_ru_send_flag;			//升级数据发送标志
__MCB_WIFI_EXE uint8_t mcb_wifi_info_send_flag;			//信息数据发送标志
__MCB_WIFI_EXE uint8_t mcb_wifi_alm_send_flag;			//报警数据发送标志
__MCB_WIFI_EXE uint8_t mcb_wifi_set_succ; 				//设置成功标志


//#ifdef __MCB_WIFI_GLOBAL
	//char *GET_CMD_STR[GET_CMD_NUM] = {"LGTD","LTTD","STCT","MNGN","IPSN","IPCN","IPTP","IPBP","FDDV","L1OP","L2OP",};   	
	//char *SET_CMD_STR[SET_CMD_NUM] = {"ATT","MNGN","IPCN","IPTP","IPBP","FDDV","R1GN","R2GN","RDGN","PMGN","PSGN","YZMD",};   		
	//char *WARN_DEF_STR[WARN_DEF_NUM] = {"MATA","SATA","MAPW","SAPW","PTBD","OPPT","CTBD",};   	
//#else
	//extern char *GET_CMD_STR[GET_CMD_NUM];   	
	//extern char *SET_CMD_STR[SET_CMD_NUM];   		
	//extern char *WARN_DEF_STR[WARN_DEF_NUM];   	
//#endif
/*
********************************************************************************
私有函数定义
********************************************************************************
*/
void usart1_init(uint32_t br);
void wifi_comm_ru_data_deal(void);						//升级数据解析处理函数
void wifi_comm_info_data_deal(void);					//信息数据解析处理函数
void wifi_comm_data_send(void);							//信息发送数据函数
void wifi_comm_sys_ru_send(void);						//系统发送升级数据函数
void wifi_comm_sys_warn_send(void);						//系统发送告警数据函数

//下行
void wifi_cmd_80_deal(void);
void wifi_cmd_FC_deal(void);
void wifi_cmd_FD_deal(void);
void wifi_cmd_FE_deal(void);
void wifi_cmd_FF_deal(void);
//
//上行
void wifi_cmd_7A_deal(void);							//上行命令解帧函数
void wifi_cmd_7B_deal(void);							//同上...
void wifi_cmd_7C_deal(void);
void wifi_cmd_7D_deal(void);
void wifi_cmd_7D_deal_reply(void);
void wifi_cmd_7E_deal(void);
void wifi_cmd_7F_deal(void);

//GET FUNC
void get_lgtd(void);
void get_lttd(void);
void get_stct(void);
void get_mngn(void);
void get_ipsn(void);
void get_ipcn(void);
void get_iptp(void);
void get_ipbp(void);
void get_fddv(void);
void get_l1op(void);
void get_l2op(void);
void get_r1gn(void);
void get_r2gn(void);
void get_rdgn(void);        
void get_pmgn(void);            
void get_psgn(void); 
//SET FUNC
void set_atte(uint8_t num);
void set_mngn(uint8_t num);
void set_ipcn(uint8_t num);
void set_iptp(uint8_t num);
void set_ipbp(uint8_t num);
void set_fddv(uint8_t num);
void set_r1gn(uint8_t num);
void set_r2gn(uint8_t num);
void set_rdgn(uint8_t num);             
void set_pmgn(uint8_t num);             
void set_psgn(uint8_t num);             
void set_yzmd(uint8_t num);
//ASCII FIND CMD NO
uint8_t find_get_cmd_index(uint8_t num);
uint8_t find_set_cmd_index(uint8_t num);



#ifdef __cplusplus
	}
#endif

#endif
