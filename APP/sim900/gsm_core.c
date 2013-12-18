/**
 * GSMģ��ĺ���ͨ��Э��ģ��
 *
 * ע�⣺��Э��������»��ƣ�����������ȴ�����ֵ��ʱ����ʱ�󷵻صĵ�һ�����ݱ�������
 *
 * @author ���ء��ö���
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
			// ��������̬�Ľ�������״̬
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
			
			// ����ִ��״̬
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_EXECUTE;
		} else {
			// ��������̬
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
			// ��IDLE��SUCCESS��ERROR״̬���ܷ�������
			return FALSE;
		}
	} else if (GSM_STATUS == GSM_STATUS_TRANS_DATA) {
		if (GSM_STATUS_DATA_DATA == GSM_Data_Record.Status) {
			// DATA״̬���ܷ�������
			return FALSE;
		}
	}
	// ��¼����䳤��
	strcpy((char *)GSM_Command_Record.Tx_Data, data);
	GSM_Command_Record.Tx_Data_Count = strlen(data);
	GSM_Command_Record.Rx_Data_Count = 0;
	GSM_Command_Record.Echo_Count = 0;
	
	// �л�״̬
	GSM_STATUS = GSM_STATUS_TRANS_COMMAND;
	GSM_Command_Record.Status = GSM_STATUS_COMMAND_ECHO;
	
	// �������ʱ
	GSM_SetTimeCommand();
	
	// USART��������
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
		
		// ���͡�\r\n��ʹ�����һ�������Ĵ���
		GSM_USART_TxStr("\r\n");
		
		// �л�������̬�����ܸ��ĵ��ϲ�ִ��
		// GSM_STATUS = GSM_STATUS_TRANS_DATA;
	}
	
	// �ر�TIM2��ʱ��
	GSM_ShutTIMCommand();
}

void Timeout_Data(void)
{
	if (GSM_STATUS == GSM_STATUS_TRANS_COMMAND) {
		if (GSM_Command_Record.Status == GSM_STATUS_COMMAND_DATA) {	
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_SUCCESS;
			// �ϲ��������̬���ݺ�����ʱ����Ҫ��
		}
		
		// �л�������̬�����ܸ��ĵ��ϲ�ִ��
		GSM_STATUS = GSM_STATUS_TRANS_DATA;
		
	} else if (GSM_STATUS == GSM_STATUS_TRANS_DATA) {
		GSM_Data_Record.Status = GSM_STATUS_DATA_SUCCESS;
		// base���������̬���ݺ���
		
		// �ر�TIM3��ʱ��
	//	GSM_ShutTIMData();
		
	//	GSM_Receive_KeyWord();
	}
	// �ر�TIM3��ʱ��
	GSM_ShutTIMData();
}
