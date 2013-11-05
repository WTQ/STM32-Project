/**
 * USART2上位机通信
 *
 * @author 王特
 */
 
#include "main.h"
#include "usr_usart2.h"
#include "usr_dsp.h"
#include "usr_flash.h"
#include <stdio.h>
#include <string.h> 
#include <stdarg.h>

void USART2_Handle(void)
{
	uint8_t temp;
	int32_t i;
	WM_Data t_WMData;
	WM_Record t_WMRecord;
	
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET) {
		return;
	}
	temp = USART_ReceiveData(USART2);
	
	if(temp == 0xaa) {
		DSP_Start();
	} else if(temp == 0xa1) {
		for(i = 0; i < WMFlag.WM_Record_Num; ++i) {
			GetRecord(&t_WMRecord, i);
			USART_SendRecord(&t_WMRecord);
		}
	} else if(temp == 0xa2) {
		for(i = 0; i < WMFlag.WM_Data_Num; ++i) {
			GetWM(&t_WMData, i);
			USART_SendWMData(&t_WMData);
		}
	} else if(temp == 0xa3) {
		EraseRecord();
	} else if(temp == 0x55) {
		DSP_Stop();
	} else {
		USART_SendData(USART3, temp);
	}
}

void USART_SendStr(char *data, int32_t len)
{
	int32_t i;
	for(i = 0; i < len; ++i) {
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
		USART_SendData(USART2, (uint8_t)data[i]);
	}
}

void USART_SendRecord(WM_Record *WMRecord)
{
	float fWMTime;
	char WMTempData[45], strTemp[10];
	WMTempData[0] = '\0';
	
	sprintf(strTemp, "%d", WMRecord->WMData_ID);
	strcat(WMTempData, strTemp);
	strcat(WMTempData, ",");
	
	fWMTime = WMRecord->FrameNum / 25;
	strTemp[sprintf(strTemp, "%.2f", fWMTime)] = '\0';
	strcat(WMTempData, (char *)strTemp);
	strcat(WMTempData, "\r\n");
	
	USART_SendStr(WMTempData, strlen(WMTempData));
}

void USART_SendWMData(WM_Data *WMData)
{
	int32_t j;
	char WMTempData[45], strTemp[10], WMContent[37];
	
	WMTempData[0] = '\0';
	sprintf(strTemp, "%d", WMData->ID);
	strcat(WMTempData, strTemp);
	strcat(WMTempData, ",");
				
	for(j = 0; j < 12; ++j) {
		sprintf((WMContent + j * 3), "%02X ", (char)WMData->WaterMark[j]);
	}
	WMContent[36] = '\0';
	
	strcat(WMTempData, WMContent);
	strcat(WMTempData, "\r\n");
			
	USART_SendStr(WMTempData, strlen(WMTempData));
}

void USART2_Printf(const char *format,...)
{
//	USART_SendStr(format, strlen(format));
	int iOutLen;
	char buf[256];
	va_list arg_ptr;
	
	va_start(arg_ptr, format);
	iOutLen = vsprintf(buf, format, arg_ptr);
	va_end(arg_ptr);
	USART_SendStr(buf, iOutLen);
}
