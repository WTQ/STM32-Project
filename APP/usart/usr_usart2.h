/**
 * USART2��λ��ͨ��
 *
 * @author ����
 */

#ifndef USR_USART2_H
#define USR_USART2_H

#include "usr_flash.h"

void USART2_Handle(void);
void USART_SendStr(char *data, int32_t len);
void USART_SendRecord(WM_Record *WMRecord);
void USART_SendWMData(WM_Data *WMData);

// ���ڵ���LWIPЭ��ջ��
void USART2_Printf(const char *format,...);

#endif
