/**
 * GSM模块的基础命令操作
 *
 * @author 王特、矫东航
 */

#include "gsm_base.h"

#include <string.h>

extern GSM_STATUS_TRANS GSM_STATUS;
extern GSM_COMMAND_RECORD GSM_Command_Record;
extern GSM_DATA_RECORD GSM_Data_Record;
GSM_RECEIVE_RECORD Receive;

bool GSM_AT_Only(char *data)
{	
	if (GSM_Command_Record.Status != GSM_STATUS_COMMAND_SUCCESS) {
		return FALSE;
	}
	return GSM_Core_Tx_AT(data);
}

bool GSM_AT_Receive(char *data, GSM_RECEIVE_RECORD *pReceive)
{
	if (!GSM_AT_Only(data)) {
		return FALSE;
	}
	// 数据搬移
	memcpy(pReceive->Data, GSM_Command_Record.Rx_Data, GSM_Command_Record.Rx_Data_Count);
	
	// 去掉了首尾的\r\n，没有则不去掉
	pReceive->Data_Count = Remove_CR(pReceive->Data, GSM_Command_Record.Rx_Data_Count);
	
	return TRUE;
}

bool GSM_AT_Recall(char *data, char *waitstr)
{	
	if (!GSM_AT_Receive(data, &Receive)) {
		return FALSE;
	}
	// 上面函数已经去掉了首尾的\r\n，这里不用重复删除了
	if (strncmp((char*)Receive.Data, waitstr, strlen(waitstr)) != 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}

void GSM_Receive_KeyWord(void)
{
	// 数据搬移
	memcpy(Receive.Data, GSM_Data_Record.Rx_Data, GSM_Data_Record.Rx_Data_Count);
	// 去掉了首尾的\r\n，没有则不去掉
	Receive.Data_Count = Remove_CR(Receive.Data, Receive.Data_Count);
	
	if (strncmp((char*)Receive.Data, "", strlen("")) == 0) {
	
	} else if (strncmp((char*)Receive.Data, "", strlen("")) == 0) {
	
	} else if (strncmp((char*)Receive.Data, "", strlen("")) == 0) {
	
	}
}

void GSM_Receive_Data(GSM_RECEIVE_RECORD *pReceive)
{
	while (1) {
		if (GSM_Data_Record.Status == GSM_STATUS_DATA_SUCCESS) {
			// 数据搬移
			memcpy(pReceive->Data, GSM_Data_Record.Rx_Data, GSM_Data_Record.Rx_Data_Count);
			// 去掉了首尾的\r\n，没有则不去掉
			pReceive->Data_Count = Remove_CR(pReceive->Data, pReceive->Data_Count);
			
			break;
		}
	}
}

bool GSM_Receive_Recall(char *waitstr)
{
	while (1) {
		GSM_Receive_Data(&Receive);
		if (strncmp((char*)Receive.Data, waitstr, strlen(waitstr)) == 0) {
			break;
		}
	}
	return TRUE;
}

int Remove_CR(UINT8* Data, int count)
{
	int i;
	int temp_c = count;
	// 去除结尾回车
	if ((*(Data+count-2) == '\r') && (*(Data+count-1) == '\n')) {
		temp_c = temp_c - 2;
	}
	// 去除开头回车
	if ((*Data == '\r') && (*(Data+1) == '\n')) {	
		for (i = 0; i < temp_c-2; i++) {
			*(Data + i) = *(Data + i + 2);
		}
		temp_c = temp_c - 2;
	}
	return temp_c;
}
