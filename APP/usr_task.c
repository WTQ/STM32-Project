/**
 * uC-OS任务模块
 *
 * @author 王特
 */
#include "usr_task.h"

#include "gsm_driver.h"
#include "gsm_core.h"
#include "gsm_gprs.h"

// 任务堆栈空间
static OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK App_LWIPStk[LWIP_TASK_STK_SIZE];
static OS_STK App_GPRSStk[GPRS_TASK_STK_SIZE];

// static OS_STK App_GSMStk[GSM_TASK_STK_SIZE];

// 下一个发送的记录
static Next_Send Next_Record = {0};


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
	
	// 建立GSM发送的任务
	// OSTaskCreateExt(GSM_Task, NULL, (App_GSMStk + GSM_TASK_STK_SIZE - 1), GSM_TASK_PRIO, GSM_TASK_PRIO, 
	//	App_GSMStk, GSM_TASK_STK_SIZE, NULL,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}

static void App_GPRSSend(void* p_arg)
{
	uint8_t index = 0;
	int len;
	WM_Record WMRecord;
	// HTTP发送的缓冲区
	char GPRSBuffer[400];

	// GSM模块初始化
	GSM_Driver_Int();
	
//	GPRS_Init1();

//	GSM_Core_Tx_AT("AT\r\n");
//	GSM_Core_Tx_AT("AT+IPR=115200\r\n");
//	GSM_Core_Tx_AT("AT+CIPHEAD=1\r\n");
//	OSTimeDlyHMSM(0,0,1,0);
//	GSM_Core_Tx_AT("AT+CSCS=\"GSM\"\r\n");
//	OSTimeDlyHMSM(0,0,1,0);
//	GSM_Core_Tx_AT("AT+CGATT=1\r\n");
//	OSTimeDlyHMSM(0,0,1,0);
//	GSM_Core_Tx_AT("AT+CSTT\r\n");
//	OSTimeDlyHMSM(0,0,1,0);
//	GSM_Core_Tx_AT("AT+CIICR\r\n");
//	OSTimeDlyHMSM(0,0,1,0);
//	GSM_Core_Tx_AT("AT+CIFSR\r\n");
//	OSTimeDlyHMSM(0,0,1,0);
	

	OSTimeDlyHMSM(0,0,10,0);
	
//	while (!GSM_Receive_Recall("Call Ready")) {
//	}
	// 初始化GSM和GPSR
	GSM_Config();
	GPRS_Init();
	
//	GSM_Core_Tx_AT("AT+CIPSTART=\"TCP\",\"202.204.81.57\",80\r\n");
//	GetRecord(&WMRecord, 1);
//	GPRSBuffer[0] = 0;
//	GSM_Post_Record(GPRSBuffer, &WMRecord);
//	GSM_Core_Tx_AT(GPRSBuffer);




	while (1) {
	
	
//		if(Next_Record.Record_ID < WMFlag.WM_Record_Last_ID) {
//			while(Next_Record.Record_ID < WMFlag.WM_Record_Last_ID) {
//				GPRS_TCP_Connet("202.204.81.57","80");
//				index = GetRecordIndexById(Next_Record.Record_ID);
//				GetRecord(&WMRecord, index);
//				GPRSBuffer[0] = 0;
//				GSM_Post_Record(GPRSBuffer, &WMRecord);
//				GSM_GPRS_SEND((unsigned char *) GPRSBuffer);
//				OSTimeDlyHMSM(0, 0, 5, 0);
//				GRRS_TCP_Close();
//				Next_Record.Record_ID++;
//			}
//		} else {
//			GPRS_TCP_Connet("202.204.81.57","80");
//			GPRSBuffer[0] = 0;
//			GSM_Post_Beat(GPRSBuffer);
//			GSM_GPRS_SEND((unsigned char *) GPRSBuffer);
//			OSTimeDlyHMSM(0, 0, 4,0);
//			GRRS_TCP_Close();
//		}


		if(Next_Record.Record_ID < WMFlag.WM_Record_Last_ID) {
			while(Next_Record.Record_ID < WMFlag.WM_Record_Last_ID) {
				GPRS_TCP_Connect("202.204.81.57","80");
				index = GetRecordIndexById(Next_Record.Record_ID);
				GetRecord(&WMRecord, index);
				GPRSBuffer[0] = 0;
				GSM_Post_Record(GPRSBuffer, &WMRecord);
				len = strlen(GPRSBuffer);
				GPRSBuffer[len] = 0x1A;
				GPRSBuffer[len + 1] = '\0';
				GPRS_TCP_Send(GPRSBuffer);
				GRRS_TCP_Closed();
				Next_Record.Record_ID++;
			}
		} else {
			GPRS_TCP_Connect("202.204.81.57","80");
			GPRSBuffer[0] = 0;
			GSM_Post_Beat(GPRSBuffer);
			len = strlen(GPRSBuffer);
			GPRSBuffer[len] = 0x1A;
			GPRSBuffer[len + 1] = '\0';
			GPRS_TCP_Send(GPRSBuffer);
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

