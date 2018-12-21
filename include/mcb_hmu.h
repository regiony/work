/*
********************************************************************************
�ļ���	��mcb_hmu.h
�ļ����������ذ��˻��ӿ�ͷ�ļ�����Ƶ���Э��忪�����治ͬ,���Ͳ�ͬ��ʾ������ͬ
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#ifndef __MCB_HMU_H
#define __MCB_HMU_H

#ifdef __MCB_HMU_GLOBAL
	#define __MCB_HMU_EXE 
#else
	#define __MCB_HMU_EXE extern
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
#include "lcd_api.h"     
#include "ziku.h"
        
/*
********************************************************************************
�궨��
********************************************************************************
*/
//__TEST_HMU__ //������

#define		LCD_AUTO_REFRESH_CNT		50		//50*10=500ms,û�а����������¼�ʱ��Ĭ��500msˢ��һ�ν��棬���������Ƿ���ˢ�²���
#define		LCD_AUTO_RETURN_MAIN_CNT	2500	//2500*10=25000ms,��ֹ25S�Զ�����������
#define		LCD_CLOSE_BL_CNT			30000	//5Min��30000*10=300000ms
#define		LCD_CLOSE_BL_CNT_NUM		12		//12�Σ�30000/2500=12

#define		PWR_ON_PAGE					0x01	//��������
#define		MAIN_PAGE					0x02	//������
#define		SUB_1_PAGE					0x03	//�ӽ���1
#define		SUB_2_PAGE					0x04	//�ӽ���2
#define		SUB_3_PAGE					0x05	//�ӽ���3
#define		CH1_TEST_PAGE				0x11	//ͨ��1�ֶ����Խ��棬�ֶ�up�������룬�ֶ��˳�
#define		CH2_TEST_PAGE				0x12	//ͨ��2�ֶ����Խ��棬�ֶ�down�������룬�ֶ��˳�
#define		AUTO_TEST_PAGE				0x13	//�Զ����Խ��棬�ֶ�left�������룬�ֶ��˳�
#define		DEV_FAULT_PAGE				0x20	//�豸ά��ҳ�棬�ݶ����������� up+dw������ϼ� �������룬�ֶ��˳�
#define		DEV_PARA_PAGE				0x30	//�豸����ҳ��

/*
********************************************************************************
������������
********************************************************************************
*/
__MCB_HMU_EXE uint32_t mcb_hmu_task_flag;		//�˻�ģ�������־
__MCB_HMU_EXE uint8_t mcb_hmu_refresh_flag;		//�˻�����ˢ�±�־����Ҫˢ��ʱ������һ��ƽʱ������ˢ��
__MCB_HMU_EXE uint8_t mcb_hmu_0hour_clr_flag;	//ÿ��0���LCD������ʾ��ͨ�������ߺ͵�Դ�ĸ澯�����Լ�����/GPS�����С��������
												//��Ƶ��ӿڲ����ж�0�㣬֪ͨ������Ԫ�����Ӧ�������˻���������ʾ�������˻�������Ҫ�˱�־��ˢ������
__MCB_HMU_EXE uint16_t mcb_hmu_auto_return_cnt;	//��ֹ25S�Զ����������������
__MCB_HMU_EXE uint8_t mcb_hmu_bl_cnt;			//��ֹ5min�ر��������
__MCB_HMU_EXE uint8_t mcb_hmu_bl_state;			//����״̬0���򿪣�1�ر�
__MCB_HMU_EXE uint8_t mcb_hmu_bl_close_flag;	//����رձ�ʶ
__MCB_HMU_EXE uint8_t mcb_hmu_disp_pages;		//��ʾҳ�����1~5
__MCB_HMU_EXE uint8_t mcb_hmu_disp_pages_bak;	//��ʾҳ�����bak

/*
********************************************************************************
������������
********************************************************************************
*/
void mcb_hmu_init(void);						//�˻�ģ���ʼ��
void mcb_hmu_task(void);						//�˻�ģ������

/*
********************************************************************************
˽�б�������
********************************************************************************
*/
__MCB_HMU_EXE uint8_t mcb_hmu_modify_mode;		//
__MCB_HMU_EXE uint8_t mcb_hmu_para_index;		//
__MCB_HMU_EXE uint8_t mcb_hmu_para_bit;			//
__MCB_HMU_EXE uint8_t mcb_hmu_para_bak;			//
__MCB_HMU_EXE uint8_t ant_used_flag;
__MCB_HMU_EXE uint8_t ant_used_gain;
__MCB_HMU_EXE uint8_t wire_used_att;
__MCB_HMU_EXE uint8_t resv_para_def;
__MCB_HMU_EXE uint8_t read_att_flag;

#ifdef __MCB_HMU_GLOBAL
	//����1
	const uint8_t *tianxian1[3] = {hz16_st1_tian, hz16_st1_xian, NULL,};					//����
	const uint8_t *dianyuan1[3] = {hz16_st1_dian, hz16_st1_yuan, NULL,};					//��Դ
	const uint8_t *tongdao1[3] = {hz16_st1_tong, hz16_st1_dao, NULL,};						//ͨ��
	const uint8_t *haotongdao1[4] = {hz16_st1_hao, hz16_st1_tong, hz16_st1_dao, NULL,};		//��ͨ��
	const uint8_t *zhengchang1[3] = {hz16_st1_zheng, hz16_st1_chang, NULL,};				//����	
	const uint8_t *gaojing1[3] = {hz16_st1_gao, hz16_st1_jing, NULL,};						//�澯
	const uint8_t *weijie1[3] = {hz16_st1_wei, hz16_st1_jie, NULL,};						//δ��
	const uint8_t *duanlu1[3] = {hz16_st1_duan, hz16_st1_lu, NULL,};						//��·
	const uint8_t *zhukong1[3] = {hz16_st1_zhu, hz16_st1_kong, NULL,};						//����
	const uint8_t *shuru1[3] = {hz16_st1_shu, hz16_st1_ru, NULL,};							//����
	const uint8_t *zhuchu1[3] = {hz16_st1_zhu, hz16_st1_chu, NULL,};						//����
	const uint8_t *congchu1[3] = {hz16_st1_cong, hz16_st1_chu, NULL,};						//�ӳ�
	const uint8_t *banben1[3] = {hz16_st1_ban, hz16_st1_ben, NULL,};						//�汾
	const uint8_t *zhujishuchu1[5] = {hz16_st1_zhu, hz16_st1_ji, hz16_st1_shu, hz16_st1_chu, NULL,};						//�������
	const uint8_t *congjishuchu1[5] = {hz16_st1_cong, hz16_st1_ji, hz16_st1_shu, hz16_st1_chu, NULL,};						//�ӻ����
	const uint8_t *nvlisousuozhong1[6] = {hz16_st1_nu, hz16_st1_li, hz16_st1_sou, hz16_st1_suo, hz16_st1_zhong,  NULL,};	//Ŭ��������
	const uint8_t *wucongji1[4] = {hz16_st1_wu, hz16_st1_cong, hz16_st1_ji, NULL,};											//�޴ӻ�
	const uint8_t *zhujipanglu1[5] = {hz16_st1_zhu, hz16_st1_ji, hz16_st1_pang, hz16_st1_lu, NULL,};						//������·
	const uint8_t *zhengzaisouxing1[5] = {hz16_st1_zheng, hz16_st1_zai, hz16_st1_sou, hz16_st1_xing, NULL,};				//��������
	const uint8_t *shi1[2] = {hz16_st1_shi, NULL,};											//��
	const uint8_t *fou1[2] = {hz16_st1_fou, NULL,};											//��
	const uint8_t *shebeiztxx1[7] = {hz16_st1_she, hz16_st1_bei, hz16_st1_zhuang, hz16_st1_tai, hz16_st1_xin, hz16_st1_xi,  NULL,};	//�豸״̬��Ϣ
	const uint8_t *shebeizt1[5] = {hz16_st1_she, hz16_st1_bei, hz16_st1_zhuang, hz16_st1_tai, NULL,};						//�豸״̬

	//����2
	const uint8_t *tianxian2[3] = {hz16_st2_tian, hz16_st2_xian, NULL,};					//����
	const uint8_t *dianyuan2[3] = {hz16_st2_dian, hz16_st2_yuan, NULL,};					//��Դ
	const uint8_t *tongdao2[3] = {hz16_st2_tong, hz16_st2_dao, NULL,};						//ͨ��
	const uint8_t *haotongdao2[4] = {hz16_st2_hao, hz16_st2_tong, hz16_st2_dao, NULL,};		//��ͨ��
	const uint8_t *zhengchang2[3] = {hz16_st2_zheng, hz16_st2_chang, NULL,};				//����	
	const uint8_t *gaojing2[3] = {hz16_st2_gao, hz16_st2_jing, NULL,};						//�澯
	const uint8_t *weijie2[3] = {hz16_st2_wei, hz16_st2_jie, NULL,};						//δ��
	const uint8_t *duanlu2[3] = {hz16_st2_duan, hz16_st2_lu, NULL,};						//��·
	const uint8_t *zhukong2[3] = {hz16_st2_zhu, hz16_st2_kong, NULL,};						//����
	const uint8_t *shuru2[3] = {hz16_st2_shu, hz16_st2_ru, NULL,};							//����
	const uint8_t *zhuchu2[3] = {hz16_st2_zhu, hz16_st2_chu, NULL,};						//����
	const uint8_t *congchu2[3] = {hz16_st2_cong, hz16_st2_chu, NULL,};						//�ӳ�
	const uint8_t *banben2[3] = {hz16_st2_ban, hz16_st2_ben, NULL,};						//�汾
	const uint8_t *zhujishuchu2[5] = {hz16_st2_zhu, hz16_st2_ji, hz16_st2_shu, hz16_st2_chu, NULL,};						//�������
	const uint8_t *congjishuchu2[5] = {hz16_st2_cong, hz16_st2_ji, hz16_st2_shu, hz16_st2_chu, NULL,};						//�ӻ����
	const uint8_t *nvlisousuozhong2[6] = {hz16_st2_nu, hz16_st2_li, hz16_st2_sou, hz16_st2_suo, hz16_st2_zhong,  NULL,};	//Ŭ��������
	const uint8_t *wucongji2[4] = {hz16_st2_wu, hz16_st2_cong, hz16_st2_ji, NULL,};											//�޴ӻ�
	const uint8_t *zhujipanglu2[5] = {hz16_st2_zhu, hz16_st2_ji, hz16_st2_pang, hz16_st2_lu, NULL,};						//������·
	const uint8_t *zhengzaisouxing2[5] = {hz16_st2_zheng, hz16_st2_zai, hz16_st2_sou, hz16_st2_xing, NULL,};				//��������
	const uint8_t *shi2[2] = {hz16_st2_shi, NULL,};											//��
	const uint8_t *fou2[2] = {hz16_st2_fou, NULL,};											//��
	const uint8_t *shebeiztxx2[7] = {hz16_st2_she, hz16_st2_bei, hz16_st2_zhuang, hz16_st2_tai, hz16_st2_xin, hz16_st2_xi,  NULL,};	//�豸״̬��Ϣ
	const uint8_t *shebeizt2[5] = {hz16_st2_she, hz16_st2_bei, hz16_st2_zhuang, hz16_st2_tai, NULL,};						//�豸״̬
	
	//5*7 �ַ��� (����������У����ܲ��ã�Ч�����⣩
	const uint8_t *zyd_null[7] = {&ascii_5x7[CHAR_SPACE_INDEX][0], &ascii_5x7[CHAR_SPACE_INDEX][0], &ascii_5x7[CHAR_SPACE_INDEX][0], \
								  &ascii_5x7[CHAR_SPACE_INDEX][0], &ascii_5x7[CHAR_SPACE_INDEX][0], &ascii_5x7[CHAR_SPACE_INDEX][0],  NULL,};//clr all .   
	const uint8_t *zyd_1dot[2] = {zyd_icon_5x7, NULL,};																		 //.
	const uint8_t *zyd_2dot[3] = {zyd_icon_5x7, zyd_icon_5x7, NULL,};														 //..
	const uint8_t *zyd_3dot[4] = {zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, NULL,};											 //...
	const uint8_t *zyd_4dot[5] = {zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, NULL,};							 //....
	const uint8_t *zyd_5dot[6] = {zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, NULL,};				 //.....
	const uint8_t *zyd_6dot[7] = {zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, zyd_icon_5x7, NULL,};//......
	
	const uint8_t *yjt_null[7] = {&ascii_5x7[CHAR_SPACE_INDEX][0], &ascii_5x7[CHAR_SPACE_INDEX][0], &ascii_5x7[CHAR_SPACE_INDEX][0], \
								  &ascii_5x7[CHAR_SPACE_INDEX][0], &ascii_5x7[CHAR_SPACE_INDEX][0], &ascii_5x7[CHAR_SPACE_INDEX][0],  NULL,};//clr all >   
	const uint8_t *yjt_1dot[2] = {&ascii_5x7[CHAR_ZKHY_INDEX][0], NULL,};																	 //>
	const uint8_t *yjt_2dot[3] = {&ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], NULL,};									 //>>
	const uint8_t *yjt_3dot[4] = {&ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], NULL,};	 //>>>
	const uint8_t *yjt_4dot[5] = {&ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], \
									&ascii_5x7[CHAR_ZKHY_INDEX][0], NULL,};																	 //>>>>
	const uint8_t *yjt_5dot[6] = {&ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], \
									&ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], NULL,};				 					 //>>>>>
	const uint8_t *yjt_6dot[7] = {&ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], \
									&ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], &ascii_5x7[CHAR_ZKHY_INDEX][0], NULL,};	 //>>>>>>
									
	const uint8_t *charBD_5x7[3] = {&ascii_5x7[CHAR_A_INDEX + 'B' - 'A'][0], &ascii_5x7[CHAR_A_INDEX + 'D' - 'A'][0], NULL,};				//BD
	const uint8_t *charGPS_5x7[4] = {&ascii_5x7[CHAR_A_INDEX + 'G' - 'A'][0], &ascii_5x7[CHAR_A_INDEX + 'P' - 'A'][0], \
									 &ascii_5x7[CHAR_A_INDEX + 'S' - 'A'][0], NULL,};														//GPS
	const uint8_t *charMaxmh_5x7[5] = {&ascii_5x7[CHAR_A_INDEX + 'M' - 'A'][0], &ascii_5x7[CHAR_a_INDEX][0], \
								  	   &ascii_5x7[CHAR_a_INDEX + 'x' - 'a'][0], &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};					//Max:
	const uint8_t *charMinmh_5x7[5] = {&ascii_5x7[CHAR_A_INDEX + 'M' - 'A'][0], &ascii_5x7[CHAR_a_INDEX + 'i' - 'a'][0], \
								  	   &ascii_5x7[CHAR_a_INDEX + 'n' - 'a'][0], &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};					//Min:
	const uint8_t *charMCBkmh_5x7[6] = {&ascii_5x7[CHAR_A_INDEX + 'M' - 'A'][0], &ascii_5x7[CHAR_A_INDEX + 'C' - 'A'][0], \
		 &ascii_5x7[CHAR_A_INDEX + 'B' - 'A'][0], &ascii_5x7[CHAR_SPACE_INDEX][0], &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};				//MCB :
	const uint8_t *charRFXYmh_5x7[6] = {&ascii_5x7[CHAR_A_INDEX + 'R' - 'A'][0], &ascii_5x7[CHAR_A_INDEX + 'F' - 'A'][0], \
		 &ascii_5x7[CHAR_A_INDEX + 'X' - 'A'][0], &ascii_5x7[CHAR_A_INDEX + 'Y' - 'A'][0], &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};		//RFXY:		
	const uint8_t *charPD8Mmh_5x7[6] = {&ascii_5x7[CHAR_A_INDEX + 'P' - 'A'][0], &ascii_5x7[CHAR_A_INDEX + 'D' - 'A'][0], \
		 &ascii_5x7[CHAR_0_INDEX + 8][0], &ascii_5x7[CHAR_A_INDEX + 'M' - 'A'][0], &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};				//PD8M:		
	const uint8_t *charPD8Smh_5x7[6] = {&ascii_5x7[CHAR_A_INDEX + 'P' - 'A'][0], &ascii_5x7[CHAR_A_INDEX + 'D' - 'A'][0], \
		 &ascii_5x7[CHAR_0_INDEX + 8][0], &ascii_5x7[CHAR_A_INDEX + 'S' - 'A'][0], &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};				//PD8S:		
	const uint8_t *charHWmh_5x7[4] = {&ascii_5x7[CHAR_A_INDEX + 'H' - 'A'][0], &ascii_5x7[CHAR_A_INDEX + 'W' - 'A'][0], \
		 								 &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};															//HW:	
	const uint8_t *charSWmh_5x7[4] = {&ascii_5x7[CHAR_A_INDEX + 'S' - 'A'][0], &ascii_5x7[CHAR_A_INDEX + 'W' - 'A'][0], \
		 								 &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};															//SW:		
		 
	const uint8_t *char1mh_5x7[3] = {&ascii_5x7[CHAR_0_INDEX + 1][0], &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};	//1:
	const uint8_t *char2mh_5x7[3] = {&ascii_5x7[CHAR_0_INDEX + 2][0], &ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};	//2:		
	const uint8_t *charmh_5x7[2] = {&ascii_5x7[CHAR_MAOHAO_INDEX][0], NULL,};				//:		
	const uint8_t *charP_5x7[2] = {&ascii_5x7[CHAR_A_INDEX + 'P' - 'A'][0], NULL,};			//P  
	//const uint8_t *charP_5x12[2] = {charP_5x12, NULL,};										//P  
	const uint8_t *char1_8_5x7[9] = {&ascii_5x7[CHAR_0_INDEX + 1][0], &ascii_5x7[CHAR_0_INDEX + 2][0], &ascii_5x7[CHAR_0_INDEX + 3][0], \
									 &ascii_5x7[CHAR_0_INDEX + 4][0], &ascii_5x7[CHAR_0_INDEX + 5][0], &ascii_5x7[CHAR_0_INDEX + 6][0], \
									 &ascii_5x7[CHAR_0_INDEX + 7][0], &ascii_5x7[CHAR_0_INDEX + 8][0], NULL,};				//12345678 test used	   

	//8*16 �ַ���
	const uint8_t *char1_8x16[2] = {&ascii_8x16[CHAR_0_INDEX + 1][0], NULL,};				//1
	const uint8_t *char2_8x16[2] = {&ascii_8x16[CHAR_0_INDEX + 2][0], NULL,};				//2
	const uint8_t *charmh_8x16[2] = {&ascii_8x16[CHAR_MAOHAO_INDEX][0], NULL,};				//:
	const uint8_t *charP_8x16[2] = {&ascii_8x16[CHAR_A_INDEX + 'P' - 'A'][0], NULL,};		//P
	const uint8_t *char1mh_8x16[3] = {&ascii_8x16[CHAR_0_INDEX + 1][0], &ascii_8x16[CHAR_MAOHAO_INDEX][0], NULL,};	//1:
	const uint8_t *char2mh_8x16[3] = {&ascii_8x16[CHAR_0_INDEX + 2][0], &ascii_8x16[CHAR_MAOHAO_INDEX][0], NULL,};	//2:
	
	const uint8_t *char1_8x16_xiao5[2] = {char1_xiao5, NULL,};								//1 ����1��������ʾ��
	const uint8_t *char2_8x16_xiao5[2] = {char2_xiao5, NULL,};								//2 ����1��������ʾ��
								  
	const uint8_t *charHandTest1_8x16[14] =                   //<Hand Test 1>
	{							  
		&ascii_8x16[CHAR_ZKHZ_INDEX][0], 			\
		&ascii_8x16[CHAR_A_INDEX + 'H' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX][0], 			  	\
		&ascii_8x16[CHAR_a_INDEX + 'n' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'd' - 'a'][0],	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_0_INDEX + 1][0],		  	\
		&ascii_8x16[CHAR_ZKHY_INDEX][0], 			\
		NULL,
	};		
	const uint8_t *charHandTest2_8x16[14] =                   //<Hand Test 2>
	{							  
		&ascii_8x16[CHAR_ZKHZ_INDEX][0], 			\
		&ascii_8x16[CHAR_A_INDEX + 'H' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX][0], 			  	\
		&ascii_8x16[CHAR_a_INDEX + 'n' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'd' - 'a'][0],	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_0_INDEX + 2][0],		  	\
		&ascii_8x16[CHAR_ZKHY_INDEX][0], 			\
		NULL,
	};		
	const uint8_t *charAutoTest_8x16[12] =                   //<Auto Test>
	{							  
		&ascii_8x16[CHAR_ZKHZ_INDEX][0], 			\
		&ascii_8x16[CHAR_A_INDEX][0], 				\
		&ascii_8x16[CHAR_a_INDEX + 'u' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'o' - 'a'][0],	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_8x16[CHAR_ZKHY_INDEX][0], 			\
		NULL,
	};		
	const uint8_t *charTestModeRF1_8x16[16] =                //Test Mode (RF1)
	{							  
		&ascii_8x16[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_A_INDEX + 'M' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'o' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'd' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_YKHZ_INDEX][0], 			\
		&ascii_8x16[CHAR_A_INDEX + 'R' - 'A'][0], 	\
		&ascii_8x16[CHAR_A_INDEX + 'F' - 'A'][0], 	\
		&ascii_8x16[CHAR_0_INDEX + 1][0],		  	\
		&ascii_8x16[CHAR_YKHY_INDEX][0], 			\
		NULL,
	};										
	const uint8_t *charTestModeRF2_8x16[16] =                //Test Mode (RF2)
	{							  
		&ascii_8x16[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_A_INDEX + 'M' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'o' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'd' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_YKHZ_INDEX][0], 			\
		&ascii_8x16[CHAR_A_INDEX + 'R' - 'A'][0], 	\
		&ascii_8x16[CHAR_A_INDEX + 'F' - 'A'][0], 	\
		&ascii_8x16[CHAR_0_INDEX + 2][0],		  	\
		&ascii_8x16[CHAR_YKHY_INDEX][0], 			\
		NULL,
	};			
	const uint8_t *charTestModeTwo_8x16[16] =                //Test Mode (Two)
	{							  
		&ascii_8x16[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_A_INDEX + 'M' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'o' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'd' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_8x16[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_8x16[CHAR_YKHZ_INDEX][0], 			\
		&ascii_8x16[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'w' - 'a'][0], 	\
		&ascii_8x16[CHAR_a_INDEX + 'o' - 'a'][0], 	\
		&ascii_8x16[CHAR_YKHY_INDEX][0], 			\
		NULL,
	};	
	
	//char 5*7
		const uint8_t *charHandTest1_5x7[14] =                   //<Hand Test 1>
	{							  
		&ascii_5x7[CHAR_ZKHZ_INDEX][0], 			\
		&ascii_5x7[CHAR_A_INDEX + 'H' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX][0], 			  	\
		&ascii_5x7[CHAR_a_INDEX + 'n' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'd' - 'a'][0],	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_0_INDEX + 1][0],		  	\
		&ascii_5x7[CHAR_ZKHY_INDEX][0], 			\
		NULL,
	};		
	const uint8_t *charHandTest2_5x7[14] =                   //<Hand Test 2>
	{							  
		&ascii_5x7[CHAR_ZKHZ_INDEX][0], 			\
		&ascii_5x7[CHAR_A_INDEX + 'H' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX][0], 			  	\
		&ascii_5x7[CHAR_a_INDEX + 'n' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'd' - 'a'][0],	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_0_INDEX + 2][0],		  	\
		&ascii_5x7[CHAR_ZKHY_INDEX][0], 			\
		NULL,
	};		
	const uint8_t *charAutoTest_5x7[12] =                   //<Auto Test>
	{							  
		&ascii_5x7[CHAR_ZKHZ_INDEX][0], 			\
		&ascii_5x7[CHAR_A_INDEX][0], 				\
		&ascii_5x7[CHAR_a_INDEX + 'u' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'o' - 'a'][0],	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_5x7[CHAR_ZKHY_INDEX][0], 			\
		NULL,
	};		
	const uint8_t *charTestModeRF1_5x7[16] =                //Test Mode (RF1)
	{							  
		&ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_A_INDEX + 'M' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'o' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'd' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_YKHZ_INDEX][0], 			\
		&ascii_5x7[CHAR_A_INDEX + 'R' - 'A'][0], 	\
		&ascii_5x7[CHAR_A_INDEX + 'F' - 'A'][0], 	\
		&ascii_5x7[CHAR_0_INDEX + 1][0],		  	\
		&ascii_5x7[CHAR_YKHY_INDEX][0], 			\
		NULL,
	};										
	const uint8_t *charTestModeRF2_5x7[16] =                //Test Mode (RF2)
	{							  
		&ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_A_INDEX + 'M' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'o' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'd' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_YKHZ_INDEX][0], 			\
		&ascii_5x7[CHAR_A_INDEX + 'R' - 'A'][0], 	\
		&ascii_5x7[CHAR_A_INDEX + 'F' - 'A'][0], 	\
		&ascii_5x7[CHAR_0_INDEX + 2][0],		  	\
		&ascii_5x7[CHAR_YKHY_INDEX][0], 			\
		NULL,
	};			
	const uint8_t *charTestModeTwo_5x7[16] =                //Test Mode (Two)
	{							  
		&ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 's' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 't' - 'a'][0], 	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_A_INDEX + 'M' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'o' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'd' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'e' - 'a'][0], 	\
		&ascii_5x7[CHAR_SPACE_INDEX][0], 		  	\
		&ascii_5x7[CHAR_YKHZ_INDEX][0], 			\
		&ascii_5x7[CHAR_A_INDEX + 'T' - 'A'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'w' - 'a'][0], 	\
		&ascii_5x7[CHAR_a_INDEX + 'o' - 'a'][0], 	\
		&ascii_5x7[CHAR_YKHY_INDEX][0], 			\
		NULL,
	};																															
#else
	//����1
	extern const uint8_t *tian_xian1[3];
	extern const uint8_t *dianyuan1[3];
	extern const uint8_t *tongdao1[3];
	extern const uint8_t *haotongdao1[4];
	extern const uint8_t *zhengchang1[3];
	extern const uint8_t *gaojing1[3];
	extern const uint8_t *weijie1[3];
	extern const uint8_t *duanlu1[3];
	extern const uint8_t *zhukong1[3];
	extern const uint8_t *shuru1[3];
	extern const uint8_t *zhuchu1[3];
	extern const uint8_t *congchu1[3];
	extern const uint8_t *zhujishuchu1[5];
	extern const uint8_t *congjishuchu1[5];
	extern const uint8_t *nvlisousuozhong1[6];
	extern const uint8_t *wucongji1[4];
	extern const uint8_t *zhujipanglu1[5];
	extern const uint8_t *zhengzaisouxing1[5];
	extern const uint8_t *shi1[2];
	extern const uint8_t *fou1[2];
	extern const uint8_t *shebeiztxx1[7];
	extern const uint8_t *shebeizt1[5];
	extern const uint8_t *banben1[3];
	
	//����2
	extern const uint8_t *tian_xian2[3];
	extern const uint8_t *dianyuan2[3];
	extern const uint8_t *tongdao2[3];
	extern const uint8_t *haotongdao2[4];
	extern const uint8_t *zhengchang2[3];
	extern const uint8_t *gaojing2[3];
	extern const uint8_t *weijie2[3];
	extern const uint8_t *duanlu2[3];
	extern const uint8_t *zhukong2[3];
	extern const uint8_t *shuru2[3];
	extern const uint8_t *zhuchu2[3];
	extern const uint8_t *congchu2[3];
	extern const uint8_t *zhujishuchu2[5];
	extern const uint8_t *congjishuchu2[5];
	extern const uint8_t *nvlisousuozhong2[6];
	extern const uint8_t *wucongji2[4];
	extern const uint8_t *zhujipanglu2[5];
	extern const uint8_t *zhengzaisouxing2[5];
	extern const uint8_t *shi2[2];
	extern const uint8_t *fou2[2];
	extern const uint8_t *shebeiztxx2[7];
	extern const uint8_t *shebeizt2[5];
	extern const uint8_t *banben2[3];
	
	//5*7 �ַ���
	extern const uint8_t *zyd_null[7];
	extern const uint8_t *zyd_1dot[2];
	extern const uint8_t *zyd_2dot[3];
	extern const uint8_t *zyd_3dot[4];	
	extern const uint8_t *zyd_4dot[5];
	extern const uint8_t *zyd_5dot[6];
	extern const uint8_t *zyd_6dot[7];
	
	extern const uint8_t *yjt_null[7];
	extern const uint8_t *yjt_1dot[2];
	extern const uint8_t *yjt_2dot[3];
	extern const uint8_t *yjt_3dot[4];	
	extern const uint8_t *yjt_4dot[5];
	extern const uint8_t *yjt_5dot[6];
	extern const uint8_t *yjt_6dot[7];	
	
	extern const uint8_t *charBD_5x7[3];
	extern const uint8_t *charGPS_5x7[4];
	extern const uint8_t *charMaxmh_5x7[5];
	extern const uint8_t *charMinmh_5x7[5];
	
	extern const uint8_t *charMCBkmh_5x7[6];
	extern const uint8_t *charRFXYmh_5x7[6];	
	extern const uint8_t *charPD8Mmh_5x7[6];
	extern const uint8_t *charPD8Smh_5x7[6];
	
	extern const uint8_t *charHWmh_5x7[4];
	extern const uint8_t *charSWmh_5x7[4];
	
	extern const uint8_t *char1mh_5x7[3];
	extern const uint8_t *char2mh_5x7[3];
	
	extern const uint8_t *charmh_5x7[2];
	extern const uint8_t *charP_5x7[2];
	//extern const uint8_t *charP_5x12[2];
	extern const uint8_t *char1_8_5x7[9];
	
	extern const uint8_t *charHandTest1_5x7[14];
	extern const uint8_t *charHandTest2_5x7[14];
	extern const uint8_t *charAutoTest_5x7[12]; 
	extern const uint8_t *charTestModeRF1_5x7[16];
	extern const uint8_t *charTestModeRF2_5x7[16];
	extern const uint8_t *charTestModeTwo_5x7[16];
	
	//8*16 �ַ���
	extern const uint8_t *char1_8x16[2];
	extern const uint8_t *char2_8x16[2];
	extern const uint8_t *charmh_8x16[2];
	extern const uint8_t *charP_8x16[2];
	extern const uint8_t *char1mh_8x16[3];
	extern const uint8_t *char2mh_8x16[3];
	extern const uint8_t *charHandTest1_8x16[14];
	extern const uint8_t *charHandTest2_8x16[14];
	extern const uint8_t *charAutoTest_8x16[12]; 
	extern const uint8_t *charTestModeRF1_8x16[16];
	extern const uint8_t *charTestModeRF2_8x16[16];
	extern const uint8_t *charTestModeTwo_8x16[16];
	
	extern const uint8_t *char1_8x16_xiao5[2];
	extern const uint8_t *char2_8x16_xiao5[2];
	
#endif
/*
********************************************************************************
˽�к�������
********************************************************************************
*/
void hmu_pwr_on_page(void); 
void hmu_main_page(void); 	
void hmu_sub1_page(void); 	
void hmu_sub2_page(void); 	
void hmu_sub3_page(void); 	
void hmu_ch1_test_page(void);
void hmu_ch2_test_page(void);
void hmu_auto_test_page(void);
void hmu_dev_fault_page(void);
void hmu_dev_para_page(void);
//
void pwr_on_page_rfb_jx1_disp(void);
void pwr_on_page_rfb_jx2_disp(void);
void pwr_on_page_xyb_jx1_disp(void);	
void pwr_on_page_xyb_jx2_disp(void);	
void pwr_on_page_key_deal(void);
//
void main_page_rfb_jx1_disp(void);
void main_page_rfb_jx2_disp(void);
void main_page_xyb_jx1_disp(void);	
void main_page_xyb_jx2_disp(void);	
void main_page_key_deal(void);
//
void sub1_page_rfb_jx1_disp(void);
void sub1_page_rfb_jx2_disp(void);
void sub1_page_xyb_jx1_disp(void);	
void sub1_page_xyb_jx2_disp(void);	
void sub1_page_key_deal(void);
//
void sub2_page_rfb_jx1_disp(void);
void sub2_page_rfb_jx2_disp(void);
void sub2_page_xyb_jx1_disp(void);	
void sub2_page_xyb_jx2_disp(void);	
void sub2_page_key_deal(void);
//
void sub3_page_rfb_jx1_disp(void);
void sub3_page_rfb_jx2_disp(void);
void sub3_page_xyb_jx1_disp(void);	
void sub3_page_xyb_jx2_disp(void);	
void sub3_page_key_deal(void);
//
void ch1t_page_rfb_jx1_disp(void);
void ch1t_page_rfb_jx2_disp(void);
void ch1t_page_xyb_jx1_disp(void);	
void ch1t_page_xyb_jx2_disp(void);	
void ch1t_page_key_deal(void);
//
void ch2t_page_rfb_jx1_disp(void);
void ch2t_page_rfb_jx2_disp(void);
void ch2t_page_xyb_jx1_disp(void);	
void ch2t_page_xyb_jx2_disp(void);	
void ch2t_page_key_deal(void);
//
void auto_page_rfb_jx1_disp(void);
void auto_page_rfb_jx2_disp(void);
void auto_page_xyb_jx1_disp(void);	
void auto_page_xyb_jx2_disp(void);	
void auto_page_key_deal(void);
//
void fault_page_rfb_jx1_disp(void);
void fault_page_rfb_jx2_disp(void);
void fault_page_xyb_jx1_disp(void);	
void fault_page_xyb_jx2_disp(void);	
void fault_page_key_deal(void);
//


#ifdef __cplusplus
	}
#endif

#endif
