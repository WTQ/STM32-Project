/**
 * GSM模块与STM32的接口层
 *
 * @author 王特
 */

#include "gsm_driver.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"


void GSM_TimeInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 设置优先级的分组方式
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	// 设置TIM2的中断通道
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// 启用TIM2的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	// 开启命令超时
	GSM_SetTimeCommand();
}

void GSM_SetTime(uint16_t cnt, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	// 先关闭TIM2的使能和中断
	TIM_Cmd(TIM2, DISABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	
	// 设置时基单元
	TIM_TimeBaseStructure.TIM_Period = cnt;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	// 清除中断挂起位标志
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	// 使能TIM2预装载寄存器
	TIM_ARRPreloadConfig(TIM2, ENABLE);

	// 使能TIM2更新事件中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	// 使能TIM2 
	TIM_Cmd(TIM2, ENABLE);  
}

// 启动命令处理等待超时
void GSM_SetTimeCommand(void)
{
	// 设置10S的超时时间
	GSM_SetTime(20000u, 36000u);
}

// 启动接收数据超时
void GSM_SetTimeData(void)
{
	// 设置2个115200波特率的超时时间
	GSM_SetTime(2u, 6250u);
}

void GSM_ShutTime(void)
{
	// 关闭TIM2的使能和中断
	TIM_Cmd(TIM2, DISABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
}

int iTest = 0;
void GSM_TimeHandle(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		// 先清除中断标志位
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		++iTest;
		if(iTest == 100) {
			GSM_ShutTime();
		}
	}
}
