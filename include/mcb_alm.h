/*
********************************************************************************
文件名	：mcb_alm.h
文件描述：主控板报警输出头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __MCB_ALM_H
#define __MCB_ALM_H

#ifdef __MCB_ALM_GLOBAL
	#define __MCB_ALM_EXE 
#else
	#define __MCB_ALM_EXE extern
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
#include "mcb_rfb.h"

/*
********************************************************************************
宏定义
********************************************************************************
*/
#define	__DAN_SE_LED__			//单色led闪烁灯，双色第三色不明显也可以用单色的

#define	ALARM_CHECK_TIME		20			//20 * 10 = 200ms
//BEEP NEED 2731HZ方波
#define TIM1_PERIOD_BEEP		17576		//48000000 / 2731 = 17575.9	（0.366ms）
#define TIM1_PULSE_BEEP			8788

#define BEEP_ALARM_TIME			6000		//6000 * 10 = 60s

#ifdef __OLD_MCB_VER__
	#define BEEP_H				(GPIOC->BSRR = GPIO_Pin_10)		//SET
	#define BEEP_L				(GPIOC->BRR = GPIO_Pin_10)		//RESET
#endif

#define	MCB_ALM_UP_ARRAY_NUM	10			//报警缓冲区个数
#define	MCB_ALM_UP_DATA_NUM		100			//每个报警缓冲区成员数量

//WARN
#define WARN_DEF_NUM			13
#define WARN_MATA				100			//主天线，天线1       
#define WARN_SATA				101         //从天线，天线2         
#define WARN_RCH1				102			//射频板通道1          
#define WARN_RCH2				103         //射频板通道2   
#define WARN_MAPW				104         //主电源，电源1           
#define WARN_SAPW				105         //从电源，电源2 
#define WARN_RLY1				106         //继电器1           
#define WARN_RLY2				107         //继电器2      
#define WARN_RLY3				108         //继电器3           
#define WARN_RLY4				109         //继电器4 
#define WARN_PTBD				154         //协议板、射频板
#define WARN_OPPT				155         //输出端口 （主从机八功分板）  
#define WARN_CTBD				156         //控制板 、主控板   
/*
********************************************************************************
公共变量定义
********************************************************************************
*/
__MCB_ALM_EXE uint32_t mcb_alm_task_flag;		//输出报警任务标志
__MCB_ALM_EXE uint8_t mcb_alm_upwifi_flag;		//输出报警记录上传WIFI标志
__MCB_ALM_EXE uint8_t mcb_alm_0hour_clr;		//0点清除标志


/*
********************************************************************************
公共函数定义
********************************************************************************
*/
void mcb_alm_init(void);						//输出报警初始化
void mcb_alm_task(void);						//输出报警任务 

/*
********************************************************************************
私有变量定义
********************************************************************************
*/
__MCB_ALM_EXE uint8_t beep_ctrl_en;				//BEEP允许开关，0:关闭，1:允许
__MCB_ALM_EXE uint8_t beep_alarm_flag;			//需要BEEP报警标志，0:不需要鸣叫，1:需要鸣叫
__MCB_ALM_EXE uint8_t beep_alarm_cnt_en;		//BEEP报警计时器允许
__MCB_ALM_EXE uint16_t beep_alarm_cnt;			//BEEP报警计时器

__MCB_ALM_EXE char mcb_alm_up_buf[MCB_ALM_UP_ARRAY_NUM][MCB_ALM_UP_DATA_NUM]; //报警上传数据缓冲区定义
__MCB_ALM_EXE uint8_t mcb_alm_cur_num;			//当前报警数据数量
__MCB_ALM_EXE uint8_t mcb_alm_in_index;			//当前报警数据in索引
__MCB_ALM_EXE uint8_t mcb_alm_out_index;		//当前报警数据out索引
__MCB_ALM_EXE uint8_t mcb_alm_cur_index;		//当前报警数据索引，用于报警上传数据提取

__MCB_ALM_EXE uint16_t ch1_alm_cnt;				//通道1告警计数器
__MCB_ALM_EXE uint16_t ch2_alm_cnt;				//通道2告警计数器
__MCB_ALM_EXE uint16_t ant1_alm_cnt;			//天线1告警计数器
__MCB_ALM_EXE uint16_t ant2_alm_cnt;			//天线2告警计数器
__MCB_ALM_EXE uint16_t pwr1_alm_cnt;			//电源1告警计数器
__MCB_ALM_EXE uint16_t pwr2_alm_cnt;			//电源2告警计数器

__MCB_ALM_EXE uint8_t cur_gps_max_star;			//当前通道GPS最大星数
__MCB_ALM_EXE uint8_t cur_gps_min_star;			//当前通道GPS最小星数
__MCB_ALM_EXE uint8_t cur_bd_max_star;			//当前通道BD最大星数
__MCB_ALM_EXE uint8_t cur_bd_min_star;			//当前通道BD最小星数
__MCB_ALM_EXE uint8_t cur_gps_star;				//当前通道GPS星数
__MCB_ALM_EXE uint8_t cur_bd_star;				//当前通道BD星数

__MCB_ALM_EXE uint8_t ch1_rf_state;				//射频板通道1状态
__MCB_ALM_EXE uint8_t ch2_rf_state;				//射频板通道2状态
__MCB_ALM_EXE uint8_t ant1_rf_state;			//射频板天线1状态
__MCB_ALM_EXE uint8_t ant2_rf_state;			//射频板天线2状态
__MCB_ALM_EXE uint8_t pwr1_pd8_state;			//电源1状态
__MCB_ALM_EXE uint8_t pwr2_pd8_state;			//电源2状态
__MCB_ALM_EXE uint8_t old_ch1_rf_state;			//射频板通道1状态
__MCB_ALM_EXE uint8_t old_ch2_rf_state;			//射频板通道2状态
__MCB_ALM_EXE uint8_t old_ant1_rf_state;		//射频板天线1状态
__MCB_ALM_EXE uint8_t old_ant2_rf_state;		//射频板天线2状态
__MCB_ALM_EXE uint8_t old_pwr1_pd8_state;		//电源1状态
__MCB_ALM_EXE uint8_t old_pwr2_pd8_state;		//电源2状态

__MCB_ALM_EXE uint8_t relay1_alm_state_rf;		//继电器1告警状态
__MCB_ALM_EXE uint8_t relay1_alm_state_mpd8;	//继电器1告警状态
__MCB_ALM_EXE uint8_t relay1_alm_state_spd8[8];
__MCB_ALM_EXE uint8_t relay2_alm_state;			//继电器2告警状态
__MCB_ALM_EXE uint8_t relay3_alm_state;			//继电器3告警状态
__MCB_ALM_EXE uint8_t relay4_alm_state;			//继电器4告警状态

__MCB_ALM_EXE uint8_t old_relay1_alm_state_rf;	//继电器1
__MCB_ALM_EXE uint8_t old_relay1_alm_state_mpd8;//继电器1
__MCB_ALM_EXE uint8_t old_relay1_alm_state_spd8[8];
__MCB_ALM_EXE uint8_t old_relay2_alm_state;		//继电器2
__MCB_ALM_EXE uint8_t old_relay3_alm_state;		//继电器3
__MCB_ALM_EXE uint8_t old_relay4_alm_state;		//继电器4

__MCB_ALM_EXE uint8_t rf_pd8_alm;				//0-rf,1-mpd8,0x81~0x88-spd8

__MCB_ALM_EXE uint8_t mpd8_out_state;			//主机八功分板输出状态
__MCB_ALM_EXE uint8_t spd8_out_state[8];		//从机八功分板输出状态
__MCB_ALM_EXE uint8_t old_mpd8_out_state;		//主机八功分板输出状态
__MCB_ALM_EXE uint8_t old_spd8_out_state[8];	//从机八功分板输出状态

__MCB_ALM_EXE char rf_date_time_str[20];		//日期时间字符串
__MCB_ALM_EXE char mp_date_time_str[20];		//日期时间字符串

//射频板和八功分板状态各见其模块，主从八功分板端口外接状态见八功分模块


/*
********************************************************************************
私有函数定义
********************************************************************************
*/
void alarm_check_deal(void);
void gps_bd_max_min_star_judge(void);
void alarm_data_in(uint8_t alm_type);
void alarm_data_out(void);
void beep_en_on_off(void);
void rfb_date_time_to_str(MR_COMM_BT_TIME_DEF bt_time);
void pd8_date_time_to_str(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

void warn_mata_deal(void);
void warn_sata_deal(void);
void warn_rch1_deal(void);
void warn_rch2_deal(void);
void warn_mapw_deal(void);
void warn_sapw_deal(void);
void warn_rly1_deal(uint8_t rf_pd8);
void warn_rly2_deal(void);
void warn_rly3_deal(void);
void warn_rly4_deal(void);
void warn_ptbd_deal(void);
void warn_oppt_deal(void);
void warn_ctbd_deal(void);

//LED1:天线1、通道1、干接点1、主、从机输出 	LED2:天线2、通道2、干接点2  LED3:双电源指示
//刚上电 LED1 LED2灭，LED3绿灯亮
//运行中根据状态点亮，红灯亮有问题，绿灯亮正常，黄灯是故障
//
//根据功能定义
void ALL_LED_OFF(void);							//写报警缓存
void LED_1_NORMAL(void);						//同上
void LED_2_NORMAL(void);
void LED_3_NORMAL(void);
void LED_1_ALARM(void);
void LED_2_ALARM(void);
void LED_1_FAULT(void);
void LED_2_FAULT(void);
void LED_3_ALARM(void);
void LED_IO_UPDATE(void);						//真正刷新IO，判断完LED状态，最后必须调用一次

/*
  继电器1(干接点1）指示天线1、通道1、主机输出和从机输出；
  继电器2(干接点2）指示天线2、通道2；
  继电器3(干接点3）指示电源1；
  继电器4(干接点4）指示电源2；
*/
void ALL_RELAY_NORMAL(void);					//写报警缓存
void RELAY_1_NORMAL(void);						//同上
void RELAY_2_NORMAL(void);
void RELAY_3_NORMAL(void);
void RELAY_4_NORMAL(void);
void RELAY_1_ALARM(void);
void RELAY_2_ALARM(void);
void RELAY_3_ALARM(void);
void RELAY_4_ALARM(void);
void RELAY_IO_UPDATE(void);						//真正刷新IO，判断完RELAY状态，最后必须调用一次
//
void BEEP_init(void);							//蜂鸣器初始化

void TIM1_Config(void);							//TIM1时钟IO配置
void TIM1_PWM_init(uint16_t period, uint16_t pulse);//TIM1 PWM初始化
    
#ifdef __cplusplus
	}
#endif

#endif
