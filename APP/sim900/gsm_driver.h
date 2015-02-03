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

// ��ʼ��TIM2��TIM3
void GSM_CommandTimeInit(void);
void GSM_DataTimeInit(void);
void GSM_CountTimeInit(void);
// ����������TIM2��TIM3��TIM4
void GSM_Set_TIM2(uint16_t cnt, uint16_t psc);
void GSM_Set_TIM3(uint16_t cnt, uint16_t psc);
void GSM_Set_TIM4(uint16_t cnt, uint16_t psc);
// ���������ȴ���ʱ
void GSM_SetTimeCommand(void);;
// �����������ݳ�ʱ
void GSM_SetTimeData(void);
// ������ʱ����ʱ
void GSM_SetTimeCount(void);
// �ر�TIM2
void GSM_ShutTIMCommand(void);
// �ر�TIM3
void GSM_ShutTIMData(void);
// �ر�TIM4
void GSM_ShutTIMCount(void);
// TIM�����¼�����
void GSM_CommandTimeHandle(void);
void GSM_DataTimeHandle(void);
void GSM_CountTimeHandle(void);
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
// ��λGPIO��ʼ��
void GSM_Reset_Int(void);
// GSM��λ����
void GSM_Reset_Set(void);
#endif
