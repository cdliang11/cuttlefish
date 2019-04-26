#include "key.h"
#include "protocal.h"
#include "string.h"
#include "stdio.h"
#include "config.h"

void EXTI1_IRQHandler(void)
{
#ifdef MOYU
	char filename[] = "moyu1.0.odr";
#endif

	
	LED0 = 0;
	delay_ms(50);//消抖
	LED1 = 1;
	
	Command_Buffer.InsType = ExecuteOrderFile;	
	sprintf((char*)Command_Buffer.Data, filename);
	Command_Buffer.Length = strlen(filename);
	GetProtocal = 1;
	

	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE0上的中断标志位  
	
}
 


void EXTIX_Init(void)
{

 	EXTI_InitTypeDef EXTI_InitStructure;
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

    //GPIOC.1 中断线以及中断初始化配置   下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;			//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 					//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


  	NVIC_Init(&EXIT1_NVIC_CONFIG); 
}

//外部中断1服务程序 

void key_init(void)
{
	RCC->APB2ENR |= 1 << 4;     //使能PORTC时钟
	GPIOC->CRL &= 0XFFFFFF0F;	//PC1设置成输入  
	GPIOC->CRL |= 0X00000080; 	//上拉 / 下拉输入
	  
	GPIOC->BSRR |= 1 << 1;	   	//PC1 上拉
	EXTIX_Init();	
} 


