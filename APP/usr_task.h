/**
 * uC-OS����ģ��
 *
 * @author ���ء��ö���
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

// �����ջ��С����
#define  APP_TASK_START_STK_SIZE               ((INT32U) 64)
#define  LWIP_TASK_STK_SIZE					   ((INT32U) 1024)
#define  GPRS_TASK_STK_SIZE					   ((INT32U) 1024)
//#define  GSM_TASK_STK_SIZE					   ((INT32U) 128)
//#define	 MONITOR_TASK_STK_SIZE				   ((INT32U) 50)
//#define	 DSP_TASK_STK_SIZE					   ((INT32U) 1024)

// �������ȼ�����
#define  APP_TASK_START_PRIO                   ((INT8U) 3)
//#define  DSP_TASK_PRIO                 		   ((INT8U) 4)
#define  LWIP_TASK_PRIO						   ((INT8U) 5)
//#define  MONITOR_TASK_PRIO					   ((INT8U) 6)
#define  GPRS_TASK_PRIO						   ((INT8U) 7)
//#define  GSM_TASK_PRIO						   ((INT8U) 8)
//extern uint8_t DSP_FLAG;

// ��¼��һ�����ͽڵ�
typedef struct _Next_Send {
	uint32_t Record_ID;
	uint32_t index;
} Next_Send;


// ucos������ʼ��������
INT8U Task_Init(void);

// ��ʼ����������
static void App_TaskStart(void *p_arg);

// APP���񴴽�����
static void App_TaskCreate(void);
// GPRSSend���Ӻ���
//static void App_Monitor(void *p_arg);

// ����Ĵ�����
static void App_LWIP(void *p_arg);
static void App_GPRSSend(void *p_arg);
//static void GSM_Task(void *p_arg);
//static void DSP_Task(void *p_arg);

// App_GPRSSend�ĸ�������
static void Send_Task(void);

#endif
