#ifndef USART2_TREN_H
#define USART2_TREN_H	
 
#include "hardware.h"


#define TXEN PBout(1)	// PB0
#define RXEN PBout(0)	// PB0


void usart_tren_init(void);
void usart_moderx(void);
void usart_modetx(void);

#endif	   



