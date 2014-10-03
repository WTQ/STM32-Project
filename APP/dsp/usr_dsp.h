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


// 水印判定数组长度（帧数） 控制时间分辨率（时间分辨率 WM_GROUP_NUM*2/25 秒）
#define WM_GROUP_NUM	13

// 水印预判决的阀值	控制预判决灵敏度（越高，则虚警率越高，漏检率越低，经验值70）
#define WM_AND_MIN		80

// 水印众数判决的阈值 控制判决灵敏度（越高，则虚警率越低，漏检率越高，26帧经验值55）
#define WM_FINAL_MIN	55

// 水印连续存在帧数阈值
#define WM_FRAME_MIN	100

// 水印连续不存在帧数阈值
#define WM_FRAME_NONE	1000 // 初步定1000，调试用100

// 水印匹配的时间间隔，单位：ms
// #define DSP_TMR_INTERVAL	100

// 停止水印接收并处理SIM900发送数据时间(/分钟)
#define WM_SEND_TIME	10

// 计算帧数的乘数
#define DSP_FRAME_MUL		WM_GROUP_NUM

// 启动DSP处理task的信号量
//extern uint8_t DSP_FLAG;
// DSP让出task的信号量
extern uint8_t DSP_WMFINISH_FLAG;

// 似然水印（0或1）
extern uint8_t Frsgn[12]; 

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
// 处理调用
void PreHandle(void);
void ZhongArr(uint8_t *, uint8_t *);
void WriteHandle(void);
int mode_same(uint8_t [][12], uint8_t [][12], uint8_t);

// 找出字符串中1的个数
int8_t Find_One(uint8_t n);
int8_t Find_Str(uint8_t *str, int8_t num);

extern Message_Data MessageData;
extern uint8_t DSPState;
#endif
