/**
 * GSMģ��Ļ����������
 *
 * @author ���ء��ö���
 */

#ifndef __USR_GSM_BASE_H__
#define __USR_GSM_BASE_H__

#include "gsm_type.h"
#include "gsm_core.h"

#include "usr_task.h"

// ֻ����AT����
bool GSM_AT_Only(char *data);
// ����AT����������������
bool GSM_AT_Receive(char *data, GSM_RECEIVE_RECORD *pReceive);
// ����AT�������������recall����
bool GSM_AT_Recall(char *data, char *waitstr);
// ����AT�������������OK/ERROR/ERROR...ALREADY CONNECT
int GSM_AT_Recall_Connect(char *data);

// ���չؼ���
void GSM_Receive_KeyWord(void);
// ������ͨ����
void GSM_Receive_Data(GSM_RECEIVE_RECORD *pReceive);
// ��������״̬�ַ���
int GSM_Receive_Data_Connect(void);
// �����ڴ��ַ���
bool GSM_Receive_Recall(char *waitstr);

// ȥ��\r\n���������ص�ǰData����
int Remove_CR(UINT8* Data, int count);

// ������ʱ��
void Timer_Start_base(void);

extern TASK_EXECUTE Task_Execute;

#endif
