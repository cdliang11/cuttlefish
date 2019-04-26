#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "led.h"
#include "usart.h"


#define GoUp 1
#define  GoBack 2
#define GoLeft 3
#define GoRight 4
#define Stop 0
void TIM3_Int_Init(u16 arr,u16 psc);

#endif
