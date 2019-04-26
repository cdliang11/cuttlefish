#ifndef FILE_H
#define FILE_H

#include "hardware.h"
#include "device.h"
#include "app.h"

/*文件系统指令集*/
#define ReplyActionName			0x15	//上传微动作文件名
#define ReplyOrderName			0x16	//上传动作序列文件名
#define ReplyOrder				0x17	//上传动作序列文件

/*文件系统回复包指令集*/
#define GOOD_MESSAGE			0x18	//执行命令成功
#define BAD_MESSAGE				0x19	//执行命令失败

/*数据包缓存刷新频率*/
#define RefreshNum            	5  		//每5次刷新一下缓存区

/*文件命令上传返回类型*/
typedef enum
{
	FileSend_OK,						//数据包发送成功
	FileSend_LengthError,				//数据包长度溢出
	FileSend_CheckError,				//数据包校验位错误
	FileSend_UnknownError				//未知错误
}FSS;									//FileSendState

/*文件下载状态机*/
typedef enum
{
	Download_Free,						//下载空闲
	Action_Sending,						//正在下载微动作文件
	Order_Sending						//正在下载动作序列文件
}FDS;									//FileDownloadState


FSS SendFileCommand(u8 INS_TYPE,u16 length,volatile u8 *data);	
														//文件数据发送函数
void FileSystemInit(void);								//文件下载上传系统初始化
void UploadActionName_Function(void);					//上传所有微动作
void DownloadActionFile_Function(void);					//下载微动作文件
void ExecuteActionFile_Function(void);					//执行指定名称的微动作
void DownloadSequence_Function(void);					//下载动作序列文件
void ExecuteSequenceFile_Function(void);				//执行动作序列文件
void ForceExecuteActionFile_Function(u8 *buf, u8 len); 	//强制执行某个动作文件														
void MoveServo_Function(void);							//移动指定舵机
void DeleteActionFile_Function(void);					//删除指定动作文件
void UploadOrderName_Function(void);					//上传所有动作序列文件名
void UploadOrderFile_Function(void);					//上传指定动作序列文件
void DeleteOrderFile_Function(void);					//删除指定动作序列文件
void ReNameActionFile_Function(void);					//重命名指定动作文件
void EnableAllServo(void);
void ForceExecuteSequenceFile_Function(u8 *buf, u8 len);//强行执行序列文件
u8 exf_getfree(u8 *drv,u32 *total,u32 *free);
/*接口型函数*/
FRESULT ExcuteOrder(const TCHAR *path);					//执行指定的动作序列文件
FRESULT ExcuteAction(const TCHAR *path,u8 first_or_not);
u8* PRINT_FREUSTL(FRESULT res);






#endif

