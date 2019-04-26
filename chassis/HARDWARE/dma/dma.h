#ifndef DMA_H
#define DMA_H


#include "stm32f10x.h"


extern u8 DMA1_BUSY_FLAG;				//DMA1忙碌状态标记

void DMA1_Init(void);					//DMA1初始化

u32 DMA1_SendDat(u8 *buf, u8 numdat);	//开启一次DMA传输

/*
typedef struct tagDMA1U3BUF				//DMA串口3传输缓存区
{
	u8 dat[256];
	u16 num;
	struct tagDMA1U3BUF *next;
} DMA1U3BUF, *PDMA1U3BUF;
*/

#endif
