/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    						  -��ͨ����ԭ������-          		          						  �~
�~ ���ߣ�Ī����																					  �~
�~   QQ: 1695627698 																			  �~
�~ ������һ��ͨ���ӿƼ�һ 																		  �~
�~ ���䣺mojianlin2011@126.com																	  �~
�~ �Ա�ר���꣺etmcu.taobao.com																	  �~
�~ ����:�˳�����Ϊ��ͨ���߼����������д�ģ�����������ҵĿ�ģ���Ҫ�����븽���ҵ���ϵ��ʽ��		  �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/

#include "stm32f10x.h"

#include "GSM_TypeDefine.h"
#include "GSM.h"
//#include "flash.h"
#include "app_cfg.h"
#include <stdio.h>

typedef unsigned char  INT8U;
typedef unsigned short INT16U;

extern 	GSM_RX_STATE  GSM_RX_nState; /*RXD.c ���涨��*/

#if (GSM_MODULE == DEF_ENABLED)
/*��������������������������������������������������*/
/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    						GSM��������         		          								  �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/


void  GSM_InitTarget(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE); 

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_WWDG, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

	USART_Cmd(USART1, ENABLE);
	
	// �������������͵�һ���ֽڵ�����
	// USART_GetFlagStatus(USART2, USART_FLAG_TC);
}

/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    							��ֹGSM���ڽ����ж�    		          							  �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/

void  GSM_RxIntDis (void)
{
#if (GSM_COMM_SEL == GSM_UART_1)
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
#endif

#if (GSM_COMM_SEL == GSM_UART_2)
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
#endif

#if (GSM_COMM_SEL == GSM_UART_3)
    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
#endif
}
/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    						����GSM���ڽ����ж�      		       								  �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/
void  GSM_RxIntEn (void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    							GSM_RxISRHandler (void)          								  �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/
void  GSM_RxISRHandler (void)
{
}
/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    						GSM_RxTxISRHandler (void)      								          �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/
void  GSM_RxTxISRHandler(void)
{

    USART_TypeDef  *usart;
    unsigned char   rx_data;
    unsigned int    cpu_sr;

 

#if   (GSM_COMM_SEL == GSM_UART_1)
    usart = USART1;
#elif (GSM_COMM_SEL == GSM_UART_2)
    usart = USART2;
#elif (GSM_COMM_SEL == GSM_UART_3)
    usart = USART3;
#else
    return;
#endif
	//���-������������Ҫ�ȶ�SR,�ٶ�DR�Ĵ����������������жϵ�����
    if(USART_GetFlagStatus(usart,USART_FLAG_ORE)==SET)
     {
      USART_ClearFlag(usart,USART_FLAG_ORE); //��SR��ʵ���������־
      USART_ReceiveData(usart); //��DR 
     } 

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) 
	{
        rx_data = USART_ReceiveData(usart) & 0xFF; 
             
        GSM_RxHandler(rx_data);
	   //�п������������û��ִ���꣬�����ֵ������ͻ����USART_FLAG_ORE�ж�
    	
		USART_ClearITPendingBit(usart, USART_IT_RXNE);
	}

    if (USART_GetITStatus(usart, USART_IT_TXE) != RESET)
	{
													   //�����Ϊ�˱���STM32 USART ��һ���ֽڷ�����ȥ��BUG 	
		USART_ITConfig(usart, USART_IT_TXE, DISABLE); //��ֹ�����������жϣ� 
        USART_ClearITPendingBit(usart, USART_IT_TXE);
	}
                                                
}
/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    							ͨ��GSM���ڷ���һ���ֽ�          								  �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/
void  GSM_TxChar(INT8U c)
{
	USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); // ������������usart1�˿ڴ������ݣ���Ҫ��չ

}

void  GSM_TxString(INT8U *str)
{

   while(*str!='\0')
    {	
	   GSM_TxChar(*str++);
    }
}
/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    							��ֹGSM���ڷ����ж�             								  �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/

void  GSM_TxIntDis (void)
{
#if (GSM_COMM_SEL == GSM_UART_1)
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
#endif

#if (GSM_COMM_SEL == GSM_UART_2)
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
#endif

#if (GSM_COMM_SEL == GSM_UART_3)
    USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
#endif
}
/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    							����GSM���ڷ����ж�          								      �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/
	void  GSM_TxIntEn (void)
	{
	#if (GSM_COMM_SEL == GSM_UART_1)
	    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	#endif					
	
	#if (GSM_COMM_SEL == GSM_UART_2)
	    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	#endif
	
	#if (GSM_COMM_SEL == GSM_UART_3)
	    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	#endif
	}



void NVIC_Config(void)
{
  /* Configure the NVIC Preemption Priority Bits */ 
  NVIC_InitTypeDef NVIC_InitStructure; 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //����ͨ����������������usart1�˿ڣ���Ҫ��չ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    						  ������ʼ��          		          								  �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	//����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}



void EXIT_Config(void)
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0); // �ܽ�ѡ��
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource3);
	EXTI_ClearITPendingBit(EXTI_Line0 | EXTI_Line1 | EXTI_Line3);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line3;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~
�~    						  GSM��ʼ��          		          								  �~
�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~*/
void  GSM_Init (void)
{
	GSM_RX_nState = RX_STATE_IDLE; 
    GSM_InitTarget();
//	GPIO_Config();
//	EXIT_Config();
	NVIC_Config();
	GSM_RxIntEn();
	OSTimeDlyHMSM(0,0,5,0);	//����������15s������GSMģ��
	GSM_Configuration();
	//Usr_Flash_Erase(SMS_DATA_ADDR_START);
}

/*��������������������������������������������������*/
#endif
