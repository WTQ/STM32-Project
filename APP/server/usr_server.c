/**
 * C/S架构封包相关
 *
 * @author 王特
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "usr_server.h"
#include "urlencode.h"

const char *cStart = "\xFF";
const char *cEnd = "\x00";

char perTemp[200];
bool SendNetPacket(struct tcp_pcb *pcb, NetPacket *npPacket)
{
	err_t err;
	uint32_t npTotalLen;
	// tcp_write最后的参数为0，不拷贝缓冲区，为1时拷贝缓冲区，目测拷贝缓冲区BUG严重
	perTemp[0] = 0xFF;
	memcpy(perTemp + 1, &(npPacket->netPacketHead), sizeof(NetPacketHead));
	memcpy(perTemp + 1 + sizeof(NetPacketHead), npPacket->packetBody, npPacket->netPacketHead.nLen);
	perTemp[1 + sizeof(NetPacketHead) + npPacket->netPacketHead.nLen] = 0x00;
	
	npTotalLen = 1 + sizeof(NetPacketHead) + npPacket->netPacketHead.nLen + 1;
	err = tcp_write(pcb, perTemp,  npTotalLen, 1);
	
	if(err != ERR_OK) {
		return FALSE;
	}
	
	// 连续的tcp_write后，调用第一次tcp_output有效，直接输出，之后在调用tcp_output无效?
	err = tcp_output(pcb);
	
	if(err != ERR_OK) {
		return FALSE;
	}

	return TRUE;
}

bool SendCharBuff(struct tcp_pcb *pcb, char *buf, int len, PACKET_TYPE ePType)
{
	NetPacket npPacket;
	npPacket.netPacketHead.version = 0x02;
	npPacket.netPacketHead.nLen = len;
	npPacket.netPacketHead.ePType = ePType;
	npPacket.packetBody = buf; 
	return SendNetPacket(pcb, &npPacket);
}

bool SendPacketEnd(struct tcp_pcb *pcb)
{
	NetPacket npPacket;
	npPacket.netPacketHead.version = 0x02;
	npPacket.netPacketHead.nLen = 0;
	npPacket.netPacketHead.ePType = PT_CMDEND;
	npPacket.packetBody = NULL; 
	return SendNetPacket(pcb, &npPacket);
}

bool TCP_SendRecord(struct tcp_pcb *pcb, WM_Record *WMRecord)
{
	char WMTempData[50], strTemp[10];
	WMTempData[0] = '\0';
	
	sprintf(strTemp, "%d", WMRecord->ID);
	strcat(WMTempData, strTemp);
	strcat(WMTempData, ",");
	
	sprintf(strTemp, "%d", WMRecord->WMData_ID);
	strcat(WMTempData, strTemp);
	strcat(WMTempData, ",");
	
	sprintf(strTemp, "%d", WMRecord->FrameNum);
	strcat(WMTempData, strTemp);
	
	return SendCharBuff(pcb, WMTempData, strlen(WMTempData), PT_TEXT);
}

bool TCP_AddWMData(char *data)
{
	int i, j;
	WM_Data WMData;
	char WMContent[37];

	WMData.ID = atoi(data + 7);
	j = 0;
	for(i = 7; i < 200; ++i) {
		if(*(data + i) != '\0') {
			continue;
		}
		if(j == 0) {
			URL_Decode(data + i + 1, WMContent);
			WMData.WaterMark = (uint8_t *)WMContent;
		}
		if(j > 0) {
			break;
		}
		++j;
	}
	WriteWM(&WMData);
	return TRUE;
}

bool TCP_SendWMData(struct tcp_pcb *pcb, WM_Data *WMData)
{
	int j;
	char strTemp[37] = {0};
	char WMTempData[45] = {0};
	
	sprintf(strTemp, "%d", WMData->ID);
	strcat(WMTempData, strTemp);
	strcat(WMTempData, ",");
				
	for(j = 0; j < 12; ++j) {
		sprintf((strTemp + j * 3), "%02X ", (char)WMData->WaterMark[j]);
	}
	strTemp[36] = '\0';
	strcat(WMTempData, strTemp);
	strcat(WMTempData, "\r\n");
			
	return SendCharBuff(pcb, WMTempData, strlen(WMTempData), PT_TEXT);
}
