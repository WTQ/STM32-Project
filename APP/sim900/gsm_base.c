/**
 * GSM模块的基础命令操作
 *
 * @author 王特、矫东航
 */

#include "gsm_base.h"

#include <string.h>

#include "gsm_gprs.h"
#include "usr_task.h"

extern GSM_STATUS_TRANS GSM_STATUS;
extern GSM_COMMAND_RECORD GSM_Command_Record;
extern GSM_DATA_RECORD GSM_Data_Record;
GSM_RECEIVE_RECORD Receive_AT;
GSM_RECEIVE_RECORD Receive_Data;
GPRS_TIME GPRS_Timer;

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
	// 数据搬移
	memcpy(pReceive->Data, GSM_Command_Record.Rx_Data, GSM_Command_Record.Rx_Data_Count);
	
	// 去掉了首尾的\r\n，没有则不去掉
	pReceive->Data_Count = Remove_CR(pReceive->Data, GSM_Command_Record.Rx_Data_Count);
	
	// 清除GSM_Command_Record.Rx_Data
//	memset(GSM_Command_Record.Rx_Data, 0, sizeof(GSM_Command_Record.Rx_Data));
//	GSM_Command_Record.Rx_Data[0] = '\0';
//	GSM_Command_Record.Rx_Data_Count = 0;
	GSM_Command_Record.Status = GSM_STATUS_COMMAND_IDLE;
	
	return TRUE;
}

bool GSM_AT_Recall(char *data, char *waitstr)
{	
//	memset(&Receive_AT, 0, sizeof(Receive_AT));
	Receive_AT.Data[0] = '\0';
	Receive_AT.Data_Count = 0;
	
	if (!GSM_AT_Receive(data, &Receive_AT)) {
		return FALSE;
	}
	// 上面函数已经去掉了首尾的\r\n，这里不用重复删除了
	if (strncmp((char*)Receive_AT.Data, waitstr, strlen(waitstr)) != 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}

int GSM_AT_Recall_Connect(char *data)
{	
//	memset(&Receive_AT, 0, sizeof(Receive_AT));
	Receive_AT.Data[0] = '\0';
	Receive_AT.Data_Count = 0;
	

	if (!GSM_AT_Receive(data, &Receive_AT)) {
		return FALSE;
	}
	// 上面函数已经去掉了首尾的\r\n，这里不用重复删除了
	if (strncmp((char*)Receive_AT.Data, "OK\r\n\r\nCONNECT OK", strlen("OK\r\n\r\nCONNECT OK")) == 0) {
		return 2;
	}

	if (strncmp((char*)Receive_AT.Data, "OK", strlen("OK")) == 0) {
		return 1;
	}

	if (strncmp((char*)Receive_AT.Data, "ERROR\r\n\r\nALREADY CONNECT", strlen("ERROR\r\n\r\nALREADY CONNECT")) == 0) {
		return -2;
	}

	if (strncmp((char*)Receive_AT.Data, "ERROR", strlen("ERROR")) == 0) {
		return -1;
	}
	return 0;
}

void GSM_Receive_KeyWord(void)
{
/*	// 数据搬移

	memcpy(Receive_Data.Data, GSM_Data_Record.Rx_Data, GSM_Data_Record.Rx_Data_Count);
	// 去掉了首尾的\r\n，没有则不去掉

	Receive_Data.Data_Count = Remove_CR(Receive_Data.Data, GSM_Data_Record.Rx_Data_Count);
*/

/*	if (strncmp((char*)GSM_Data_Record.Rx_Data, "\r\nCall Ready\r\n", strlen("\r\nCall Ready\r\n")) == 0) {
		// 重启GPRSSend任务
		Task_Execute = EXECUTE;
		OSTaskResume(MONITOR_TASK_PRIO);
	} else if (strncmp((char*)GSM_Data_Record.Rx_Data, "\r\nCLOSED\r\n", strlen("\r\nCLOSED\r\n")) == 0) {
		// 重启GPRSSend任务
		Task_Execute = EXECUTE;
		OSTaskResume(MONITOR_TASK_PRIO);
	} else if (strncmp((char*)GSM_Data_Record.Rx_Data, "\r\nSTATE: TCP CLOSED\r\n\r\nCONNECT FAIL\r\n", strlen("\r\nSTATE: TCP CLOSED\r\n\r\nCONNECT FAIL\r\n")) == 0) {
		// 重启GPRSSend任务
		Task_Execute = EXECUTE;
		OSTaskResume(MONITOR_TASK_PRIO);
	} else if (strncmp((char*)GSM_Data_Record.Rx_Data, "\r\nCLOSED\r\n", strlen("\r\nCLOSED\r\n")) == 0) {
		// 重启GPRSSend任务
		Task_Execute = EXECUTE;
		OSTaskResume(MONITOR_TASK_PRIO);
	} else if (strncmp((char*)GSM_Data_Record.Rx_Data, "\r\n+PDP: DEACT\r\n\r\nERROR\r\n", strlen("\r\n+PDP: DEACT\r\n\r\nERROR\r\n")) == 0) {
		// 重启SIM900模块
		GSM_Reset_Set();
		// 重启GPRSSend任务
		Task_Execute = EXECUTE;
		OSTaskResume(MONITOR_TASK_PRIO);
	} else if (strncmp((char*)GSM_Data_Record.Rx_Data, "\r\nSTATE: PDP DEACT\r\n\r\nCONNECT FAIL\r\n", strlen("\r\nSTATE: PDP DEACT\r\n\r\nCONNECT FAIL\r\n")) == 0) {
		// 重启SIM900模块
		GSM_Reset_Set();
		// 重启GPRSSend任务
		Task_Execute = EXECUTE;
		OSTaskResume(MONITOR_TASK_PRIO);
	}*/

}

void GSM_Receive_Data(GSM_RECEIVE_RECORD *pReceive)
{
	while (1) {
		if (GSM_Data_Record.Status == GSM_STATUS_DATA_SUCCESS) {
			// 数据搬移
			memcpy(pReceive->Data, GSM_Data_Record.Rx_Data, GSM_Data_Record.Rx_Data_Count);
			// 数据全部接收，这里\r\n不去掉
			// pReceive->Data_Count = Remove_CR(pReceive->Data, GSM_Data_Record.Rx_Data_Count);
			pReceive->Data_Count = GSM_Data_Record.Rx_Data_Count;
			// 清空GSM_Data_Record.Rx_Data
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

//	memset(&Receive_Data, 0, sizeof(Receive_Data));
	Receive_Data.Data[0] = '\0';
	Receive_Data.Data_Count = 0;
	

	GSM_Receive_Data(&Receive_Data);
	// 去掉了首尾的\r\n，没有则不去掉

	Receive_Data.Data_Count = Remove_CR(Receive_Data.Data, Receive_Data.Data_Count);

//	if (strncmp((char*)Receive_Data.Data, "STATE: TCP CLOSED\r\n\r\nCONNECT FAIL", strlen("STATE: TCP CLOSED\r\n\r\nCONNECT FAIL")) == 0) {
//		return -1;
//	}
	
	if (strncmp((char*)Receive_Data.Data, "STATE: TCP CLOSED", strlen("STATE: TCP CLOSED")) == 0) {
		return -1;
	}

	if (strncmp((char*)Receive_Data.Data, "ALREADY CONNECT", strlen("ALREADY CONNECT")) == 0) {
		return -2;
	}

	if (strncmp((char*)Receive_Data.Data, "CONNECT OK", strlen("CONNECT OK")) == 0) {
		return 1;
	}
	return 0;
}

bool GSM_Receive_Recall(char *waitstr)
{

//	memset(&Receive_Data, 0, sizeof(Receive_Data));
	Receive_Data.Data[0] = '\0';
	Receive_Data.Data_Count = 0;
	

	GSM_Receive_Data(&Receive_Data);
	// 去掉了首尾的\r\n，没有则不去掉

	Receive_Data.Data_Count = Remove_CR(Receive_Data.Data, Receive_Data.Data_Count);
	if (strncmp((char*)Receive_Data.Data, waitstr, strlen(waitstr)) != 0) {
		return FALSE;
	}
	return TRUE;
}

int Remove_CR(UINT8* Data, int count)
{
	int i;
	int temp_c = count;
	// 去除结尾回车
	if ((*(Data+count-2) == '\r') && (*(Data+count-1) == '\n')) {
//		*(Data+count-2) = '\0';
		temp_c = temp_c - 2;
	}
	// 去除开头回车
	if ((*Data == '\r') && (*(Data+1) == '\n')) {	
		for (i = 0; i < temp_c-2; i++) {
			*(Data + i) = *(Data + i + 2);
		}
//		*(Data + temp_c-2) = '\0';
		temp_c = temp_c - 2;
	}
	return temp_c;
}
