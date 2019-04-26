#include "file.h"
#include "string.h"
#include "config.h"

FDS fds=Download_Free;			//下载状态置为空闲
FATFS fs;


/*文件数据发送函数*/
FSS SendFileCommand(u8 INS_TYPE,u16 length,volatile u8 *data)
{
	int i;
	extern u8 RAND;
	for(i=0;i<10;i++)	//最多重发十次
	{
		MESSAGE_Reply(INS_TYPE,length,data);	//发送数据
		delay_ms(20);
		if(Command_Buffer.InsType==RECEIVE_OK/*&&Command_Buffer.Random==RAND*/)
			break;
		//delay_ms(15);
	}
	switch(Command_Buffer.InsType)			//对上位机回复指令进行识别
	{
		case RECEIVE_OK:									//上位机接收成功
			ptcClearCmdbuf(&Command_Buffer);
			return FileSend_OK;
		case LENGTH_Error:								//数据长度出错，一般不太可能
			ptcClearCmdbuf(&Command_Buffer);			
			return FileSend_LengthError;
		case CHECK_Error:									//校验位出错，没什么卵办法
			ptcClearCmdbuf(&Command_Buffer);
			return FileSend_CheckError;
		default:													//其它未知错误，这TM就很尴尬了
			ptcClearCmdbuf(&Command_Buffer);
			return FileSend_UnknownError;
	}
}

/*对文件下载上传系统的初始化*/
void FileSystemInit(void)
{
	DIR dir;			//目录结构体
	FRESULT res;	//通用结果代码
	fds=Download_Free;			//下载状态置为空闲
	SD_Init();							//SD卡初始化			 
	f_mount(&fs,"/",1);			//强制挂载SD卡
	
	/*判断是否存在Action目录*/
	res=f_opendir(&dir,"/Action");
	if(res==FR_NO_PATH)
		res=f_mkdir("/Action");
	else
		res=f_closedir(&dir);
	
	/*判断是否存在Order目录*/
	res=f_opendir(&dir,"/Order");
	if(res==FR_NO_PATH)
		res=f_mkdir("/Order");
	else
		res=f_closedir(&dir);
}

/*上传SD卡中所有微动作文件名*/
void UploadActionName_Function(void)
{
	FRESULT res;	//通用结果代码
	FILINFO fno;	//文件状态结构体
	FSS fss;			//文件数据发送状态
	TCHAR *fn;		//文件名
	DIR dir;			//目录结构体
	u8 Finish[2]={0xFF,0xFF};	//结束包信息位
	u8 FileNumber=0;			//文件数
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
	res=f_opendir(&dir,"/Action");	//打开Action目录计算文件数量
	while(1)
	{
		res=f_readdir(&dir,&fno);
		fn=fno.fname;		//同时支持长短文件名
		if((*fn)==NULL)
			break;
		FileNumber++;
	}
	res=f_closedir(&dir);
	
	res=f_opendir(&dir,"/Action");	//打开Action目录
	if(res==FR_OK)	//打开Action目录成功
	{
		fss=SendFileCommand(ReplyActionName,1,&FileNumber);	//向上位机发送包头
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
		while(1)
		{
			res=f_readdir(&dir,&fno);	//读取Action目录
			if(res!=FR_OK)	//读取Aciton目录文件名失败则取消这次读取
			{
				fss=SendFileCommand(BAD_MESSAGE,0,(u8*)(&res));	//向上位机返回错误类型
				if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
					return;
				break;																//取消读取
			}
			fn=fno.fname;		//同时支持长短文件名
			if((*fn)==NULL)	//读取完则结束循环
				break;
			fss=SendFileCommand(ReplyActionName,strlen(fn),(u8*)fn);	//向上位机发送文件名
			if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
				return;
		}
		
		fss=SendFileCommand(ReplyActionName,2,Finish);	//发送包尾
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
	else												//其它未知错误向上位机发送错误值
	{
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
}

/*下载指定的动作文件*/
void DownloadActionFile_Function(void)	//这个函数的报错系统不完善
{
		u8 debugnum=0;
	FRESULT res;				//通用结果代码
	FSS fss;						//文件数据发送状态?
	u8 *NONE;						//只用于发送信息占位
	UINT bw;						//没什么用的一个字节，写文件函数要用
	u8 ActBuffer[100]={"/Action/"};		//文件名缓存字节
	u8 i,p=0;								//计数器
	
	const u8 CommandNumberError[26]="Too Many Command Number !";	//报错用
	
	static FIL file_Action;			//文件命令结构体
	static u16 ADN;							//记录下载文件命令总数(ActionDownloadNumber)
	static u16 ADC;							//下载文件计数器(ActionDownloadCount)
	static u8 random_check;
	switch(fds)	//判断下载状态
	{
		case Download_Free:		//下载空闲状态
			f_sync(&file_Action);				//刷新一波缓冲区
			ADN=Command_Buffer.Data[0]*256;	//记录命令数
			ADN+=Command_Buffer.Data[1];
			for(i=2;i<Command_Buffer.Length;i++)				//文件名转存到字符串里
				ActBuffer[i+6]=Command_Buffer.Data[i];
			ActBuffer[i+6]='\0';												//最后一位记成空字符
			ADC=0;						//下载计数器归零
			random_check=Command_Buffer.Random+1;
			res=f_open(&file_Action,(const TCHAR*)ActBuffer,FA_CREATE_ALWAYS|FA_WRITE);
																	//打开文件，不存在就创建。
			MESSAGE_Reply(RECEIVE_OK,0,NONE);	//回复上位机接收成功
			if(res==FR_OK)
				fds=Action_Sending;					//状态机置为接收动作
			else
				fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
			break;
		
		case Action_Sending:	//接收动作状态
			
			if(Command_Buffer.Random!=random_check)	//先计算随机校验位是否有错
				return;
			random_check=Command_Buffer.Random+1;
			if(Command_Buffer.Random==0xFF)
				random_check=0;
			
			if(ADC>=ADN)	//下载命令数达到了总数
			{							//判断该命令是不是结束位
				res=f_close(&file_Action);			//关闭文件
				fds=Download_Free;						//状态机归位
				MESSAGE_Reply(RECEIVE_OK,0,NONE);	//回复上位机接收成功
				if(Command_Buffer.Data[0]==0xFF&Command_Buffer.Data[1]==0xFF)	//如果是结束位
				{
					if(res==FR_OK)
					{
						fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//向上位机发送传输文件成功
						if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
							return;
					}
					else	
					{						//其它未知错误向上位机发送错误值
						fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
						res=f_unlink((const TCHAR*)ActBuffer);	//删除文件 
						fss=SendFileCommand(BAD_MESSAGE,26,(u8*)CommandNumberError);	//向上位机发送传输文件失败
						if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
							return;
					}
				}
				else
				{
					res=f_unlink((const TCHAR*)ActBuffer);	//删除文件
					fss=SendFileCommand(BAD_MESSAGE,26,(u8*)CommandNumberError);	//向上位机发送传输文件失败
					if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
						return;
				}
			}
			else																			//下载命令数没达到总数
			{
				if(Command_Buffer.Data[0]==0xFF&Command_Buffer.Data[1]==0xFF)	//如果是结束位
				{
					res=f_close(&file_Action);			//关闭文件
					res=f_unlink((const TCHAR*)ActBuffer);	//删除文件
					MESSAGE_Reply(RECEIVE_OK,0,NONE);	//回复上位机接收成功
					fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
					if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
						return;
				}
				else	//不是结束位
				{
					//SendFileCommand(BAD_MESSAGE,1,&random_check);
					//SendFileCommand(BAD_MESSAGE,0,NULL);/

					res=f_lseek(&file_Action,ADC*180);	//计算出地址偏移数
					res=f_write(&file_Action,Command_Buffer.Data,Command_Buffer.Length,&bw);	//写入一个包的数据
					//usart2_senddat(Command_Buffer.Data,Command_Buffer.Length);  //使用串口3发送一段数据
					/*if(p<RefreshNum)   
					{
						p++;
						f_sync(&file_Action);
					}
					else p=0;*/
					MESSAGE_Reply(RECEIVE_OK,0,NONE);	//回复上位机接收成功
					if(res!=FR_OK)	//这地方需要加上纠错，比如上位机发下来的包有问题，数据数量和地址偏移数对不上
						fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
				}
			}
			ADC++;
			//if(ADC==23)
				//debugnum=0;
				
			break;
			
		default:
			break;
	}
}

/*执行指定的动作文件*/
void ExecuteActionFile_Function(void)	//这个函数在usart.c中的changeposition还是有点毛病
{
	FRESULT res;					//通用结果代码
	FSS fss;						//文件数据发送状态
	u8 *NONE;						//只用于发送信息占位
	//Command_Buffer.Data[Command_Buffer.Length]='\0';	//最后一位填充空字符
	//res=ExcuteAction((const TCHAR*)(Command_Buffer.Data));	//执行动作序列文件
	/*这部分我觉得可以不用对舵机进行判断，因为在ExcuteAction里面就有对舵机的判断*/
	switch(SERVO_TYPE)
	{
		case Digital:
			Command_Buffer.Data[Command_Buffer.Length]='\0';	//最后一位填充空字符
			res=ExcuteAction((const TCHAR*)(Command_Buffer.Data),0);	//执行动作文件
			break;
		case Simulate_LD2701:
			Command_Buffer.Data[Command_Buffer.Length]='\0';	//最后一位填充空字符
			res=ExcuteAction((const TCHAR*)(Command_Buffer.Data),0);	//执行动作文件
			break;
		default:
			break;
	}
	/*
	Command_Buffer.Data[Command_Buffer.Length]='\0';	//最后一位填充空字符
	res=ExcuteAction((const TCHAR*)(Command_Buffer.Data));	//执行动作文件
	*/
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
	else	
	{						//其它未知错误向上位机发送错误值
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
}

/*下载动作序列文件*/
void DownloadSequence_Function(void)
{
	FRESULT res;				//通用结果代码
	FSS fss;						//文件数据发送状态
	u8 *NONE;						//只用于发送信息占位
	static UINT bw;			//写文件函数要用
	u8 OrdBuffer[100]={"/Order/"};		//文件名缓存字节
	u8 i;								//计数器
	static FIL file_Order;	//文件命令结构体
	static u16 ODN;					//记录下载文件命令总数(OrderDownloadNumber)
	static u16 ODC;					//下载文件计数器(OrderDownloadCount)
	static u16 OAO;					//写入文件地址偏移量(OrderAddressOffset)
	switch(fds)	//判断下载状态
	{
		case Download_Free:		//下载空闲状态
			ODN=Command_Buffer.Data[0]*256;	//记录命令数
			ODN+=Command_Buffer.Data[1];
			for(i=2;i<Command_Buffer.Length;i++)				//文件名转存到字符串里
				OrdBuffer[i+5]=Command_Buffer.Data[i];
			ODC=0;						//下载计数器归零
			OrdBuffer[i+5]='\0';				//最后一位空字符
			OAO=0;						//地址偏移量记为0
			res=f_open(&file_Order,(const TCHAR*)OrdBuffer,FA_CREATE_ALWAYS|FA_WRITE);
																	//打开文件，不存在就创建。这里+2的原因是前两位是命令数
			MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
			fds=Order_Sending;					//状态机置为接收动作
			break;
		
		case Order_Sending:	//接收动作状态
			if(ODC>=ODN)	//下载命令数达到了总数
			{																					//判断该命令是不是结束位
				res=f_close(&file_Order);			//关闭文件
				fds=Download_Free;						//状态机归位
				MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
				if(Command_Buffer.Data[0]==0xFF&Command_Buffer.Data[1]==0xFF)	//如果是结束位
				{
					if(res==FR_OK)
					{
						fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//向上位机发送传输文件成功
						if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
							return;
					}
					else	
					{						//其它未知错误向上位机发送错误值
						fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
						if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
							return;
					}
				}
				else
				{
					fss=SendFileCommand(BAD_MESSAGE,0,NONE);	//向上位机发送传输文件失败
					if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
						return;
				}
			}
			else																			//下载命令数没达到总数
			{
				Command_Buffer.Data[Command_Buffer.Length]=' ';					//以空格分隔
				res=f_lseek(&file_Order,OAO);	//计算出地址偏移数
				if(res==FR_OK)	//这地方需要加上纠错，比如上位机发下来的包有问题，数据数量和地址偏移数对不上
					res=f_write(&file_Order,Command_Buffer.Data,Command_Buffer.Length+1,&bw);	//写入一个包的数据
				MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
			}
			ODC++;
			OAO+=(Command_Buffer.Length+1);	//加一是因为有空格
			break;
			
		default:
			break;
	}
}

/*执行动作序列文件*/
void ExecuteSequenceFile_Function(void)
{
	FRESULT res;			//通用结果代码
	FSS fss;					//文件数据发送状态
	u8 *NONE;					//只用于发送信息占位
	MESSAGE_Reply(RECEIVE_OK,0,NONE);	//回复上位机接收成功
	Command_Buffer.Data[Command_Buffer.Length]='\0';	//最后一位置空
	res=ExcuteOrder((TCHAR*)(Command_Buffer.Data));		//执行动作序列
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//向上位机发送传输文件成功
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	} 
	else	
	{						//其它未知错误向上位机发送错误值
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
}

/*移动指定舵机*/
void MoveServo_Function(void)
{
	volatile u8 jishuqi;
	FSS fss;					//文件数据发送状态
	u16 Servo_Position[100];
	u8 i;
	//LD2701_MoveServoPosition(Command_Buffer.Data);
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
	switch(SERVO_TYPE)
	{
		case Digital:
			for(i=1;i<=SERVO_NUMBER;i++)
			{
				//此处应该有检验舵机号是否正确的
				Servo_Position[i - 1]=Command_Buffer.Data[(i-1)*3+1]*256+Command_Buffer.Data[(i-1)*3+2];	//计算舵机角度
				//Servo_Position=Servo_Position*240/1023;	//还是计算舵机角度 我们用0-1023来表示0-300度
				//UBT12HB_SendPosition(i,Servo_Position,0,0);							
			}
			
			axAllPosition(SERVO_NUMBER, Servo_Position, 0x0320);
#ifdef KONGLONG
			steel_pst(22, Servo_Position[21]);
			steel_pst(23, Servo_Position[22]);
			steel_pst(24, Servo_Position[23]);
#endif			
			
			break;
			
		case Simulate_LD2701:
			Command_Buffer.Data[Command_Buffer.Length]='\0';	//最后一位填充空字符
			//LD2701_MoveServoPosition(Command_Buffer.Data);
			break;
		
		default:
			SendFileCommand(BAD_MESSAGE,0,NULL);
			return;
	}
	fss=SendFileCommand(GOOD_MESSAGE,0,NULL);	//向上位机发送移动舵机成功
	if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
		return;
}

/*删除指定动作文件*/
void DeleteActionFile_Function(void)
{
	FRESULT res;			//通用结果代码
	FSS fss;					//文件数据发送状态
	u8 *NONE;					//只用于发送信息占位
	char ActBuffer[100]={"/Action/"};			//动作文件名缓存字节
	u8 i;							//计数器
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
	
	for(i=0;i<Command_Buffer.Length;i++)			//动作文件存在Action目录里，需要转换
		ActBuffer[i+8]=Command_Buffer.Data[i];
	ActBuffer[i+8] = '\0';
	delay_ms(100);
	res=f_unlink(ActBuffer);
	delay_ms(100);	
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//向上位机发送传输文件成功
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
	else	
	{						//其它未知错误向上位机发送错误值
		fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
}

/*上传所有动作序列文件名*/
void UploadOrderName_Function(void)
{
	FRESULT res;	//通用结果代码
	DIR dir;		//目录结构体
	FILINFO fno;	//文件状态结构体
	FSS fss;		//文件数据发送状态
	TCHAR *fn;		//文件名
	u8 Finish[2]={0xFF,0xFF};	//结束包信息位
	u8 FileNumber=0;			//文件数
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
	res=f_opendir(&dir,"/Order");	//打开Order目录计算文件数量
	while(1)
	{
		res=f_readdir(&dir,&fno);
		fn=fno.fname;		//同时支持长短文件名
		if((*fn)==NULL)
			break;
		FileNumber++;
	}
	res=f_closedir(&dir);
	
	res=f_opendir(&dir,"/Order");	//打开Order目录
	if(res==FR_OK)	//打开Order目录成功
	{
		fss=SendFileCommand(ReplyOrderName,1,&FileNumber);	//向上位机发送包头
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
		while(1)
		{
			res=f_readdir(&dir,&fno);	//读取Order目录
			if(res!=FR_OK)	//读取Order目录文件名失败则取消这次读取
			{
				fss=SendFileCommand(BAD_MESSAGE,0,(u8*)(&res));	//向上位机返回错误类型
				if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
					return;
				break;																//取消读取
			}
			fn=fno.fname;		//同时支持长短文件名
			if((*fn)==NULL)	//读取完则结束循环
				break;
			fss=SendFileCommand(ReplyOrderName,strlen(fn),(u8*)fn);	//向上位机发送文件名
			if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
				return;
		}
		
		fss=SendFileCommand(ReplyOrderName,2,Finish);	//发送包尾
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
	else												//其它未知错误向上位机发送错误值
	{
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
}

/*上传动作指定序列文件内容*/
void UploadOrderFile_Function(void)
{
	FRESULT res;	//通用结果代码
	static FIL fp;				//文件命令结构体
	u8 i;					//计数器
	UINT br;			//已读取字节数
	FSS fss;			//文件数据发送状态		
	static u8 FileName[100];	//返回上位机文件名
	u8 file_number=0;	//记录序列中文件数目
	TCHAR OrdBuffer[100]={"/Order/"};		//动作序列文件名缓存字节
	TCHAR	ActBuffer[100];			//动作文件名缓存字节
	u8 Finish[2]={0xFF,0xFF};	//结束包信息位
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
	
	for(i=0;i<Command_Buffer.Length;i++)
		OrdBuffer[i+7]=Command_Buffer.Data[i];
	OrdBuffer[i+7]='\0';
	
	res=f_open(&fp,(const TCHAR*)OrdBuffer,FA_READ|FA_OPEN_EXISTING);			//以只读方式打开文件名
	
	if(res==FR_OK)
	{
		i=1;
		for(;;i++)
		{
			res=f_read(&fp,ActBuffer+i-1,1,&br);
			if(res!=FR_OK)	//读取文件不成功就结束循环
				break;
			if(br==0)	//已读取字节数小于1则表明读到了结尾
				break;
			if(*(ActBuffer+i-1)==' ')	//以空格分隔
			{
				file_number++;
				i=0;		//置零计数器
			}
		}
	}
	
	FileName[0]=file_number;	//第一位是文件数目
	for(i=0;OrdBuffer[i+7]!='\0';i++)
		FileName[i+1]=OrdBuffer[i+7];
	FileName[i+1]='\0';
	
	
	res=f_lseek(&fp,0);			//指针移动到文件头

	if(res==FR_OK)
	{
		fss=SendFileCommand(ReplyOrder,strlen((const char*)FileName),FileName);	//向上位机发送包头
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
		i=1;
		for(;;i++)
		{
			res=f_read(&fp,ActBuffer+i-1,1,&br);
			if(res!=FR_OK)	//读取文件不成功就结束循环
				break;
			if(br==0)	//已读取字节数小于1则表明读到了结尾
				break;
			if(*(ActBuffer+i-1)==' ')	//以空格分隔
			{
				*(ActBuffer+i-1)='\0';	//置为空字符
				fss=SendFileCommand(ReplyOrder,strlen(ActBuffer),(u8*)ActBuffer);	//向上位机发送文件名
				if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
					return;
				i=0;		//置零计数器
			}
		}
		res=f_close(&fp);
		fss=SendFileCommand(ReplyOrder,2,Finish);	//发送包尾
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
	else												//其它未知错误向上位机发送错误值
	{
		fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
		//fss=SendFileCommand(BAD_MESSAGE,10,OrdBuffer);
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
}

/*删除动作序列文件*/
void DeleteOrderFile_Function(void)
{
	FRESULT res;			//通用结果代码
	FSS fss;					//文件数据发送状态
	u8 *NONE;					//只用于发送信息占位
	TCHAR	ActBuffer[100]={"/Order/"};			//动作文件名缓存字节
	u8 i;							//计数器
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//回复上位机接收成功
	
	for(i=0;i<Command_Buffer.Length;i++)			//动作文件存在Action目录里，需要转换
		ActBuffer[i+7]=Command_Buffer.Data[i];
	ActBuffer[i+7]='\0';
	printf("%d",res);
	res=f_unlink(ActBuffer);
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//向上位机发送传输文件成功
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
	else	
	{						//其它未知错误向上位机发送错误值
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
}


void EnableAllServo()
{
	u8 i;
	u16 position;
	//void FlexMargin(void);
	for(i=0;i<Command_Buffer.Length;i+=2)
	{
		if(Command_Buffer.Data[i+1]==1)
		{
			axTorque(Command_Buffer.Data[i],AX_ON) ;
			delay_ms(20);
		}
		else 
		{
			axTorque(Command_Buffer.Data[i],AX_OFF) ;
			/*position=axReadPosition(Command_Buffer.Data[i]);
			axSendPosition(Command_Buffer.Data[i],position,0x00);*/
			delay_ms(20);
		}
	}
    MESSAGE_Reply(RECEIVE_OK,0,NULL);
}

/*重命名动作文件*/
void ReNameActionFile_Function(void)
{
	FRESULT res;			//通用结果代码
	FSS fss;					//文件数据发送状态
	u8 *NONE;					//只用于发送信息占位
	TCHAR	OldActBuffer[100]={"/Action/"};			//动作文件名缓存字节
	TCHAR	NewActBuffer[100]={"/Action/"};			//动作文件名缓存字节
	u8 i;							//计数器
	for(i=0;i<Command_Buffer.Data[0];i++)			//动作文件存在Action目录里，需要转换
		OldActBuffer[i+7]=Command_Buffer.Data[i];
	OldActBuffer[i+7]='\0';
	for(i=Command_Buffer.Data[0]+1;i<Command_Buffer.Length;i++)			//动作文件存在Action目录里，需要转换
		NewActBuffer[i+7]=Command_Buffer.Data[i];
	NewActBuffer[i+7]='\0';
	printf("%d",res);
	res=f_rename(OldActBuffer,NewActBuffer);
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//向上位机发送传输文件成功
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
	else	
	{						//其它未知错误向上位机发送错误值
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//如果没发送到上位机，结束函数
			return;
	}
}

u32 cur_time_ms = 0;
u32 next_action_ms = 0;
u8 order_flag = 0;		//是否在执行动作序列
/*执行指定的动作文件(接口型函数)*/
FRESULT ExcuteAction(const TCHAR *path,u8 first_or_not)
{
	u8 debugnum=0;
	FRESULT res;			//通用结果代码
	FIL fp;					//文件命令结构体
	u8 i;					//计数器
	u8 send[1];
	u8 *NONE;						//只用于发送信息占位
	volatile u8 jishuqi=0;
	UINT br;				//已读取字节数
	u16 Servo_Position[100];
	u8 ServoPosition[SERVO_NUMBER*3];					//舵机位置缓存字节
	TCHAR	ActBuffer[100]={"0:/Action/"};				//动作文件名缓存字节
	
	if(order_flag == 0)
	{
		cur_time_ms = clock_ms();
		next_action_ms = cur_time_ms + 20;
	}
	
	for(i=0;path[i]!='\0';i++)							//动作文件存在Action目录里，需要转换
		ActBuffer[i+10]=path[i];
	res=f_open(&fp,ActBuffer,FA_READ|FA_OPEN_EXISTING);	//以只读方式打开文件名
	
	//u3_printf("%s",ActBuffer);
	if(res==FR_OK)
	{
		MESSAGE_Reply(RECEIVE_OK,0,NONE);
		while(1)
		{
			res=f_read(&fp,ServoPosition,SERVO_NUMBER*3,&br);

			if(res!=FR_OK)								//读取文件不成功就结束循环
				break;
			if(br<(SERVO_NUMBER*3))						//说明读到了结尾
				break;
			else
			
			/*这部分提前break有点看不懂，觉得移动舵机这个动作没有执行吧*/
			switch(SERVO_TYPE)
			{
				case Simulate_LD2701:
					//LD2701_MoveServoPosition(ServoPosition);	//移动舵机
				case Digital:
					
					for(i=1;i<=SERVO_NUMBER;i++)
					{
						//此处应该有检验舵机号是否正确的						
						Servo_Position[i - 1]=ServoPosition[(i-1)*3+1]*256+ServoPosition[(i-1)*3+2];	//计算舵机角度前面的是高位后面的是低位、						
					}					
			}	
			//printf("%.2lf°n", pstdata.angle.z);

			while(clock_ms() < next_action_ms)
				;	//等待
			if(first_or_not==1){
			axAllPosition(SERVO_NUMBER, Servo_Position, 0x03E8);
				delay_ms(500);
				first_or_not = 0;
			}
			else axAllPosition(SERVO_NUMBER, Servo_Position, 0x0010);
#ifdef KONGLONG
			steel_pst(22, Servo_Position[21]);
			steel_pst(23, Servo_Position[22]);
			steel_pst(24, Servo_Position[23]);
#endif			
			delay_ms(10);								//老款机器人每帧间隔20ms，在使用遥控器的时候也要打开
			next_action_ms = next_action_ms + 20;
		}
	}
	res=f_close(&fp);
	return res;
}

/*执行指定的动作序列文件(接口型函数)*/
FRESULT ExcuteOrder(const TCHAR *path)
{
	u8 first_or_not = 1;
	u8 excute_or_not = 0;
	FRESULT res;			//通用结果代码
	FIL fp;						//文件命令结构体
	u8 i;							//计数器
	UINT br;					//已读取字节数
	TCHAR OrdBuffer[100]={"0:/Order/"};		//动作序列文件名缓存字节
	TCHAR	ActBuffer[100];			//动作文件名缓存字节

	
	//记录当前系统时间	
	cur_time_ms = clock_ms();
	next_action_ms = cur_time_ms + 20;
	order_flag = 1;
	
	
	for(i=0;path[i]!='\0';i++)
		OrdBuffer[i+9]=path[i];
	res=f_open(&fp,OrdBuffer,FA_READ|FA_OPEN_EXISTING);			//以只读方式打开文件名
	if(res==FR_OK)
	{
		i=1;
		for(;;i++)	//读到空字符说明读到了结尾
		{
			res=f_read(&fp,ActBuffer+i-1,1,&br);
			if(res!=FR_OK)	//读取文件不成功就结束循环
				break;
			if(br==0)	//已读取字节数小于1则表明读到了结尾
				break;
			if(*(ActBuffer+i-1)==' ')	//以空格分隔
			{
				*(ActBuffer+i-1)='\0';	//置为空字符
				res=ExcuteAction(ActBuffer,first_or_not);	//执行微动作
				excute_or_not = 1;
				i=0;		//置零计数器
			}
			if(excute_or_not == 1)first_or_not = 0;
		}
	}
	res=f_close(&fp);
	
	order_flag = 0;
	return res;
}

/*调试用报错代码*/
/*这里会有20个warning*/
u8* PRINT_FREUSTL(FRESULT res)
{
	switch(res)
	{
		case FR_OK: 					return (u8*)"FR_OK"; 							
		case FR_DISK_ERR: 				return (u8*)"FR_DISK_ERR";						/* (1) A hard error occured in the low level disk I/O layer */
		case FR_INT_ERR: 				return (u8*)"FR_INT_ERR"; 						/* (2) Assertion failed */
		case FR_NOT_READY: 				return (u8*)"FR_NOT_READY"; 					/* (3) The physical drive cannot work */
		case FR_NO_FILE: 				return (u8*)"FR_NO_FILE"; 						/* (4) Could not find the file */
		case FR_NO_PATH: 				return (u8*)"FR_NO_PATH"; 						/* (5) Could not find the path */
		case FR_INVALID_NAME: 			return (u8*)"FR_INVALID_NAME"; 					/* (6) The path name format is invalid */
		case FR_DENIED: 				return (u8*)"FR_DENIED"; 						/* (7) Acces denied due to prohibited access or directory full */
		case FR_EXIST: 					return (u8*)"FR_EXIST"; 						/* (8) Acces denied due to prohibited access */
		case FR_INVALID_OBJECT: 		return (u8*)"FR_INVALID_OBJECT"; 				/* (9) The file/directory object is invalid */
		case FR_WRITE_PROTECTED:		return (u8*)"FR_WRITE_PROTECTED"; 				/* (10) The physical drive is write protected */
		case FR_INVALID_DRIVE: 			return (u8*)"FR_INVALID_DRIVE"; 				/* (11) The logical drive number is invalid */
		case FR_NOT_ENABLED: 			return (u8*)"FR_NOT_ENABLED"; 					/* (12) The volume has no work area */
		case FR_NO_FILESYSTEM: 			return (u8*)"FR_NO_FILESYSTEM"; 				/* (13) There is no valid FAT volume */
		case FR_MKFS_ABORTED: 			return (u8*)"FR_MKFS_ABORTED";	 				/* (14) The f_mkfs() aborted due to any parameter error */
		case FR_TIMEOUT: 				return (u8*)"FR_TIMEOUT"; 						/* (15) Could not get a grant to access the volume within defined period */
		case FR_LOCKED: 				return (u8*)"FR_LOCKED"; 						/* (16) The operation is rejected according to the file shareing policy */
		case FR_NOT_ENOUGH_CORE: 		return (u8*)"FR_NOT_ENOUGH_CORE"; 				/* (17) LFN working buffer could not be allocated */
		case FR_TOO_MANY_OPEN_FILES: 	return (u8*)"FR_TOO_MANY_OPEN_FILES"; 			/* (18) Number of open files > _FS_SHARE */
		case FR_INVALID_PARAMETER: 		return (u8*)"FR_INVALID_PARAMETER"; 	
		default: 						return 0;
	}
}




