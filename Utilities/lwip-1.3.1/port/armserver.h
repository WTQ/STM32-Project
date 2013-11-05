/**
 * C/S架构server端，采用LWIP的RAM API实现
 *
 * @author 王特
 */ 

#ifndef __ARMSERVER_H__
#define __ARMSERVER_H__

#include "main.h"
#include "lwip/tcp.h"

// 函数申明
void Server_init(void);
static err_t Server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static err_t Server_accept(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t Server_sent(void *arg, struct tcp_pcb *pcb, u16_t len);
static void Server_conn_err(void *arg, err_t err);

#endif
