/**
 * GSM模块的核心通信协议模块
 *
 * @author 王特、矫东航
 */

#include "gsm_core.h"

#include <string.h>

GSM_STATUS_TRANS GSM_STATUS;
GSM_COMMAND_RECORD GSM_Command_Record;
GSM_DATA_RECORD GSM_Data_Record;

//void GSM_Init(void)
//{
//	
//}

void GSM_CORE_Rx_Handle(UINT8 data)
{	
	if (GSM_STATUS == GSM_STATUS_TRANS_DATA) {
		
		GSM_SetTimeData();
		GSM_Core_Rx_Data(data);
		GSM_Data_Record.Status = GSM_STATUS_DATA_DATA;
		
	} else if (GSM_STATUS == GSM_STATUS_TRANS_COMMAND) {
	
		switch (GSM_Command_Record.Status) {			
		case GSM_STATUS_COMMAND_ECHO :
			GSM_Core_Rx_Echo(data);
			break;
		case GSM_STATUS_COMMAND_EXECUTE :
			GSM_SetTimeData();
			GSM_Core_Rx_Command(data);
			// 进入命令态的接收数据状态
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_DATA;
			break;
		case GSM_STATUS_COMMAND_DATA :
			GSM_SetTimeData();
			GSM_Core_Rx_Command(data);
			break;
		case GSM_STATUS_COMMAND_SUCCESS :
			// 切换至数据态
			GSM_STATUS = GSM_STATUS_TRANS_DATA;
			GSM_SetTimeData();
			GSM_Data_Record.Status = GSM_STATUS_DATA_DATA;
			GSM_Core_Rx_Data(data);
		}
	}
}

void GSM_Core_Rx_Data(UINT8 data)
{
	if (GSM_Data_Record.Status != GSM_STATUS_DATA_DATA) {	
		GSM_Data_Record.Rx_Data_Count = 0;
	}
	GSM_Data_Record.Rx_Data[GSM_Data_Record.Rx_Data_Count++] = data;
}

void GSM_Core_Rx_Command(UINT8 data)
{
	GSM_Command_Record.Rx_Data[GSM_Command_Record.Rx_Data_Count++] = data;
}

void GSM_Core_Rx_Echo(UINT8 data)
{
	GSM_Command_Record.Echo_Data[GSM_Command_Record.Echo_Count++] = data;
	
	if ((GSM_Command_Record.Echo_Count == GSM_Command_Record.Tx_Data_Count)
		&& (strncmp( GSM_Command_Record.Echo_Data, GSM_Command_Record.Tx_Data, GSM_Command_Record.Tx_Data_Count) == 0)) {
		
		// 清空回显内容
		GSM_Command_Record.Echo_Count = 0;
		// 进入执行状态
		GSM_Command_Record.Status = GSM_STATUS_COMMAND_EXECUTE;
		GSM_SetTimeCommand();
	}
}

void GSM_CORE_AT(char *data)
{
	// 记录命令及其长度
	strcpy(GSM_Command_Record.Tx_Data, data);
	GSM_Command_Record.Tx_Data_Count = strlen(data);
	GSM_Command_Record.Rx_Data_Count = 0;
	GSM_Command_Record.Echo_Count = 0;
	// 切换状态
	GSM_STATUS = GSM_STATUS_TRANS_COMMAND;
	GSM_Command_Record.Status = GSM_STATUS_COMMAND_ECHO;
}

void Timeout_Handle(void)
{
	if (GSM_STATUS == GSM_STATUS_TRANS_COMMAND) {
		if (GSM_Command_Record.Status == GSM_STATUS_COMMAND_EXECUTE) {
			// 标记超时（暂时未用到）
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_TIMEOUT;
					
		} else if (GSM_Command_Record.Status == GSM_STATUS_COMMAND_DATA) {
			
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_SUCCESS;
			// @todo 上层接收命令态数据函数
			
		}
		// 切换至数据态，可能更改到上层执行
		GSM_STATUS == GSM_STATUS_TRANS_DATA;
		GSM_Data_Record.Status = GSM_STATUS_DATA_IDLE;
		
	} else if (GSM_STATUS == GSM_STATUS_TRANS_DATA) {
	
		GSM_Data_Record.Status = GSM_STATUS_DATA_SUCCESS;
		// @todo 上层接收数据态数据函数
		
		// 切换至IDLE状态，可能更改到上层执行
		GSM_Data_Record.Status = GSM_STATUS_DATA_IDLE;
	}
}
