/**
 * GSMģ��ĺ���ͨ��Э��ģ��
 *
 * @author ���ء��ö���
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
			// ��������̬�Ľ�������״̬
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_DATA;
			break;
		case GSM_STATUS_COMMAND_DATA :
			GSM_SetTimeData();
			GSM_Core_Rx_Command(data);
			break;
		case GSM_STATUS_COMMAND_SUCCESS :
			// �л�������̬
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
		
		// ��ջ�������
		GSM_Command_Record.Echo_Count = 0;
		// ����ִ��״̬
		GSM_Command_Record.Status = GSM_STATUS_COMMAND_EXECUTE;
		GSM_SetTimeCommand();
	}
}

void GSM_CORE_AT(char *data)
{
	// ��¼����䳤��
	strcpy(GSM_Command_Record.Tx_Data, data);
	GSM_Command_Record.Tx_Data_Count = strlen(data);
	GSM_Command_Record.Rx_Data_Count = 0;
	GSM_Command_Record.Echo_Count = 0;
	// �л�״̬
	GSM_STATUS = GSM_STATUS_TRANS_COMMAND;
	GSM_Command_Record.Status = GSM_STATUS_COMMAND_ECHO;
}

void Timeout_Handle(void)
{
	if (GSM_STATUS == GSM_STATUS_TRANS_COMMAND) {
		if (GSM_Command_Record.Status == GSM_STATUS_COMMAND_EXECUTE) {
			// ��ǳ�ʱ����ʱδ�õ���
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_TIMEOUT;
					
		} else if (GSM_Command_Record.Status == GSM_STATUS_COMMAND_DATA) {
			
			GSM_Command_Record.Status = GSM_STATUS_COMMAND_SUCCESS;
			// @todo �ϲ��������̬���ݺ���
			
		}
		// �л�������̬�����ܸ��ĵ��ϲ�ִ��
		GSM_STATUS == GSM_STATUS_TRANS_DATA;
		GSM_Data_Record.Status = GSM_STATUS_DATA_IDLE;
		
	} else if (GSM_STATUS == GSM_STATUS_TRANS_DATA) {
	
		GSM_Data_Record.Status = GSM_STATUS_DATA_SUCCESS;
		// @todo �ϲ��������̬���ݺ���
		
		// �л���IDLE״̬�����ܸ��ĵ��ϲ�ִ��
		GSM_Data_Record.Status = GSM_STATUS_DATA_IDLE;
	}
}
