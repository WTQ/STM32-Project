#include "GSM.h"
#include "GSM_TypeDefine.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "main.h"
#include "usr_server.h"
#include "usr_dsp.h"
#include "armserver.h"
#include "urlencode.h"

int strmycmp(unsigned char* str1, unsigned char* str2, int len)
{
	int i=0;
	int num=0;
	for(i=0;i<len;i++)
	{
		if(str1[i]==str2[i]) ;
		else num++;
	}
	return num;
}

void GSM_Handle(void)
{
	int i;
	uint32_t lastRecordID;
	WM_Data WMData;
	WM_Record WMRecord;
	char netBuffer[30];
	extern GSM_CMT CMT;
	extern INT8U GSM_pData[GSM_MAX_DATA_LEN];
	unsigned char* data;
	INT8U* num;
	char Buffer[512]={0};

	data=&(CMT.Content[0]);
	num=&(GSM_pData[2]);

	if(strmycmp(data, "/connect", strlen("/connect")) == 0) {
			CMT.Content[0]=0;
			// 连接发送包
		} else if(strmycmp(num, "13303948368", strlen("13303948368")) == 0) {
		    GSM_pData[2]=0;
			DSP_Start();
		} else if(strmycmp(data, "/stop", strlen("/stop")) == 0) {
			CMT.Content[0]=0;
			DSP_Stop();
		} else if(strmycmp(data, "/status", strlen("/status")) == 0) {
			CMT.Content[0]=0;
			if(DSP_RUNNING == DSPState) {
				sprintf(netBuffer, "started,%d,%d", WMFlag.WM_Record_Address - WM_RECORD_ADDR_START, WM_RECORD_ADDR_LEN);
			} else {
				sprintf(netBuffer, "stopped,%d,%d", WMFlag.WM_Record_Address - WM_RECORD_ADDR_START, WM_RECORD_ADDR_LEN);
			}

			strcat(Buffer,"Post / HTTP/1.1\r\nHost: 50.116.10.26\r\n\r\nRecord=");
			strcat(Buffer,netBuffer);
			GSM_GPRS_TCP_Connect("50.116.10.26","80");
			GSM_GPRS_SEND(Buffer);
			strcpy(Buffer,"\0");

		} else if(strmycmp(data, "/updrecord", strlen("/updrecord")) == 0) {
			CMT.Content[0]=0;
			lastRecordID = atoi(data + strlen("/updrecord") + 1);
			i = GetRecordIndexById(lastRecordID + 1);
			if(i != -1) {
			strcat(Buffer,"Post / HTTP/1.1\r\nHost: 50.116.10.26\r\n\r\nRecord=");				
				while(i < WMFlag.WM_Record_Num) {
					GetRecord(&WMRecord, i);
					GPRS_CopyWMRecord(Buffer,&WMRecord);
					++i;
				}
			GSM_GPRS_TCP_Connect("50.116.10.26","80");
			GSM_GPRS_SEND(Buffer);
			strcpy(Buffer, "\0");	
			}
		
		} else if(strmycmp(data, "/eraserecord", strlen("/eraserecord")) == 0) {
			CMT.Content[0]=0;
			EraseRecord();
		} else if(strmycmp(data, "/wmdata", strlen("/wmdata")) == 0) {
			CMT.Content[0]=0;
		 strcat(Buffer,"Post / HTTP/1.1\r\nHost: 50.116.10.26\r\n\r\nRecord=");
			for(i = 0; i < WMFlag.WM_Data_Num; ++i) {
				GetWM(&WMData, i);
				GPRS_CopyWMData(Buffer,&WMData);
			}

			GSM_GPRS_TCP_Connect("50.116.10.26","80");
			GSM_GPRS_SEND(Buffer);
			strcpy(Buffer, "\0");

		} else if(strmycmp(data, "/wmadd", strlen("/wmadd")) == 0) {
			CMT.Content[0]=0;
	    	TCP_AddWMData(data);
		} else if(strmycmp(data, "/wmerase", strlen("/wmerase")) == 0) {
			CMT.Content[0]=0;
			EraseWM();
		} else {
//			strcat(Buffer,"Post / HTTP/1.1\r\nHost: 50.116.10.26\r\n\r\nRecord=");
//			strcat(Buffer, "This is a string for test.\r\n");
//		 	GSM_GPRS_TCP_Connect("50.116.10.26","80");
//			GSM_GPRS_SEND(Buffer);
			// 连接测试用
			// SendPacketEnd(pcb);
		}

}
