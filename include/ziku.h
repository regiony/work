/*
********************************************************************************
文件名	：ziku.h
文件描述：字库头文件
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#ifndef __ZIKU_H
#define __ZIKU_H

#ifdef __ZIKU_GLOBAL
	#define __ZIKU_EXE 
#else
	#define __ZIKU_EXE extern
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

#define __8_14_CHAR_0_9__						//定义8*12 0-9字符

//常用ASCII码索引定义
#define CHAR_SPACE_INDEX 	0x00 				//' '
#define CHAR_YKHZ_INDEX 	0x08				//'('
#define CHAR_YKHY_INDEX 	0x09				//')'
#define CHAR_DIAN_INDEX 	0x0E				//'.'
#define CHAR_MAOHAO_INDEX 	0x1A				//':'
#define CHAR_ZKHZ_INDEX 	0x1C				//'<'
#define CHAR_ZKHY_INDEX 	0x1E				//'>'
#define CHAR_HLINE_INDEX 	0x0D				//'-'

#define CHAR_0_INDEX 		0x10 				//C = '0'~'9': 0x10 + (0 ~ 9)
#define CHAR_A_INDEX		0x21				//C = 'A'~'Z': 0x21 + (0 ~ 26)
#define CHAR_a_INDEX		0x41				//C = 'a'~'z': 0x41 + (0 ~ 26)

/*
********************************************************************************
公共变量定义
********************************************************************************
*/
__ZIKU_EXE const uint8_t ascii_5x7[95][5];			//5*7 ascii
__ZIKU_EXE const uint8_t ascii_8x16[95][16];		//8*16 ascii

//
//开机画面 机型1 此处开机界面借用之前的，可能需要修改
__ZIKU_EXE const uint8_t ZK_GPS1[896];				//北斗/GPS
__ZIKU_EXE const uint8_t ZK_ZNFLXT[512];			//智能分路系统
//开机画面 机型2 此处开机界面借用之前的，可能需要修改
__ZIKU_EXE const uint8_t ZK_SZTB[400];				//时钟同步
__ZIKU_EXE const uint8_t ZK_SXZNFLXT[512];			//双星智能分路系统

__ZIKU_EXE const uint8_t BEEP_on_icon_16x16[32];	//蜂鸣器允许图标16*16
__ZIKU_EXE const uint8_t beep_off_icon_16x16[32];	//蜂鸣器关闭图标
__ZIKU_EXE const uint8_t BEEP_on_icon_12x8[12];		//蜂鸣器允许图标12*8
__ZIKU_EXE const uint8_t beep_off_icon_12x8[12];	//蜂鸣器关闭图标
__ZIKU_EXE const uint8_t zyd_icon_5x7[5];			//中圆点
__ZIKU_EXE const uint8_t charP_5x12[10];			//P 5*12
__ZIKU_EXE const uint8_t weixing_icon16x16[128];	//卫星图标
__ZIKU_EXE const uint8_t bts_icon16x16[32];			//BTS图标
__ZIKU_EXE const uint8_t char1_xiao5[16];
__ZIKU_EXE const uint8_t char2_xiao5[16];

//宋体 机型1 五号？
__ZIKU_EXE const uint8_t hz16_st1_tian[32];			//天
__ZIKU_EXE const uint8_t hz16_st1_xian[32];			//线
__ZIKU_EXE const uint8_t hz16_st1_dian[32];			//电
__ZIKU_EXE const uint8_t hz16_st1_yuan[32];			//源
__ZIKU_EXE const uint8_t hz16_st1_hao[32];			//号
__ZIKU_EXE const uint8_t hz16_st1_tong[32];			//通
__ZIKU_EXE const uint8_t hz16_st1_dao[32];			//道
__ZIKU_EXE const uint8_t hz16_st1_zheng[32];		//正
__ZIKU_EXE const uint8_t hz16_st1_chang[32];		//常
__ZIKU_EXE const uint8_t hz16_st1_gao[32];			//告
__ZIKU_EXE const uint8_t hz16_st1_jing[32];			//警
__ZIKU_EXE const uint8_t hz16_st1_wei[32];			//未
__ZIKU_EXE const uint8_t hz16_st1_jie[32];			//接
__ZIKU_EXE const uint8_t hz16_st1_duan[32];			//短
__ZIKU_EXE const uint8_t hz16_st1_lu[32];			//路
__ZIKU_EXE const uint8_t hz16_st1_zhu[32];			//主
__ZIKU_EXE const uint8_t hz16_st1_cong[32];			//从
__ZIKU_EXE const uint8_t hz16_st1_ji[32];			//机
__ZIKU_EXE const uint8_t hz16_st1_shu[32];			//输
__ZIKU_EXE const uint8_t hz16_st1_chu[32];			//出
__ZIKU_EXE const uint8_t hz16_st1_nu[32];			//努
__ZIKU_EXE const uint8_t hz16_st1_li[32];			//力
__ZIKU_EXE const uint8_t hz16_st1_sou[32];			//搜
__ZIKU_EXE const uint8_t hz16_st1_suo[32];			//索
__ZIKU_EXE const uint8_t hz16_st1_zhong[32];		//中
__ZIKU_EXE const uint8_t hz16_st1_pang[32];			//旁
__ZIKU_EXE const uint8_t hz16_st1_shi[32];			//是
__ZIKU_EXE const uint8_t hz16_st1_fou[32];			//否
__ZIKU_EXE const uint8_t hz16_st1_wu[32];			//无
__ZIKU_EXE const uint8_t hz16_st1_zai[32];			//在
__ZIKU_EXE const uint8_t hz16_st1_xing[32];			//星
__ZIKU_EXE const uint8_t hz16_st1_she[32];			//设
__ZIKU_EXE const uint8_t hz16_st1_bei[32];			//备
__ZIKU_EXE const uint8_t hz16_st1_zhuang[32];		//状
__ZIKU_EXE const uint8_t hz16_st1_tai[32];			//态
__ZIKU_EXE const uint8_t hz16_st1_xin[32];			//信
__ZIKU_EXE const uint8_t hz16_st1_xi[32];			//息
__ZIKU_EXE const uint8_t hz16_st1_kong[32];			//控
__ZIKU_EXE const uint8_t hz16_st1_ru[32];			//入
__ZIKU_EXE const uint8_t hz16_st1_ban[32];			//版
__ZIKU_EXE const uint8_t hz16_st1_ben[32];			//本


//宋体 机型2 小五？
__ZIKU_EXE const uint8_t hz16_st2_tian[32];			//天
__ZIKU_EXE const uint8_t hz16_st2_xian[32];			//线
__ZIKU_EXE const uint8_t hz16_st2_dian[32];			//电
__ZIKU_EXE const uint8_t hz16_st2_yuan[32];			//源
__ZIKU_EXE const uint8_t hz16_st2_hao[32];			//号
__ZIKU_EXE const uint8_t hz16_st2_tong[32];			//通
__ZIKU_EXE const uint8_t hz16_st2_dao[32];			//道
__ZIKU_EXE const uint8_t hz16_st2_zheng[32];		//正
__ZIKU_EXE const uint8_t hz16_st2_chang[32];		//常
__ZIKU_EXE const uint8_t hz16_st2_gao[32];			//告
__ZIKU_EXE const uint8_t hz16_st2_jing[32];			//警
__ZIKU_EXE const uint8_t hz16_st2_wei[32];			//未
__ZIKU_EXE const uint8_t hz16_st2_jie[32];			//接
__ZIKU_EXE const uint8_t hz16_st2_duan[32];			//短
__ZIKU_EXE const uint8_t hz16_st2_lu[32];			//路
__ZIKU_EXE const uint8_t hz16_st2_zhu[32];			//主
__ZIKU_EXE const uint8_t hz16_st2_cong[32];			//从
__ZIKU_EXE const uint8_t hz16_st2_ji[32];			//机
__ZIKU_EXE const uint8_t hz16_st2_shu[32];			//输
__ZIKU_EXE const uint8_t hz16_st2_chu[32];			//出
__ZIKU_EXE const uint8_t hz16_st2_nu[32];			//努
__ZIKU_EXE const uint8_t hz16_st2_li[32];			//力
__ZIKU_EXE const uint8_t hz16_st2_sou[32];			//搜
__ZIKU_EXE const uint8_t hz16_st2_suo[32];			//索
__ZIKU_EXE const uint8_t hz16_st2_zhong[32];		//中
__ZIKU_EXE const uint8_t hz16_st2_pang[32];			//旁
__ZIKU_EXE const uint8_t hz16_st2_shi[32];			//是
__ZIKU_EXE const uint8_t hz16_st2_fou[32];			//否
__ZIKU_EXE const uint8_t hz16_st2_wu[32];			//无
__ZIKU_EXE const uint8_t hz16_st2_zai[32];			//在
__ZIKU_EXE const uint8_t hz16_st2_xing[32];			//星
__ZIKU_EXE const uint8_t hz16_st2_she[32];			//设
__ZIKU_EXE const uint8_t hz16_st2_bei[32];			//备
__ZIKU_EXE const uint8_t hz16_st2_zhuang[32];		//状
__ZIKU_EXE const uint8_t hz16_st2_tai[32];			//态
__ZIKU_EXE const uint8_t hz16_st2_xin[32];			//信
__ZIKU_EXE const uint8_t hz16_st2_xi[32];			//息
__ZIKU_EXE const uint8_t hz16_st2_kong[32];			//控
__ZIKU_EXE const uint8_t hz16_st2_ru[32];			//入
__ZIKU_EXE const uint8_t hz16_st2_ban[32];			//版
__ZIKU_EXE const uint8_t hz16_st2_ben[32];			//本

/*
********************************************************************************
公共函数定义
********************************************************************************
*/
//void ziku_init(void);				//字库初始化

/*
********************************************************************************
私有变量定义
********************************************************************************
*/


/*
********************************************************************************
私有函数定义
********************************************************************************
*/


#ifdef __cplusplus
	}
#endif

#endif
