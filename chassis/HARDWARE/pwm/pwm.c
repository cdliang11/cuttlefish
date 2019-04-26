#include "pwm.h"


u8 cur_steel_tim4 = 1;
u8 cur_steel_tim3 = 1;

#define steel1_mdf 0
#define steel2_mdf 0 	//向上增大 向下减小
#define steel3_mdf 0	//向上减小 向下增大

u16 steel1_pst = 1500 + steel1_mdf;	//ID = 22
u16 steel2_pst = 1500 + steel2_mdf;	//ID = 23
u16 steel3_pst = 1500 + steel3_mdf;	//ID = 24
u16 steel4_pst = 1500;


//750us - 2250us
void steel_pst(u8 ID, u16 pst)
{
	if(ID == 22)
	{
		steel1_pst = pst / 1024.0 * 1500 + 750 + steel1_mdf;
		steel1_pst = steel1_pst;
	}
	else if(ID == 23)
	{
		steel2_pst = pst / 1024.10 * 1500 + 750 + steel2_mdf;
		steel2_pst = steel2_pst;

	}
	else if(ID == 24)
	{
		steel3_pst = pst / 1024.0 * 1500 + 750 + steel3_mdf;	
		steel3_pst = steel3_pst;		
	}
	
	
}
void TIM4_IRQHandler(void)   							//TIM4中断
{
	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  	//清除TIMx更新中断标志 
		
		cur_steel_tim4++;
		if(cur_steel_tim4 > 4)
			cur_steel_tim4 = 1;
		
		TIM4->CCR1 = steel1_pst;	
		TIM4->CCR2 = steel2_pst;	
		TIM4->CCR3 = steel3_pst;	
		
		if(cur_steel_tim4 == 1)
		{
			TIM4->CCER |= 0x0001;
		}
		else if(cur_steel_tim4 == 2)
		{
			TIM4->CCER |= 0x0010;
		}		
		else if(cur_steel_tim4 == 3)
		{
			TIM4->CCER |= 0x0100;
		}		
		else if(cur_steel_tim4 == 4)
		{
			TIM4->CCER |= 0x0000;
		}		
		
	}
}



//TIM4 PWM部分初始化 

void TIM4_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//使能定时器4时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  
																	//使能GPIO外设和AFIO复用功能模块时钟
	
 
	//设置该引脚为复用输出功能,输出TIM4 CH1的PWM脉冲波形	GPIOB.6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
																	//TIM4_CH1, TIM4_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  				//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//初始化GPIO
 
	//初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = 19999; 						//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 						//设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 					//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 				//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); 						//使能指定的TIM4中断,允许更新中断


	NVIC_Init(&TIM4_NVIC_CONFIG);  									//初始化NVIC寄存器


	//初始化TIM4 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  						//根据T指定的参数初始化外设TIM4 OC1
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  						//根据T指定的参数初始化外设TIM4 OC2
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  						//根据T指定的参数初始化外设TIM4 OC3

	TIM_Cmd(TIM4, ENABLE); 							 				//使能TIM4	

}



