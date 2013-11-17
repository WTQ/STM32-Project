/**
 * GSMģ����STM32�Ľӿڲ�
 *
 * @author ���ء��ö���
 */

#include "gsm_driver.h"

#include <string.h>

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#include "gsm_core.h"

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

void GSM_TimeHandle(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		// ������жϱ�־λ
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		// ����core�㳬ʱ������
		Timeout_Handle();
	}
}

void GSM_USART_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
#if (GSM_COMM_SEL == GSM_USART_1)
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE); 

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);
	// ȫ˫������PB6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// ȫ˫������PB7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
}

void GSM_USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
#if (GSM_COMM_SEL == GSM_USART_1)	
	USART_Init(USART1, &USART_InitStructure);
	// ʹ��USART1����
	USART_Cmd(USART1, ENABLE);
#endif
}

void GSM_USART_NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
#if (GSM_COMM_SEL == GSM_USART_1)	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
#endif
}

void GSM_USART_TxChar(UINT8 c)
{
#if (GSM_COMM_SEL == GSM_USART_1)
	USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
#endif	
}

void GSM_USART_TxData(UINT8 *data, int len)
{
	UINT8 *p = data;
	while(len-- > 0) {
		GSM_USART_TxChar(*p);
		++p;
	}
}

void GSM_USART_TxStr(char *str)
{
	GSM_USART_TxData((UINT8 *) str, strlen(str));
}

void GSM_USART_Rx(void)
{
	USART_TypeDef	*usart;
	unsigned char	rx_data;
	
#if (GSM_COMM_SEL == GSM_UART_1)
    usart = USART1;
#endif

	// ���-������������Ҫ�ȶ�SR,�ٶ�DR�Ĵ����������������жϵ�����
	if(USART_GetFlagStatus(usart, USART_FLAG_ORE) == SET){
		USART_ClearFlag(usart, USART_FLAG_ORE);		// ��SR��ʵ���������־
		USART_ReceiveData(usart);					// ��DR 
	} 
	if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
		rx_data = USART_ReceiveData(usart);
		GSM_CORE_Rx_Handle(rx_data);
		// �п������������û��ִ���꣬�����ֵ������ͻ����USART_FLAG_ORE�ж�	
		USART_ClearITPendingBit(usart, USART_IT_RXNE);
	}
}

void GSM_USART_Init(void)
{
	GSM_USART_GPIO();
	GSM_USART_Config();
	GSM_USART_NVIC();
}

void GSM_Driver_Int(void)
{
	GSM_TimeInit();
	GSM_USART_Init();
	//GSM_Init();
}
