/**
 * uC-OS任务模块
 *
 * @author 王特、矫东航
 *
 */
#include "usr_task.h"

#include "gsm_type.h"
#include "gsm_driver.h"
#include "gsm_core.h"
#include "gsm_gprs.h"
#include "gsm_base.h"

// 任务堆栈空间
static OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK App_LWIPStk[LWIP_TASK_STK_SIZE];
static OS_STK App_GPRSStk[GPRS_TASK_STK_SIZE];
static OS_STK App_MONITORStk[MONITOR_TASK_STK_SIZE];
static OS_STK App_DSPStk[DSP_TASK_STK_SIZE];
// static OS_STK App_GSMStk[GSM_TASK_STK_SIZE];

// 下一个发送的记录
static Next_Send Next_Record = {0};

TASK_EXECUTE Task_Execute = IDLE;

OS_EVENT* Com1_Mbox;

INT8U Task_Init(void)
{
	INT8U err;
	err = OSTaskCreateExt(App_TaskStart, NULL, (App_TaskStartStk + APP_TASK_START_STK_SIZE - 1), APP_TASK_START_PRIO, 
		APP_TASK_START_PRIO, App_TaskStartStk, APP_TASK_START_STK_SIZE, NULL, OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);
	return err;
}

static void App_TaskStart(void *p_arg)
{
	OS_CPU_SysTickInit();
#if(OS_TASK_STAT_EN > 0)
	OSStatInit();
#endif
	App_TaskCreate();
	while(1)
	{
		// 挂起任务
		OSTaskSuspend(OS_PRIO_SELF);
		OSTimeDlyHMSM(0,0,10,0);
	}
}

static void App_TaskCreate(void)
{
	// 创建LWIP的任务
	OSTaskCreateExt(App_LWIP, NULL, (App_LWIPStk + LWIP_TASK_STK_SIZE - 1), LWIP_TASK_PRIO, LWIP_TASK_PRIO,
		App_LWIPStk, LWIP_TASK_STK_SIZE, NULL,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	// 创建GPRS发送数据的任务
	OSTaskCreateExt(App_GPRSSend, NULL, (App_GPRSStk + GPRS_TASK_STK_SIZE - 1), GPRS_TASK_PRIO, GPRS_TASK_PRIO, 
		App_GPRSStk, GPRS_TASK_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		
	// 创建GPRSSend监视的任务
	OSTaskCreateExt(App_Monitor, NULL, (App_MONITORStk + MONITOR_TASK_STK_SIZE - 1), MONITOR_TASK_PRIO, MONITOR_TASK_PRIO, 
		App_MONITORStk, MONITOR_TASK_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	// 建立DSP处理任务
	OSTaskCreateExt(DSP_Task, NULL, (App_DSPStk + DSP_TASK_STK_SIZE - 1), DSP_TASK_PRIO, DSP_TASK_PRIO, 
		App_DSPStk, DSP_TASK_STK_SIZE, NULL,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	
	// 建立GSM发送的任务
	// OSTaskCreateExt(GSM_Task, NULL, (App_GSMStk + GSM_TASK_STK_SIZE - 1), GSM_TASK_PRIO, GSM_TASK_PRIO, 
	//	App_GSMStk, GSM_TASK_STK_SIZE, NULL,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}

static void App_Monitor(void *p_arg)
{
while (1) {
	OSTimeDlyHMSM(0,0,0,10);
}
/*	while (1) {
		OSTaskSuspend(OS_PRIO_SELF);
//		if (Task_Execute == EXECUTE) {
		
			// 删除App_GPRSSend任务
			OSTaskDel(GPRS_TASK_PRIO);
			while (OSTaskDelReq(GPRS_TASK_PRIO) != OS_TASK_NOT_EXIST) {
				OSTimeDlyHMSM(0,0,0,1);
			}
		
			// 创建GPRS发送数据的任务
			OSTaskCreateExt(App_GPRSSend, NULL, (App_GPRSStk + GPRS_TASK_STK_SIZE - 1), GPRS_TASK_PRIO, GPRS_TASK_PRIO, 
				App_GPRSStk, GPRS_TASK_STK_SIZE, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
			
//			Task_Execute = IDLE;
//		}
	}*/
}

static void App_GPRSSend(void* p_arg)
{
	while (1) {
		Send_Task();
		OSTimeDlyHMSM(0,0,1,0);
	}
}
static void Send_Task(void)
{
	int32_t index = 0;
	WM_Record WMRecord;
	// HTTP发送的缓冲区
	char GPRSBuffer[400];	
	GSM_RECEIVE_RECORD Receive;
	
	// GSM模块初始化
	GSM_Driver_Int();
	
	OSTimeDlyHMSM(0,0,10,0);
//	if (Task_Execute == IDLE) {
//		while (!GSM_Receive_Recall("Call Ready")) {
//		}
//	}
	// 初始化GSM和GPSR
	if (!GSM_Config()) {
		return;
	}
	GPRS_Init();

	if (!GPRS_TCP_Connect("202.204.81.57","80")) {
		return;
	}
	GPRSBuffer[0] = 0;
	GSM_Get_Record(GPRSBuffer);
	GPRS_TCP_Send(GPRSBuffer);
	GSM_Receive_Record(&Receive);
	Next_Record.Record_ID = Calculate(&Receive);
	GRRS_TCP_Closed();



	while (1) {
		
		
		if(Next_Record.Record_ID < WMFlag.WM_Record_Last_ID) {
			while(Next_Record.Record_ID < WMFlag.WM_Record_Last_ID) {
				GPRS_TCP_Connect("202.204.81.57","80");
				index = GetRecordIndexById(Next_Record.Record_ID);
				GetRecord(&WMRecord, index);
				GPRSBuffer[0] = 0;
				GSM_Post_Record(GPRSBuffer, &WMRecord);
//				len = strlen(GPRSBuffer);
//				GPRSBuffer[len] = 0x1A;
//				GPRSBuffer[len + 1] = '\0';
				GPRS_TCP_Send(GPRSBuffer);
				GSM_Receive_Record(&Receive); // @todo 未接到success要重传
				GRRS_TCP_Closed();
				Next_Record.Record_ID++;
			}
		} else {
			GPRS_TCP_Connect("202.204.81.57","80");
			GPRSBuffer[0] = 0;
			GSM_Post_Beat(GPRSBuffer);
//			len = strlen(GPRSBuffer);
//			GPRSBuffer[len] = 0x1A;
//			GPRSBuffer[len + 1] = '\0';
			GPRS_TCP_Send(GPRSBuffer);
			GSM_Receive_Record(&Receive); // @todo 未接到success要重传
			GRRS_TCP_Closed();
		}
		OSTimeDlyHMSM(0, 0, 1, 0);

	}
}
static void App_LWIP(void* p_arg)
{
	LwIP_Init();
	OSTimeDlyHMSM(0,0,1,0);
	Server_init();
	OSTimeDlyHMSM(0,0,1,0);
	while(1)
	{
		System_Periodic_Handle();
		OSTimeDlyHMSM(0,0,1,0);
	}
}

static void DSP_Task(void *p_arg)
{
	while (1) {

		if (DSP_FLAG == 1) {
			Handle_Water(); // 循环队列已满，开始处理
		}
		if (DSP_WMFINISH_FLAG == 1) {
			OSTimeDlyHMSM(0,30,0,0); // 30分钟时间来传水印数据到服务器端
			DSP_WMFINISH_FLAG = 0;
		}
	}
}
