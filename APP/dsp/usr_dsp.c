/**
 * USART控制和水印接收相关封装
 *
 * @author 王特
 */

#include <stdlib.h>
#include <string.h>
#include "usr_dsp.h"
#include "usr_flash.h"
#include "usr_usart2.h"
#include "usr_server.h"
#include "GSM.h"

// 相关全局变量
Message_Data MessageData;
uint8_t DSPState = DSP_STOPPING;
WM_Record g_WMRecord;
extern struct tcp_pcb *gPcb;

void DSP_Init(void)
{
	MessageData.TransState = TRANS_IDLE;
	MessageData.HandleState = HANDLE_IDLE;
	MessageData.Command = M_CMD_UNDEFNIE;
	MessageData.rDataLen = 0;
	MessageData.sTime = 0;
	g_WMRecord.WMData_ID = 0;
	g_WMRecord.FrameNum = 0;
	
	// 设置PA5管脚为高电平，LED灯灭
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

void DSP_Start(void)
{
	USART_SendData(USART3, M_CMD_START);
	
	// 设置PA5管脚为低电平，LED灯亮
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	DSPState = DSP_START_HANDS;
}

void DSP_Stop(void)
{
	USART_SendData(USART3, M_CMD_STOP);
	
	// 设置PA5管脚为高电平，LED灯灭
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	DSPState = DSP_STOPPING;
}

// 接收DSP传过来的水印数据
void DSP_ReseveMsg(void)
{
	uint8_t temp;
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET) {
		return;
	}
	temp = USART_ReceiveData(USART3);	                      // 接收串口数据1byte
	++MessageData.rDataLen;
	
	if(DSPState == DSP_START_HANDS && temp == 0xbb) {		  // 如果DSP刚开始传输数据
		MessageData.rDataLen = 0;
		MessageData.TransState = TRANS_ING;
		MessageData.HandleState = HANDLE_IDLE;
		DSPState = DSP_RUNNING;
		return;
	}
	
	if(MessageData.rDataLen > 24) {							  // 如果？数据大于24
		MessageData.rDataLen = 1;
		MessageData.TransState = TRANS_ING;
		MessageData.HandleState = HANDLE_IDLE;
	}
	
	if(MessageData.rDataLen == 24) {						  // 如果接收到的字节数等于24
		MessageData.TransState = TRANS_OK;
		MessageData.HandleState = HANDLE_READY;
	}
	
	MessageData.Data[MessageData.rDataLen-1] = temp;		  // 把接收到的水印数据保存到水印接收消息结构体中
	return;
}

// 处理水印DSP传过来的水印数据
void Handle_Water(void)
{
	int i, j;
	uint8_t temp[12] = {0};
	int nMaxSim1 = 0, nMaxSim2 = 0, num;
	WM_Data t_WMData;
	WM_Record t_WMRecord1 = {0};
	WM_Record t_WMRecord2 = {0};
	
	if(MessageData.TransState == TRANS_OK && MessageData.HandleState == HANDLE_READY) {		// 如果传输结束并且准备处理
		MessageData.HandleState = HANDLE_OK;
		MessageData.sTime = 0;
		if(WMFlag.WM_Data_Num <= 0) {
			return;
		}
		for(i = 0; i < WMFlag.WM_Data_Num; ++i) {
			GetWM(&t_WMData, i);
			
			// 前12个字节部分的水印匹配
			for(j = 0; j < 12; ++j) {
				temp[j] = ~(MessageData.Data[j] ^ t_WMData.WaterMark[j]);					// 按位异或
			}
			num = Find_Str(temp, 12);
			if(num >= nMaxSim1) {
				nMaxSim1 = num;
				t_WMRecord1.WMData_ID = t_WMData.ID;
			}
			
			// 后12个字节部分的水印匹配
			for(j = 0; j < 12; ++j) {
				temp[j] = ~(MessageData.Data[j+12] ^ t_WMData.WaterMark[j]);
			}
			num = Find_Str(temp, 12);
			if(num >= nMaxSim2) {
				nMaxSim2 = num;
				t_WMRecord2.WMData_ID = t_WMData.ID;
			}
		}
		
		// 前100帧的数据匹配写入
		if(nMaxSim1 >= WM_AND_MIN) {
			if(t_WMRecord1.WMData_ID == g_WMRecord.WMData_ID) {
				g_WMRecord.FrameNum += DSP_FRAME_MUL;
			} else {
				if(g_WMRecord.FrameNum > 0) {
					WriteRecord(&g_WMRecord);
					USART_SendRecord(&g_WMRecord);
					TCP_SendRecord(gPcb, &g_WMRecord);
				}
				g_WMRecord.WMData_ID = t_WMRecord1.WMData_ID;
				g_WMRecord.FrameNum = DSP_FRAME_MUL;
			}
		} else {
			if(g_WMRecord.FrameNum > 0) {
				WriteRecord(&g_WMRecord);
				USART_SendRecord(&g_WMRecord);
				GSM_SendRecord(&g_WMRecord);
//				TCP_SendRecord(gPcb, &g_WMRecord);
			}
			g_WMRecord.WMData_ID = 0;
			g_WMRecord.FrameNum = 0;
		}

		// 后100帧的数据匹配写入
		if(nMaxSim2 >= WM_AND_MIN) {
			if(t_WMRecord2.WMData_ID == g_WMRecord.WMData_ID) {
				g_WMRecord.FrameNum += DSP_FRAME_MUL;
			} else {
				if(g_WMRecord.FrameNum > 0) {
					WriteRecord(&g_WMRecord);
					USART_SendRecord(&g_WMRecord);
					GSM_SendRecord(&g_WMRecord);
//					TCP_SendRecord(gPcb, &g_WMRecord);
				}
				g_WMRecord.WMData_ID = t_WMRecord2.WMData_ID;
				g_WMRecord.FrameNum = DSP_FRAME_MUL;
			}
		} else {
			if(g_WMRecord.FrameNum > 0) {
				WriteRecord(&g_WMRecord);
				USART_SendRecord(&g_WMRecord);
				TCP_SendRecord(gPcb, &g_WMRecord);
			}
			g_WMRecord.WMData_ID = 0;
			g_WMRecord.FrameNum = 0;
		}
	}
}

void DSP_Water_Handle(__IO uint32_t localtime)
{
	// 执行任务时间
	static uint32_t DSP_Handle_Timer = 0;
	if (localtime - DSP_Handle_Timer >= DSP_TMR_INTERVAL)
	{
		DSP_Handle_Timer = localtime;
		
		// 空闲200多帧的样子写入缓冲的记录
		if(MessageData.HandleState == HANDLE_OK) {
			++MessageData.sTime;
			if(MessageData.sTime > 130 && g_WMRecord.FrameNum > 0) {
				WriteRecord(&g_WMRecord);
				USART_SendRecord(&g_WMRecord);
				TCP_SendRecord(gPcb, &g_WMRecord);
				MessageData.sTime = 0;
				MessageData.HandleState = HANDLE_IDLE;
				
				g_WMRecord.WMData_ID = 0;
				g_WMRecord.FrameNum = 0;
			}
		}
	}
}

int8_t Find_One(uint8_t n) 
{
	int8_t count = 0; 
	while(n > 0){ 
		n &= (n - 1); 
		++count; 
	}
	return count; 
}

int8_t Find_Str(uint8_t *str, int8_t num)
{
	int i, count = 0;
	for(i = 0; i < num; ++i) {
		count += Find_One((uint8_t)str[i]);
	}
	return count;
}
