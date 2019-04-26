#include "usart3.h"



u8 u3buf[512] = { 0 };					//串口2接收缓冲
u32 u3len = 0;							//串口2接受缓冲区数据个数



void USART3_IRQHandler()				//串口2中断函数
{
	if (USART3->SR & (1 << 5))
	{
		u3buf[u3len] = USART3->DR;		//收到的数据压入缓冲区
		//USART1->DR = u3buf[u3len];		//通过另一个串口发送出去
		u3len = (u3len + 1) & 0x1FF;	//满512则清零
	}
}



void usart3_init(u32 bound)				//串口2初始化 bound: 波特率
{	
	USART_InitTypeDef usart3ini =		//串口2初始化结构体
	{
		115200,							//波特率
		USART_WordLength_8b,			//8位数据格式
		1,								//1个停止位
		USART_Parity_No,				//无奇偶校验位
		USART_Mode_Rx | USART_Mode_Tx,	//接收 / 发送模式
		USART_HardwareFlowControl_None	//无硬件数据流控制		
	};
	usart3ini.USART_BaudRate = bound;
	
	RCC->APB1RSTR |= 1 << 18;			//串口3复位
	RCC->APB1RSTR &= 0xFFFFFFFF ^ (1 << 18);	
	
	RCC->APB2ENR |= 1 << 3;				//使能PB时钟  
	RCC->APB1ENR |= 1 << 18;			//使能串口3时钟


	GPIOB->CRH &= 0xFFFF00FF;			//PB.10 复用推挽输出 最大速度50MHz
	GPIOB->CRH |= 0X00008B00;			//PB.11 上拉 / 下拉 输入
	
	RCC->APB1RSTR |= 1 << 18;			//串口3复位
	RCC->APB1RSTR &= 0xFFFFFFFF ^ (1 << 18);
										//串口3复位停止 
	NVIC_Init(&USART3_NVIC_CONFIG); 	//配置串口2接收优先级	  

	USART_Init(USART3, &usart3ini); 	//初始化串口2
	USART3->CR1 |= 1 << 5;				//接收缓冲区 (寄存器RDR) 非空中断使能
	USART3->CR1 |= 1 << 13;				//串口3工作状态使能
	
	//USART2->CR3 |= 1 << 7;			//串口2DMA发送使能
}



void usart3_senddat(u8 *buf, u8 len)	//使用串口3发送一段数据
{										//buf:发送区首地址
	u8 i;								//len:发送的字节数
	
	for (i = 0; i <= len - 1; i++)
	{
		while (!(USART3->SR & (1 << 6)));
										//等待发送完成
		USART3->DR = buf[i];
	}
	while (!(USART3->SR & (1 << 6)));
}



