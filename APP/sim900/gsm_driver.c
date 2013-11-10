/**
 * GSMģ����STM32�Ľӿڲ�
 *
 * @author ����
 */

#include "gsm_driver.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"


void GSM_TimeInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// �������ȼ��ķ��鷽ʽ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	// ����TIM2���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// ����TIM2��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	// �������ʱ
	GSM_SetTimeCommand();
}

void GSM_SetTime(uint16_t cnt, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	// �ȹر�TIM2��ʹ�ܺ��ж�
	TIM_Cmd(TIM2, DISABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	
	// ����ʱ����Ԫ
	TIM_TimeBaseStructure.TIM_Period = cnt;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	// ����жϹ���λ��־
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	// ʹ��TIM2Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM2, ENABLE);

	// ʹ��TIM2�����¼��ж�
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	// ʹ��TIM2 
	TIM_Cmd(TIM2, ENABLE);  
}

// ���������ȴ���ʱ
void GSM_SetTimeCommand(void)
{
	// ����10S�ĳ�ʱʱ��
	GSM_SetTime(20000u, 36000u);
}

// �����������ݳ�ʱ
void GSM_SetTimeData(void)
{
	// ����2��115200�����ʵĳ�ʱʱ��
	GSM_SetTime(2u, 6250u);
}

void GSM_ShutTime(void)
{
	// �ر�TIM2��ʹ�ܺ��ж�
	TIM_Cmd(TIM2, DISABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
}

int iTest = 0;
void GSM_TimeHandle(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		// ������жϱ�־λ
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		++iTest;
		if(iTest == 100) {
			GSM_ShutTime();
		}
	}
}
