/**
 * GSMģ��ĺ���ͨ��Э��ģ��
 *
 * @author ����
 */

#ifndef __USR_GSM_CORE_H__
#define __USR_GSM_CORE_H__

#include "gsm_type.h"
#include "gsm_driver.h"


void GSM_Init(void);

// GSMЭ��ջ���ͺͽ����������
void GSM_CORE_RX_Handle(UINT8 data);
void GSM_CORE_TX_Handle(UINT8 *data);

// GSMЭ��ջ������
void GSM_CORE_Processor(UINT8 data);



#endif
