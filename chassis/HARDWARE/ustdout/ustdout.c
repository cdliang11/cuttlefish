#include "ustdout.h"

USART_TypeDef *stdout_USARTx = USART1;	//标准输出缓冲区指向

//加入以下代码,支持printf函数******************************
#pragma import(__use_no_semihosting)                             
struct __FILE
{
	int handle;
};


FILE __stdout;

void _sys_exit(int x)
{
	x = x;
}

int fputc(int ch, FILE *f)
{
	while ((stdout_USARTx->SR & 0X40) == 0);
	stdout_USARTx->DR = (u8)ch;
	return ch;
}
//**********************************************************


void usartx_senddat(USART_TypeDef *USARTx, u8 *buf, u8 len)
{
	if(USARTx == USART1)
		usart1_senddat(buf, len);	//使用串口1发送一段数据
	else if(USARTx == USART2)
		usart2_senddat(buf, len);	//使用串口2发送一段数据
	else if(USARTx == USART3)
		usart3_senddat(buf, len);	//使用串口3发送一段数据	
}







