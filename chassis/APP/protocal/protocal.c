
#include "protocal.h"
#include "file.h"
#include "string.h"
#include "config.h"


STATE receive_state;			//状态机指令
COMMAND Command_Buffer;			//解析串口消息为指令
u16 Present_DataLength; 		//当前接收字节长度，用于计数
u8 GetProtocal = 0;				//接收到指令标志位

/*用户定义串口发送函数*******************************************/
void ptc_usartx_senddat(u8 *buf, u8 len)
{
	usart1_senddat(buf, len);
}
/****************************************************************/

int rand = 0;


void ptcClearCmdbuf(PCOMMAND cmd)
{
	memset((void*)cmd, 0, sizeof(COMMAND));
	cmd->InsType = 0xff;
}


//向上位机发送信息函数
void MESSAGE_Reply(u8 INS_TYPE, u16 length, volatile u8 *data)
{
	u32 check = 0;					//校验码计算
	u8 buf[256] = { 0 };		//发送数据缓存

	u16 i = 0;					//数据发送计数器
	extern u8 RAND;

	if(INS_TYPE == BAD_MESSAGE)
		LED0 = ~LED0;
	
	buf[0] = PACK_HEAD;			//数据头							
//	buf[1] = (INS_TYPE == RECEIVE_OK) ? Command_Buffer.Random : (rand++);
	buf[1] = Command_Buffer.Random;
	//如果是回复OK指令就返回随机数, 其它情况产生一个随机数发送出去		
	buf[2] = COMPUTER_ID; 		//主机号(机器人编号)
	buf[3] = ROBOT_ID; 			//从机号(上位机编号)	
	buf[4] = length >> 8; 		//数据长度高位		
	buf[5] = length % 256; 		//数据长度低位	
	buf[6] = INS_TYPE; 			//数据类型

	for (i = 0; i <= length - 1; i++)
		buf[7 + i] = data[i];

	for (i = 1; i <= length + 6; i++)
		check += buf[i];
	check = check % 255;

	buf[7 + length] = check;

	ptc_usartx_senddat(buf, length + 8);
}

//将串口消息解析为指令
void ptcGetCommand(u8 byte_data)
{
	u8 *NONE;					

	switch (receive_state)
	{
		case STATE_BEGIN:							//**命令起始位
			if (byte_data == PACK_HEAD)
			{
				Command_Buffer.check = PACK_HEAD;
				Present_DataLength = 0;
				receive_state = STATE_RAMDOM;
			}
			break;

		case STATE_RAMDOM:
			Command_Buffer.Random = byte_data;
			Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
			receive_state = STATE_SENDERID;
			break;

		case STATE_SENDERID:						//**主机号
			if (byte_data == COMPUTER_ID)			//允许接收的主机号
			{
				Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
				receive_state = STATE_RECEIVERID;
			}
			else
				receive_state = STATE_BEGIN;		//接收失败状态机复位
			break;

		case STATE_RECEIVERID:						//**从机号
			if (byte_data == ROBOT_ID
			|| byte_data == BRODCAST_SIGNAL)	//广播信号
			{
				Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
				receive_state = STATE_LENGTH_H;
			}
			else
				receive_state = STATE_BEGIN;		//接收失败状态机复位
			break;

		case STATE_LENGTH_H:						//**数据长度高位
			Command_Buffer.Length = byte_data;
			Command_Buffer.Length <<= 8;
			Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
			receive_state = STATE_LENGTH_L;
			break;

		case STATE_LENGTH_L:						//**数据长度低位
			Command_Buffer.Length += byte_data;
			if (Command_Buffer.Length < MAX_DATA_LENGTH)
			{
				Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
				receive_state = STATE_INSTYPE;
			}
			else
			{
				receive_state = STATE_BEGIN;		//接收失败状态机复位
				MESSAGE_Reply(LENGTH_Error, 0, NONE);
													//回复上位机错误类型
			}
			break;

		case STATE_INSTYPE:							//**接收数据类型
			Command_Buffer.InsType = byte_data;
			Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
			receive_state = STATE_DATA;
			if (Command_Buffer.Length == 0)			//如果数据长度等于0，则跳过接收数据直接校验
				receive_state = STATE_CHECK;
			break;

		case STATE_DATA:							//**接收数据
			if (Present_DataLength < Command_Buffer.Length)
			{
				Command_Buffer.Data[Present_DataLength++] = byte_data;
				Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
			}
			if (Present_DataLength == Command_Buffer.Length)
			{										//接收完毕
				Command_Buffer.check = Command_Buffer.check % 255;
				receive_state = STATE_CHECK;
			}
			break;

		case STATE_CHECK:							//**接收完毕校验位
			if (byte_data == Command_Buffer.check % 255)
			{
				if(Command_Buffer.InsType != RECEIVE_OK
					&& Command_Buffer.InsType != LENGTH_Error
					&& Command_Buffer.InsType != CHECK_Error
					&& Command_Buffer.InsType != 0xff)
					GetProtocal=1;		
					receive_state = STATE_BEGIN;	//接收成功状态机复位				
			}
			else
			{
				receive_state = STATE_BEGIN;		//接收失败状态机复位
				MESSAGE_Reply(CHECK_Error, 0, 0);	//回复上位机错误类型								
				Command_Buffer.check = 0;
			}
			break;

		default:
			receive_state = STATE_BEGIN;			//接收失败状态机复位
			break;
	}
	
					//判断接收到指令
}

extern u16 InitialPosition[SERVO_NUMBER];
extern const u8 SERVO_NAME[SERVO_NUMBER][100];	//查找在.h文件中定义的机器人舵机名	
u8 Ax_Position[100];
u8 ax_receive;	
u16 ax_long_receive[100];

//将上位机发送下来的指令进行分类
void pctCommandClassify(COMMAND command)
{

	u8 INS = command.InsType;		//获取命令类型
	u8 *NONE = 0;
	u8 i;
	switch (INS)
	{

		case GetServoConnection:	//获取下位机信息
			Reply_GetServoConnection();	//回复机器人信息
			break;

		case GetServoPosition:		//获得数字舵机的当前位置
				MESSAGE_Reply(RECEIVE_OK,0,NONE);	//回复上位机接收成功
				Ax_Position[0]=Digital;
				Ax_Position[1]=SERVO_NUMBER;
				for(i = 1; i <= SERVO_NUMBER; i++)
				{
						ax_long_receive[i] = axGetPosition(i);
						Ax_Position[3*i-1]=i;
						Ax_Position[3*i]=ax_long_receive[i]>>8;
						Ax_Position[3*i+1]=ax_long_receive[i];
						if(ax_long_receive[i]==0xFFFF)
							MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//回复上位机舵机类型错误
				}
				SendFileCommand(ReplyServoPosition,SERVO_NUMBER*3+2,(u8*)Ax_Position);

		
			break;

		case EnableDigitalServo:	//使能全部数字舵机
				if(SERVO_TYPE==Digital)	//只有数字舵生效
					 EnableAllServo();
				else
					MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//回复上位机舵机类型错误
			break;


		case DisableDigitalServo:	//解使能全部数字舵机
				if(SERVO_TYPE==Digital)	//只有数字舵生效
				{
					int i;
					for(i=1;i<=SERVO_NUMBER;i++)
					{
						ax_receive=axTorque(i,AX_OFF) ;
						if(ax_receive==0xFF)
							MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//回复上位机舵机类型错误
					}
				}
				else
					MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//回复上位机舵机类型错误
			break;

		case UploadActionName:		//上传FLASH中微动作文件名
				UploadActionName_Function();
			break;

		case DownloadActionFile:	//下载指定的动作文件
				DownloadActionFile_Function();
			break;

		case ExecuteActionFile:		//执行指定文件名的微动作文件
				ExecuteActionFile_Function();
			break;

		case DownloadOrder:			//下载指定文件名的动作序列
				DownloadSequence_Function();
			break;

		case ExecuteOrderFile:		//执行指定文件名的动作序列
				ExecuteSequenceFile_Function();
			break;

		case MoveServo:				//移动指定舵机(用于动态控制)
				MoveServo_Function();
			break;

		case DeleteActionFile:		//删除指定动作文件
				DeleteActionFile_Function();
			break;

		case UploadOrderName:		//上传所有动作序列文件
				UploadOrderName_Function();
			break;

		case UploadOrderFile:		//上传动作序列文件内容	
				UploadOrderFile_Function();
			break;

		case DeleteOrderFile:		//删除指定动作序列文件
				DeleteOrderFile_Function();
			break;

		default:
				MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//回复上位机指令类型错误
			break;
	}
}

void Reply_GetServoConnection(void)
{
	u8 i;
	FSS fss;										//文件数据发送状态
	u8 Data[100];								//即将发送的数据
	u8 Finish[2]={0xFF,0xFF};		//结束包信息位
	u8 ax_num;
	
	const u8 ServoOffLine[]="Some Servo Lost Connection.";	//舵机掉线错误信息
	const u8 ServoTypeError[]="No Such Servo Type.";				//舵机类型错误信息
	
	Data[0]=SERVO_TYPE;					//写入舵机类型
	Data[1]=SERVO_NUMBER;				//写入舵机数目
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
	
	/*switch(SERVO_TYPE)	//针对不同类型的舵机要有检验
	{
		case Digital:	//对于UBT12HB型号舵机
			for(ax_num=1;ax_num<=SERVO_NUMBER;ax_num++)
				if(AX_Pin(ax_num)!=0x01)
				{
					SendFileCommand(BAD_MESSAGE,27,(u8*)ServoOffLine);
					return;
				}
			break;
				
		case Simulate_LD2701:	//对于LD1602舵机无法检验
			break;
		
		default:	//其它情况返回舵机型号错误
			SendFileCommand(BAD_MESSAGE,19,(u8*)ServoTypeError);
			break;
	}*/
	fss=SendFileCommand(ReplyServoConnection,2,Data);
	if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
		return;
	
	for(i=0;i<SERVO_NUMBER;i++)	//写入舵机初态
	{
		Data[0]=i+1;										//舵机号
		Data[1]=(*(InitialPosition+i))/256;	//角度高位
		Data[2]=(*(InitialPosition+i))%256;	//角度低位
		Data[3]='\0';										//把字符串第三位设置为字符串的终止
		strcat((char*)Data,(char*)SERVO_NAME[i]);	//链接两个字符串
		fss=SendFileCommand(ReplyServoConnection,strlen((char*)Data),Data);
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
	fss=SendFileCommand(ReplyServoConnection,2,Finish);
	if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
		return;
}




