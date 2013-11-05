#ifndef __BSP_H
#define __BSP_H

#include    <cpu.h>

#include    <stm32f10x_conf.h>

#include  <ucos_ii.h>

void BSP_Init(void);

void  BSP_IntDisAll (void);

#endif
