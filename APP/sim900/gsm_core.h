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

// GSM协议栈发送和接收数据入口
void GSM_CORE_Rx_Handle(UINT8 data);
void GSM_CORE_AT(char *data);

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

#endif
