/**
 * GSM模块的基础类型定义
 *
 * @author 王特
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


typedef enum _GSM_STATUS_TRANS {
	GSM_STATUS_TRANS_IDLE = 0,
	GSM_STATUS_TRANS_COMMAND,
	GSM_STATUS_TRANS_DATA,
	GSM_STATUS_TRANS_RESPONS
} GSM_STATUS_TRANS;

typedef enum _GSM_STATUS_COMMAND {
	GSM_STATUS_COMMAND_IDLE = 0,
	GSM_STATUS_COMMAND_ECHO,
	GSM_STATUS_COMMAND_DATA,
	GSM_STATUS_COMMAND_ACK,
	GSM_STATUS_COMMAND_SUCCESS
} GSM_STATUS_COMMAND;

typedef enum _GSM_STATUS_DATA {
	GSM_STATUS_DATA_IDLE = 0,
	GSM_STATUS_DATA_TYPE,
	GSM_STATUS_DATA_DATA,
	GSM_STATUS_DATA_ACK,
	GSM_STATUS_DATA_SUCCESS
} GSM_STATUS_DATA;

typedef enum _GSM_STATUS_RESPONS {
	GSM_STATUS_RESPONS_IDLE = 0,
	GSM_STATUS_RESPONS_TYPE,
	GSM_STATUS_RESPONS_DATA,
	GSM_STATUS_RESPONS_ACK,
	GSM_STATUS_RESPONS_SUCCESS
} GSM_STATUS_RESPONS;

#endif
