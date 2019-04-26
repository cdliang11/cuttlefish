
#ifndef __JY61_H
#define __JY61_H

#include "hardware.h"

#define JYPACKHEADER	0x55
typedef enum tagJYRCVSTATE
{
	JYRCVSTA_HEADER,
	JYRCVSTA_PACKTYPE,
	JYRCVSTA_XL,
	JYRCVSTA_XH,
	JYRCVSTA_YL,
	JYRCVSTA_YH,
	JYRCVSTA_ZL,
	JYRCVSTA_ZH,
	JYRCVSTA_TL,
	JYRCVSTA_TH,
	JYRCVSTA_CHECK
}JYRCVSTATE;

typedef enum JYPACKTYPE
{
	JYTYPE_ACCE = 0x51,
	JYTYPE_ANGLEV = 0x52,
	JYTYPE_ANGLE = 0x53
}JYPACKTYPE;

typedef struct tagVECTORD
{
	double x;
	double y;
	double z;	
}VECTORD, *PVECTORD;
	
typedef struct tagVECTORI
{
	u16 x;
	u16 y;
	u16 z;	
}VECTORI, *PVECTORI;
	
typedef struct tagJYPACK
{
	JYPACKTYPE packtype;
	VECTORI data;
}JYPACK, *PJYPACK;

typedef struct tagPSTDATA
{
	VECTORD angle;	//角度
	VECTORD anglev;	//角速度
	VECTORD acce;	//加速度
}PSTDATA, *PPSTDATA;


void jyStateIterator(u8 data);
extern PSTDATA pstdata;

#endif



