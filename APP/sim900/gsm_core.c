/**
 * GSM模块的核心通信协议模块
 *
 * 注意：该协议存在如下机制，若发送命令等待返回值超时，则超时后返回的第一段数据被抛弃！
 *
 * @author 王特、矫东航
 */

#include "gsm_core.h"

#include <string.h>
#include "main.h"

#include "gsm_base.h"


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
		case GSM_STATUS_COMMAND_TIMEOUT :
			GSM_SetTimeData();
			break;
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
	
	if ((GSM_Command_Record.Echo_Count == GSM_Command_Record.Tx_Data_Count)) {
		
		if (strncmp( (char *)GSM_Command_Record.Echo_Data, 
			(char *)GSM_Command_Record.Tx_Data, 
			GSM_Command_Record.Tx_Data_Count) == 0) {
			
			// 进入执行状态
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_EXECUTE;
		} else {
			// 进入数据态
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_ERROR;
			GSM_STATUS = GSM_STATUS_TRANS_DATA;
		}
	}
}

bool GSM_CORE_Tx_Handle(char *data)
{
	OSTimeDlyHMSM(0, 0, 0, 500);
	if (GSM_STATUS == GSM_STATUS_TRANS_COMMAND) {
		if ((GSM_STATUS_COMMAND_SUCCESS != GSM_Command_Record.Status)
			&& (GSM_STATUS_COMMAND_ERROR != GSM_Command_Record.Status)
			&& (GSM_STATUS_COMMAND_IDLE != GSM_Command_Record.Status)) {
			// 非IDLE或SUCCESS或ERROR状态不能发送命令
			return FALSE;
		}
	} else if (GSM_STATUS == GSM_STATUS_TRANS_DATA) {
		if (GSM_STATUS_DATA_DATA == GSM_Data_Record.Status) {
			// DATA状态不能发送命令
			return FALSE;
		}
	}
	// 记录命令及其长度
	strcpy((char *)GSM_Command_Record.Tx_Data, data);
	GSM_Command_Record.Tx_Data_Count = strlen(data);
	GSM_Command_Record.Rx_Data_Count = 0;
	GSM_Command_Record.Echo_Count = 0;
	
	// 切换状态
	GSM_STATUS = GSM_STATUS_TRANS_COMMAND;
	GSM_Command_Record.Status = GSM_STATUS_COMMAND_ECHO;
	
	// 开启命令超时
	GSM_SetTimeCommand();
	
	// USART发送命令
	GSM_USART_TxStr(data);
	
	while (1) {
		if ((GSM_STATUS_COMMAND_SUCCESS == GSM_Command_Record.Status)
				|| (GSM_STATUS_COMMAND_TIMEOUT == GSM_Command_Record.Status) 
				|| (GSM_STATUS_COMMAND_ERROR == GSM_Command_Record.Status)) {
			break;
		}
	}
	return TRUE;
}

bool GSM_Core_Tx_AT(char *data)
{
	return GSM_CORE_Tx_Handle(data);
}
bool GSM_Core_Tx_Data(char *data)
{
	return GSM_CORE_Tx_Handle(data);
}

void Timeout_Command(void)
{
	if (GSM_STATUS == GSM_STATUS_TRANS_COMMAND) {
		GSM_Command_Record.Status = GSM_STATUS_COMMAND_TIMEOUT;
		
		// 发送“\r\n”使清除上一条命名的错误
		GSM_USART_TxStr("\r\n");
		
		// 切换至数据态，可能更改到上层执行
		// GSM_STATUS = GSM_STATUS_TRANS_DATA;
	}
	
	// 关闭TIM2定时器
	GSM_ShutTIMCommand();
}

void Timeout_Data(void)
{
	if (GSM_STATUS == GSM_STATUS_TRANS_COMMAND) {
		if (GSM_Command_Record.Status == GSM_STATUS_COMMAND_DATA) {	
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_SUCCESS;
			// 上层接收命令态数据函（暂时不需要）
		}
		
		// 切换至数据态，可能更改到上层执行
		GSM_STATUS = GSM_STATUS_TRANS_DATA;
		
	} else if (GSM_STATUS == GSM_STATUS_TRANS_DATA) {
		GSM_Data_Record.Status = GSM_STATUS_DATA_SUCCESS;
		// base层接收数据态数据函数
		
		// 关闭TIM3定时器
	//	GSM_ShutTIMData();
		
	//	GSM_Receive_KeyWord();
	}
	// 关闭TIM3定时器
	GSM_ShutTIMData();
}
