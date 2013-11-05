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

#define TX_HOST_NAME		"tx.te168.cn"

void post_data(char *buffer, char *name, char *value);
void post_http(char *buffer, char *temp);
void GSM_Post_Record(char *Buffer, WM_Record *record);
void GSM_Post_Beat(char *Buffer);
void GSM_SendRecord(WM_Record* WMRecord);

#endif
