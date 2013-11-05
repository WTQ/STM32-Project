/**
 * USART���ƺ�ˮӡ������ط�װ
 *
 * @author ����
 */

#include <stdlib.h>
#include <string.h>
#include "usr_dsp.h"
#include "usr_flash.h"
#include "usr_usart2.h"
#include "usr_server.h"
#include "GSM.h"

// ���ȫ�ֱ���
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
	
	// ����PA5�ܽ�Ϊ�ߵ�ƽ��LED����
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

void DSP_Start(void)
{
	USART_SendData(USART3, M_CMD_START);
	
	// ����PA5�ܽ�Ϊ�͵�ƽ��LED����
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	DSPState = DSP_START_HANDS;
}

void DSP_Stop(void)
{
	USART_SendData(USART3, M_CMD_STOP);
	
	// ����PA5�ܽ�Ϊ�ߵ�ƽ��LED����
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	DSPState = DSP_STOPPING;
}

// ����DSP��������ˮӡ����
void DSP_ReseveMsg(void)
{
	uint8_t temp;
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET) {
		return;
	}
	temp = USART_ReceiveData(USART3);	                      // ���մ�������1byte
	++MessageData.rDataLen;
	
	if(DSPState == DSP_START_HANDS && temp == 0xbb) {		  // ���DSP�տ�ʼ��������
		MessageData.rDataLen = 0;
		MessageData.TransState = TRANS_ING;
		MessageData.HandleState = HANDLE_IDLE;
		DSPState = DSP_RUNNING;
		return;
	}
	
	if(MessageData.rDataLen > 24) {							  // ��������ݴ���24
		MessageData.rDataLen = 1;
		MessageData.TransState = TRANS_ING;
		MessageData.HandleState = HANDLE_IDLE;
	}
	
	if(MessageData.rDataLen == 24) {						  // ������յ����ֽ�������24
		MessageData.TransState = TRANS_OK;
		MessageData.HandleState = HANDLE_READY;
	}
	
	MessageData.Data[MessageData.rDataLen-1] = temp;		  // �ѽ��յ���ˮӡ���ݱ��浽ˮӡ������Ϣ�ṹ����
	return;
}

// ����ˮӡDSP��������ˮӡ����
void Handle_Water(void)
{
	int i, j;
	uint8_t temp[12] = {0};
	int nMaxSim1 = 0, nMaxSim2 = 0, num;
	WM_Data t_WMData;
	WM_Record t_WMRecord1 = {0};
	WM_Record t_WMRecord2 = {0};
	
	if(MessageData.TransState == TRANS_OK && MessageData.HandleState == HANDLE_READY) {		// ��������������׼������
		MessageData.HandleState = HANDLE_OK;
		MessageData.sTime = 0;
		if(WMFlag.WM_Data_Num <= 0) {
			return;
		}
		for(i = 0; i < WMFlag.WM_Data_Num; ++i) {
			GetWM(&t_WMData, i);
			
			// ǰ12���ֽڲ��ֵ�ˮӡƥ��
			for(j = 0; j < 12; ++j) {
				temp[j] = ~(MessageData.Data[j] ^ t_WMData.WaterMark[j]);					// ��λ���
			}
			num = Find_Str(temp, 12);
			if(num >= nMaxSim1) {
				nMaxSim1 = num;
				t_WMRecord1.WMData_ID = t_WMData.ID;
			}
			
			// ��12���ֽڲ��ֵ�ˮӡƥ��
			for(j = 0; j < 12; ++j) {
				temp[j] = ~(MessageData.Data[j+12] ^ t_WMData.WaterMark[j]);
			}
			num = Find_Str(temp, 12);
			if(num >= nMaxSim2) {
				nMaxSim2 = num;
				t_WMRecord2.WMData_ID = t_WMData.ID;
			}
		}
		
		// ǰ100֡������ƥ��д��
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

		// ��100֡������ƥ��д��
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
	// ִ������ʱ��
	static uint32_t DSP_Handle_Timer = 0;
	if (localtime - DSP_Handle_Timer >= DSP_TMR_INTERVAL)
	{
		DSP_Handle_Timer = localtime;
		
		// ����200��֡������д�뻺��ļ�¼
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
