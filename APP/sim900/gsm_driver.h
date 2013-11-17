/**
 * GSMģ����STM32�Ľӿڲ�
 *
 * @author ���ء��ö���
 */

#ifndef __USR_GSM_DRIVER_H__
#define __USR_GSM_DRIVER_H__

#include "stm32f10x.h"
#include "stm32f107.h"

#include "gsm_type.h"

// ѡ��ͨ�ŵ�USART
#define GSM_COMM_SEL	GSM_USART_1

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
// USART_GPIO����
void GSM_USART_GPIO(void);
// USART����
void GSM_USART_Config(void);
// USART�ж�����
void GSM_USART_NVIC(void);
// ���ڷ���һ���ֽ�
void GSM_USART_TxChar(UINT8 c);
// ���ڷ�������
void GSM_USART_TxData(UINT8 *data, int len);
// ���ڷ����ַ���
void GSM_USART_TxStr(char *str);
// ���ڽ�������
void GSM_USART_Rx(void);
// ���ڳ�ʼ��
void GSM_USART_Init(void);
// ������ʼ��
void GSM_Driver_Int(void);
#endif
