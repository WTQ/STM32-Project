/**
 * GSMģ��Ļ����������
 *
 * @author ����
 */

#include "gsm_base.h"

#include <string.h>

extern GSM_STATUS_TRANS GSM_STATUS;
extern GSM_COMMAND_RECORD GSM_Command_Record;
extern GSM_DATA_RECORD GSM_Data_Record;

bool GSM_AT_Only(char *data)
{	
	if (GSM_Command_Record.Status != GSM_STATUS_COMMAND_SUCCESS) {
		return FALSE;
	}
	return GSM_Core_Tx_AT(data);
}

bool GSM_AT_Receive(char *data, GSM_RECEIVE_RECORD *Receive)
{
	if (!GSM_AT_Only(data)) {
		return FALSE;
	}
	// ���ݰ���
	memcpy(Receive->Data, GSM_Command_Record.Rx_Data, GSM_Command_Record.Rx_Data_Count);
	// @todo ע��\r\n �� count-4 ��\0 ����
	Receive->Data_Count = GSM_Command_Record.Rx_Data_Count;
	return TRUE;
}

bool GSM_AT_Recall(char *data, char *waitstr)
{
	GSM_RECEIVE_RECORD Receive;
	
	if (!GSM_AT_Receive(data, &Receive)) {
		return FALSE;
	}
	if (!strncmp((char*)Receive.Data, waitstr, Receive.Data_Count)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

void GSM_Receive_KeyWord()
{
	// @todo ע��\r\n �� count-4 ��\0 ����
	if (strncmp((char*)GSM_Data_Record.Rx_Data, "", GSM_Data_Record.Rx_Data_Count)) {
	
	}
}

void GSM_Receive_Data(GSM_RECEIVE_RECORD *Receive)
{
	while (1) {
		if (GSM_Data_Record.Status == GSM_STATUS_DATA_SUCCESS) {
		// @todo ע��\r\n �� count-4 ��\0 ����
			memcpy(Receive->Data, GSM_Data_Record.Rx_Data, GSM_Data_Record.Rx_Data_Count);
			Receive->Data_Count = GSM_Data_Record.Rx_Data_Count;
			break;
		}
	}
}

void Remove_CR(UINT8* Data, int count)
{
	for () {
		
	}
}
