/**
 * GSMģ��Ļ������Ͷ���
 *
 * @author ���ء��ö���
 */

#ifndef __USR_GSM_TYPE_H__
#define __USR_GSM_TYPE_H__

#ifndef	UINT8
typedef unsigned char  UINT8;                    
#endif
#ifndef	INT8
typedef signed   char  INT8;                   
#endif
#ifndef UINT16
typedef unsigned short UINT16;                  
#endif
#ifndef	INT16
typedef signed   short INT16;                  
#endif
#ifndef UINT32
typedef unsigned int   UINT32;                   
#endif
#ifndef	INT32
typedef signed   int   INT32;                   
#endif

// ����״̬
typedef enum _GSM_STATUS_TRANS {
	GSM_STATUS_TRANS_DATA = 0,
	GSM_STATUS_TRANS_COMMAND
} GSM_STATUS_TRANS;

// ����״̬�����ͣ�
typedef enum _GSM_STATUS_COMMAND {
	GSM_STATUS_COMMAND_IDLE = 0,
	GSM_STATUS_COMMAND_ECHO,
	GSM_STATUS_COMMAND_EXECUTE,
	GSM_STATUS_COMMAND_DATA,
	GSM_STATUS_COMMAND_SUCCESS,
	GSM_STATUS_COMMAND_TIMEOUT,
	GSM_STATUS_COMMAND_ERROR
} GSM_STATUS_COMMAND;

// ����״̬�����գ�
typedef enum _GSM_STATUS_DATA {
	GSM_STATUS_DATA_IDLE = 0,
	GSM_STATUS_DATA_DATA,
	GSM_STATUS_DATA_SUCCESS
} GSM_STATUS_DATA;

// GSM�����¼
typedef struct _GSM_COMMAND_RECORD {
	UINT8 Tx_Data[500];
	UINT16 Tx_Data_Count;
	UINT8 Echo_Data[500];
	UINT16 Echo_Count;
	UINT8 Rx_Data[500];
	UINT16 Rx_Data_Count;
	GSM_STATUS_COMMAND Status;
} GSM_COMMAND_RECORD;

// GSM���ݼ�¼
typedef struct _GSM_DATA_RECORD {
	UINT8 Rx_Data[500];
	UINT16 Rx_Data_Count;
	GSM_STATUS_DATA Status;
} GSM_DATA_RECORD;

#endif
