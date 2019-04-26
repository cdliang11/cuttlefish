#ifndef INTCONFIG_H
#define INTCONFIG_H

#include "hardware.h"


extern NVIC_InitTypeDef USART1_NVIC_CONFIG;			//串口1接收中断优先级初始化结构体

extern NVIC_InitTypeDef USART2_NVIC_CONFIG;			//串口2接收中断优先级初始化结构体

extern NVIC_InitTypeDef USART3_NVIC_CONFIG;			//串口3接收中断优先级初始化结构体

extern NVIC_InitTypeDef TIM2_NVIC_CONFIG;			//TIM2中断优先级初始化结构体

extern NVIC_InitTypeDef TIM3_NVIC_CONFIG;			//TIM3中断优先级初始化结构体

extern NVIC_InitTypeDef TIM4_NVIC_CONFIG;			//TIM4中断优先级初始化结构体

extern NVIC_InitTypeDef DMA1_Channel7_NVIC_CONFIG;	//DMA1全局中断初始化结构体

extern NVIC_InitTypeDef EXIT1_NVIC_CONFIG;			//外部中断1全局中断初始化结构体


#endif



