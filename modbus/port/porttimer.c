/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us ) //配置50us时钟
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t PrescalerValue = 0;
	
	/* TIM1 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 20000) - 1; // 1/20000=50us 
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = (uint16_t) usTim1Timerout50us;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //两级占先 八级副优先
	/* Enable the TIM1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
	/* TIM IT DISABLE */
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
	/* TIM1 DISABLE counter */
	TIM_Cmd(TIM1,  DISABLE);
        
    printf("\n\rTIM1 config IS done!\n\r");
	return TRUE;;
}


void vMBPortTimersEnable(  ) //打开时钟
{
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM1,0x0000); 
	TIM_Cmd(TIM1, ENABLE);
}

void vMBPortTimersDisable(  ) //关闭时钟
{
	TIM_Cmd(TIM1, DISABLE);
	TIM_SetCounter(TIM1,0x0000); 
	TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
void prvvTIMERExpiredISR( void ) //在时钟中断内调用
{
    ( void )pxMBPortCBTimerExpired(  );
}

