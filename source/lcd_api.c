/*
********************************************************************************
�ļ���	��lcd_api.c
�ļ�������LCD API,�����˻�Ӧ�õ��õĲ�����������
������	�������
�������ڣ�2016.12.15
�޸����ݣ�
�޸���	��
�޸����ڣ�
********************************************************************************
*/

#define __LCD_API_GLOBAL
#include "lcd_if.h" 
#include "lcd_drv.h"  
#include "lcd_api.h"                 
  

/*
********************************************************************************
�������ƣ�lcd_api_init
����������LCD API ��ʼ��
����������
����ֵ	��
********************************************************************************
*/
void lcd_api_init(void) 
{
	lcd_drv_init();
	
}

/*
********************************************************************************
�������ƣ�lcd_clear_
����������LCD����
����������
����ֵ	��
********************************************************************************
*/
void lcd_clear_(void)
{
	uint8_t page, column;
	
	for(page = LCD_PAGE_BADDR; page < (LCD_PAGE_BADDR + LCD_PAGE_NUM); page++)
	{
		lcd_wr_cmd(page);
		lcd_wr_cmd(LCD_COLU_HADDR);
		lcd_wr_cmd(LCD_COLU_LADDR);
		for(column = 0; column < LCD_COLUMN_NUM; column++)
			lcd_wr_data(0x00);
	}
}	


/*
********************************************************************************
�������ƣ�lcd_clear
����������LCD��buf
����������
����ֵ	��
********************************************************************************
*/
void lcd_clear(void)
{
	uint32_t i;
	
	for(i = 0; i < DISPLAY_BUF_SIZE; i++)
		display_buf[i] = 0x00;
}

/*
********************************************************************************
�������ƣ�lcd_display
����������LCD��ʾ
����������
����ֵ	��
********************************************************************************
*/
void lcd_display(void)
{
	uint8_t page, column;
	
	for(page = 0; page < LCD_PAGE_NUM; page++)
	{
		lcd_wr_cmd(LCD_PAGE_BADDR + page);
		lcd_wr_cmd(LCD_COLU_HADDR); 
		lcd_wr_cmd(LCD_COLU_LADDR);
		for(column = 0; column < LCD_COLUMN_NUM; column++)
			lcd_wr_data(display_buf[(page << 7) + column]);
	}	
}

/*
********************************************************************************
�������ƣ�lcd_set_xy
����������LCD��������
����������
����ֵ	��
********************************************************************************
*/
void lcd_set_xy(uint8_t line, uint8_t column)
{ 
	lcd_wr_cmd(LCD_LINE_BADDR | line);
	lcd_wr_cmd(LCD_COLU_HADDR | (column >> 4));
	lcd_wr_cmd(LCD_COLU_LADDR | (column & 0x0F));
}

/*
********************************************************************************
�������ƣ�lcd_draw_dot
����������LCD���
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_dot(uint8_t line, uint8_t column, uint8_t color)
{
	uint8_t bit, page;
	uint16_t addr;       
	
	page = line >> 0x03;		
	bit = line & 0x07;  
	addr = (page << 7) + column; 
	
	if(color == 0)
		display_buf[addr] &= ~(1 << bit);
	else
	if(color == 1)
		display_buf[addr] |= (1 << bit);	
}

/*
********************************************************************************
�������ƣ�lcd_draw_char_5x7
����������LCD��5*7�ַ���
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_char_5x7(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt)
{
	uint8_t i, j, nbit;

	while(*pt != NULL)
	{		 
		for(i = 0; i < 5; i++) 
		{
			for(nbit = 0; nbit < 8; nbit++) 
			{
				j = *(*pt + i) & (1 << nbit);
				if(j)
					j = 1;
				else
					j = 0;
				lcd_draw_dot(line++, column, j);
			}
			//(*pt)++;	
			line -= 8;
			column++;
	   	}
	   	column += gap;
		pt++;
	}	
}

/*
********************************************************************************
�������ƣ�lcd_draw_char_8x16
����������LCD��8*16�ַ���
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_char_8x16(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt)
{
	uint8_t i, j, z, nbit;
	
	while(*pt != NULL)
	{
		for(j = 0; j < 2; j++) 			 			/*deal with /8 part */
		{
			for(i = 0; i < 8; i++) 
			{
				for(nbit = 0; nbit < 8; nbit++) 
				{
					z = *(*pt + i + (j << 3)) & (1 << nbit);
					if(z)
						z = 1;
					else
						z = 0;
					lcd_draw_dot(line++, column, z);
				}
				line-=8;
				column++;
                //(*pt)++;
			}
			line += 8;
			column -= 8;
		}
		line -= 16;
		column += 8;
		column += gap;
		pt++;		
	}	
}

/*
********************************************************************************
�������ƣ�lcd_draw_hz_nx16_1
����������LCD��n*16���ִ�������1
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_hz_nx16_1(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt)
{
	uint8_t i, z, nbit;
	
	while(*pt != NULL)
	{
		for(i = 0; i < ZITI_NUM_1; i++) 
		{
			for(nbit = 0; nbit < 8; nbit++) 
			{
				z = *(*pt + i) & (1 << nbit);
				if(z)
					z = 1;
				else
					z = 0;
				lcd_draw_dot(line++, column, z);
			}
			line -= 8;
			column++;
			//(*pt)++;
		}
		line += 8;
		column -= ZITI_NUM_1;
		for(i = 0; i < ZITI_NUM_1; i++) 
		{
			for(nbit = 0; nbit < (ZITI_NUM_1 - 8); nbit++) 
			{
				z = *(*pt + i + ZITI_NUM_1) & (1 << nbit);
				if(z)
					z = 1;
				else
					z = 0;
				lcd_draw_dot(line++, column, z);
			}
			line -= (ZITI_NUM_1 - 8);
			column++;
			//(*pt)++;
		}
		line -= 8;
		column += gap;
		pt++;
	}	
}


/*
********************************************************************************
�������ƣ�lcd_draw_hz_nx16_2
����������LCD��n*16���ִ�������2
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_hz_nx16_2(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt)
{
	uint8_t i, z, nbit;
	
	while(*pt != NULL)
	{
		for(i = 0; i < ZITI_NUM_2; i++) 
		{
			for(nbit = 0; nbit < 8; nbit++) 
			{
				z = *(*pt + i) & (1 << nbit);
				if(z)
					z = 1;
				else
					z = 0;
				lcd_draw_dot(line++, column, z);
			}
			line -= 8;
			column++;
			//(*pt)++;
		}
		line += 8;
		column -= ZITI_NUM_2;
		for(i = 0; i < ZITI_NUM_2; i++) 
		{
			for(nbit = 0; nbit < (ZITI_NUM_2 - 8); nbit++) 
			{
				z = *(*pt + i + ZITI_NUM_2) & (1 << nbit);
				if(z)
					z = 1;
				else
					z = 0;
				lcd_draw_dot(line++, column, z);
			}
			line -= (ZITI_NUM_2 - 8);
			column++;
			//(*pt)++;
		}
		line -= 8;
		column += gap;
		pt++;
	}	
}

/*
********************************************************************************
�������ƣ�lcd_draw_hz_32x32
����������LCD��32*32���ִ�
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_hz_32x32(uint8_t line, uint8_t column, uint8_t gap, const uint8_t **pt)
{
	uint8_t i, j, z, nbit;
	
	while(*pt != NULL)
	{
		for(j = 0; j < 4; j++) 			 			/*deal with /8 part */
		{
			for(i = 0; i < 32; i++) 
			{
				for(nbit = 0; nbit < 8; nbit++) 
				{
					z = *(*pt + i + (j << 5)) & (1 << nbit);
					if(z)
						z = 1;
					else
						z = 0;
					lcd_draw_dot(line++, column, z);
				}
				line -= 8;
				column++;
				//(*pt)++;
			}
			line += 8;
			column -= 32;
		}
		line -= 32;
		column += 32;
		column += gap;
		pt++;
	}		
}

/*
********************************************************************************
�������ƣ�lcd_draw_graph
������������ͼƬ
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_graph(uint8_t line, uint8_t column, uint8_t width, uint8_t high, const uint8_t *pt)
{
	uint8_t i, j, z, nbit;
	
	for(j = 0; j < (high >> 3); j++) 			 /*deal with /8 part */
	{
		for(i = 0; i < width; i++) 
		{
			for(nbit = 0; nbit < 8; nbit++) 
			{
				z = *pt & (1 << nbit);
				if(z)
					z = 1;
				else
					z = 0;
				lcd_draw_dot(line++, column, z);
			}
			line -= 8;
			column++;
			pt++;
		}
		column -= width;
		line += 8;
	}
	
	for(i = 0; i < width; i++) 					/*deal with leaving part */
	{
		for(nbit = 0; nbit < 8; nbit++)
		{
			if(nbit < (high & 0x07))
			{
				z = *pt & (1 << nbit);
				if(z)
					z = 1;
				else
					z = 0;
				lcd_draw_dot(line++, column, z);				
			}
			else
				line++;
		}	
		line -= 8;
		column++;
		pt++;
	}	
}

/*
********************************************************************************
�������ƣ�lcd_draw_hline
����������������
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_hline(uint8_t line, uint8_t column, uint8_t len)
{
	while(len--)
		lcd_draw_dot(line, column++, 1);
}

/*
********************************************************************************
�������ƣ�lcd_draw_hline
����������������
����������
����ֵ	��
********************************************************************************
*/
void lcd_clr_hline(uint8_t line, uint8_t column, uint8_t len)
{
	while(len--)
		lcd_draw_dot(line, column++, 0);
}

/*
********************************************************************************
�������ƣ�lcd_draw_vline
����������������
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_vline(uint8_t line, uint8_t column, uint8_t len)
{
	while(len--)
		lcd_draw_dot(line++, column, 1);	
}

/*
********************************************************************************
�������ƣ�lcd_draw_rect
����������������
����������
����ֵ	��
********************************************************************************
*/
void lcd_draw_rect(uint8_t line, uint8_t column, uint8_t width, uint8_t high)
{
	uint8_t i;
	
	for(i = 0; i < width; i++) 
	{
		lcd_draw_dot(line, column + i, 1);
	    lcd_draw_dot(line + high - 1, column + i, 1);	
	}
	for(i = 1; i < high - 1; i++) {
		lcd_draw_dot(line + i, column, 1);
	    lcd_draw_dot(line + i, column + width - 1, 1);
	}	
}


