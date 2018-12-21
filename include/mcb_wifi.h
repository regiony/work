/*
********************************************************************************
�ļ���	��mcb_wifi.h
�ļ����������ذ���WIFIģ��ͨ��ͷ�ļ�
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
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
ͷ�ļ�
********************************************************************************
*/
#include "stm32f0xx.h"

/*
********************************************************************************
�궨��
********************************************************************************
*/
#define MCB_WIFI_NW_BR				57600		//��������������
#define MCB_WIFI_RU_BR				1000000		//Զ������������

#define WIFI_COMM_RX_BUF_LEN		235			//�����ɸ���Э���֡�Ż���RAM����ʱ
#define WIFI_COMM_TX_BUF_LEN		235			//�����ɸ���Э���֡�Ż���RAM����ʱ
#define WIFI_BUF_MAX_LEN			230			//��������󳤶�230

#define WIFI_COMM_SYN_CHAR_1		0xAA		//ͬ����12����
#define WIFI_COMM_SYN_CHAR_2		0x55
#define WIFI_COMM_OK				0xA5
#define WIFI_COMM_FAIL				0x5A

//ascii��Ϣ����Э��
//wifi - mcu ��ѯ��������к����У�
//GET+�ո�+��ѯ�ֶ���+�ո�+��ѯ�ֶεı���[+�ո�+�ֶα���+��]+*
//GET+�ո�+��ѯ�ֶ���+�ո�+��ѯ�ֶεı���+�ո�+��ѯ�ֶη���ֵ[+�ո�+�ֶα���+�ո�+��ѯ�ֶη���ֵ��]+*
//wifi - mcu ������������к����У�
//SET+�ո�+�����ֶ���+�ո�+�����ֶεı���+�ո�+�����ֶ�ֵ[+�ո�+�ֶα���+�����ֶ�ֵ��]+*
//SET+�ո�+�����ֶ���+�ո�+���óɰܵı�־(OK/FAIL)+*
//mcu - wifi mcu�����ϱ��澯��Ϣ��wifi�����У�
//WARN+�ո�+�澯���ͱ���+�ո�+����/��ʧ��־+�ո�+ʱ��+�ո�+��ֵ+*
#define WIFI_MCU_GET_STR			"GET"		//ͬ����GET�ַ�������
#define WIFI_MCU_SET_STR			"SET"		//ͬ����SET�ַ�������
#define WIFI_MCU_SPACE_CHAR			' '			//�ո��ַ�����
#define WIFI_MCU_STAR_CHAR			'*'			//�Ǻ��ַ�����
#define WIFI_MCU_POUND_CHAR			'#'			//�����ַ�����
//char *GET_CMD_STR[GET_CMD_NUM] = {"LGTD","LTTD","STCT","MNGN","IPSN","IPCN","IPTP","IPBP","FDDV","L1OP","L2OP",};   	
//char *SET_CMD_STR[SET_CMD_NUM] = {"ATTE","MNGN","IPCN","IPTP","IPBP","FDDV","R1GN","R2GN","RDGN","PMGN","PSGN","YZMD",};   		
//char *WARN_DEF_STR[WARN_DEF_NUM] = {"MATA","SATA","MAPW","SAPW","PTBD","OPPT","CTBD",};   
//GET CMD
#define GET_CMD_NUM					16
#define GET_LGTD					1			//����
#define GET_LTTD					2			//γ��
#define GET_STCT					3			//����
#define GET_MNGN					4			//������
#define GET_IPSN					5			//�����ź�?�����avg
#define GET_IPCN					6			//����ͨ��
#define GET_IPTP					7			//��������0-����,1-�ֶ�,2-�Զ�,4-ͨ���Լ�
#define GET_IPBP					8			//������·
#define GET_FDDV					9			//�����豸
#define GET_L1OP					10			//�����˹��ְ����
#define GET_L2OP					11			//�ӻ��˹��ְ����
#define GET_R1GN					12			//��Ƶ��ͨ��1����-˥��      											
#define GET_R2GN					13			//��Ƶ��ͨ��2����-˥��       											
#define GET_RDGN					14			//��Ƶ����ͨ������-˥��      											
#define GET_PMGN					15			//�����˹��ְ��������-˥�������ͨ��������-˥��									
#define GET_PSGN					16			//�ӻ��˹��ְ��������-˥�������ͨ��������-˥��		
//SET CMD
#define SET_CMD_NUM					12
#define SET_ATTE					50			//˥��            											
#define SET_MNGN					51			//������           											
#define SET_IPCN					52			//����ͨ��            											
#define SET_IPTP					53			//�����Զ�ģʽ        											
#define SET_IPBP					54			//������· 											
#define SET_FDDV					55			//�����豸        											
#define SET_R1GN					56			//��Ƶ��ͨ��1����-˥��      											
#define SET_R2GN					57			//��Ƶ��ͨ��2����-˥��       											
#define SET_RDGN					58			//��Ƶ����ͨ������-˥��      											
#define SET_PMGN					59			//�����˹��ְ�����-˥��											
#define SET_PSGN					60			//�ӻ��˹��ְ�����-˥��											
#define SET_YZMD					61	        //������ʾģʽ
//WARN
//#define WARN_DEF_NUM				7
//#define WARN_MATA					100			//������          
//#define WARN_SATA					101         //������        
//#define WARN_MAPW					102         //����Դ           
//#define WARN_SAPW					103         //�ӵ�Դ         
//#define WARN_PTBD					104         //Э���
//#define WARN_OPPT					105         //����˿�    
//#define WARN_CTBD					106         //���ư�      


#define	WIFI_RECV_TIME_OUT			0x01		//���ճ�ʱλ����
#define	WIFI_RECV_CHKSUM_ERR		0x02		//У��ʹ���λ����
#define WIFI_RECV_TIME_OUT_CNT		50			//50*10ms=500ms������WIFI�ظ���ʱʱ�䣬������Э����
#define WIFI_BD_RECV_TO_CNT			20			//30*10=300ms����������WIFI���ݳ�ʱʱ�䣬ASCII��Ͷ����ƶ���
#define WIFI_RECV_TO_ALLOW_NUM		3			//���ճ�ʱ�����ط�����

//���������
#define WIFI_HFSPD8SLDZ_CMD_7A		0x7A		//�ظ��ӻ��˹��ְ�����������ַ
#define WIFI_HFRFBHXYB_CMD_7B		0x7B		//�ظ�����Ƶ�廹��Э���
#define WIFI_ZDTZSJCG_CMD_7C		0x7C		//����֪ͨ�����ɹ�
#define WIFI_HFJRSJQR_CMD_7D		0x7D		//�ظ���������ȷ��
#define WIFI_ZDTZTCYCSJ_CMD_7E		0x7E		//����֪ͨ�˳�Զ������
#define WIFI_HFSBWJM_CMD_7F			0x7F		//�ظ��豸�ļ���
//���������                          	
#define WIFI_QRSBFSSS_CMD_80		0x80		//ȷ���豸���͵���������
#define WIFI_QQSPD8SLDZ_CMD_FC		0xFC		//�����ȡ�ӻ��˹��ְ�����������ַ
#define WIFI_QQRFBHXYB_CMD_FD		0xFD		//�����ȡ����Ƶ�廹��Э���
#define WIFI_QQDQSBWJM_CMD_FE		0xFE		//�����ȡ�豸�ļ���
#define WIFI_QQJRYCSJ_CMD_FF		0xFF		//�������Զ������

//ͨ�Žṹ�嶨��
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
������������
********************************************************************************
*/
__MCB_WIFI_EXE uint32_t mcb_wifi_task_flag;		//���ذ���WIFIģ��ͨ�������־
__MCB_WIFI_EXE uint32_t mcb_wifi_ru_state;		//Զ������״̬��=0����������, bit6~bit0: 1��xxb������
						//0-��������״̬��bit7-resv=0, bit6-mcb��bit5-pd8ms��bit4-pd8s��bit3-pd8m��bit2-xybf��bit1-xybm��bit0-rfb
__MCB_WIFI_EXE uint32_t mcb_wifi_ru_xxb;		//�����İ� bit7-resv=0, bit6-mcb��bit5-pd8ms��bit4-pd8s��bit3-pd8m��bit2-xybf��bit1-xybm��bit0-rfb
__MCB_WIFI_EXE uint32_t mcb_wifi_sys_tx_cmd;	//�������ϱ����� 7C / 7E	

/*
********************************************************************************
������������
********************************************************************************
*/
void mcb_wifi_init(void);						//���ذ���WIFIģ��ͨ�ų�ʼ��
void mcb_wifi_task(void);						//���ذ���WIFIģ��ͨ������

/*
********************************************************************************
˽�б�������
********************************************************************************
*/
__MCB_WIFI_EXE WIFI_COMM_TYPE_DEF wifi_comm_ru_data;	//Զ������ͨ�Žṹ�����
__MCB_WIFI_EXE WIFI_COMM_TYPE_DEF wifi_comm_info_data;	//��Ϣ����ͨ�Žṹ�����
__MCB_WIFI_EXE WIFI_COMM_TYPE_DEF wifi_comm_alm_data;	//��������ͨ�Žṹ�����
__MCB_WIFI_EXE uint8_t wifi_comm_recv_end_flag;			//������ɱ�־
__MCB_WIFI_EXE uint8_t wifi_comm_link_tx_flag;			//��·���ͱ�־
__MCB_WIFI_EXE uint8_t wifi_comm_sys_tx_flag;			//ϵͳ���ͱ�־
__MCB_WIFI_EXE uint8_t wifi_comm_sys_tx_to_flag;		//ϵͳ���ͳ�ʱ��־
__MCB_WIFI_EXE uint8_t wifi_comm_data_deal_flag;		//���ݴ����б�־
__MCB_WIFI_EXE uint8_t wifi_comm_time_out_cnt;			//ϵͳ���ͳ�ʱ������
__MCB_WIFI_EXE uint8_t wifi_comm_time_out_en;			//ϵͳ���ͳ�ʱ�������־
__MCB_WIFI_EXE uint8_t wifi_comm_send_recv_end;			//һ�η������������־����������֡����
__MCB_WIFI_EXE uint8_t wifi_comm_sys_tx_end;			//��Ϣ���ݷ��ͱ�־
__MCB_WIFI_EXE uint8_t wifi_to_cnt1;                    //

__MCB_WIFI_EXE uint8_t mcb_wifi_recv_cp_cnt;			//ͨ��Э���жϼ�����
__MCB_WIFI_EXE uint8_t mcb_wifi_recv_cp_arr[3];			//ͨ��Э���жϽ��ջ�����
__MCB_WIFI_EXE uint8_t mcb_wifi_recv_cp_type;			//����ͨ��Э������ 1��ASCII��Ϣ����Э�飬2��BINARY��������Э��
__MCB_WIFI_EXE uint8_t mcb_wifi_ru_deal_flag;			//�������ݴ����־
__MCB_WIFI_EXE uint8_t mcb_wifi_info_deal_flag;			//��Ϣ���ݴ����־
__MCB_WIFI_EXE uint8_t mcb_wifi_ru_send_flag;			//�������ݷ��ͱ�־
__MCB_WIFI_EXE uint8_t mcb_wifi_info_send_flag;			//��Ϣ���ݷ��ͱ�־
__MCB_WIFI_EXE uint8_t mcb_wifi_alm_send_flag;			//�������ݷ��ͱ�־
__MCB_WIFI_EXE uint8_t mcb_wifi_set_succ; 				//���óɹ���־


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
˽�к�������
********************************************************************************
*/
void usart1_init(uint32_t br);
void wifi_comm_ru_data_deal(void);						//�������ݽ���������
void wifi_comm_info_data_deal(void);					//��Ϣ���ݽ���������
void wifi_comm_data_send(void);							//��Ϣ�������ݺ���
void wifi_comm_sys_ru_send(void);						//ϵͳ�����������ݺ���
void wifi_comm_sys_warn_send(void);						//ϵͳ���͸澯���ݺ���

//����
void wifi_cmd_80_deal(void);
void wifi_cmd_FC_deal(void);
void wifi_cmd_FD_deal(void);
void wifi_cmd_FE_deal(void);
void wifi_cmd_FF_deal(void);
//
//����
void wifi_cmd_7A_deal(void);							//���������֡����
void wifi_cmd_7B_deal(void);							//ͬ��...
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
