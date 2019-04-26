#ifndef USART3_H
#define USART3_H	

#include "hardware.h"	 								  

extern u8 u3buf[512];					//串口3接收缓冲
extern u32 u3len;						//串口3接受缓冲区数据个数



void usart3_init(u32 bound);			//串口3初始化 bound: 波特率

void usart3_senddat(u8 *buf,u8 len);	//使用串口3发送一段数据
										//buf:发送区首地址
										//len:发送的字节数


#endif	   



