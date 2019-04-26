#include "stm32f10x.h"
#include "hardware.h"
#include "device.h"
#include "app.h"
#include "config.h"
#include "string.h"
#include "action.h"

extern u8 action ;
extern u8 getAction ;
extern int speed ;

int main(void)
{	
	

	delay_init();
	LED_Init();
	key_init();
	usart1_init(115200);
	//usart2_init(115200);
	usart3_init(115200);
	usart_tren_init();
	TIM2_Init();
	SD_Init();
	FileSystemInit();
	delay_ms(1000);
	axAllPosition(SERVO_NUMBER_WITH_ARM, InitialPosition_with_arm, 0x03E8);
	//Go_forward(0.5 ); //SLOW 0.3 fast 0.5
 	while(1)
	{
		if(getAction){
			ExAction(action);
			if(continue_or_not == 0){
				action = -1;
			getAction = 0;
			}else {
				continue_or_not = 0;
			}
			LED0 = ~LED0;
		}
	}
	
}








	