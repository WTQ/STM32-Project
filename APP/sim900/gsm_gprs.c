/**
 * GSM模块的GPRS操作封装
 *
 * @author 王特、矫东航
 */

#include "gsm_gprs.h"

#include <string.h>

void GSM_Config(void)
{
	// 测试命令
	GSM_AT_Recall("AT\r\n", "OK");
			
	// 设置模块的字符集
	GSM_AT_Recall("AT+CSCS=\"GSM\"\r\n", "OK");
	
}

void GPRS_Init(void)
{	
	// 设置接收数据是否显示IP头，1设置IP头，0未设置
	GSM_AT_Recall("AT+CIPHEAD=1\r\n", "OK");
	
	// 附着网络
	GSM_AT_Recall("AT+CGATT=1\r\n", "OK");
	
	// 设置APN
	GSM_AT_Recall("AT+CSTT\r\n", "OK");
	
	// 激活移动场景
	GSM_AT_Recall("AT+CIICR\r\n", "OK");
	
	// 获取本地IP地址(此处没有接收地址字符串)
	GSM_AT_Only("AT+CIFSR");
	
}

bool GPRS_TCP_Connect(char *IP, char *PORT)
{
	char TCP_str[50] = "AT+CIPSTART=\"TCP\",\"";
	strcat(TCP_str, IP);
	strcat(TCP_str, "\",");
	strcat(TCP_str, PORT);
	strcat(TCP_str, "\r\n");
	// 建立TCP连接
	GSM_AT_Recall(TCP_str, "OK");
	
	// 阻塞等待建立成功消息
	return GSM_Receive_Recall("CONNECT OK");
	
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
	// 保存”:“位置信息
	int pos;
	// 保存信息长度
	int length = 0;
	GSM_Receive_Data(pReceive);
	
	// 识别命令信息是否正确
	if (strncmp((char *)pReceive->Data, "+IPD,", 5) != 0) {
		return FALSE;
	}
	
	// 读取信息长度信息
	for (pos=5; ((pReceive->Data)[pos]!=':') && (pos<pReceive->Data_Count); pos++) {
		length = length * 10 + (int)((pReceive->Data)[pos]) - 48; // ”0“的ASCII码为十进制的48
	}
	
	// 防止未接收到“:”而引发的溢出问题
	if (pos == pReceive->Data_Count) {
		return FALSE;
	}
	
	// 验证信息长度信息
	if (length != pReceive->Data_Count - pos -1) {
		return FALSE;
	}
	
	// 抛弃命令信息
	for (n=0; n<length; n++) {
		(pReceive->Data)[n] = (pReceive->Data)[n + pos + 1];
	}
	pReceive->Data_Count = length;
	
	return TRUE;
}

void GRRS_TCP_Closed(void)
{
	// 关闭TCP连接
	GSM_AT_Recall("AT+CIPCLOSE=1\r\n", "OK");
}
