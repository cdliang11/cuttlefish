#ifndef ACTION_H
#define ACTION_H	 

#include "hardware.h"

enum STATE
{
   ROBOT_ARM,ROBOT_BODY  //GO_AHEAD,TURN_LEFT,TURN_RIGHT,MIDDLE,UP,DOWN
};

void Go_forward(float speed);
void Turn_left(float speed);
void Turn_right(float speed);
void Speed_up(void);
void Speed_down(void);
void Dive(void);
void Come_up(void);
void ExAction(u8 action);
void Init_Pos(void);
void Shou_qi(void);

void Arm_turn_right(void);
void Arm_turn_left(void);
void Arm_lean_forward(void);
void Arm_lean_backward(void);
void Arm_up(void);
void Arm_down(void);

extern enum STATE state;
extern u8 stop ;			    
#endif
