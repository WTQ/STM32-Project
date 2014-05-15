/**
 * GSM模块的GPRS操作封装
 *
 * @author 王特、矫东航
 */

#ifndef __USR_GSM_GPRS_H__
#define __USR_GSM_GPRS_H__

#include "gsm_type.h"

#include "gsm_base.h"

// GSM初始化
bool GSM_Config(void);
// GPRS初始化
void GPRS_Init(void);
// TCP连接并阻塞等待成功消息
bool GPRS_TCP_Connect(char *IP, char *PORT);
// 发送数据
bool GPRS_TCP_Send(char *Data);
// 接收数据
bool GPRS_TCP_Receive(GSM_RECEIVE_RECORD *pReceive);
// 关闭TCP连接
void GRRS_TCP_Closed(void);

#endif
