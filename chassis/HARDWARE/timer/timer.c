#include "timer.h"



int count_sec = -1;


void TIM2_IRQHandler(void)   							//TIM4中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  	//清除TIMx更新中断标志 
		
		//LED0 = ~LED0;
		count_sec++;
	}
}


void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 			//时钟使能
												
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = 9999; 						//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =7199; 						//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 				//根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 						//使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置

	NVIC_Init(&TIM2_NVIC_CONFIG);  									//初始化NVIC寄存器


	TIM_Cmd(TIM2, ENABLE);  										//使能TIMx					 
}


u32 clock_ms()
{
	return count_sec * 1000 + TIM2->CNT / 10.0;
}










