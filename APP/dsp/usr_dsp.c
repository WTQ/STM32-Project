/**
 * USART���ƺ�ˮӡ������ط�װ
 *
 * @author ����
 * @�޸�   �ö���
 */

#include <stdlib.h>
#include <string.h>
#include "usr_dsp.h"
#include "usr_flash.h"
#include "usr_usart2.h"
#include "usr_server.h"

// ���ȫ�ֱ���
Message_Data MessageData;
WM_Array Array_Data[WM_GROUP_NUM];
uint8_t DSPState = DSP_STOPPING;
WM_Record g_WMRecord;
int FrameNum;	//֡����α���������ж����������Ч֡��
int flag;	// ���Ź����ܣ��ж�ˮӡ������
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
	FrameNum = 0;
	flag = 0;
	
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
	WM_Array MessageTemp = {0};
	
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
	
	if(MessageData.rDataLen > 12) {							  // ��������ݴ���12
		MessageData.rDataLen = 1;
		MessageData.TransState = TRANS_ING;
		MessageData.HandleState = HANDLE_IDLE;
	}
	
	if(MessageData.rDataLen == 12) {						  // ������յ����ֽ�������12
		MessageData.TransState = TRANS_OK;
		MessageData.HandleState = HANDLE_READY;
	}
	
	MessageData.Data[MessageData.rDataLen-1] = temp;		  // �ѽ��յ���ˮӡ���ݱ��浽ˮӡ������Ϣ�ṹ����
	
	if(MessageData.TransState == TRANS_OK && MessageData.HandleState == HANDLE_READY) {		// ��������������׼������
		MessageData.HandleState = HANDLE_OK;
		MessageData.sTime = 0;
		if(WMFlag.WM_Data_Num <= 0) {
			return;
		}
		memcpy((void *) MessageTemp.Data, (void *) MessageData.Data, 12);
		Push_Data(MessageTemp);
		if(FrameNum < WM_GROUP_NUM)
			return;
		else
			PreHandle();		
	}
	return;
}

//��֡ˮӡ����ѹ����������
void Push_Data(WM_Array MessageTemp)
{
	int i = 0;
	if(flag < FRAME_FLAG)
		flag++;
	if((flag == FRAME_FLAG) && (g_WMRecord.FrameNum > 0)) {
		WriteRecord(&g_WMRecord);
		USART_SendRecord(&g_WMRecord);
		TCP_SendRecord(gPcb, &g_WMRecord);	
		g_WMRecord.WMData_ID = 0;
		g_WMRecord.FrameNum = 0;
	} 

	if(FrameNum < WM_GROUP_NUM)
	{
		FrameNum++;
		Array_Data[FrameNum-1] = MessageTemp;
	}
	else
	{
		for(i = 0; i < FrameNum - 1; i++)
		{
			Array_Data[i] = Array_Data[i+1];
		}
		Array_Data[i] = MessageTemp;
	}
}

//�ж�ˮӡ�Ƿ���ڵ�Ԥ�о�
void PreHandle(void)
{
	int i = 0, j = 0, num = 0;
	uint8_t temp[12] = {0};
	for(i = 0; i < FrameNum; i += 2)
	{
		for(j = 0; j < 12; j++)
		{
			temp[j] = ~(Array_Data[i].Data[j] ^ Array_Data[i+1].Data[j]);
		}
		num = Find_Str(temp,12);
		if(num > WM_AND_MIN)
			return;
	}
	Handle_Water();		//��ʼ����
}

// ����ˮӡDSP��������ˮӡ����
void Handle_Water(void)
{
	int i, j, k;
	uint8_t temp1[12] = {0}, temp2[12] = {0}, test[12] = {0};
	int repeat = 0;
	uint16_t WM_ID = 0;
//	int nMaxSim1 = 0, nMaxSim2 = 0;
	int num = 0;
	WM_Data t_WMData;
//	WM_Record t_WMRecord1 = {0};
//	WM_Record t_WMRecord2 = {0};
	
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	// 	��ƥ��ˮӡ���
	for(i = 0; i < WMFlag.WM_Data_Num; ++i){
		GetWM(&t_WMData, i);											// WMFlag.WM_Data_NumΪ�������ݿ�ˮӡ��������������
		for (k = 0; k < WM_GROUP_NUM; k += 2)
		{
			// ǰ12���ֽڲ��ֵ�ˮӡƥ��
			for(j = 0; j < 12; ++j) {
				temp1[j] = ~(Array_Data[k].Data[j] ^ t_WMData.WaterMark[j]);					// ��λͬ��
				temp2[j] = (Array_Data[k+1].Data[j] ^ t_WMData.WaterMark[j]);					// ��λ���
			}
			num = Find_Str(temp1, 12) + Find_Str(temp2, 12);
			if(num >= /*nMaxSim1*/Array_Data[k].Right_Num) {
				//nMaxSim1 = num;
				//t_WMRecord1.WMData_ID = t_WMData.ID;
				Array_Data[k].Right_Num = num;
				Array_Data[k].WM_ID = t_WMData.ID;
				memcpy((void *) Array_Data[k].WM_Data, (void *) t_WMData.WaterMark, 12);
				Array_Data[k+1].Right_Num = num;
				Array_Data[k+1].WM_ID = t_WMData.ID;
				memcpy((void *) Array_Data[k+1].WM_Data, (void *) t_WMData.WaterMark, 12);
			}
		}
	}
	// ƥ��������ˮӡ�����ж��飬��ǰ��Ϊ׼���˴����Ż���
	for(k = 0; k < WM_GROUP_NUM; k += 2)
	{
		num = 0;
		for(j = k; j < WM_GROUP_NUM; j += 2)
			if(Array_Data[j].Right_Num == Array_Data[k].Right_Num)
				num++;
		if(num > repeat)
		{
			repeat = num;
			WM_ID = Array_Data[k].WM_ID;
			memcpy((void *) test, (void *) Array_Data[k].WM_Data, 12);
		}
	}
	// ����ƥ���������ͬ���򷵻�
	if(repeat <= 1)
		return;
	// ȡ����
	ZhongArr(temp1, temp2);// ���ô˿ռ䣬��С����
	// ���������Ԥ�о�ˮӡƥ��λ��
	for(j = 0; j < 12; ++j) {
		temp1[j] = ~(temp1[j] ^ test[j]);					// ��λͬ��
		temp2[j] = (temp2[j] ^ test[j]);					// ��λ���
	}
	num = Find_Str(temp1, 12)+Find_Str(temp2, 12);
	if(num <= 65)
		return;
	//�ж��о���ID����һ��ID�Ƿ���ͬ����ͬ���ٽ���ʱ���ۼ�
	if(WM_ID == g_WMRecord.WMData_ID) {
		if(flag < FRAME_FLAG){
			g_WMRecord.FrameNum += flag;
		}else {
			g_WMRecord.FrameNum += DSP_FRAME_MUL;
		}
		FrameNum = 0;	//����α���
		flag = 0;
		return;
	} 
	//����ͬ����֮ǰ�����ݴ�������ʾ�����������´洢��¼
	if(g_WMRecord.FrameNum > 0) {
		WriteRecord(&g_WMRecord);
		USART_SendRecord(&g_WMRecord);
		TCP_SendRecord(gPcb, &g_WMRecord);
		g_WMRecord.WMData_ID = WM_ID;
		g_WMRecord.FrameNum = DSP_FRAME_MUL;
		FrameNum = 0;	//����α���
		flag = 0;
		return;
	}	
	//
	g_WMRecord.WMData_ID = WM_ID;
	g_WMRecord.FrameNum = DSP_FRAME_MUL;
	FrameNum = 0;	//����α���
	flag = 0;
	////////////////////////////////////////////////////////
}

void DSP_Water_Handle(__IO uint32_t localtime)
{
	// ִ������ʱ��
	static uint32_t DSP_Handle_Timer = 0;
/*	if (localtime - DSP_Handle_Timer >= DSP_TMR_INTERVAL)
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
*/
}

int8_t Find_One(uint8_t n) 						//����1�ĸ���
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
// output����Ϊ����24�ֽڵĻ���ռ䣬�������
void ZhongArr(uint8_t *output1, uint8_t *output2)
{
	int odd = 0;
	unsigned char temp;
	unsigned char zhong = 0;
	int i, j, k;
	for(i = 0; i < 24; ++i) {
		for(j = 0; j < 8; ++j) {
			for(k = 0; k < WM_GROUP_NUM/2; k++) {
				if(i < 12) {
					// 2k���ж�
					temp = Array_Data[2*k].Data[i] >> j;
				} else {
					// 2k+1���ж�
					temp = Array_Data[2*k + 1].Data[i%12] >> j;
				}
				if(temp % 2 != 0) {
					++odd;
				}
			}
			// һλ�ļ���������jλȡ1
			if(odd > WM_GROUP_NUM/4) {
				zhong += (1 << j);
			}
			odd = 0;
		}
		// һ���ֽڵļ�����
		if(i<12) {
			output1[i] = zhong;
		}
		else {
			output2[i%12] = zhong;
		}
		zhong = 0;
	}
}
