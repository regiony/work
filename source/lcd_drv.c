/*
********************************************************************************
�ļ���	��lcd_drv.c
�ļ�������LCD������128*64 COG, 2��Һ��������������ST7565R-573->����1 \ UC1701X-091->����2��
		  ע����ݾ����ԣ�����1/2��ӦҺ���ͺŲ��ܻ�����������ԱȶȾͲ�������
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __LCD_DRV_GLOBAL
#include "lcd_if.h" 
#include "lcd_drv.h"    
#include "public.h"             


/*
********************************************************************************
�������ƣ�lcd_drv_init
����������
����������
����ֵ	��
********************************************************************************
*/
void lcd_drv_init(void) 
{
	lcd_if_init();
	lcd_init();
}

/*
********************************************************************************
�������ƣ�lcd_init
����������lcd��ʼ��
����������
����ֵ	��
********************************************************************************
*/
void lcd_init(void)
{
	lcd_reset();
	
	lcd_wr_cmd(0xe2);	//����λ
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	lcd_wr_cmd(0x2c);  //��ѹ����1
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();	
	lcd_wr_cmd(0x2e);  //��ѹ����2
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	lcd_wr_cmd(0x2f);  //��ѹ����3
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	lcd_wr_cmd(0x23);  //�ֵ��Աȶȣ������÷�Χ0x20��0x27
	lcd_wr_cmd(0x81);  //΢���Աȶ�
    if(dev_type == DEV_TYPE_2)
        lcd_wr_cmd(0x2a);  //0x2a,΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f ---> for 6��������  091 uc1701x
    else
    if(dev_type == DEV_TYPE_1)
        lcd_wr_cmd(0x1a);//0x1a,΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f  ---> for 5�������� 573 st7565r
	lcd_wr_cmd(0xa2);  //1/9ƫѹ�ȣ�bias��
	lcd_wr_cmd(0xc8);  //��ɨ��˳�򣺴��ϵ���
	lcd_wr_cmd(0xa0);  //��ɨ��˳�򣺴�����
	lcd_wr_cmd(0x40);  //��ʼ�У���һ�п�ʼ
	lcd_wr_cmd(0xaf);  //����ʾ
}

/*
********************************************************************************
�������ƣ�lcd_re_init
����������lcd���³�ʼ��
����������
����ֵ	��
********************************************************************************
*/
void lcd_re_init(void)
{	
	lcd_wr_cmd(0xe2);	//����λ
    /*
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	lcd_wr_cmd(0x2c);  //��ѹ����1
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();	
	lcd_wr_cmd(0x2e);  //��ѹ����2
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	lcd_wr_cmd(0x2f);  //��ѹ����3
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
    */
	lcd_wr_cmd(0x23);  //�ֵ��Աȶȣ������÷�Χ0x20��0x27
	lcd_wr_cmd(0x81);  //΢���Աȶ�
    if(dev_type == DEV_TYPE_2)
        lcd_wr_cmd(0x2a);  //0x2a,΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f ---> for 6�������� 091 uc1701x
    else
    if(dev_type == DEV_TYPE_1)
        lcd_wr_cmd(0x1a);//0x1a,΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f  ---> for 5�������� 573 st7565r
	lcd_wr_cmd(0xa2);  //1/9ƫѹ�ȣ�bias��
	lcd_wr_cmd(0xc8);  //��ɨ��˳�򣺴��ϵ���
	lcd_wr_cmd(0xa0);  //��ɨ��˳�򣺴�����
	lcd_wr_cmd(0x40);  //��ʼ�У���һ�п�ʼ
	lcd_wr_cmd(0xaf);  //����ʾ
}

/*
********************************************************************************
�������ƣ�lcd_reset
����������lcd��λ
����������
����ֵ	��
********************************************************************************
*/
void lcd_reset(void)
{
	LCD_RST_H;
	LCD_RST_L;
	delay_xms(1);
	LCD_RST_H;	
	delay_xms(10);
}

/*
********************************************************************************
�������ƣ�lcd_wr_byte
����������lcdд�ֽڣ�ʱ����⣬̫����Լ�nop
����������
����ֵ	��
********************************************************************************
*/
void lcd_wr_byte(uint8_t val)
{
	uint8_t i;
	
	for(i = 8; i > 0; i--)
	{
		LCD_SCK_L;	
		__nop();
		
		if(val & 0x80)
			LCD_DI_H;
		else
			LCD_DI_L;
		
		val <<= 1;
		
		LCD_SCK_H;
		__nop();
		//__nop();
	}
}

/*
********************************************************************************
�������ƣ�lcd_wr_cmd
����������lcdд���ʱ����⣬̫����Լ�nop
����������
����ֵ	��
********************************************************************************
*/
void lcd_wr_cmd(uint8_t cmd)
{
	LCD_CS_L;
	__nop();
	//__nop();
	LCD_A0_L;
	
	__nop();
	lcd_wr_byte(cmd);
	
	__nop();
	LCD_CS_H;
	__nop();
	LCD_A0_H;
}

/*
********************************************************************************
�������ƣ�lcd_wr_data
����������lcdд���ݣ�ʱ����⣬̫����Լ�nop
����������
����ֵ	��
********************************************************************************
*/
void lcd_wr_data(uint8_t data)
{
	LCD_CS_L;
	__nop();
	//__nop();
	LCD_A0_H;
	
	__nop();
	lcd_wr_byte(data);
	
	__nop();
	LCD_CS_H;
	__nop();
	LCD_A0_H;
}