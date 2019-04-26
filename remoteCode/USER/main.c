#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"

#include "usart.h"	 
#include "adc.h"
#include "timer.h"
#include "oled.h"

//按键的宏定义
#define keyup 1
#define keydown 2
#define keyleft 3
#define keyright 4
#define key4 5
#define key5 6
#define key6 7


extern u8 speed_flag;
void DataSend(u8 m,u16 n); //m号动作组 执行n次

int main(void)
{
	u8 key=0,i=5,long_first=0,key_flag=0,rocker_first=1;//
	u16 first=0;
	char buf[5];
	float voltage;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart1_init(115200);//无线串口模块 
	USART_Cofig(USART3, 115200);//调试串口
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	Adc_Init();             //初始化adc
	TIM3_Int_Init(5000,7200); 
	
	LED1=1;
	LED0=1;
	OLED_Init();
	OLED_Clear();
	OLED_ShowString(0,0,"State:Stop   ");
  OLED_ShowString(0,3,"Speed:");
	OLED_ShowChar(60,3,i+'0');
	OLED_ShowString(75,3,"Gear");
		//OLED_ShowString(8,2,"ZHONGJINGYUAN");  
	 //	OLED_ShowString(20,4,"2014/05/01");  
	OLED_ShowString(0,6,"NO BUG,COME ON");  
	//OLED_ShowString(63,6,"CODE:"); 
	
	
	while(1)
	{
		//电池电压显示
		//voltage=Get_Adc_Average(ADC_Channel_12,10);
		//voltage=(float)voltage*(3.3/4096)*2;
		//sprintf(buf,"%.1f",voltage);
		//buf[3]='V';
		//buf[4]='\0';
		//OLED_ShowString(70,3,buf);
		LED0=!LED0;
		key=KEY_Scan(1);
			if(key)
			{
				
				switch(key)
				{
					case KEYFront_PRES:
						key_flag=keyup;
						
						break;
					case KEYBack_PRES:
						key_flag=keydown;
						
						break;
					case KEYLeft_PRES:
						key_flag=keyleft;
						break;
					case KEYRight_PRES:
						key_flag=keyright;
						break;
					case KEY4_PRES:
						first++;
						if(first<10)
						{
						key_flag=key4;
						}
						else if(first>20)
						{
							long_first++;
							first=21;
							if(long_first==1)
							{
						  DataSend(12,1);
							printf("key4long \r\n");
							OLED_ShowString(0,0,"State:Key4_Long ");
							delay_ms(100);
							}
						}
						else
						{
							key_flag=0;
						}
						break;
					case KEY5_PRES:
						first++;
						if(first<10)
						{
							key_flag=key5;
						}
						else if(first>20)
						{
							first=21;
							long_first++;
							if(long_first==1)
							{
							DataSend(13,1);
							printf("key5long\r\n");
							OLED_ShowString(0,0,"State:Key5_long ");
							delay_ms(100);
							}
						}
						else key_flag=0;
						break;
					case KEY6_PRES:
						first++;
						if(first<10)
						{
							key_flag=key6;
						}
						else if(first>20)
						{
							first=21;
							long_first++;
							if(long_first==1)
							{
							DataSend(14,1);
							printf("key6long\r\n");
							OLED_ShowString(0,0,"State:Reset     ");
							delay_ms(100);
							}
						}
						else key_flag=0;
						break;
				}
			}
			else 
			{
				first=0;
				long_first=0;
				switch(key_flag)
				{
					case keyup:
						DataSend(1,1);
						OLED_ShowString(0,0,"State:Come-up  ");
						printf("Front\r\n");
						//delay_ms(100);
						break;
					case keydown:
						DataSend(2,1);
					  OLED_ShowString(0,0,"State:Diving    ");
						printf("Back\r\n");
						break;
					case keyleft:
						DataSend(3,1);
					  OLED_ShowString(0,0,"State:Turn left ");
					  printf("Left\r\n");
						break;
					case keyright:
						DataSend(4,1);
						OLED_ShowString(0,0,"State:Turn right");
						printf("Right\r\n");
						break;
					case key4:
						DataSend(5,1);
						OLED_ShowString(0,0,"State:Go forward");
						printf("4\r\n");
						break;
					case key5:
						DataSend(6,1);
						OLED_ShowString(0,0,"State:Stop      ");
						printf("5\r\n");
						break;
					case key6:
						DataSend(7,1);
						OLED_ShowString(0,0,"State:Lift      ");
						printf("6\r\n");
					  break;
				}
				key_flag=0;
			}
			///摇杆部分
			switch(speed_flag)
			{
				
				case GoUp: //加速
					if(rocker_first)
					{
						DataSend(8,1);
						printf("Speed up\r\n");
						speed_flag = -1;
						i++;
						if(i>7) i=7;
						OLED_ShowChar(60,3,i+'0');
						rocker_first=0;
					}
					break;
				case GoBack: //减速
					if(rocker_first)
					{
						DataSend(9,1);				
						printf("Slow down\r\n");
						speed_flag = -1;
						i--;
						if(i<1) i=1;
						OLED_ShowChar(60,3,i+'0');
						rocker_first=0;
					}
				  break;
				case Stop:
					rocker_first=1;
				  break;
//				case GoLeft: 
//					DataSend(10,1);
//					printf("GoLeft\r\n");
//				speed_flag = -1;break;
//				case GoRight: 
//					DataSend(11,1);
//					printf("GoRight\r\n");
//				speed_flag = -1;break;
			}
		
	
		//if(i==0) OLED_ShowString(0,0,"State:Stop   ");
		//i=0;
		delay_ms(10);
		}
}

void DataSend(u8 m,u16 n) //m号动作组 执行n次
{
//	USART_SendData(USART1 , 0x55);
//  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);
  USART_SendData(USART1 , 0x55);
  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);
  USART_SendData(USART1 , 0x05);
  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);         
  USART_SendData(USART1 , 0x06);
  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET); 
	USART_SendData(USART1 , m);//m号动作组
  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET); 
	USART_SendData(USART1 , n&0xff); //低八位
  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET); 
	USART_SendData(USART1 , (n>>8)&0xff); //高八位
  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET); 
}










































//	while(NRF24L01_Check())  //如果没有检查到无线模块两个灯会不停的闪烁
//	{
//		LED0=1;
//		LED1=1;
//		delay_ms(500);
//		LED0=0;
//		LED1=0;
//		delay_ms(500);
//		//printf("is xunhuan");
//	} 	 
//	TIM3_Int_Init(5000,7200); //检测到无线模块再开启
//	NRF24L01_TX_Mode();	//发射模式
//	while(1)
//	{
//		LED1=!LED1;
//		NRF24L01_TxPacket(speed_target);
//		delay_ms(500);
//	}

//}
//	
	
//		while(1)
//		{
//			  //printf("ok");
//				LED1=!LED1;	
//				
//			 flag=NRF24L01_TxPacket(buf);
//			 if(flag==TX_OK) printf("TX_OK");
//			 if(flag==0xff) printf("TX_fail");
//			 if(flag==MAX_TX) printf("MAX_TX");
//			 
//			  delay_ms(2000);  
//		}



