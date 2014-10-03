#include "usr_http.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gsm_gprs.h"

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
	int len;
	sprintf(c, "%d", length);
	buffer[0] = '\0';
	strcat(buffer, "POST /wm/submit HTTP/1.1\r\nHost: tx.te168.cn\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ");
	strcat(buffer, c);
	strcat(buffer, "\r\n\r\n");
	strcat(buffer, postdata);
	len = strlen(buffer);
	buffer[len] = 0x1A;
	buffer[len + 1] = '\0';
}

void GSM_Get_Record(char *buffer)
{
	int len;
	buffer[0] = '\0';
	strcat(buffer, "GET /wm/getid HTTP/1.1\r\nHost: tx.te168.cn\r\nContent-Length: 0\r\n\r\n");
	len = strlen(buffer);
	buffer[len] = 0x1A;
	buffer[len + 1] = '\0';
}

void GSM_Post_Record(char *Buffer, WM_Record *WMRecord)
{
	char WMTempData[100], strTemp[10], str[25];
	int i = 0;
	WMTempData[0] = '\0';
	for (i = 0; i < 12; i ++) {
		sprintf(str + i * 2 , "%02x", WMRecord->WMData[i]);
	}
	str[24] = '\0';
		
//	strcat(WMTempData, str);


	post_data(WMTempData, "type","data");
	
	sprintf(strTemp, "%d", WMRecord->ID); 
	post_data(WMTempData, "WMRecord_ID", strTemp);
	
	sprintf(strTemp, "%d", WMRecord->FrameNum);
	post_data(WMTempData, "FrameNum", strTemp);	
	
	post_data(WMTempData, "WMData", str);
	
	post_http(Buffer, WMTempData);
}

// ������
void GSM_Post_Beat(char *Buffer)
{
	char WMTempData[50] = {0};

	post_data(WMTempData, "type", "beat");
	post_http(Buffer, WMTempData);
}

// New Function
void GSM_SendRecord(WM_Record* WMRecord)
{
	//������������
	char msg[50];
	GPRS_CopyWMRecord(msg, WMRecord);

	OSMboxPost(Com1_Mbox, (void*)&msg);	
}

// ������Ϣͷ��ֻ�������յ�����Ϣ����
bool GSM_Receive_Record(GSM_RECEIVE_RECORD *pReceive)
{
	// ��ʱ����
	int n;
	
	// λ����Ϣ
	int pos = 0;
	
	// �Ƿ��״�������\r\n\r\n����ʶ
	bool flag = FALSE;
	
	if (!GPRS_TCP_Receive(pReceive)) {
		return FALSE;
	}

	// ����������Ϣ
	for (n = 0; n < pReceive->Data_Count; n++) {
		if ((!flag) &&(n < pReceive->Data_Count - 4) // ��ֹ���γ��֡�\r\n\r\n��������ֹ���
			&& ((pReceive->Data)[n] == '\r')
			&& ((pReceive->Data)[n + 1] == '\n')
			&& ((pReceive->Data)[n + 2] == '\r')
			&& ((pReceive->Data)[n + 3] == '\n')) {
				
				pos = n + 4;
				//��־λ˵���Ѿ�������˹���
				flag = TRUE;
		} else if (flag && (n >= pos)) {
			(pReceive->Data)[n - pos] = (pReceive->Data)[n];
		}
	}
	pReceive->Data_Count = pReceive->Data_Count - pos;

	return TRUE;
}

// �����յ�������
int Calculate(GSM_RECEIVE_RECORD *pReceive)
{
	int Num;
	char buffer[10] = {0};
	memcpy(buffer, pReceive->Data, pReceive->Data_Count);
	Num = atoi(buffer);
	return Num;
}
