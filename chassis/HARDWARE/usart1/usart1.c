#include "hardware.h"
#include "usart1.h"
#include "protocal.h"
#include "action.h"
#include "config.h"

u8 continue_or_not = 0;
u8 u1buf[256] = { 0 };					//串口1接收缓冲
u32 u1len = 0;							//串口1接受缓冲区数据个数
u8 action = -1;
u8 getAction = 0;

u8 u1buf_action[256] = { 0 };					//串口1接收缓冲
u32 u1len_action = 0;							//串口1接受缓冲区数据个数

void USART1_IRQHandler()				//串口1中断函数
{
	static u8 flag = 0;
	if (USART1->SR & (1 << 5))
	{
		u1buf[u1len] = USART1->DR;		//收到的数据压入缓冲区	
//		u1buf_action[u1len_action] = u1buf[u1len];
		if(u1buf[u1len] == 0x55)flag = 1;
		if(flag == 1){
				u1len = (u1len + 1) & 0xFF;		//满256则清零		
				if(u1len == 6){
					u1len = 0;
					action = u1buf[3];
					getAction = 1;
					flag = 0;
					if(action == 1||action == 2|| action == 3||action == 4||action == 5){
						stop = 1;
						continue_or_not = 1;
					}
					else if(action == 6){
						getAction = 0;
						stop =  1;
					}
					else if(action == 7){
					}
					else if(action == 8)Speed_up();
					else if(action == 9){
						Speed_down();
						LED0 = ~LED0;
					}
					else if(action == 10){
					}
					else if(action == 11){
					}
					else if(action == 12){
					}
//					else if(action == 13){
//						if(state == ROBOT_ARM )state = ROBOT_BODY;
//						else state = ROBOT_ARM;
//					}
					else if(action == 14){
						axAllPosition(SERVO_NUMBER_WITH_ARM, InitialPosition_with_arm, 0x03E8);
						delay_ms(1000);
					}
				}
		}
//		ptcGetCommand(u1buf_action[u1len_action]);
//		u1len_action = (u1len_action + 1) & 0xFF;
	}
}
//55 05 06 01 01 00

void usart1_init(u32 bound)				//串口1初始化 bound: 波特率
{	
	USART_InitTypeDef usart1ini = 		//串口1初始化结构体
	{
		115200,							//波特率
		USART_WordLength_8b,			//8位数据格式
		1,								//1个停止位
		USART_Parity_No,				//无奇偶校验位
		USART_Mode_Rx | USART_Mode_Tx,	//接收 / 发送模式
		USART_HardwareFlowControl_None	//无硬件数据流控制
	};
	usart1ini.USART_BaudRate = bound;
	
	RCC->APB2RSTR |= 1 << 14;			//串口1复位
	RCC->APB2RSTR &= 0xFFFFFFFF ^ (1 << 14);

	RCC->APB2ENR |= 1 << 2;   			//使能PA时钟  
	RCC->APB2ENR |= 1 << 14;  			//使能串口1时钟 

	GPIOA->CRH &= 0XFFFFF00F;			//PA.9 	复用推挽输出	最大输出频率50MHz
	GPIOA->CRH |= 0X000008F0;			//PA.10 上拉 / 下拉输入

	NVIC_Init(&USART1_NVIC_CONFIG);		//配置串口1接收优先级	  

	USART_Init(USART1, &usart1ini);		//初始化串口1	
	USART1->CR1 |= 1 << 5;				//接收缓冲区 (寄存器RDR) 非空中断使能
	USART1->CR1 |= 1 << 13;				//串口1工作状态使能
}

void usart1_senddat(u8 *buf, u8 len)	//使用串口1发送一段数据
{										//buf:发送区首地址
	u8 i;								//len:发送的字节数
	for (i = 0; i <= len - 1; i++)
	{
		while (!(USART1->SR & (1 << 6)));
										//等待发送完成
		USART1->DR = buf[i];
	}
	while (!(USART1->SR & (1 << 6)));
}




