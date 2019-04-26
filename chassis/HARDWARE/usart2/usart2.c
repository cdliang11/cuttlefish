#include "usart2.h"
#include "jy61.h"
#include "config.h"



u8 u2buf[256] = { 0 };					//串口2接收缓冲
u32 u2len = 0;							//串口2接受缓冲区数据个数



void USART2_IRQHandler()				//串口2中断函数
{
	if (USART2->SR & (1 << 5))
	{
		u2buf[u2len] = USART2->DR;		//收到的数据压入缓冲区
		//USART1->DR = u2buf[u2len];	//通过另一个串口发送出去
#ifdef MOFANG
		jyStateIterator(u2buf[u2len]);
#endif
		u2len = (u2len + 1) & 0xFF;		//满128则清零										
	}
}


void usart2_init(u32 bound)				//串口2初始化 bound: 波特率
{	
	USART_InitTypeDef usart2ini =		//串口2初始化结构体
	{
		115200,							//波特率
		USART_WordLength_8b,			//8位数据格式
		1,								//1个停止位
		USART_Parity_No,				//无奇偶校验位
		USART_Mode_Rx | USART_Mode_Tx,	//接收 / 发送模式
		USART_HardwareFlowControl_None	//无硬件数据流控制		
	};
	usart2ini.USART_BaudRate = bound;
	
	RCC->APB1RSTR |= 1 << 17;			//串口2复位
	RCC->APB1RSTR &= 0xFFFFFFFF ^ (1 << 17);	
	
	RCC->APB2ENR |= 1 << 2;				//使能PA时钟  
	RCC->APB1ENR |= 1 << 17;			//使能串口2时钟


	GPIOA->CRL &= 0xFFFF00FF;			//PA.2 复用推挽输出 最大速度50MHz
	GPIOA->CRL |= 0X00008B00;			//PA.3 上拉 / 下拉 输入
	
	RCC->APB1RSTR |= 1 << 17;			//串口2复位
	RCC->APB1RSTR &= 0xFFFFFFFF ^ (1 << 17);
										//串口2复位停止 
	NVIC_Init(&USART2_NVIC_CONFIG); 	//配置串口2接收优先级	  

	USART_Init(USART2, &usart2ini); 	//初始化串口2
	USART2->CR1 |= 1 << 5;				//接收缓冲区 (寄存器RDR) 非空中断使能
	USART2->CR1 |= 1 << 13;				//串口2工作状态使能
	
	//USART2->CR3 |= 1 << 7;			//串口2DMA发送使能
}



void usart2_senddat(u8 *buf, u8 len)	//使用串口2发送一段数据
{										//buf:发送区首地址
	u8 i;								//len:发送的字节数

	while (!(USART2->SR & (1 << 6)));

	for (i = 0; i <= len - 1; i++)		//等待发送完成
	{		
										
		USART2->DR = buf[i];
		while (!(USART2->SR & (1 << 7)));
										//等待发送数据寄存器空
	}
	while (!(USART2->SR & (1 << 6)));	//等待发送完成
}


