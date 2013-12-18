#ifndef USR_HTTP_H
#define USR_HTTP_H

#include "app_cfg.h"
#include "ucos_ii.h"
#include <stdio.h>
#include <string.h>
#include "GSM.h"
#include "GSM_TypeDefine.h"
#include "stm32f10x.h"
#include "usr_flash.h"
#include "gsm_type.h"

#define TX_HOST_NAME		"tx.te168.cn"

void post_data(char *buffer, char *name, char *value);
void post_http(char *buffer, char *temp);
void GSM_Get_Record(char *buffer);
void GSM_Post_Record(char *Buffer, WM_Record *record);
void GSM_Post_Beat(char *Buffer); // 心跳包
void GSM_SendRecord(WM_Record* WMRecord);
bool GSM_Receive_Record(GSM_RECEIVE_RECORD *pReceive); // 抛弃信息头，只保留接收到的信息内容
int Calculate(GSM_RECEIVE_RECORD *pReceive); // 计算收到的数字

#endif
