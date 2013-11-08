/**
 * GSM模块与STM32的接口层
 *
 * @author 王特
 */

#ifndef __USR_GSM_DRIVER_H__
#define __USR_GSM_DRIVER_H__

#include "stm32f10x.h"
#include "stm32f107.h"

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

#endif
