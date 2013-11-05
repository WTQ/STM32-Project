/**
 * USART2上位机通信
 *
 * @author 王特
 */

#ifndef USR_USART2_H
#define USR_USART2_H

#include "usr_flash.h"

void USART2_Handle(void);
void USART_SendStr(char *data, int32_t len);
void USART_SendRecord(WM_Record *WMRecord);
void USART_SendWMData(WM_Data *WMData);

// 用于调试LWIP协议栈用
void USART2_Printf(const char *format,...);

#endif
