/**
 * GSMģ��ĺ���ͨ��Э��ģ��
 *
 * @author ���ء��ö���
 */

#ifndef __USR_GSM_CORE_H__
#define __USR_GSM_CORE_H__

#include "gsm_type.h"
#include "gsm_driver.h"

//void GSM_Init(void);

// GSMЭ��ջ�����������
void GSM_CORE_Rx_Handle(UINT8 data);

// GSMЭ��ջ�����������
bool GSM_CORE_Tx_Handle(char *data);
bool GSM_Core_Tx_AT(char *data);
bool GSM_Core_Tx_Data(char *data);

// GSMЭ��ջ������
void GSM_CORE_Processor(UINT8 data);

// GSM�����������
void GSM_Core_Rx_Command(UINT8 data);
void GSM_Core_Rx_Echo(UINT8 data);

// GSM�������ݴ�����
void GSM_Core_Rx_Data(UINT8 data);

// ��ʱ������
void Timeout_Command(void);
void Timeout_Data(void);
void Timer_Start_core(void);
void Timeout_Count(void);

#endif
