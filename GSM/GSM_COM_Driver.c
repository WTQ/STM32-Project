/*
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    						  -易通电子原创代码-          		          						  ▇
▇ 作者：莫建林																					  ▇
▇   QQ: 1695627698 																			  ▇
▇ 旺旺：一易通电子科技一 																		  ▇
▇ 邮箱：mojianlin2011@126.com																	  ▇
▇ 淘宝专卖店：etmcu.taobao.com																	  ▇
▇ 声明:此程序是为易通无线技术开发板而写的，请勿用于商业目的，若要引用请附上我的联系方式！		  ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/

#include "stm32f10x.h"

#include "GSM_TypeDefine.h"
#include "GSM.h"
//#include "flash.h"
#include "app_cfg.h"
#include <stdio.h>

typedef unsigned char  INT8U;
typedef unsigned short INT16U;

extern 	GSM_RX_STATE  GSM_RX_nState; /*RXD.c 里面定义*/

#if (GSM_MODULE == DEF_ENABLED)
/*★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★*/
/*
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    						GSM串口配置         		          								  ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/


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
	
	// 添加用来清除发送第一个字节的问题
	// USART_GetFlagStatus(USART2, USART_FLAG_TC);
}

/*
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    							禁止GSM串口接收中断    		          							  ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/

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
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    						开启GSM串口接收中断      		       								  ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/
void  GSM_RxIntEn (void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
/*
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    							GSM_RxISRHandler (void)          								  ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/
void  GSM_RxISRHandler (void)
{
}
/*
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    						GSM_RxTxISRHandler (void)      								          ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/
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
	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题
    if(USART_GetFlagStatus(usart,USART_FLAG_ORE)==SET)
     {
      USART_ClearFlag(usart,USART_FLAG_ORE); //读SR其实就是清除标志
      USART_ReceiveData(usart); //读DR 
     } 

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) 
	{
        rx_data = USART_ReceiveData(usart) & 0xFF; 
             
        GSM_RxHandler(rx_data);
	   //有可能这个函数还没有执行完，数据又到来，就会产生USART_FLAG_ORE中断
    	
		USART_ClearITPendingBit(usart, USART_IT_RXNE);
	}

    if (USART_GetITStatus(usart, USART_IT_TXE) != RESET)
	{
													   //这段是为了避免STM32 USART 第一个字节发不出去的BUG 	
		USART_ITConfig(usart, USART_IT_TXE, DISABLE); //禁止发缓冲器空中断， 
        USART_ClearITPendingBit(usart, USART_IT_TXE);
	}
                                                
}
/*
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    							通过GSM串口发送一个字节          								  ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/
void  GSM_TxChar(INT8U c)
{
	USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); // 这里设置了向usart1端口传输数据，需要扩展

}

void  GSM_TxString(INT8U *str)
{

   while(*str!='\0')
    {	
	   GSM_TxChar(*str++);
    }
}
/*
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    							禁止GSM串口发送中断             								  ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/

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
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    							开启GSM串口发送中断          								      ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/
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
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //串口通道，这里设置了向usart1端口，需要扩展
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    						  按键初始化          		          								  ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	//带上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}



void EXIT_Config(void)
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0); // 管脚选择
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
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇
▇    						  GSM初始化          		          								  ▇
▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇*/
void  GSM_Init (void)
{
	GSM_RX_nState = RX_STATE_IDLE; 
    GSM_InitTarget();
//	GPIO_Config();
//	EXIT_Config();
	NVIC_Config();
	GSM_RxIntEn();
	OSTimeDlyHMSM(0,0,5,0);	//开机启动后15s，配置GSM模块
	GSM_Configuration();
	//Usr_Flash_Erase(SMS_DATA_ADDR_START);
}

/*★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★*/
#endif
