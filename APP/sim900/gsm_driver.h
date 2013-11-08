/**
 * GSMģ����STM32�Ľӿڲ�
 *
 * @author ����
 */

#ifndef __USR_GSM_DRIVER_H__
#define __USR_GSM_DRIVER_H__

#include "stm32f10x.h"
#include "stm32f107.h"

// ��ʼ��TIM2
void GSM_TimeInit(void);
// ����������TIM2
void GSM_SetTime(uint16_t cnt, uint16_t psc);
// ���������ȴ���ʱ
void GSM_SetTimeCommand(void);
// �����������ݳ�ʱ
void GSM_SetTimeData(void);
// �ر�TIM2
void GSM_ShutTime(void);
// TIM�����¼�����
void GSM_TimeHandle(void);

#endif
