/**
 * GSM模块与STM32的接口层
 *
 * @author 王特、矫东航
 */

#include "gsm_driver.h"

#include <string.h>

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

void GSM_USART_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
#if (GSM_COMM_SEL == GSM_USART_1)
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE); 

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);
	// 全双工发送PB6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// 全双工接收PB7
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
	// 使能USART1外设
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

	// 溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题
	if(USART_GetFlagStatus(usart, USART_FLAG_ORE) == SET){
		USART_ClearFlag(usart, USART_FLAG_ORE);		// 读SR其实就是清除标志
		USART_ReceiveData(usart);					// 读DR 
	} 
	if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
		rx_data = USART_ReceiveData(usart) & 0xFF;
		// GSM_RxHandler(rx_data);
		// 有可能这个函数还没有执行完，数据又到来，就会产生USART_FLAG_ORE中断	
		USART_ClearITPendingBit(usart, USART_IT_RXNE);
	}
}

void GSM_USART_Init(void)
{
	GSM_USART_GPIO();
	GSM_USART_Config();
	GSM_USART_NVIC();
}
