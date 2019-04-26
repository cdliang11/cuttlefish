
#ifndef AX12_H
#define AX12_H

#include "hardware.h"



#define	ID_ALL		0Xfe			//广播ID
//LX-16的控制表
#define LOBOT_SERVO_FRAME_HEADER         0x55
#define LOBOT_SERVO_MOVE_TIME_WRITE      1
#define LOBOT_SERVO_MOVE_TIME_READ       2
#define LOBOT_SERVO_MOVE_TIME_WAIT_WRITE 7
#define LOBOT_SERVO_MOVE_TIME_WAIT_READ  8
#define LOBOT_SERVO_MOVE_START           11
#define LOBOT_SERVO_MOVE_STOP            12
#define LOBOT_SERVO_ID_WRITE             13
#define LOBOT_SERVO_ID_READ              14
#define LOBOT_SERVO_ANGLE_OFFSET_ADJUST  17
#define LOBOT_SERVO_ANGLE_OFFSET_WRITE   18
#define LOBOT_SERVO_ANGLE_OFFSET_READ    19
#define LOBOT_SERVO_ANGLE_LIMIT_WRITE    20
#define LOBOT_SERVO_ANGLE_LIMIT_READ     21
#define LOBOT_SERVO_VIN_LIMIT_WRITE      22
#define LOBOT_SERVO_VIN_LIMIT_READ       23
#define LOBOT_SERVO_TEMP_MAX_LIMIT_WRITE 24
#define LOBOT_SERVO_TEMP_MAX_LIMIT_READ  25
#define LOBOT_SERVO_TEMP_READ            26
#define LOBOT_SERVO_VIN_READ             27
#define LOBOT_SERVO_POS_READ             28
#define LOBOT_SERVO_OR_MOTOR_MODE_WRITE  29
#define LOBOT_SERVO_OR_MOTOR_MODE_READ   30
#define LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE 31
#define LOBOT_SERVO_LOAD_OR_UNLOAD_READ  32
#define LOBOT_SERVO_LED_CTRL_WRITE       33
#define LOBOT_SERVO_LED_CTRL_READ        34
#define LOBOT_SERVO_LED_ERROR_WRITE      35
#define LOBOT_SERVO_LED_ERROR_READ       36


//AX-12控制表
#define P_MODEL_NUMBER_L      		0
#define P_MODOEL_NUMBER_H     		1
#define P_VERSION             		2
#define P_ID                  		3
#define P_BAUD_RATE           		4
#define P_RETURN_DELAY_TIME   		5
#define P_CW_ANGLE_LIMIT_L    		6
#define P_CW_ANGLE_LIMIT_H    		7
#define P_CCW_ANGLE_LIMIT_L   		8
#define P_CCW_ANGLE_LIMIT_H   		9
#define P_SYSTEM_DATA2        		10
#define P_LIMIT_TEMPERATURE   		11
#define P_DOWN_LIMIT_VOLTAGE  		12
#define P_UP_LIMIT_VOLTAGE    		13
#define P_MAX_TORQUE_L        		14
#define P_MAX_TORQUE_H        		15
#define P_RETURN_LEVEL        		16
#define P_ALARM_LED           		17
#define P_ALARM_SHUTDOWN      		18
#define P_OPERATING_MODE      		19
#define P_DOWN_CALIBRATION_L  		20
#define P_DOWN_CALIBRATION_H  		21
#define P_UP_CALIBRATION_L    		22
#define P_UP_CALIBRATION_H    		23

#define P_TORQUE_ENABLE         	24
#define P_LED                   	25
#define P_CW_COMPLIANCE_MARGIN  	26
#define P_CCW_COMPLIANCE_MARGIN 	27
#define P_CW_COMPLIANCE_SLOPE   	28
#define P_CCW_COMPLIANCE_SLOPE  	29
#define P_GOAL_POSITION_L      	 	30
#define P_GOAL_POSITION_H      	 	31
#define P_GOAL_SPEED_L          	32
#define P_GOAL_SPEED_H          	33
#define P_TORQUE_LIMIT_L        	34
#define P_TORQUE_LIMIT_H        	35
#define P_PRESENT_POSITION_L    	36
#define P_PRESENT_POSITION_H    	37
#define P_PRESENT_SPEED_L       	38
#define P_PRESENT_SPEED_H       	39
#define P_PRESENT_LOAD_L        	40
#define P_PRESENT_LOAD_H        	41
#define P_PRESENT_VOLTAGE       	42
#define P_PRESENT_TEMPERATURE   	43
#define P_REGISTERED_INSTRUCTION	44
#define P_PAUSE_TIME            	45
#define P_MOVING 					46
#define P_LOCK                  	47
#define P_PUNCH_L               	48
#define P_PUNCH_H               	49


//指令类型
typedef enum tagAXORDER
{
	AXORDER_PING           = 0x01,
	AXORDER_READ           = 0x02,
	AXORDER_WRITE          = 0x03,
	AXORDER_REG_WRITE      = 0x04,
	AXORDER_ACTION         = 0x05,
	AXORDER_RESET          = 0x06,
	AXORDER_DIGITAL_RESET  = 0x07,
	AXORDER_SYSTEM_READ    = 0x0C,
	AXORDER_SYSTEM_WRITE   = 0x0D,
	AXORDER_SYNC_WRITE     = 0x83,
	AXORDER_SYNC_REG_WRITE = 0x84	
}AXORDER;


/*AX指令执行结果*/
typedef enum
{	
	AXERO_VOLTAGE,			//电压范围出错
	AXERO_ANGLE,			//角度限制错误
	AXERO_HEAT,				//过热错误
	AXERO_RANGE,			//指令范围错误
	AXERO_CHECK,			//校验码错误
	AXERO_LOAD,				//过载错误
	AXERO_INST,				//指令错误
	AXERO_OTHER,			//出现其它错误
	AXERO_OK,				//应答成功		
//	AXERO_ID,				//返回ID不正确
//	Timeout,				//应答超时
}AXERROR;


//AX12舵机反馈包内容
typedef struct tagAXFB
{
	u8 id;					//舵机号1
	u8 length;             	//数据长度
	u8 axerror;            	//错误类型
	u8 param[20];          	//其他参数
	u8 check;				//校验位
}AXFB, *PAXFB;



//AX12舵机反馈包状态机
typedef enum tagAXSTATE
{
	AXSTATE_HEAD1,
	AXSTATE_HEAD2,
	AXSTATE_ID,
	AXSTATE_LENGTH,
	AXSTATE_ERROR,
	AXSTATE_DATA,
	AXSTATE_CHECK
}AXSTATE;


typedef struct tagFLEXCONFIG
{
	u8 cwbound;		//顺时针柔性边距
	u8 ccwbound;	//逆时针柔性边距
	u8 cwslope;		//顺时针柔性斜率
	u8 ccwslope;	//逆时针柔性斜率
	u8 hit_high;	//撞击(高)
	u8 hit_low;		//撞击(低)	
}FLEXCONFIG, *PFLEXCONFIG;

//函数声明

u8 axReset(u8 ID);

//发送给某一个舵机一个要移动到的位置
//ID: 			舵机ID
//target_pos: 	目标位置	0-1023
//target_speed: 指定速度	0-1023
uint8_t axSendPosition(u8 ID, u16 target_pos, u16 target_speed);


//解使能 / 使能舵机
//ID: 	舵机ID
//mode:	0为解使能, 1为使能
u8 axTorque(u8 ID, u8 mode);

//修改舵机ID
//ID: 		舵机ID
//trgID:	修改后ID	
void axModifyID(u8 ID, u8 trgID);

//批量发送舵机位置预备数据
//num_ax:		舵机数量, 发送的ID从1~numax
//position[]:	位置数组
//speed:		速度
u8 axAllPosition(u8 num_ax, u16 position[], u16 speed);

u16 axGetPosition(u8 ID);

//修改柔性斜率等
u8 axFlexible(u8 ID, PFLEXCONFIG pflexconfig);
//LX16舵机
u8 LobotCheckSum(u8 buf[]);
void LobotSerialServoMove(u8 id, u16 position, u16 time);
void LobotSerialServoSetID(u8 oldID, u8 newID);

#define AX_ON	1
#define AX_OFF	0

#endif



