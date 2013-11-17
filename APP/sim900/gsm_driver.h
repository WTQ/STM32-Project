/**
 * GSM模块与STM32的接口层
 *
 * @author 王特、矫东航
 */

#ifndef __USR_GSM_DRIVER_H__
#define __USR_GSM_DRIVER_H__

#include "stm32f10x.h"
#include "stm32f107.h"

#include "gsm_type.h"

// 选择通信的USART
#define GSM_COMM_SEL	GSM_USART_1

// 初始化TIM2
void GSM_TimeInit(void);
// 启动并设置TIM2
void GSM_SetTime(uint16_t cnt, uint16_t psc);
// 启动命令处理等待超时
void GSM_SetTimeCommand(void);
// 启动接收数据超时
void GSM_SetTimeData(void);
// 关闭TIM2
void GSM_ShutTime(void);
// TIM更新事件处理
void GSM_TimeHandle(void);
// USART_GPIO配置
void GSM_USART_GPIO(void);
// USART配置
void GSM_USART_Config(void);
// USART中断配置
void GSM_USART_NVIC(void);
// 串口发送一个字节
void GSM_USART_TxChar(UINT8 c);
// 串口发送数据
void GSM_USART_TxData(UINT8 *data, int len);
// 串口发送字符串
void GSM_USART_TxStr(char *str);
// 串口接收数据
void GSM_USART_Rx(void);
// 串口初始化
void GSM_USART_Init(void);
// 驱动初始化
void GSM_Driver_Int(void);
#endif
