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

// 初始化TIM2、TIM3
void GSM_CommandTimeInit(void);
void GSM_DataTimeInit(void);
void GSM_CountTimeInit(void);
// 启动并设置TIM2、TIM3、TIM4
void GSM_Set_TIM2(uint16_t cnt, uint16_t psc);
void GSM_Set_TIM3(uint16_t cnt, uint16_t psc);
void GSM_Set_TIM4(uint16_t cnt, uint16_t psc);
// 启动命令处理等待超时
void GSM_SetTimeCommand(void);;
// 启动接收数据超时
void GSM_SetTimeData(void);
// 启动计时器超时
void GSM_SetTimeCount(void);
// 关闭TIM2
void GSM_ShutTIMCommand(void);
// 关闭TIM3
void GSM_ShutTIMData(void);
// 关闭TIM4
void GSM_ShutTIMCount(void);
// TIM更新事件处理
void GSM_CommandTimeHandle(void);
void GSM_DataTimeHandle(void);
void GSM_CountTimeHandle(void);
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
// 复位GPIO初始化
void GSM_Reset_Int(void);
// GSM复位设置
void GSM_Reset_Set(void);
#endif
