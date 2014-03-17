/**
 * USART控制DSP和ARM接收水印接收相关封装
 *
 * @author 王特
 * @修改   矫东航
 */

#ifndef USR_DSP_H
#define USR_DSP_H

#include "main.h"

// 数据最大长度
#define MESSAGE_MAX_LEN	30

// 控制命令
#define M_CMD_UNDEFNIE	0x00
#define M_CMD_START		0xAA
#define M_CMD_STOP		0x55
#define M_CMD_WMDATA	0x03
// 等待握手命令
#define M_CMD_START_HANDLE	0xbb

// DSP状态
#define DSP_UNDEFINE	0x00
#define DSP_RUNNING		0x01
#define DSP_STOPPING	0x02
#define DSP_START_HANDS	0x03


// 水印判定数组长度（帧数） 控制时间分辨率，必须是2的倍数（时间分辨率 WM_GROUP_NUM/25 秒）
#define WM_GROUP_NUM	(2*7)

// 水印毛刺消除灵敏度 保证水印连续性
#define FRAME_FLAG		(5*WM_GROUP_NUM)

// 水印预判决的阀值	控制预判决灵敏度（越高，则虚警率越高，漏检率越低，经验值70）
#define WM_AND_MIN		70

// 水印众数判决的阈值 控制判决灵敏度（越高，则虚警率越低，漏检率越高，经验值65）
#define WM_FINAL_MIN	65

// 水印匹配的时间间隔，单位：ms
// #define DSP_TMR_INTERVAL	100

// 计算帧数的乘数
#define DSP_FRAME_MUL		WM_GROUP_NUM

// 水印传输的状态
typedef enum {
	TRANS_OK = 0,
	TRANS_ING,
	TRANS_ERR,
	TRANS_IDLE
} Trans_State;

// 水印处理的状态
typedef enum {
	HANDLE_OK = 0,
	HANDLE_READY,
	HANDLE_ERR,
	HANDLE_IDLE
} Handle_State;

// 水印接收消息结构体
typedef struct {
	int32_t TransState;
	int32_t HandleState;
	int8_t	Command;
	int8_t	cDataLen;
	int8_t	rDataLen;
	uint8_t Data[MESSAGE_MAX_LEN];
	uint32_t sTime;
} Message_Data;

// 水印队列的结构体
typedef struct 
{
	uint8_t		Data[MESSAGE_MAX_LEN];	// 收到的数据
	uint8_t		WM_Data[MESSAGE_MAX_LEN];	// 最匹配的数据
	int			WM_ID;		//两帧此数相同
	int			Right_Num;	//这个是两帧正确位数的总和，该两帧此数相同
}WM_Array;

void DSP_Init(void);
void DSP_Start(void);
void DSP_Stop(void);
void DSP_ReseveMsg(void);

// 中断调用
void Handle_Water(void);
// 任务调用
void DSP_Water_Handle(__IO uint32_t localtime);
// 处理调用
void Push_Data(WM_Array);
void PreHandle(void);
void ZhongArr(uint8_t *, uint8_t *);

// 找出字符串中1的个数
int8_t Find_One(uint8_t n);
int8_t Find_Str(uint8_t *str, int8_t num);

extern Message_Data MessageData;
extern uint8_t DSPState;
#endif
