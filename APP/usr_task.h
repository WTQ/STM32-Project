/**
 * uC-OS任务模块
 *
 * @author 王特、矫东航
 */

#ifndef USR_USART2_H
#define USR_USART2_H

#include "main.h"
#include "armserver.h"
#include "flash/usr_flash.h"
#include "http/usr_sim900.h"
#include "http/usr_http.h"
#include "dsp/usr_dsp.h"
#include "urlencode.h"
#include "netconf.h"

// 任务堆栈大小配置
#define  APP_TASK_START_STK_SIZE               ((INT32U) 64)
#define  LWIP_TASK_STK_SIZE					   ((INT32U) 1024)
#define  GPRS_TASK_STK_SIZE					   ((INT32U) 1024)
//#define  GSM_TASK_STK_SIZE					   ((INT32U) 128)
//#define	 MONITOR_TASK_STK_SIZE				   ((INT32U) 50)
//#define	 DSP_TASK_STK_SIZE					   ((INT32U) 1024)

// 任务优先级分配
#define  APP_TASK_START_PRIO                   ((INT8U) 3)
//#define  DSP_TASK_PRIO                 		   ((INT8U) 4)
#define  LWIP_TASK_PRIO						   ((INT8U) 5)
//#define  MONITOR_TASK_PRIO					   ((INT8U) 6)
#define  GPRS_TASK_PRIO						   ((INT8U) 7)
//#define  GSM_TASK_PRIO						   ((INT8U) 8)
//extern uint8_t DSP_FLAG;

// 记录下一个发送节点
typedef struct _Next_Send {
	uint32_t Record_ID;
	uint32_t index;
} Next_Send;


// ucos创建初始化任务函数
INT8U Task_Init(void);

// 初始化任务处理函数
static void App_TaskStart(void *p_arg);

// APP任务创建函数
static void App_TaskCreate(void);
// GPRSSend监视函数
//static void App_Monitor(void *p_arg);

// 任务的处理函数
static void App_LWIP(void *p_arg);
static void App_GPRSSend(void *p_arg);
//static void GSM_Task(void *p_arg);
//static void DSP_Task(void *p_arg);

// App_GPRSSend的辅助函数
static void Send_Task(void);

#endif
