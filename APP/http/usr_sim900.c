/**
 * SIM900操作相关封装
 * 
 * @author 王特
 */
 
#include "usr_sim900.h"
 
 
void GPRS_Init1(void)
{
	// 附着网络
	GSM_TxString("AT+CGATT=1\r\n");
	OSTimeDlyHMSM(0, 0, 1, 0);
	
	// 设置APN
	GSM_TxString("AT+CSTT\r\n");
	OSTimeDlyHMSM(0, 0, 1, 0);
	
	// 激活移动场景
	GSM_TxString("AT+CIICR\r\n");
	OSTimeDlyHMSM(0, 0, 3, 0);
	
	// 获得本地模块内部IP地址
	GSM_TxString("AT+CIFSR\r\n");
	OSTimeDlyHMSM(0, 0, 2, 0);
}

void GPRS_DNS_Init(void)
{
	// 查询DNS的设置情况
	GSM_TxString("AT+CDNSCFG?\r\n");
	OSTimeDlyHMSM(0, 0, 1, 0);
	
	// 设置主DNS和次DNS
	GSM_TxString("AT+CDNSCFG=" GPRS_DNS_PRI "," GPRS_DNS_SEC);
	OSTimeDlyHMSM(0, 0, 1, 0);
}

void GPRS_TCP_Connet(char *ip, char *port)
{
	// 建立TCP/IP连接
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
	// 关闭TCP连接
	GSM_TxString("AT+CIPCLOSE=1\r\n");
	OSTimeDlyHMSM(0, 0, 1, 0);
}

void GRPS_TCP_Recv(void)
{
}
