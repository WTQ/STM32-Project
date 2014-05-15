/**
 * GSMģ���GPRS������װ
 *
 * @author ���ء��ö���
 */

#ifndef __USR_GSM_GPRS_H__
#define __USR_GSM_GPRS_H__

#include "gsm_type.h"

#include "gsm_base.h"

// GSM��ʼ��
bool GSM_Config(void);
// GPRS��ʼ��
void GPRS_Init(void);
// TCP���Ӳ������ȴ��ɹ���Ϣ
bool GPRS_TCP_Connect(char *IP, char *PORT);
// ��������
bool GPRS_TCP_Send(char *Data);
// ��������
bool GPRS_TCP_Receive(GSM_RECEIVE_RECORD *pReceive);
// �ر�TCP����
void GRRS_TCP_Closed(void);

#endif
