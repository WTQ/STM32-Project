/**
 * C/S架构封包相关
 *
 * @author 王特
 */

#ifndef USR_SERVER_H
#define USR_SERVER_H

#include "main.h"
#include "lwip/tcp.h"
#include "usr_netpacket.h"
#include "usr_flash.h"

#define ARM_SERVER_PORT		23

bool SendNetPacket(struct tcp_pcb *pcb, NetPacket *npPacket);
bool SendCharBuff(struct tcp_pcb *pcb, char *buf, int len, PACKET_TYPE ePType);
bool SendPacketEnd(struct tcp_pcb *pcb);
bool TCP_SendRecord(struct tcp_pcb *pcb, WM_Record *WMRecord);
bool TCP_AddWMData(char *data);
bool TCP_SendWMData(struct tcp_pcb *pcb, WM_Data *WMData);

#endif
