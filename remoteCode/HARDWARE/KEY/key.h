#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define WK_UP PAin(0)	//PA0  WK_UP

#define KEYFront  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//前进
#define KEYBack  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//后退
#define KEYLeft  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//左
#define KEYRight  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)//右
#define KEY4  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)//
#define KEY5  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)//
#define KEY6  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)//


 

#define KEYFront_PRES 	1	//
#define KEYBack_PRES	  2	//
#define KEYLeft_PRES   3
#define KEYRight_PRES  4
#define KEY4_PRES       5
#define KEY5_PRES       6
#define KEY6_PRES       7
#define doubleClick 8
#define longPress 9
#define singleClick 10


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数				
u8 click_KEYBack (u8 time);
u8 click_KEYFront (u8 time);


#endif
