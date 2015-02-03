/**
 * GSM模块的核心通信协议模块
 *
 * @author 王特、矫东航
 */

#ifndef __USR_GSM_CORE_H__
#define __USR_GSM_CORE_H__

#include "gsm_type.h"
#include "gsm_driver.h"

//void GSM_Init(void);

// GSM协议栈接收数据入口
void GSM_CORE_Rx_Handle(UINT8 data);

// GSM协议栈发送数据入口
bool GSM_CORE_Tx_Handle(char *data);
bool GSM_Core_Tx_AT(char *data);
bool GSM_Core_Tx_Data(char *data);

// GSM协议栈处理函数
void GSM_CORE_Processor(UINT8 data);

// GSM接收命令处理函数
void GSM_Core_Rx_Command(UINT8 data);
void GSM_Core_Rx_Echo(UINT8 data);

// GSM接收数据处理函数
void GSM_Core_Rx_Data(UINT8 data);

// 超时处理函数
void Timeout_Command(void);
void Timeout_Data(void);
void Timer_Start_core(void);
void Timeout_Count(void);

#endif
