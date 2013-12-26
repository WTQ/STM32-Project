/**
 * GSM模块的基础命令操作
 *
 * @author 王特、矫东航
 */

#ifndef __USR_GSM_BASE_H__
#define __USR_GSM_BASE_H__

#include "gsm_type.h"
#include "gsm_core.h"

#include "usr_task.h"

// 只发送AT命令
bool GSM_AT_Only(char *data);
// 发送AT命令，并保存接收数据
bool GSM_AT_Receive(char *data, GSM_RECEIVE_RECORD *pReceive);
// 发送AT命令，并期望接收recall数据
bool GSM_AT_Recall(char *data, char *waitstr);

// 接收关键字
void GSM_Receive_KeyWord(void);
// 接收普通数据
void GSM_Receive_Data(GSM_RECEIVE_RECORD *pReceive);
// 接收期待字符串
bool GSM_Receive_Recall(char *waitstr);

// 去除\r\n操作，返回当前Data长度
int Remove_CR(UINT8* Data, int count);

extern TASK_EXECUTE Task_Execute;

#endif
