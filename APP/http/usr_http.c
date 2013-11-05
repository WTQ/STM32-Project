#include "usr_http.h"
#include <stdlib.h>
#include <string.h>

extern 	OS_EVENT* Com1_Mbox;

void post_data(char *buffer, char *name, char *value)
{
	strcat(buffer, name);
	strcat(buffer, "=");
	strcat(buffer, value);
	strcat(buffer, "&");	
}

void post_http(char *buffer, char *postdata)
{
	char c[5] = {0};
	int length = strlen(postdata);
	sprintf(c, "%d", length);
	buffer[0] = '\0';
	strcat(buffer, "POST /wm/submit HTTP/1.1\r\nHost: tx.te168.cn\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ");
	strcat(buffer, c);
	strcat(buffer, "\r\n\r\n");
	strcat(buffer, postdata);
}

void GSM_Post_Record(char *Buffer, WM_Record *WMRecord)
{
	char WMTempData[100], strTemp[10];
	WMTempData[0] = '\0';

	post_data(WMTempData, "type","data");
	
	sprintf(strTemp, "%d", WMRecord->ID); 
	post_data(WMTempData, "WMRecord_ID", strTemp);
	
	sprintf(strTemp, "%d", WMRecord->WMData_ID); 
	post_data(WMTempData, "WMData_ID", strTemp);
	
	sprintf(strTemp, "%d", WMRecord->FrameNum);
	post_data(WMTempData, "FrameNum", strTemp);

	post_http(Buffer, WMTempData);
}

//心跳包
void GSM_Post_Beat(char *Buffer)
{
	char WMTempData[50] = {0};

	post_data(WMTempData, "type", "beat");
	post_http(Buffer, WMTempData);
}

// New Function
void GSM_SendRecord(WM_Record* WMRecord)
{
	//用来发送邮箱
	char msg[50];
	GPRS_CopyWMRecord(msg, WMRecord);

	OSMboxPost(Com1_Mbox, (void*)&msg);	
}

