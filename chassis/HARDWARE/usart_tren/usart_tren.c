 
#include "usart_tren.h"


void usart_modetx(void)
{
	RXEN = 0;
	TXEN = 1;	
	USART3->CR2 &= ~(1 << 5);	//接收禁止
}

void usart_moderx(void)
{
	USART3->CR2 |= 1 << 5;	//接收使能

	TXEN = 0;
	RXEN = 1;	
}


void usart_tren_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 
															//使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
															//LED0-->PB.15 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//根据设定参数初始化GPIOB.0/1		

	usart_moderx();
}





