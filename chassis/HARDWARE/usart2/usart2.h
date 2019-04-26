#ifndef USART2_H
#define USART2_H	
 
#include "hardware.h"

extern u8 u2buf[256];					//串口2接收缓冲
extern u32 u2len;						//串口2接受缓冲区数据个数



void usart2_init(u32 bound);			//串口2初始化 bound: 波特率

void usart2_senddat(u8 *buf,u8 len);	//使用串口2发送一段数据
										//buf:发送区首地址
										//len:发送的字节数


#endif	   



