/**
 * GSM模块的核心通信协议模块
 *
 * @author 王特
 */

#ifndef __USR_GSM_CORE_H__
#define __USR_GSM_CORE_H__

#include "gsm_type.h"
#include "gsm_driver.h"


void GSM_Init(void);

// GSM协议栈发送和接收数据入口
void GSM_CORE_RX_Handle(UINT8 data);
void GSM_CORE_TX_Handle(UINT8 *data);

// GSM协议栈处理函数
void GSM_CORE_Processor(UINT8 data);



#endif
