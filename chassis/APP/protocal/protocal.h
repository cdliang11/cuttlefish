#ifndef PROTOCAL_H
#define PROTOCAL_H

#include "device.h"




#define MAX_DATA_LENGTH			200 	//最大数据长度
//#define EN_USART3_RX			1		//使能(1)/禁止(0)串口3接收

#define PACK_HEAD 				0xFF	//起始位为0xFF

//编号配置
#define COMPUTER_ID				0x05	//允许接收的主机编号

#define BRODCAST_SIGNAL			0x5F	//广播信号



//上位机通讯状态机
typedef enum tagSTATE
{
	STATE_BEGIN,		//起始位
	STATE_RAMDOM,		//随机校验位
	STATE_SENDERID,		//主机号
	STATE_RECEIVERID,	//从机号
	STATE_LENGTH_H,		//数据长度高位
	STATE_LENGTH_L,		//数据长度低位
	STATE_INSTYPE,		//指令类型
	STATE_DATA,			//指令
	STATE_CHECK			//校验位
}STATE;


typedef struct tagCOMMAND
{
	u8	Random;								//随机校验位(这一位在下位机被挂起，暂时没用到)
	u16 Length;								//命令长度
	u8 	InsType;							//指令类型
    u8  Data[MAX_DATA_LENGTH];				//数据
	u16 check;								//校验
}COMMAND, *PCOMMAND;
extern COMMAND Command_Buffer;			//解析串口消息为指令

void MESSAGE_Reply(u8 INS_TYPE,u16 length,volatile u8 *data);


#define SERVO_TYPE					Digital	//舵机类型

/*舵机类型配置*/
#define Digital						0x01	//UBT12HB数字舵机
#define Simulate_LD2701				0x02	//LD1602模拟舵机


/*下位机回复包指令集*/
#define RECEIVE_OK					0x00	//接收成功
#define LENGTH_Error				0x01	//数据长度超出最大值
#define CHECK_Error					0x02	//校验位不正确

/*上位机命令包指令集*/
#define GetServoConnection			0x03	//获取下位机信息(机器人名，舵机类型，舵机数)
#define GetServoPosition			0x04	//获得数字舵机的当前位置(仅数字舵机有用)
#define EnableDigitalServo			0x05	//使能全部数字舵机(仅数字舵机有用)
#define DisableDigitalServo			0x06	//解使能全部数字舵机(仅数字舵机有用)
#define MoveServo					0x07	//移动指定舵机(用于动态控制)

/*文件系统动作文件指令集*/
#define DownloadActionFile			0x08	//下载指定的动作文件
#define ExecuteActionFile			0x09	//执行指定文件名的微动作文件
#define DeleteActionFile			0x0A	//删除指定动作文件
#define ReNameActionFile			0x0B	//重命名动作文件
#define UploadActionName			0x0C	//上传FLASH中微动作文件名

/*文件系统序列文件指令集*/
#define DownloadOrder				0x0D	//下载指定的动作序列文件
#define ExecuteOrderFile			0x0E	//执行指定文件名的动作序列
#define UploadOrderName				0x0F	//上传动作序列文件名
#define UploadOrderFile				0x10	//上传动作序列文件
#define RenameOrderFile				0x11	//重命名序列文件
#define DeleteOrderFile				0x12	//删除指定动作序列文件

/*下位机正常回复包指令集*/
#define ReplyServoConnection		0x13	//回复下位机信息(舵机类型，舵机数，初始状态)
#define ReplyServoPosition			0x14	//回复数字舵机当前位置(仅数字舵机可用)



//将串口消息解析为指令
void ptcGetCommand(u8 byte_data);

//指令解析
void pctCommandClassify(COMMAND Command_Buffer);	

//回复上位机数据
void Reply_GetServoConnection(void);		

//向上位机发送信息函数
void MESSAGE_Reply(u8 INS_TYPE, u16 length, volatile u8 *data);


extern COMMAND Command_Buffer;
extern u8 GetProtocal;
void ptcClearCmdbuf(PCOMMAND cmd);

FRESULT ExcuteAction(const TCHAR *path,u8 first_or_not);
FRESULT ExcuteOrder(const TCHAR *path);

#endif
