#include "intconfig.h"


NVIC_InitTypeDef USART1_NVIC_CONFIG =			
{									//串口1接收中断优先级初始化结构体
	USART1_IRQn,					
	3,							
	3,								
	ENABLE							//使能
};

NVIC_InitTypeDef USART2_NVIC_CONFIG =			
{									//串口2接收中断优先级初始化结构体
	USART2_IRQn,					
	2,								
	2,								
	ENABLE							
};

NVIC_InitTypeDef USART3_NVIC_CONFIG =			
{									//串口3接收中断优先级初始化结构体
	USART3_IRQn,					
	2,								
	2,								
	ENABLE							
};

NVIC_InitTypeDef TIM2_NVIC_CONFIG =
{									//TIM2中断优先级初始化结构体
	TIM2_IRQn,			
	1,
	1,		
	ENABLE
};


NVIC_InitTypeDef TIM3_NVIC_CONFIG =
{									//TIM3中断优先级初始化结构体
	TIM3_IRQn,			
	0,
	0,		
	ENABLE
};

NVIC_InitTypeDef TIM4_NVIC_CONFIG =
{									//TIM4中断优先级初始化结构体
	TIM4_IRQn,			
	0,
	0,		
	ENABLE
};

NVIC_InitTypeDef DMA1_Channel7_NVIC_CONFIG = 			
{									//DMA1全局中断初始化结构体
	DMA1_Channel7_IRQn,
	2,
	2,
	ENABLE	
};

NVIC_InitTypeDef EXIT1_NVIC_CONFIG = 			
{									//外部中断1全局中断初始化结构体
	EXTI1_IRQn,
	3,
	3,
	ENABLE	
};













