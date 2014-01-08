/**
 * GSMģ���GPRS������װ
 *
 * @author ���ء��ö���
 */

#include "gsm_gprs.h"

#include <string.h>

extern GSM_RECEIVE_RECORD Receive;

void GSM_Config(void)
{
	// ��������
	GSM_AT_Recall("AT\r\n", "OK");
			
	// ����ģ����ַ���
	GSM_AT_Recall("AT+CSCS=\"GSM\"\r\n", "OK");
	
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
	GSM_AT_Receive("AT+CIFSR", &Receive);
	//GSM_AT_Only("AT+CIFSR"); // ������䲻�У���Ϊ�з���ֵ
	
}

bool GPRS_TCP_Connect(char *IP, char *PORT)
{
	int connect_state = 0;
	char TCP_str[50] = "AT+CIPSTART=\"TCP\",\"";
	strcat(TCP_str, IP);
	strcat(TCP_str, "\",");
	strcat(TCP_str, PORT);
	strcat(TCP_str, "\r\n");
	
	// �����ȴ������ɹ���Ϣ
	while ((connect_state != 1) && (connect_state != -2)) { // [-2] ALREADY CONNECT @todo ����Ը�״̬���Ƿ���Լ�����������Ĳ���
		
		// �����ȴ�����TCP����
		while (!GSM_AT_Recall(TCP_str, "OK")) {
			OSTimeDlyHMSM(0,0,1,0);
		}
	
		connect_state = GSM_Receive_Data_Connect();
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
