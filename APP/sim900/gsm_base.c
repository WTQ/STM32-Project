/**
 * GSMģ��Ļ����������
 *
 * @author ���ء��ö���
 */

#include "gsm_base.h"

#include <string.h>

#include "gsm_gprs.h"
#include "usr_task.h"

extern GSM_STATUS_TRANS GSM_STATUS;
extern GSM_COMMAND_RECORD GSM_Command_Record;
extern GSM_DATA_RECORD GSM_Data_Record;
GSM_RECEIVE_RECORD Receive;

bool GSM_AT_Only(char *data)
{	
	if ((GSM_Command_Record.Status == GSM_STATUS_COMMAND_ECHO)
			|| (GSM_Command_Record.Status == GSM_STATUS_COMMAND_EXECUTE)
			|| (GSM_Command_Record.Status == GSM_STATUS_COMMAND_DATA)) {
		return FALSE;
	}
	if (!GSM_Core_Tx_AT(data)) {
		return FALSE;
	}	
	return TRUE;
}

bool GSM_AT_Receive(char *data, GSM_RECEIVE_RECORD *pReceive)
{
	if (!GSM_AT_Only(data)) {
		return FALSE;
	}
	// ���ݰ���
	memcpy(pReceive->Data, GSM_Command_Record.Rx_Data, GSM_Command_Record.Rx_Data_Count);
	
	// ȥ������β��\r\n��û����ȥ��
	pReceive->Data_Count = Remove_CR(pReceive->Data, GSM_Command_Record.Rx_Data_Count);
	
	// ���GSM_Command_Record.Rx_Data
//	memset(GSM_Command_Record.Rx_Data, 0, sizeof(GSM_Command_Record.Rx_Data));
//	GSM_Command_Record.Rx_Data[0] = '\0';
//	GSM_Command_Record.Rx_Data_Count = 0;
	GSM_Command_Record.Status = GSM_STATUS_COMMAND_IDLE;
	
	return TRUE;
}

bool GSM_AT_Recall(char *data, char *waitstr)
{	
//	memset(&Receive, 0, sizeof(Receive));
	Receive.Data[0] = '\0';
	Receive.Data_Count = 0;
	
	if (!GSM_AT_Receive(data, &Receive)) {
		return FALSE;
	}
	// ���溯���Ѿ�ȥ������β��\r\n�����ﲻ���ظ�ɾ����
	if (strncmp((char*)Receive.Data, waitstr, strlen(waitstr)) != 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}

void GSM_Receive_KeyWord(void)
{
/*	// ���ݰ���
	memcpy(Receive.Data, GSM_Data_Record.Rx_Data, GSM_Data_Record.Rx_Data_Count);
	// ȥ������β��\r\n��û����ȥ��
	Receive.Data_Count = Remove_CR(Receive.Data, GSM_Data_Record.Rx_Data_Count);
*/
	if (strncmp((char*)GSM_Data_Record.Rx_Data, "\r\nCall Ready\r\n", strlen("\r\nCall Ready\r\n")) == 0) {
		// ����GPRSSend����
		Task_Execute = EXECUTE;
		// OSTaskResume(MONITOR_TASK_PRIO);
	} else if (strncmp((char*)GSM_Data_Record.Rx_Data, "\r\nCLOSED\r\n", strlen("\r\nCLOSED\r\n")) == 0) {
		// ����GPRSSend����
		Task_Execute = EXECUTE;
		// OSTaskResume(MONITOR_TASK_PRIO);
	}

}

void GSM_Receive_Data(GSM_RECEIVE_RECORD *pReceive)
{
	while (1) {
		if (GSM_Data_Record.Status == GSM_STATUS_DATA_SUCCESS) {
			// ���ݰ���
			memcpy(pReceive->Data, GSM_Data_Record.Rx_Data, GSM_Data_Record.Rx_Data_Count);
			// ����ȫ�����գ�����\r\n��ȥ��
			// pReceive->Data_Count = Remove_CR(pReceive->Data, GSM_Data_Record.Rx_Data_Count);
			pReceive->Data_Count = GSM_Data_Record.Rx_Data_Count;
			// ���GSM_Data_Record.Rx_Data
//			memset(GSM_Data_Record.Rx_Data, 0, sizeof(GSM_Data_Record.Rx_Data));
//			GSM_Data_Record.Rx_Data[0] = '\0';
//			GSM_Data_Record.Rx_Data_Count = 0;
			
			break;
		}
	}
	GSM_Data_Record.Status = GSM_STATUS_DATA_IDLE;
}

int GSM_Receive_Data_Connect(void)
{
//	memset(&Receive, 0, sizeof(Receive));
	Receive.Data[0] = '\0';
	Receive.Data_Count = 0;
	
	GSM_Receive_Data(&Receive);
	// ȥ������β��\r\n��û����ȥ��
	Receive.Data_Count = Remove_CR(Receive.Data, Receive.Data_Count);
	if (strncmp((char*)Receive.Data, "ALREADY CONNECT", strlen("ALREADY CONNECT")) != 0) {
		return -2;
	}
	if (strncmp((char*)Receive.Data, "CONNECT FAIL", strlen("CONNECT FAIL")) != 0) {
		return -1;
	}
	if (strncmp((char*)Receive.Data, "CONNECT OK", strlen("CONNECT OK")) != 0) {
		return 1;
	}
	return 0;
}

bool GSM_Receive_Recall(char *waitstr)
{
//	memset(&Receive, 0, sizeof(Receive));
	Receive.Data[0] = '\0';
	Receive.Data_Count = 0;
	
	GSM_Receive_Data(&Receive);
	// ȥ������β��\r\n��û����ȥ��
	Receive.Data_Count = Remove_CR(Receive.Data, Receive.Data_Count);
	if (strncmp((char*)Receive.Data, waitstr, strlen(waitstr)) != 0) {
		return FALSE;
	}
	return TRUE;
}

int Remove_CR(UINT8* Data, int count)
{
	int i;
	int temp_c = count;
	// ȥ����β�س�
	if ((*(Data+count-2) == '\r') && (*(Data+count-1) == '\n')) {
//		*(Data+count-2) = '\0';
		temp_c = temp_c - 2;
	}
	// ȥ����ͷ�س�
	if ((*Data == '\r') && (*(Data+1) == '\n')) {	
		for (i = 0; i < temp_c-2; i++) {
			*(Data + i) = *(Data + i + 2);
		}
//		*(Data + temp_c-2) = '\0';
		temp_c = temp_c - 2;
	}
	return temp_c;
}
