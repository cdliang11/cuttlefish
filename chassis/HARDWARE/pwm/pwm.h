#ifndef _PWM_H_
#define _PWM_H_


#include "hardware.h"

void TIM3_Init(void);
void TIM4_Init(void);

void steel_pst(u8 ID, u16 pst);

#endif
