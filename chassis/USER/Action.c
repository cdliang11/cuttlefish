#include "hardware.h"
#include "action.h"
#include "config.h"
#include "math.h"

#define SLOWSPEED 0x07D0
#define FASTSPEED 0x03E8
#define MAXSPEED 0.6
#define MINSPEED 0.3
#define GAP 100


enum STATE state = ROBOT_BODY;
float speed = 0.5;
u8 stop = 0;

void Dive(){
	axAllPosition(SERVO_NUMBER, InitialPosition, SLOWSPEED);
	delay_ms(SLOWSPEED/4);
	delay_ms(SLOWSPEED/4);
	delay_ms(SLOWSPEED/4);
	axAllPosition(SERVO_NUMBER, shouqi, FASTSPEED);
	delay_ms(FASTSPEED/2);
	delay_ms(FASTSPEED/2);
	axAllPosition(SERVO_NUMBER, InitialPosition, SLOWSPEED);
	delay_ms(SLOWSPEED/4);

}

void Come_up(){
	axAllPosition(SERVO_NUMBER, InitialPosition, SLOWSPEED);
	delay_ms(SLOWSPEED/4);
	delay_ms(SLOWSPEED/4);
	delay_ms(SLOWSPEED/4);
	axAllPosition(SERVO_NUMBER, fangxia, FASTSPEED);
	delay_ms(FASTSPEED/2);
	delay_ms(FASTSPEED/2);
	axAllPosition(SERVO_NUMBER, InitialPosition, SLOWSPEED);
	delay_ms(SLOWSPEED/4);
}

void Speed_up(){
	speed += 0.05;
	if(speed < MINSPEED)speed = MINSPEED;
	else if(speed > MAXSPEED)speed = MAXSPEED;
}

void Speed_down(){
	speed -= 0.05;
	if(speed < MINSPEED)speed = MINSPEED;
	else if(speed > MAXSPEED)speed = MAXSPEED;
}


void Init_Pos(){
			axAllPosition(SERVO_NUMBER, InitialPosition, 0x03E8);
			return;
}

void Shou_qi()
{
	axAllPosition(SERVO_NUMBER,shouqi,0x03E8);
	return;
}



void Go_forward(float speed){
	float buffer[12];
	u16 pos[12];
	float start = 0;
	u8 i =0;
	u8 first_or_not = 1;
	while(1){
		if(stop == 1){
			stop = 0;
			break;
		}
		start = start + speed;
	for(i = 0;i<6;i++){
		buffer[i] = start + i*speed;
		buffer[i] = 500+200*sin(buffer[i]);
	}
	for(i = 6;i<12;i++){
//		if(buffer[i-6]>500)buffer[i] = buffer[i-6]-500;
//		else buffer[i] = 500 - buffer[i-6];
		buffer[i] = 700-(buffer[i-6] - 300);
	}
	for(i = 0;i<12;i++){
		pos[i] = buffer[i];
	}
	if(first_or_not == 1){
		axAllPosition(SERVO_NUMBER, pos, 0x03E8);
		delay_ms(0x3E8);
		first_or_not = 0;
	}
	else axAllPosition(SERVO_NUMBER, pos, 0x0064);
	delay_ms(100);
	}
}

void Turn_left(float speed){
	float buffer[12];
	u16 pos[12];
	float start = 0;
	u8 i =0;
	u8 first_or_not = 1;
	while(1){
		if(stop == 1){
			stop = 0;
			break;
		}
		start = start + speed;
	for(i = 0;i<6;i++){
		buffer[i] = start + i*speed;
		buffer[i] = 500+200*sin(buffer[i]);
	}
	for(i = 6;i<12;i++){
		buffer[i] = 700-(buffer[11-i] - 300);
	}
	for(i = 0;i<12;i++){
		pos[i] = buffer[i];
	}
	if(first_or_not == 1){
		axAllPosition(SERVO_NUMBER, pos, 0x03E8);
		delay_ms(0x3E8);
		first_or_not = 0;
	}
	else axAllPosition(SERVO_NUMBER, pos, 0x0064);
	delay_ms(100);
	}
}

void Turn_right(float speed){
	float buffer[12];
	u16 pos[12];
	float start = 0;
	u8 i =0;
	u8 first_or_not = 1;
	while(1){
		if(stop == 1){
			stop = 0;
			break;
		}
		start = start + speed;
	for(i = 6;i<12;i++){
		buffer[i] = start + i*speed;
		buffer[i] = 500+200*sin(buffer[i]);
	}
	for(i = 0;i<6;i++){
		buffer[i] = 700-(buffer[11-i] - 300);
	}
	for(i = 0;i<12;i++){
		pos[i] = buffer[i];
	}
	if(first_or_not == 1){
		axAllPosition(SERVO_NUMBER, pos, 0x03E8);
		delay_ms(0x3E8);
		first_or_not = 0;
	}
	else axAllPosition(SERVO_NUMBER, pos, 0x0064);
	delay_ms(100);
	}
}

void Arm_turn_right(void){
	u16 pos;
	 pos = axGetPosition(13);
	axSendPosition(13,pos+GAP,0x1f4);
	delay_ms(0x1f4);
}

void Arm_turn_left(void)
{
		u16 pos;
	 pos = axGetPosition(13);
	axSendPosition(13,pos-GAP,0x1f4);
	delay_ms(0x1f4);
}

void Arm_lean_backward(void)
{
	u16 pos14,pos15;
	pos14 = axGetPosition(14);
	delay_ms(20);
	pos15 = axGetPosition(15);
	delay_ms(20);
	axSendPosition(14,pos14+GAP,0x1f4);
	axSendPosition(15,pos15-GAP,0x1f4);
	delay_ms(0x1f4);
}

void Arm_up(void)
{
		u16 pos;
	 pos = axGetPosition(14);
	axSendPosition(14,pos+GAP,0x1f4);
	delay_ms(0x1f4);
}

void Arm_down(void)
{
		u16 pos;
	 pos = axGetPosition(14);
	axSendPosition(14,pos-GAP,0x1f4);
	delay_ms(0x1f4);
}

void Arm_lean_forward(void)
{
	u16 pos14,pos15;
	pos14 = axGetPosition(14);
	delay_ms(20);
	pos15 = axGetPosition(15);
	delay_ms(20);
	axSendPosition(14,pos14-GAP,0x1f4);
	axSendPosition(15,pos15+GAP,0x1f4);
	delay_ms(0x1f4);
}

void ExAction(u8 action){
	//Ä«Óã±¾Ìå
	if(state == ROBOT_BODY){
	switch(action){
		case 1:Come_up();break;
		case 2:Dive();break;
		case 3:Turn_left(speed);break;
		case 4:Turn_right(speed);break;
		case 5:Go_forward(speed);break;
		case 7:Shou_qi();break;
		default: break;
//		case 8:Speed_up();break;
//		case 9:Speed_down();break;
	}
}
	//»úÐµ±Û
	else if (state == ROBOT_ARM)
	{
		switch(action){
		case 1:Arm_up();break;
		case 2:Arm_down();break;
		case 3:Arm_turn_left();break;
		case 4:Arm_turn_right();break;
		case 8:Arm_lean_forward();break;
		case 9:Arm_lean_backward();break;
			default: break;
	}
	}
		
}
