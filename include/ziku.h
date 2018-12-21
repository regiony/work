/*
********************************************************************************
�ļ���	��ziku.h
�ļ��������ֿ�ͷ�ļ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
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
ͷ�ļ�
********************************************************************************
*/
#include "stm32f0xx.h"


/*
********************************************************************************
�궨��
********************************************************************************
*/

#define __8_14_CHAR_0_9__						//����8*12 0-9�ַ�

//����ASCII����������
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
������������
********************************************************************************
*/
__ZIKU_EXE const uint8_t ascii_5x7[95][5];			//5*7 ascii
__ZIKU_EXE const uint8_t ascii_8x16[95][16];		//8*16 ascii

//
//�������� ����1 �˴������������֮ǰ�ģ�������Ҫ�޸�
__ZIKU_EXE const uint8_t ZK_GPS1[896];				//����/GPS
__ZIKU_EXE const uint8_t ZK_ZNFLXT[512];			//���ܷ�·ϵͳ
//�������� ����2 �˴������������֮ǰ�ģ�������Ҫ�޸�
__ZIKU_EXE const uint8_t ZK_SZTB[400];				//ʱ��ͬ��
__ZIKU_EXE const uint8_t ZK_SXZNFLXT[512];			//˫�����ܷ�·ϵͳ

__ZIKU_EXE const uint8_t BEEP_on_icon_16x16[32];	//����������ͼ��16*16
__ZIKU_EXE const uint8_t beep_off_icon_16x16[32];	//�������ر�ͼ��
__ZIKU_EXE const uint8_t BEEP_on_icon_12x8[12];		//����������ͼ��12*8
__ZIKU_EXE const uint8_t beep_off_icon_12x8[12];	//�������ر�ͼ��
__ZIKU_EXE const uint8_t zyd_icon_5x7[5];			//��Բ��
__ZIKU_EXE const uint8_t charP_5x12[10];			//P 5*12
__ZIKU_EXE const uint8_t weixing_icon16x16[128];	//����ͼ��
__ZIKU_EXE const uint8_t bts_icon16x16[32];			//BTSͼ��
__ZIKU_EXE const uint8_t char1_xiao5[16];
__ZIKU_EXE const uint8_t char2_xiao5[16];

//���� ����1 ��ţ�
__ZIKU_EXE const uint8_t hz16_st1_tian[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_xian[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_dian[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_yuan[32];			//Դ
__ZIKU_EXE const uint8_t hz16_st1_hao[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_tong[32];			//ͨ
__ZIKU_EXE const uint8_t hz16_st1_dao[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_zheng[32];		//��
__ZIKU_EXE const uint8_t hz16_st1_chang[32];		//��
__ZIKU_EXE const uint8_t hz16_st1_gao[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_jing[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_wei[32];			//δ
__ZIKU_EXE const uint8_t hz16_st1_jie[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_duan[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_lu[32];			//·
__ZIKU_EXE const uint8_t hz16_st1_zhu[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_cong[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_ji[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_shu[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_chu[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_nu[32];			//Ŭ
__ZIKU_EXE const uint8_t hz16_st1_li[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_sou[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_suo[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_zhong[32];		//��
__ZIKU_EXE const uint8_t hz16_st1_pang[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_shi[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_fou[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_wu[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_zai[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_xing[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_she[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_bei[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_zhuang[32];		//״
__ZIKU_EXE const uint8_t hz16_st1_tai[32];			//̬
__ZIKU_EXE const uint8_t hz16_st1_xin[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_xi[32];			//Ϣ
__ZIKU_EXE const uint8_t hz16_st1_kong[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_ru[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_ban[32];			//��
__ZIKU_EXE const uint8_t hz16_st1_ben[32];			//��


//���� ����2 С�壿
__ZIKU_EXE const uint8_t hz16_st2_tian[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_xian[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_dian[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_yuan[32];			//Դ
__ZIKU_EXE const uint8_t hz16_st2_hao[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_tong[32];			//ͨ
__ZIKU_EXE const uint8_t hz16_st2_dao[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_zheng[32];		//��
__ZIKU_EXE const uint8_t hz16_st2_chang[32];		//��
__ZIKU_EXE const uint8_t hz16_st2_gao[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_jing[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_wei[32];			//δ
__ZIKU_EXE const uint8_t hz16_st2_jie[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_duan[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_lu[32];			//·
__ZIKU_EXE const uint8_t hz16_st2_zhu[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_cong[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_ji[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_shu[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_chu[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_nu[32];			//Ŭ
__ZIKU_EXE const uint8_t hz16_st2_li[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_sou[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_suo[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_zhong[32];		//��
__ZIKU_EXE const uint8_t hz16_st2_pang[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_shi[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_fou[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_wu[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_zai[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_xing[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_she[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_bei[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_zhuang[32];		//״
__ZIKU_EXE const uint8_t hz16_st2_tai[32];			//̬
__ZIKU_EXE const uint8_t hz16_st2_xin[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_xi[32];			//Ϣ
__ZIKU_EXE const uint8_t hz16_st2_kong[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_ru[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_ban[32];			//��
__ZIKU_EXE const uint8_t hz16_st2_ben[32];			//��

/*
********************************************************************************
������������
********************************************************************************
*/
//void ziku_init(void);				//�ֿ��ʼ��

/*
********************************************************************************
˽�б�������
********************************************************************************
*/


/*
********************************************************************************
˽�к�������
********************************************************************************
*/


#ifdef __cplusplus
	}
#endif

#endif
