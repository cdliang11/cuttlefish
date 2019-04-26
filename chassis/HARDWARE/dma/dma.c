#include "stm32f10x.h"


u8 DMA1_BUSY_FLAG = 0;
//DMA1忙碌状态标记
//0: DMA1空闲
//1: DMA1忙碌

void DMA1_Channel2_IRQHandler(void)					//DMA1通道2全局中断服务函数
{													//如果是因为传输错误进入该函数, 同样处理												
	DMA1->IFCR |= 1 << 4;							//清除DMA1全局中断标志
	DMA1_Channel2->CCR &= 0xFFFE;					//关闭DMA1传输	
	DMA1_BUSY_FLAG = 0;								//DMA1置为空闲状态
}

//DMA1初始化
//使用DMA1控制器, 来控制串口3的数据传输
//初始化时, 不开启传输
//buf: 存储器地址
void DMA1_Init(void)
{
	NVIC_InitTypeDef DMA1_Channel2_NVIC_CONFIG = 			
	{												//DMA1 通道2 全局中断初始化结构体
		DMA1_Channel2_IRQn,
		2,
		2,
		ENABLE	
	};
	
	volatile u32 i, j;
/*	用于延时的变量需要避免编译优化					*/

/*	DMA1通道使能									*/
	RCC->AHBENR |= 1;								

	for(i = 1; i <= 300; i++)			
		for(i = j; j <= 300; j++);
/*	等待DMA1时钟稳定, 这样延时是为了减少外部依赖项	
	大约10ms										*/
	
/* 	根据映射表, 串口3的TX对应的是DMA1的通道2		*/
	DMA1_Channel2->CCR = 0x309A;
	
/*	0x3090 = 0011 0000 1001 1010 b					**
**	[0]     0   通道禁止							**
**  [1]		1   传输完成中断:		允许			**
**	[2]   	0 	半传输中断:			禁止 			**												
**	[3]   	1 	传输错误中断:		允许			**												
**	[4]     1   数据传输方向: 		从存储器读		**
**	[5]     0   循环模式: 			关				**
**	[6]     0   外设地址增量模式: 	关				**
**	[7]     1   存储器地址增量模式: 开				**
**	[9:8]   00  外设数据宽度: 		8位				**
**	[11:10] 00  外设数据宽度: 		8位				**
**	[13:12] 11  通道优先级:			最高			**
**	[14]    0   存储器到存储器模式:	关				**
**	[31:15]    	保留, 始终为0						*/


/*	外设地址寄存器存储串口2发送缓冲区TDR的地址		*/	
	DMA1_Channel2->CPAR	= (u32)&USART3->DR;	
	
	NVIC_Init(&DMA1_Channel2_NVIC_CONFIG);			//DMA1 通道2 中断优先级初始化	
}

//开启一次DMA传输
//buf: 存储器地址
//numdat: 需要传输的数据数量
//返回0说明开启成功, 开启失败则返回当前待传输的数据量
u32 DMA1_SendDat(u8 *buf, u8 numdat)
{
	if(DMA1_BUSY_FLAG == 0)							
	{												//如果串口2发送为DMA模式且DMA1空闲											
		DMA1_BUSY_FLAG = 1;							//DMA1置为忙碌状态
		
		DMA1_Channel2->CCR &= 0xFFFE;				//先关闭DMA1传输
		
		DMA1_Channel2->CNDTR = numdat;				//设置传输数据数量
		DMA1_Channel2->CMAR = (u32)buf;				//设置存储器地址
		
		DMA1_Channel2->CCR |= 1;					//开启传输	
		return 0;
	}
	return DMA1_Channel2->CNDTR;
	
/*		传输完成后, 将在DMA1传输完成中断服务函数中	**
**		将DMA忙碌标志位重置 						*/
}

