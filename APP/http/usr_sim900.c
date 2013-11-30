/**
 * SIM900������ط�װ
 * 
 * @author ����
 */
 
#include "usr_sim900.h"
 
 
void GPRS_Init1(void)
{
	// ��������
	GSM_TxString("AT+CGATT=1\r\n");
	OSTimeDlyHMSM(0, 0, 1, 0);
	
	// ����APN
	GSM_TxString("AT+CSTT\r\n");
	OSTimeDlyHMSM(0, 0, 1, 0);
	
	// �����ƶ�����
	GSM_TxString("AT+CIICR\r\n");
	OSTimeDlyHMSM(0, 0, 3, 0);
	
	// ��ñ���ģ���ڲ�IP��ַ
	GSM_TxString("AT+CIFSR\r\n");
	OSTimeDlyHMSM(0, 0, 2, 0);
}

void GPRS_DNS_Init(void)
{
	// ��ѯDNS���������
	GSM_TxString("AT+CDNSCFG?\r\n");
	OSTimeDlyHMSM(0, 0, 1, 0);
	
	// ������DNS�ʹ�DNS
	GSM_TxString("AT+CDNSCFG=" GPRS_DNS_PRI "," GPRS_DNS_SEC);
	OSTimeDlyHMSM(0, 0, 1, 0);
}

void GPRS_TCP_Connet(char *ip, char *port)
{
	// ����TCP/IP����
	GSM_TxString("AT+CIPSTART=\"TCP\",\"");
	GSM_TxString((INT8U *) ip);
	GSM_TxString("\",");
	GSM_TxString((INT8U *) port);
	GSM_TxString("\r\n");
	OSTimeDlyHMSM(0, 0, 4, 0);
}

void GPRS_TCP_Send1(void)
{
}

void GRRS_TCP_Close(void)
{
	// �ر�TCP����
	GSM_TxString("AT+CIPCLOSE=1\r\n");
	OSTimeDlyHMSM(0, 0, 1, 0);
}

void GRPS_TCP_Recv(void)
{
}
