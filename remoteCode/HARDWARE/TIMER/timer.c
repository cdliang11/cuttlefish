#include "stm32f10x.h"
#include "timer.h"
#include "usart.h"	
#include "adc.h"
#include "24l01.h"


u16 adcx,adcy;
u8 send_flag;

float temp;
float speed[2];
u8 speed_flag;
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}



//#define Front 1
//#define Back 2
//#define OneSpeed 1
//#define TwoSpeed 2

void speed_solve()
{
	u8 i=2;
	
	if(speed[0]<0.5&&speed[0]>-0.5)
	{
		if(speed[1]<-1.5) speed_flag=GoUp;//前进
		else if(speed[1]>1.5) speed_flag=GoBack; //后退
		else speed_flag=Stop;//
	}
	else if(speed[0]>1.5) speed_flag=GoLeft;//左转
	else if(speed[0]<-1.5) speed_flag=GoRight;//右转
	else speed_flag=Stop;
}

//定时器3中断服务程序
//摇杆正常采集为 2+-0.3

/*

右摇杆电位器采集值 (adc10,adc11)
前 (1.96,0.005)  后(1.96,3.9)
左 (3.9,1.98)  右(0.001,1.98)

speed[]={adc10,adc11,adc12,adc13)
*/

void TIM3_IRQHandler(void)     //TIM3主中断
{
	static char i = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		i++;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		if(i == 5)
		{
			LED1 = !LED1;i = 0; //正常显示
		}
		adcx=Get_Adc_Average(ADC_Channel_10,10);
		temp=(float)adcx*(4.0/4096);
		//printf("adc10 %f\r\n",temp);
		temp -= 2.0;
		if(temp<0.15&&temp>-0.15) temp = 0;
		speed[0] = temp;//left or right
		
		adcy=Get_Adc_Average(ADC_Channel_11,10);
		temp=(float)adcy*(4.0/4096);
		//printf("adc11 %f\r\n",temp);
		temp -= 2.0;
		if(temp<0.15&&temp>-0.15) temp = 0;
		speed[1] = temp;//front or back
		
//		adcq=Get_Adc_Average(ADC_Channel_12,10);
//		temp=(float)adcq*(4.0/4096);
//		//printf("adc12 %f\r\n",temp);
//		temp -= 2.0;
//		if(temp<0.15&&temp>-0.15) temp = 0;
//		speed[0] = temp;//left and right
//		
//		adch=Get_Adc_Average(ADC_Channel_13,10);
//		temp=(float)adch*(4.0/4096);
//		//printf("adc13 %f\r\n",temp);
//		temp -= 2.0;
//		if(temp<0.15&&temp>-0.15) temp = 0;
//		speed[1] = temp;//front and back
		
	   speed_solve();
		 
		}
}
			
	



