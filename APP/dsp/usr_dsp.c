/**
 * USART控制和水印接收相关封装
 *
 * @author 王特
 * @修改   矫东航
 *
 * @ todo SIM900发送水印信息是可以关掉中断，但要及时开启
 */

#include <stdlib.h>
#include <string.h>
#include "usr_dsp.h"
#include "usr_flash.h"
#include "usr_usart2.h"
#include "usr_server.h"


// 相关全局变量
Message_Data MessageData;
uint8_t DSPState = DSP_STOPPING;
WM_Record g_WMRecord;
extern struct tcp_pcb *gPcb;

// 启动DSP处理task的信号量
//uint8_t DSP_FLAG;

// DSP让出task的信号量
uint8_t DSP_WMFINISH_FLAG;

uint8_t Freven[12] = {0}, Frodd[12] = {0}; // 进入的两帧数据
uint8_t FrA[WM_GROUP_NUM][12] = {0}, FrB[WM_GROUP_NUM][12] = {0}; // 判断水印有无的缓冲区
//uint8_t FrA_r[12] = {0}, FrB_r[12] = {0}; // 临时结果存储区
int N_WM = 0; 			// 水印帧数计数器
int N_NO = 0; 			// 连续无水印短计数器
int N_EX = 0; 			// 连续有水印短计数器
int N_PART = 0; 		// 缓冲区是否处理计数器
int WM_NONE = 0; 		// 连续无水印长计数器
int FrSum[96] = {0}; 		// 水印累加和器（必须用双极性水印进行累加，否则会因N_NO的那数据部分丢失而出错）
uint8_t Frsgn[12] = {0}; 	// 似然水印（0或1）
int KK = 0;

void DSP_Init(void)
{
	MessageData.TransState = TRANS_IDLE;
	MessageData.HandleState = HANDLE_IDLE;
	MessageData.Command = M_CMD_UNDEFNIE;
	MessageData.rDataLen = 0;
	MessageData.sTime = 0;	
	g_WMRecord.FrameNum = 0;
//	DSP_FLAG = 0;
	DSP_WMFINISH_FLAG = 0;
	
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
		temp = 0;
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
	
	if(MessageData.TransState == TRANS_OK && MessageData.HandleState == HANDLE_READY) {		// 如果传输结束并且准备处理
		MessageData.HandleState = HANDLE_OK;
		MessageData.sTime = 0;
//		if(WMFlag.WM_Data_Num <= 0) {
//			return;
//		}
			
		memcpy((uint8_t *) Freven, (uint8_t *) MessageData.Data, 12);
		memcpy((uint8_t *) Frodd, (uint8_t *) (MessageData.Data + 12), 12);

		PreHandle();
	}
}

//判定水印是否存在的预判决
void PreHandle(void)
{
	
	int j;
	uint8_t temp[12] = {0};
	
	// 判断水印是否连续不存在，若不存在则中断判断水印程序，启动SIM900发送水印程序
	if (N_WM >= 100) {
		WM_NONE = 0;
	} else {
		if (DSP_WMFINISH_FLAG == 0) {
			WM_NONE++;
		}
	}
	if (WM_NONE >= WM_FRAME_NONE) {
//		DSP_WMFINISH_FLAG = 1;
		WM_NONE = 0;
	}
	
	for (j = 0; j < 12; j++) {
		temp[j] = ~(Freven[j] ^ Frodd[j]);
	}

	if (Find_Str(temp,12) > WM_AND_MIN) {	
		WriteHandle();	// 之前信息写入Flash并重置计数器
		return;
	}

	// 2帧放入循环队列里
	memcpy((uint8_t *) (*FrA + (N_PART % WM_GROUP_NUM) * 12), (uint8_t *) Freven, 12);
	memcpy((uint8_t *) (*FrB + (N_PART % WM_GROUP_NUM) * 12), (uint8_t *) Frodd, 12);
	N_PART++;
	if (N_PART < WM_GROUP_NUM) {
		return;
	}
	
//	DSP_FLAG = 1;
	Handle_Water();
}

// 处理DSP传过来的水印数据
void Handle_Water(void)
{
	uint8_t i, j, k;
//	DSP_FLAG = 0; // 标志位置零
	if (mode_same(FrA, FrB, WM_GROUP_NUM) > WM_FINAL_MIN) {
		if (N_WM == 0) {
			N_EX = 0;
			return;
		} else {
			N_NO++;
			if (N_NO < WM_GROUP_NUM) {
				return;
			} else {
				WriteHandle();	// N_WM有水印的前提下，连续WM_GROUP_NUM帧没水印了
			}
		}
	} else {
		if (N_NO == 0) {
			if (N_WM == 0) {
				if (N_EX == WM_GROUP_NUM) {
					N_WM = N_EX;
					for (i = 0; i < 12; i++) {
						for (k = 0; k < 8; k++) {	
							FrSum[i*8+k] = 0;
							for (j = 0; j < WM_GROUP_NUM; j++) {	
								FrSum[i*8+k] += 
									(((FrA[j][i] >> k) & 1) << 1) 
									- (((FrB[j][i] >> k) & 1) << 1);
							}
						}
					}
				} else { // 不到WM_GROUP_NUM帧（只能小于不可能大于，不信你试试）
					N_EX++;
				}
			} else {
				N_WM++;
				N_EX++;
				for (i = 0; i < 12; i++) {
					for (k = 0; k < 8; k++) {		
						FrSum[i*8+k] += 
						(((Freven[i] >> k) & 1) << 1) 
						- (((Frodd[i] >> k) & 1) << 1);
					}
				}
			}
		} else {
			N_WM += N_NO;
			N_NO = 0;
		}
	}

}

// 将似然水印和帧数写入Flash并重置计数器
void WriteHandle(void)
{
	int i, k;
	if (N_WM > WM_FRAME_MIN) {
		for (i = 0; i < 12; i++) {
			Frsgn[i] = 0;
			for (k = 0; k < 8; k++) {		
				Frsgn[i] += (uint8_t)(((FrSum[i*8+k] >= 0) ? 1 : 0)) << k;
			}
		}
		// 帧数N_WM和似然水印Frsgn写入Flash
		g_WMRecord.WMData = Frsgn;
		g_WMRecord.FrameNum = N_WM * 2;
		WriteRecord(&g_WMRecord);
		USART_SendRecord(&g_WMRecord);
		TCP_SendRecord(gPcb, &g_WMRecord);
	}
	N_WM = 0;
	N_NO = 0;
	N_EX = 0;
	N_PART = 0;
}

/*
// 处理水印DSP传过来的水印数据
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
	// 	最匹配水印检测
	for(i = 0; i < WMFlag.WM_Data_Num; ++i){
		GetWM(&t_WMData, i);											// WMFlag.WM_Data_Num为离线数据库水印个数（广告个数）
		for (k = 0; k < WM_GROUP_NUM; k += 2)
		{
			// 前12个字节部分的水印匹配
			for(j = 0; j < 12; ++j) {
				temp1[j] = ~(Array_Data[k].Data[j] ^ t_WMData.WaterMark[j]);					// 按位同或
				temp2[j] = (Array_Data[k+1].Data[j] ^ t_WMData.WaterMark[j]);					// 按位异或
			}
			num = Find_Str(temp1, 12) + Find_Str(temp2, 12);
			if(num >= Array_Data[k].Right_Num) {	//nMaxSim1
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
	// 匹配数最多的水印（如有多组，以前项为准，此处可优化）
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
	// 如最匹配组各不相同，则返回
	if(repeat <= 1)
		return;
	// 取众数
	ZhongArr(temp1, temp2);// 复用此空间，减小开销
	// 众数结果与预判决水印匹配位数
	for(j = 0; j < 12; ++j) {
		temp1[j] = ~(temp1[j] ^ test[j]);					// 按位同或
		temp2[j] = (temp2[j] ^ test[j]);					// 按位异或
	}
	num = Find_Str(temp1, 12)+Find_Str(temp2, 12);
	if(num <= 65)
		return;
	//判断判决的ID与上一个ID是否相同，相同且临近则时间累加
	if(WM_ID == g_WMRecord.WMData_ID) {
		if(flag < FRAME_FLAG){
			g_WMRecord.FrameNum += flag;
		}else {
			g_WMRecord.FrameNum += DSP_FRAME_MUL;
		}
		FrameNum = 0;	//队列伪清空
		flag = 0;
		return;
	} 
	//不相同，则将之前的数据传出（显示出），并重新存储记录
	if(g_WMRecord.FrameNum > 0) {
		WriteRecord(&g_WMRecord);
		USART_SendRecord(&g_WMRecord);
		TCP_SendRecord(gPcb, &g_WMRecord);
		g_WMRecord.WMData_ID = WM_ID;
		g_WMRecord.FrameNum = DSP_FRAME_MUL;
		FrameNum = 0;	//队列伪清空
		flag = 0;
		return;
	}	
	//
	g_WMRecord.WMData_ID = WM_ID;
	g_WMRecord.FrameNum = DSP_FRAME_MUL;
	FrameNum = 0;	//队列伪清空
	flag = 0;
	////////////////////////////////////////////////////////
}

void DSP_Water_Handle(__IO uint32_t localtime)
{
	// 执行任务时间
	static uint32_t DSP_Handle_Timer = 0;
//	if (localtime - DSP_Handle_Timer >= DSP_TMR_INTERVAL)
//	{
//		DSP_Handle_Timer = localtime;
//		
//		// 空闲200多帧的样子写入缓冲的记录
//		if(MessageData.HandleState == HANDLE_OK) {
//			++MessageData.sTime;
//			if(MessageData.sTime > 130 && g_WMRecord.FrameNum > 0) {
//				WriteRecord(&g_WMRecord);
//				USART_SendRecord(&g_WMRecord);
//				TCP_SendRecord(gPcb, &g_WMRecord);
//				MessageData.sTime = 0;
//				MessageData.HandleState = HANDLE_IDLE;
//				
//				g_WMRecord.WMData_ID = 0;
//				g_WMRecord.FrameNum = 0;
//			}
//		}
//	}

}


// output必须为至少24字节的缓冲空间，否则溢出
void ZhongArr(uint8_t *output1, uint8_t *output2)
{
	int odd = 0;
	unsigned char temp;
	unsigned char zhong = 0;
	int i, j, k;
	for(i = 0; i < 24; ++i) {
		for(j = 0; j < 8; ++j) {
			for(k = 0; k < WM_GROUP_NUM; k++) {
				if(i < 12) {
					// 2k的判断
					temp = Array_Data[2*k].Data[i] >> j;
				} else {
					// 2k+1的判断
					temp = Array_Data[2*k + 1].Data[i%12] >> j;
				}
				if(temp % 2 != 0) {
					++odd;
				}
			}
			// 一位的计算结果，在j位取1
			if(odd > WM_GROUP_NUM/2) {
				zhong += (1 << j);
			}
			odd = 0;
		}
		// 一个字节的计算结果
		if(i<12) {
			output1[i] = zhong;
		}
		else {
			output2[i%12] = zhong;
		}
		zhong = 0;
	}
}
*/

// 可同时计算两个12字节水印的众数值
int mode_same(uint8_t FrA[][12], uint8_t FrB[][12], uint8_t n)
{
	uint8_t i, j, k;
	int sumA, sumB;
	int total = 0;
	for (i = 0; i < 12; i++) {
//		FA_r[i] = 0;
//		FB_r[i] = 0;
		for (k = 0; k < 8; k++) {
			sumA = 0;
			sumB = 0;
			for (j = 0; j < n; j++) {
				sumA += (((FrA[j][i] >> k) & 1) << 1) - 1;
				sumB += (((FrB[j][i] >> k) & 1) << 1) - 1;
			}
//			FA_r[i] += ((sumA >= 0) ? 1 : 0) << k;
//			FB_r[i] += ((sumB >= 0) ? 1 : 0) << k;
			if ((sumA >= 0) == (sumB >= 0)) {
				total += 1;
			}
		}
	}
	return total;
}

int8_t Find_One(uint8_t n) 						//计算1的个数
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
