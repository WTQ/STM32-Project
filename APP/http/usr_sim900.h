/**
 * SIM900操作相关封装
 * 
 * @author 王特
 */

#ifndef __USR_SIM900_H
#define __USR_SIM900_H

#include "main.h"
#include "GSM.h"
#include  <ucos_ii.h>
#include  <cpu.h>

// DNS的设置
#define GPRS_DNS_PRI	"211.136.28.228"
#define GPRS_DNS_SEC	"211.136.28.234"

// 

void GPRS_Init1(void);
void GPRS_DNS_Init(void);
void GPRS_TCP_Connet(char *ip, char *port);
void GPRS_TCP_Send1(void);
void GRRS_TCP_Close(void);
void GRPS_TCP_Recv(void);

#endif
