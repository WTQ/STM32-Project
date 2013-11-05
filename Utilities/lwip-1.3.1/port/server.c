/**
 * C/S架构server端，采用LWIP的RAM API实现
 *
 * @author 王特
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "main.h"
#include "usr_server.h"
#include "usr_dsp.h"
#include "armserver.h"
#include "urlencode.h"

// 全局的gPcb指针
struct tcp_pcb *gPcb = NULL;

/**
 * recv时的回调函数
 */
static err_t Server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	char *data;
	int i;
	uint32_t lastRecordID;
	WM_Data WMData;
	WM_Record WMRecord;
	char netBuffer[30];

	if (p != NULL && err == ERR_OK) {
		tcp_recved(pcb, p->tot_len);
	
		// 获取接收到数据的指针
		data = p->payload;
	
		if(strncmp(data, "/connect", strlen("/connect")) == 0) {
			// 连接发送包
		} else if(strncmp(data, "/start", strlen("/start")) == 0) {
			DSP_Start();
		} else if(strncmp(data, "/stop", strlen("/stop")) == 0) {
			DSP_Stop();
		} else if(strncmp(data, "/status", strlen("/status")) == 0) {
			if(DSP_RUNNING == DSPState) {
				sprintf(netBuffer, "started,%d,%d", WMFlag.WM_Record_Address - WM_RECORD_ADDR_START, WM_RECORD_ADDR_LEN);
			} else {
				sprintf(netBuffer, "stopped,%d,%d", WMFlag.WM_Record_Address - WM_RECORD_ADDR_START, WM_RECORD_ADDR_LEN);
			}
			SendCharBuff(pcb, netBuffer, strlen(netBuffer), PT_TEXT);
			SendPacketEnd(pcb);
		} else if(strncmp(data, "/updrecord", strlen("/updrecord")) == 0) {
			lastRecordID = atoi(data + strlen("/updrecord") + 1);
			i = GetRecordIndexById(lastRecordID + 1);
			if(i != -1) {
				while(i < WMFlag.WM_Record_Num) {
					GetRecord(&WMRecord, i);
					TCP_SendRecord(pcb, &WMRecord);
					++i;
				}
			}
			SendPacketEnd(pcb);
		} else if(strncmp(data, "/eraserecord", strlen("/eraserecord")) == 0) {
			EraseRecord();
		} else if(strncmp(data, "/wmdata", strlen("/wmdata")) == 0) {
			for(i = 0; i < WMFlag.WM_Data_Num; ++i) {
				GetWM(&WMData, i);
				TCP_SendWMData(pcb, &WMData);
			}
		} else if(strncmp(data, "/wmadd", strlen("/wmadd")) == 0) {
	    	TCP_AddWMData(data);
		} else if(strncmp(data, "/wmerase", strlen("/wmerase")) == 0) {
			EraseWM();
		} else {
			// 连接测试用
			// SendPacketEnd(pcb);
		}
		
		// 释放pbuf资源
		pbuf_free(p);
	}
	else if (err == ERR_OK) {
		pbuf_free(p);
		return tcp_close(pcb);
	}
	return ERR_OK;
}

/**
 * accept时的回调函数
 */
static err_t Server_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
	tcp_err(pcb, Server_conn_err);
	tcp_recv(pcb, Server_recv);
  
	// tcp_sent的回调
	tcp_sent(pcb, Server_sent);
  
	// 获取全局的tcp_pcb结构体指针
	gPcb = pcb;
	return ERR_OK;
}

/**
 * tcp_write成功时的回调函数
 */
static err_t Server_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	return ERR_OK;
}

/**
 * 服务端初始化
 */
void Server_init(void)
{
	struct tcp_pcb *pcb;

	pcb = tcp_new();	                		 	
	tcp_bind(pcb, IP_ADDR_ANY, ARM_SERVER_PORT);       
	pcb = tcp_listen(pcb);
	tcp_accept(pcb, Server_accept);	
}

/**
 * 发生错误时的回调函数
 */
static void Server_conn_err(void *arg, err_t err)
{
	
}
