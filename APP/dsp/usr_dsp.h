/**
 * USART控制DSP和ARM接收水印接收相关封装
 *
 * @author 王特
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


// 水印匹配的阀值
#define WM_AND_MIN		80

// 水印匹配的时间间隔，单位：ms
#define DSP_TMR_INTERVAL	100

// 计算帧数的乘数
#define DSP_FRAME_MUL		100

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

void DSP_Init(void);
void DSP_Start(void);
void DSP_Stop(void);
void DSP_ReseveMsg(void);

// 中断调用
void Handle_Water(void);
// 任务调用
void DSP_Water_Handle(__IO uint32_t localtime);

// 找出字符串中1的个数
int8_t Find_One(uint8_t n);
int8_t Find_Str(uint8_t *str, int8_t num);

extern Message_Data MessageData;
extern uint8_t DSPState;
#endif
