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

// GSMЭ��ջ���ͺͽ����������
void GSM_CORE_Rx_Handle(UINT8 data);
void GSM_CORE_AT(char *data);

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

#endif
