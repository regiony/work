/*
********************************************************************************
文件名	：key.c
文件描述：按键程序
建立人	：杨瑞军
建立日期：2016.12.15
修改内容：
修改人	：
修改日期：
********************************************************************************
*/

#define __KEY_GLOBAL
#include "key.h"    
#include "lcd_if.h" 
#include "mcb_hmu.h" 

/*
********************************************************************************
函数名称：key_init
函数描述：key初始化
参数描述：
返回值	：
********************************************************************************
*/
void key_init(void)
{

	key_press_flag = 0;
	key_timer_en = 0;
	key_timer_cnt = 0;
	key_timer_en1 = 0;
	key_timer_cnt1 = 0;	
	key_press_val = KEY_NO;
	key_value = NO_KEY;
		
	key_gpio_int_init();
	
	
}

/*
********************************************************************************
函数名称：key_scan
函数描述：key扫描
参数描述：
返回值	：
********************************************************************************
*/
void key_scan(void)
{
	if(key_press_flag)
	{		
		if(mcb_hmu_bl_state)						//背光判断
		{
			mcb_hmu_bl_state = 0;				
			LCD_BL_ON;								//打开
			//if((RD_GPIOB & KEY_PBIO) == KEY_RIGHT)	//若右键开背光，功能忽略
			//{
            key_press_val = KEY_NO;
			key_press_flag = 0;
			key_timer_en = 0;	
			key_timer_cnt = 0;		
			key_timer_en1 = 0;	
			key_timer_cnt1 = 0;	
			return;			
			//}
		}
		
		if(key_timer_en)
		{
			if(key_timer_cnt < KEY_JITTER_CNT)		//消抖
				return ;			
		}
		if(key_timer_en1)
		{
			if(key_timer_cnt1 < KEY_JITTER_CNT)		//消抖
				return ;			
		}

		//B口按键判断		
		if((RD_GPIOB & KEY_PBIO) != KEY_PBIO)		
		{
			key_press_val = RD_GPIOB & KEY_PBIO;
			if(key_timer_cnt >= KEY_2S_CNT)		//B口长按键判断
			{
				switch(key_press_val)
				{
					case KEY_UP: 	key_value = UP_L_KEY; 		break;
					case KEY_RIGHT: key_value = RIGHT_L_KEY; 	break;
					case KEY_OK: 	key_value = OK_L_KEY; 		break;
					case KEY_LEFT: 	key_value = LEFT_L_KEY; 	break;
					case KEY_DOWN:	key_value = DOWN_L_KEY; 	break;
					case KEY_UPDW:	key_value = UD_L_KEY; 		break;
					case KEY_LFRI: 	key_value = LR_L_KEY; 		break;
					default: 		key_value = NO_KEY;		 	break;												
				}
                key_press_val = KEY_NO;
				key_press_flag = 0;
				key_timer_en = 0;	
				key_timer_cnt = 0;
			}
		}
		else
		{
			if(key_timer_cnt >= KEY_200MS_CNT)	//B口短按键判断	
			{
				switch(key_press_val)
				{
					case KEY_NO: 	key_value = NO_KEY; 		break;
					case KEY_UP: 	key_value = UP_S_KEY; 		break;
					case KEY_RIGHT: key_value = RIGHT_S_KEY; 	break;
					case KEY_OK: 	key_value = OK_S_KEY; 		break;
					case KEY_LEFT: 	key_value = LEFT_S_KEY; 	break;
					case KEY_DOWN:	key_value = DOWN_S_KEY; 	break;
					case KEY_UPDW:	key_value = UD_S_KEY; 		break;
					case KEY_LFRI: 	key_value = LR_S_KEY; 		break;
					default: 		key_value = NO_KEY; 		break;					
				}
                key_press_val = KEY_NO;
				key_press_flag = 0;
				key_timer_en = 0;	
				key_timer_cnt = 0;
			}
            /*
			else
			if(((RD_GPIOB & KEY_PBIO) == KEY_PBIO) && ((RD_GPIOC & KEY_PCIO) == KEY_PCIO))
			{	
				key_press_val = KEY_NO;
				key_press_flag = 0;
				key_timer_en = 0;
				key_value = NO_KEY;	
			}
            */			
		}		
		//
		//C口按键判断 
		if((RD_GPIOC & KEY_PCIO) != KEY_PCIO)				
		{
			key_press_val = RD_GPIOC & KEY_PCIO;
			if(key_timer_cnt1 >= KEY_2S_CNT)				//C口长按键判断
			{
				key_press_val = KEY_NO;
				key_press_flag = 0;
				key_timer_en1 = 0;
				key_value = RETURN_L_KEY;	
				key_timer_cnt1 = 0;					
			}
		}
		else
		{
			if(key_timer_cnt1 >= KEY_200MS_CNT)			//C口短按键判断
			{
				if(key_press_val == KEY_RETURN)	
				{
					key_press_val = KEY_NO;
					key_press_flag = 0;
					key_timer_en1 = 0;
					key_value = RETURN_S_KEY;	
					key_timer_cnt1 = 0;						
				}
				else
				{
					key_press_val = KEY_NO;
					key_press_flag = 0;
					key_timer_en1 = 0;
					key_value = NO_KEY;								
				}
			}		
			/*
            else
			if(((RD_GPIOB & KEY_PBIO) == KEY_PBIO) && ((RD_GPIOC & KEY_PCIO) == KEY_PCIO))
			{			
				key_press_val = KEY_NO;
				key_press_flag = 0;
				key_timer_en1 = 0;
				key_value = NO_KEY;	
			}	
            */            
		}
		//
	}	
}

/*
********************************************************************************
函数名称：key_init
函数描述：key初始化
参数描述：
返回值	：
********************************************************************************
*/
void key_gpio_int_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; 		//上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12; 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; 		//上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* EXTI line() mode config */
    EXTI_ClearITPendingBit(EXTI_Line0);
    EXTI_ClearITPendingBit(EXTI_Line1);
    EXTI_ClearITPendingBit(EXTI_Line2);
	EXTI_ClearITPendingBit(EXTI_Line10);
    EXTI_ClearITPendingBit(EXTI_Line11);
    EXTI_ClearITPendingBit(EXTI_Line12);
    
	/* EXTI line() mode config */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource11);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource12);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line2 | EXTI_Line10 | EXTI_Line11 | EXTI_Line12;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct); 

	/* Configyre P[A|B|C|D|E]0  NIVC  */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	/* Configyre P[A|B|C|D|E]0  NIVC  */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI2_3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	/* Configyre P[A|B|C|D|E]0  NIVC  */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);	
}
