#ifndef USART_H
#define USART_H

#include "hardware.h"

void usart1_init(u32 bound);			//串口1初始化 bound: 波特率

void usart1_senddat(u8 *buf, u8 len);	//使用串口1发送一段数据
										//buf:发送区首地址
										//len:发送的字节数
																				
extern u8 u1buf[256];					//串口2接收缓冲
extern u32 u1len;						//串口2接受缓冲区数据个数
extern u8 continue_or_not;

#endif	   








