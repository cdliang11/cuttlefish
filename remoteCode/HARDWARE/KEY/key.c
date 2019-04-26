#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"



//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);//使能PA PC 时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	PBout(12)=1;
	PBout(13)=1;
	PBout(14)=1;
	PBout(15)=1;
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
		PCout(6)=1;
		PCout(7)=1;
    PCout(8)=1;
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;

//注意此函数有响应优先级,KEY_Front>KEY_Back>KEY_Left>KEY_Right>KEY4>KEY5>KEY6==0
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if((key_up)&&(KEYLeft==0||KEYRight==0||KEY4==0||KEY5==0||KEY6==0||KEYFront==0||KEYBack==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEYFront==0)   return KEYFront_PRES;
		else if(KEYBack==0)return KEYBack_PRES;
		else if(KEYLeft==0) return KEYLeft_PRES;
		else if(KEYRight==0) return KEYRight_PRES;
		else if(KEY4==0) return KEY4_PRES;
		else if(KEY5==0) return KEY5_PRES;
		else if(KEY6==0) return KEY6_PRES;
		
	}
	else if(KEYLeft==1&&KEYRight==1&&KEY4==1&&KEY5==1&&KEY6==1&&KEYFront==1&&KEYBack==0) key_up=1; 	    
 	return 0;// 无按键按下
}

/**************************************************************************
函数功能：按键扫描
入口参数：双击等待时间
返回  值：按键状态 0：无动作 1：单击 2：双击 
**************************************************************************/
u8 click_KEYFront (u8 time)
{
		static	u8 flag_key,count_key,double_key;	
		static	u16 count_single,Forever_count;
	  if(KEYFront==0)  Forever_count++;   //长按标志位未置1
     else        Forever_count=0;
		if(0==KEYFront&&0==flag_key)		flag_key=1;	
	  if(0==count_key)
		{
				if(flag_key==1) 
				{
					double_key++;
					count_key=1;	
				}
				if(double_key==2) 
				{
					double_key=0;
					count_single=0;
					return doubleClick;//双击执行的指令
				}
		}
		if(1==KEYFront)			flag_key=0,count_key=0;
		
		if(1==double_key)
		{
			count_single++;
			if(count_single>time&&Forever_count<time)
			{
			double_key=0;
			count_single=0;	
			return singleClick;//单击执行的指令
			}
			if(Forever_count>time)
			{
			double_key=0;
			count_single=0;	
			//return longPress;
			}
		}	
		return 0;
}

u8 click_KEYBack (u8 time)
{
		static	u8 flag_key,count_key,double_key;	
		static	u16 count_single,Forever_count;
	  if(KEYBack==0)  Forever_count++;   //长按标志位未置1
     else        Forever_count=0;
		if(0==KEYBack&&0==flag_key)		flag_key=1;	
	  if(0==count_key)
		{
				if(flag_key==1) 
				{
					double_key++;
					count_key=1;	
				}
				if(double_key==2) 
				{
					double_key=0;
					count_single=0;
					return doubleClick;//双击执行的指令
				}
		}
		if(1==KEYBack)			flag_key=0,count_key=0;
		
		if(1==double_key)
		{
			count_single++;
			if(count_single>time&&Forever_count<time)
			{
			double_key=0;
			count_single=0;	
			return singleClick;//单击执行的指令
			}
			if(Forever_count>time)
			{
			double_key=0;
			count_single=0;	
			}
		}	
		return 0;
}


