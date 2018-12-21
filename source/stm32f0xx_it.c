/**
  ******************************************************************************
  * @file    GPIO/GPIO_IOToggle/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "public.h"

/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  USART1_IRQHandler.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	uint8_t temp;

	//USART1 recv
	if(USART1->ISR & 0x28)
	{
		USART1->ICR |= 0x1F;		
		
		temp = USART1->RDR;
		
		/*
		if(mcb_wifi_ru_state)
		{
			//下行透传，上行解析并转发给wifi，走Y-MODEM协议	
			if(mcb_wifi_ru_state & 0x07)		//rfb/xyb
				USART6->TDR = temp;
			else
			if(mcb_wifi_ru_state & 0x38)		//mspd8
				USART5->TDR = temp;
			else
			if(mcb_wifi_ru_state & 0x40)		//mcb
				;//ymodem_data[ymodem_cnt++] = temp;
		}	
		*/
		if(wifi_comm_data_deal_flag)
			return ;	
		
		if(!mcb_wifi_recv_cp_type)						//cp judge
		{
			if(mcb_wifi_recv_cp_cnt < 3)				
			{
				mcb_wifi_recv_cp_arr[mcb_wifi_recv_cp_cnt++] = temp;
				if(mcb_wifi_recv_cp_cnt == 2)
				{
					if((mcb_wifi_recv_cp_arr[0] == WIFI_COMM_SYN_CHAR_1) && (mcb_wifi_recv_cp_arr[1] == WIFI_COMM_SYN_CHAR_2))
					{
						mcb_wifi_recv_cp_type = 2;	
						wifi_comm_ru_data.rx_cnt = 2;
						mcb_wifi_recv_cp_cnt = 0;
					}					
					else
					if(((mcb_wifi_recv_cp_arr[0] == 'S') || (mcb_wifi_recv_cp_arr[0] == 'G')) && (mcb_wifi_recv_cp_arr[1] == 'E'))
						;
					else
						mcb_wifi_recv_cp_cnt = 0;
				}
				else
				if(mcb_wifi_recv_cp_cnt == 3)
				{
					if(mcb_wifi_recv_cp_arr[2] == 'T')	
					{
						mcb_wifi_recv_cp_type = 1;	
						wifi_comm_info_data.rx_cnt = 3;
						mcb_wifi_recv_cp_cnt = 0;						
					}
					else
						mcb_wifi_recv_cp_cnt = 0;	
				}
			}
			else
				mcb_wifi_recv_cp_cnt = 0;				
		}
		else
		if(mcb_wifi_recv_cp_type == 1)				//ascii
		{
			if(wifi_comm_info_data.rx_cnt < WIFI_BUF_MAX_LEN)
				wifi_comm_info_data.rx[wifi_comm_info_data.rx_cnt++] = temp;
			else
			{
				mcb_wifi_recv_cp_cnt = 0;	
				mcb_wifi_recv_cp_type = 0;						
			}	
			if(temp == WIFI_MCU_STAR_CHAR)			//收到'*'
			{
				wifi_comm_info_data.rx_len = wifi_comm_info_data.rx_cnt;
				wifi_comm_info_data.rx_cnt = 0;
				wifi_comm_data_deal_flag = 1;		//数据处理中标志
				wifi_comm_recv_end_flag = 1;		//接收完成标志
				mcb_wifi_info_deal_flag = 1;
				mcb_wifi_task_flag = 1;				//任务标志
				mcb_wifi_recv_cp_type = 0;
				mcb_wifi_recv_cp_cnt = 0;				
			}
		}
		else
		if(mcb_wifi_recv_cp_type == 2)				//binary
		{
			if(wifi_comm_ru_data.rx_cnt	== 2)
				wifi_comm_ru_data.rx[wifi_comm_ru_data.rx_cnt++] = temp;	
			else
			if(wifi_comm_ru_data.rx_cnt	== 3)
			{
				wifi_comm_ru_data.rx[wifi_comm_ru_data.rx_cnt++] = temp;
				if(wifi_comm_ru_data.rx[3] < WIFI_BUF_MAX_LEN)
					wifi_comm_ru_data.rx_len = wifi_comm_ru_data.rx[3] + 5;
				else
				{
					mcb_wifi_recv_cp_cnt = 0;	
					mcb_wifi_recv_cp_type = 0;
				}
			}
			else
			if(wifi_comm_ru_data.rx_cnt >= 4)		//接收数据、校验和
			{
				wifi_comm_ru_data.rx[wifi_comm_ru_data.rx_cnt++] = temp;
				if(wifi_comm_ru_data.rx_cnt >= wifi_comm_ru_data.rx_len)
				{
					wifi_comm_ru_data.rx_cnt = 0;
					wifi_comm_data_deal_flag = 1;	//数据处理中标志
					wifi_comm_recv_end_flag = 1;	//接收完成标志
					mcb_wifi_ru_deal_flag = 1;
					mcb_wifi_task_flag = 1;			//任务标志
					mcb_wifi_recv_cp_type = 0;
					mcb_wifi_recv_cp_cnt = 0;
				}
			}	
		}
	}	
	//USART1 send
	if(USART1->ISR & 0x40)
	{
		USART1->ICR |= 0x40;
		if(mcb_wifi_info_send_flag)			//信息数据发送-ASCII码协议
		{
			if(wifi_comm_info_data.tx_cnt < wifi_comm_info_data.tx_len)
			{
				USART1->TDR = wifi_comm_info_data.tx[wifi_comm_info_data.tx_cnt++];
			}
			else
			{
				USART1->CR1 &= ~0x40;
				mcb_wifi_info_send_flag = 0;
				wifi_comm_sys_tx_end = 1;
			}				
		}
		else
		if(mcb_wifi_ru_send_flag)			//升级数据发送-二进制协议
		{
			if(wifi_comm_ru_data.tx_cnt < wifi_comm_ru_data.tx_len)
			{
				USART1->TDR = wifi_comm_ru_data.tx[wifi_comm_ru_data.tx_cnt++];
			}
			else
			{
				USART1->CR1 &= ~0x40;
				mcb_wifi_ru_send_flag = 0;
				wifi_comm_sys_tx_end = 1;
			}				
		}
		else
		if(mcb_wifi_alm_send_flag)			//报警数据
		{
			if(wifi_comm_alm_data.tx_cnt < wifi_comm_alm_data.tx_len)
			{
				USART1->TDR = wifi_comm_alm_data.tx[wifi_comm_alm_data.tx_cnt++];
			}
			else
			{
				USART1->CR1 &= ~0x40;
				mcb_wifi_alm_send_flag = 0;
				wifi_comm_sys_tx_end = 1;
			}					
		}
	} 	
}

/**
  * @brief  USART2_IRQHandler. rs485-1
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	uint8_t temp;

	//USART2 recv
	if(USART2->ISR & 0x28)
	{
		USART2->ICR |= 0x1F;	
		
		temp = USART2->RDR;
		
		//if(rs485_1_comm_data_deal_flag)
		//	return ;
		
		if(rs485_1_comm_data.rx_cnt < RS485_COMM_RX_BUF_LEN)
			rs485_1_comm_data.rx[rs485_1_comm_data.rx_cnt++] = temp;
		//else
		//	rs485_1_comm_data.rx_cnt = 0
		
		//rs485_1_comm_data.rx_cnt = 0
		//rs485_1_comm_data_deal_flag = 1;	
		//rs485_1_comm_recv_end_flag = 1;
		//mcb_pes_task_flag = 1;						
	}	
	//USART2 send
	if(USART2->ISR & 0x40)
	{
		USART2->ICR |= 0x40;
		//
		if(rs485_1_comm_data.tx_cnt < rs485_1_comm_data.tx_len)
		{
			USART2->TDR = rs485_1_comm_data.tx[rs485_1_comm_data.tx_cnt++];
		}
		else
		{
			USART2->CR1 &= ~0x40;
			delay_xus(500);
			rs485_1_en(RS485_EN_RECV);	
		}
		//
	} 	
}

/**
  * @brief  USART3_6_IRQHandler.
  * @param  None
  * @retval None
  */
void USART3_6_IRQHandler(void)
{
	uint8_t temp;
	
	//USART5 recv
	if(USART5->ISR & 0x28)
	{
		USART5->ICR |= 0x1F;					

		temp = USART5->RDR;
		
		/*
		if(mcb_wifi_ru_state & 0x38) //mspd8
		{
			//上行解析数据并转发
			USART1->TDR = temp;
			ymodem_data[ymodem_cnt++] = temp;
		}	
		*/
		
		if(mp_comm_data_deal_flag)				//数据处理中不接收
			return ;
		
		if(mp_comm_data.rx_cnt < 2)				//接收同步头
		{
			mp_comm_data.rx[mp_comm_data.rx_cnt++] = temp;	
			if(mp_comm_data.rx_cnt == 2) 
			{
				if((mp_comm_data.rx[0] != MP_COMM_SYN_CHAR_1) || (mp_comm_data.rx[1] != MP_COMM_SYN_CHAR_2))
					mp_comm_data.rx_cnt = 0; 	//重新同步
			}	
		}
		else
		if(mp_comm_data.rx_cnt < 4)				//接收命令、长度
		{
			mp_comm_data.rx[mp_comm_data.rx_cnt++] = temp;	
			if(mp_comm_data.rx_cnt == 4) 
			{
				if(mp_comm_data.rx[3] < MP_BUF_MAX_LEN)
					mp_comm_data.rx_len = mp_comm_data.rx[3] + 5; 
				else
					mp_comm_data.rx_cnt = 0; 	//异常处理
			}					
		}	
		else
		if(mp_comm_data.rx_cnt >= 4)			//接收数据、校验和
		{
			mp_comm_data.rx[mp_comm_data.rx_cnt++] = temp;
			if(mp_comm_data.rx_cnt >= mp_comm_data.rx_len)
			{
				mp_comm_data.rx_cnt = 0;
				mp_comm_data_deal_flag = 1;		//数据处理中标志
				mp_comm_recv_end_flag = 1;		//接收完成标志
				mcb_pd8_task_flag = 1;			//任务标志
			}
		}	
	}
	//USART6 recv
	if(USART6->ISR & 0x28)
	{
		USART6->ICR |= 0x1F;	
		
		temp = USART6->RDR;
		
		/*
		if(mcb_wifi_ru_state & 0x07) //rfb/xyb
		{
			//上行解析数据并转发
			USART1->TDR = temp;
			ymodem_data[ymodem_cnt++] = temp;
		}	
		*/
		
		if(mr_comm_data_deal_flag)				//数据处理中不接收
			return ;
		
		if(mr_comm_data.rx_cnt < 2)				//接收同步头
		{
			mr_comm_data.rx[mr_comm_data.rx_cnt++] = temp;	
			if(mr_comm_data.rx_cnt == 2) 
			{
				if((mr_comm_data.rx[0] != COMM_SYN_CHAR_1) || (mr_comm_data.rx[1] != COMM_SYN_CHAR_2))
					mr_comm_data.rx_cnt = 0; 	//重新同步
			}	
		}
		else
		if(mr_comm_data.rx_cnt < 4)				//接收命令、长度
		{
			mr_comm_data.rx[mr_comm_data.rx_cnt++] = temp;	
			if(mr_comm_data.rx_cnt == 4) 
			{
				if(mr_comm_data.rx[3] < MR_BUF_MAX_LEN)
					mr_comm_data.rx_len = mr_comm_data.rx[3] + 5; 
				else
					mr_comm_data.rx_cnt = 0; 	//异常处理
			}					
		}	
		else
		if(mr_comm_data.rx_cnt >= 4)			//接收数据、校验和
		{
			mr_comm_data.rx[mr_comm_data.rx_cnt++] = temp;
			if(mr_comm_data.rx_cnt >= mr_comm_data.rx_len)
			{
				mr_comm_data.rx_cnt = 0;
				mr_comm_data_deal_flag = 1;		//数据处理中标志
				mr_comm_recv_end_flag = 1;		//接收完成标志
				mcb_rfb_task_flag = 1;			//任务标志
			}
		}	
	}	
	//USART5 send
	if(USART5->ISR & 0x40)
	{
		USART5->ICR |= 0x40;
		//
		if(mp_comm_data.tx_cnt < mp_comm_data.tx_len)
		{
			USART5->TDR = mp_comm_data.tx[mp_comm_data.tx_cnt++];
		}
		else
		{
			USART5->CR1 &= ~0x40;
			mp_comm_sys_tx_end = 1;
		}
		//
	} 	
	//USART6 send
	if(USART6->ISR & 0x40)
	{
		USART6->ICR |= 0x40;
		//
		if(mr_comm_data.tx_cnt < mr_comm_data.tx_len)
		{
			USART6->TDR = mr_comm_data.tx[mr_comm_data.tx_cnt++];
		}
		else
		{
			USART6->CR1 &= ~0x40;
			mr_comm_sys_tx_end = 1;
		}
		//
	} 
}

/**
  * @brief  This function handles TIM14 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM14_IRQHandler(void)	
{
	static uint32_t cnt0 = 0, cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0, cnt5 = 0, cnt6 = 0, cnt7 = 0;
	
	if(TIM14->SR & TIM_IT_Update)
	{
		TIM14->SR &= ~TIM_IT_Update;
		
		if(!mcb_wifi_recv_cp_type)				//与WIFI通信接收异常处理，防止接收死锁
		{
			cnt4++;
			if(cnt4 > WIFI_BD_RECV_TO_CNT)
			{
				cnt4 = 0;
				mcb_wifi_recv_cp_type = 0;	
				mcb_wifi_recv_cp_cnt = 0;	
			}
		}
		else
			cnt4 = 0;
			
		if(wifi_comm_time_out_en)				//与wifi模块通信超时判断
		{
			wifi_comm_time_out_cnt++;
			if(wifi_comm_time_out_cnt > WIFI_RECV_TIME_OUT_CNT)
			{
				wifi_comm_time_out_cnt = 0;				
				wifi_comm_sys_tx_to_flag = 1;
				mcb_wifi_task_flag = 1;	
			}	
		}
		
		cnt1++;
		if(cnt1 > ALARM_CHECK_TIME)				//输出报警任务检查周期，200ms
		{
			cnt1 = 0;
			mcb_alm_task_flag = 1;	
			mcb_wdt_task_flag = 1;				//看门狗借用此处
		}

		if(!mcb_wifi_ru_state)			//不在远程升级中
		{
			cnt0++;
			if(cnt0 > MCB_RFB_POLL_TIME)		//射频板信息周期性查询
			{
				cnt0 = 0;
				mr_comm_sys_tx_cmd1 = QQCXBXXZT_CMD_88;
				mr_comm_sys_tx_flag = 1;
				mcb_rfb_task_flag = 1;	
			}			
		}	
		if(mr_comm_time_out_en)					//与射频板通信超时判断
		{
			mr_comm_time_out_cnt++;
			if(mr_comm_time_out_cnt > RECV_TIME_OUT_CNT)
			{
				mr_comm_time_out_cnt = 0;				
				mr_comm_sys_tx_to_flag = 1;
				mcb_rfb_task_flag = 1;	
				mr_to_cnt2++;
				if(mr_to_cnt2 > RF_LINK_FAULT_NUM)
				{
					mr_to_cnt2 = 0;
					sys_fault_word |= 0x01;	
				}			
			}	
		}

		if(!mcb_wifi_ru_state)			//不在远程升级中
		{
			cnt3++;
			if(cnt3 > MCB_MP_POLL_TIME)			//八功分板信息周期性查询
			{
				cnt3 = 0;
				mp_comm_sys_tx_cmd1 = MP_QQCXBXXZT_CMD_88;
				mp_comm_sys_tx_flag = 1;
				mcb_pd8_task_flag = 1;	
			}			
		}	
		if(mp_comm_time_out_en)					//与八功分板通信超时判断
		{
			mp_comm_time_out_cnt++;
			if(mp_comm_time_out_cnt > MP_RECV_TIME_OUT_CNT)
			{
				mp_comm_time_out_cnt = 0;				
				mp_comm_sys_tx_to_flag = 1;
				mcb_pd8_task_flag = 1;	
				mp_to_cnt2++;
				if(mp_to_cnt2 > PD8_LINK_FAULT_NUM)
				{
					mp_to_cnt2 = 0;
					sys_fault_word |= 0x02;		
				}	
			}	
		}
		
		if(key_timer_en)						//按键计数器
			key_timer_cnt++;
		if(key_timer_en1)						//按键计数器
			key_timer_cnt1++;
		
		cnt2++;
		if(cnt2 > LCD_AUTO_REFRESH_CNT)			//人机显示刷新周期
		{
			cnt2 = 0;
			mcb_hmu_task_flag = 1;
			mcb_hmu_refresh_flag = 1;
		}
		
		mcb_hmu_auto_return_cnt++;				//人机静止25S自动返回主界面，除开机界面、手动和自动测试界面
		if(mcb_hmu_auto_return_cnt > LCD_AUTO_RETURN_MAIN_CNT)
		{
			mcb_hmu_auto_return_cnt = 0;
			if((mcb_hmu_disp_pages > MAIN_PAGE) && (mcb_hmu_disp_pages <= SUB_3_PAGE))
			{
				mcb_hmu_disp_pages = MAIN_PAGE;
				mcb_hmu_task_flag = 1;
				mcb_hmu_refresh_flag = 1;						
			}
			mcb_hmu_bl_cnt++;
			if(mcb_hmu_bl_cnt > LCD_CLOSE_BL_CNT_NUM) 	//人机静止5min关背光时间判断
			{
				mcb_hmu_bl_cnt = 0;
				if(!mcb_hmu_bl_state)
				{
					mcb_hmu_bl_close_flag = 1;	//I2C扩展IO关背光太慢,放在任务里做
					mcb_hmu_task_flag = 1;								
				}
			}			
		}
		
		if(beep_alarm_cnt_en)							//BEEP处理
		{
			beep_alarm_cnt++;
			if(beep_alarm_cnt > BEEP_ALARM_TIME)	
			{
				beep_alarm_cnt = 0;
				beep_alarm_cnt_en = 0;
				//beep_alarm_flag = 0;			//注释掉60s内还有其他故障60s后接着响，不注释60s内还有故障60s后接着不响
				TIM1->CR1 &= ~TIM_CR1_CEN; 
				TIM1->CNT = 0;		
			}
		}
		
		//容错处理：wifi_comm_sys_tx_end  mp_comm_sys_tx_end  mr_comm_sys_tx_end
		if(!wifi_comm_sys_tx_end)
		{
			cnt5++;
			if(cnt5 > 100)
			{
				cnt5 = 0;
				wifi_comm_sys_tx_end = 1;
			}
		}
		else
			cnt5 = 0;
			
		if(!mp_comm_sys_tx_end)
		{
			cnt6++;
			if(cnt6 > 100)
			{
				cnt6 = 0;
				mp_comm_sys_tx_end = 1;
			}
		}
		else
			cnt6 = 0;
			
		if(!mr_comm_sys_tx_end)
		{
			cnt7++;
			if(cnt7 > 100)
			{
				cnt7 = 0;
				mr_comm_sys_tx_end = 1;
			}
		}
		else
			cnt7 = 0;
	}			
}

/**
  * @brief  This function handles EXTI0_1 global interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_1_IRQHandler(void)
{
	if(EXTI->PR & (EXTI_Line0 | EXTI_Line1))
	{
		EXTI->PR = EXTI_Line0 | EXTI_Line1;
		EXTI->PR = EXTI_Line2;
		key_press_flag = 1;
		key_timer_en = 1;
		key_timer_cnt = 0;			
		mcb_hmu_task_flag = 1;	
		mcb_hmu_auto_return_cnt = 0;
		mcb_hmu_bl_cnt = 0;
	}
}

/**
  * @brief  This function handles EXTI2_3 global interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_3_IRQHandler(void)
{
	if(EXTI->PR & EXTI_Line2)
	{
		EXTI->PR = EXTI_Line2;
		key_press_flag = 1;
		key_timer_en = 1;
		key_timer_cnt = 0;			
		mcb_hmu_task_flag = 1;	
		mcb_hmu_auto_return_cnt = 0;
		mcb_hmu_bl_cnt = 0;
	}
}

/**
  * @brief  This function handles EXTI4_15 global interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
	if(EXTI->PR & (EXTI_Line10 | EXTI_Line11))
	{
		EXTI->PR = EXTI_Line10 | EXTI_Line11;
		key_press_flag = 1;
		key_timer_en = 1;
		key_timer_cnt = 0;
		mcb_hmu_task_flag = 1;
		mcb_hmu_auto_return_cnt = 0;
		mcb_hmu_bl_cnt = 0;
	}
	else
	if(EXTI->PR & EXTI_Line12)
	{
		EXTI->PR = EXTI_Line12;
		key_press_flag = 1;
		key_timer_en1 = 1;
		key_timer_cnt1 = 0;
		mcb_hmu_task_flag = 1;
		mcb_hmu_auto_return_cnt = 0;
		mcb_hmu_bl_cnt = 0;
	}
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
