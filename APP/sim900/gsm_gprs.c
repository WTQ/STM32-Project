/**
 * GSMģ���GPRS������װ
 *
 * @author ���ء��ö���
 */

#include "gsm_gprs.h"

#include <string.h>

extern GSM_RECEIVE_RECORD Receive_AT;

bool GSM_Config(void)
{
	// ��������
	if (!GSM_AT_Recall("AT\r\n", "OK")) {
		return FALSE;
	}
			
	// ����ģ����ַ���
	if (!GSM_AT_Recall("AT+CSCS=\"GSM\"\r\n", "OK")) {
		return FALSE;
	}
	return TRUE;
}

void GPRS_Init(void)
{	
	// ���ý��������Ƿ���ʾIPͷ��1����IPͷ��0δ����
	GSM_AT_Recall("AT+CIPHEAD=1\r\n", "OK");
	
	// ��������
	GSM_AT_Recall("AT+CGATT=1\r\n", "OK");
	
	// ����APN
	GSM_AT_Recall("AT+CSTT\r\n", "OK");
	
	// �����ƶ�����
	GSM_AT_Recall("AT+CIICR\r\n", "OK");
	
	// ��ȡ����IP��ַ(�˴�û�н��յ�ַ�ַ���)
	GSM_AT_Receive("AT+CIFSR", &Receive_AT);
	//GSM_AT_Only("AT+CIFSR"); // ������䲻�У���Ϊ�з���ֵ
	
}

bool GPRS_TCP_Connect(char *IP, char *PORT)
{
	int connect_state = 0;
	int AT_state = 0;
	int count = 0;
	char TCP_str[50] = "AT+CIPSTART=\"TCP\",\"";
	strcat(TCP_str, IP);
	strcat(TCP_str, "\",");
	strcat(TCP_str, PORT);
	strcat(TCP_str, "\r\n");
	
	// �����ȴ������ɹ���Ϣ
	while (connect_state != 1) { 
		
		// �����ȴ�����TCP����
		while ((AT_state != 2) && (AT_state != 1)&& (AT_state != -2)) {
			if (count == 10) {
				return FALSE;
			}
			OSTimeDlyHMSM(0,0,3,0);
			AT_state = GSM_AT_Recall_Connect(TCP_str);
			count++;			
		}
		if (AT_state != 2) {
			connect_state = GSM_Receive_Data_Connect();
			connect_state = connect_state; // �������
		}
	}
	return TRUE;
}

bool GPRS_TCP_Send(char *Data)
{
	if (!GSM_AT_Recall("AT+CIPSEND\r\n", ">")) {
		return FALSE;
	}
	
	return GSM_AT_Recall(Data, "SEND OK");
}

bool GPRS_TCP_Receive(GSM_RECEIVE_RECORD *pReceive)
{
	int n;
	// ���桱:��λ����Ϣ
	int pos;
	// ������Ϣ����
	int length = 0;
	GSM_Receive_Data(pReceive);
	
	// ʶ��������Ϣ�Ƿ���ȷ
	if (strncmp((char *)pReceive->Data, "\r\n+IPD,", 7) != 0) {
		return FALSE;
	}
	
	// ��ȡ��Ϣ������Ϣ
	for (pos=7; ((pReceive->Data)[pos]!=':') && (pos<pReceive->Data_Count); pos++) {
		length = length * 10 + (pReceive->Data)[pos] - 0x30; // ��0����ASCII��Ϊ0x30
	}
	
	// ��ֹδ���յ���:�����������������
	if (pos == pReceive->Data_Count) {
		return FALSE;
	}
	
	// ��֤��Ϣ������Ϣ
	if (length != pReceive->Data_Count - pos -1) {
		return FALSE;
	}
	
	// ����������Ϣ
	for (n=0; n<length; n++) {
		(pReceive->Data)[n] = (pReceive->Data)[n + pos + 1];
	}
	pReceive->Data_Count = length;
	
	return TRUE;
}

void GRRS_TCP_Closed(void)
{
	// �ر�TCP����
	GSM_AT_Recall("AT+CIPCLOSE=1\r\n", "CLOSE OK");
}

void Timer_Start()
{
	Timer_Start_base();
}
