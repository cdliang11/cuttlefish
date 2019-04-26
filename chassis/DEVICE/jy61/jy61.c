#include "jy61.h"
#include "usart2.h"



JYRCVSTATE jysta = JYRCVSTA_HEADER;
u32 check = 0;
JYPACK jypack;
PSTDATA pstdata;

void jyStateIterator(u8 data)
{
	switch(jysta)
	{
		case JYRCVSTA_HEADER:
			if(data == JYPACKHEADER)
			{
				check = JYPACKHEADER;
				jysta = JYRCVSTA_PACKTYPE;				
			}
			break;
		
		case JYRCVSTA_PACKTYPE:
			if(data == JYTYPE_ACCE || data == JYTYPE_ANGLEV || data == JYTYPE_ANGLE)
			{
				jypack.packtype = (JYPACKTYPE)data;
				check += data;		
				jysta = JYRCVSTA_XL;				
			}
			else
			{
				jysta = JYRCVSTA_HEADER;
			}
			break;
		
		case JYRCVSTA_XL:
			jypack.data.x = data;
			check += data;
			jysta = JYRCVSTA_XH;
			break;
		
		case JYRCVSTA_XH:
			jypack.data.x += data << 8;
			check += data;
			jysta = JYRCVSTA_YL;
			break;
		
		case JYRCVSTA_YL:
			jypack.data.y = data;
			check += data;
			jysta = JYRCVSTA_YH;			
			break;
		
		case JYRCVSTA_YH:
			jypack.data.y += data << 8;
			check += data;
			jysta = JYRCVSTA_ZL;
			break;
		
		case JYRCVSTA_ZL:
			jypack.data.z = data;
			check += data;
			jysta = JYRCVSTA_ZH;
			break;
		
		case JYRCVSTA_ZH:
			jypack.data.z += data << 8;
			check += data;
			jysta = JYRCVSTA_TL;
			break;
		
		case JYRCVSTA_TL:
			jysta = JYRCVSTA_TH;	
			check += data;		
			break;
		
		case JYRCVSTA_TH:
			check += data;
			jysta = JYRCVSTA_CHECK;			
			break;
		
		case JYRCVSTA_CHECK:
			if(data == (check & 0xff))
			{
				if(jypack.packtype == JYTYPE_ACCE)
				{
					pstdata.acce.x = jypack.data.x * 16 * 9.8 / 32768.0;
					pstdata.acce.y = jypack.data.y * 16 * 9.8 / 32768.0;
					pstdata.acce.z = jypack.data.z * 16 * 9.8 / 32768.0;				
				}
				else if(jypack.packtype == JYTYPE_ANGLEV)
				{
					pstdata.anglev.x = jypack.data.x / 32768.0 * 2000;
					pstdata.anglev.y = jypack.data.y / 32768.0 * 2000;
					pstdata.anglev.z = jypack.data.z / 32768.0 * 2000;	
					
				}
				else if(jypack.packtype == JYTYPE_ANGLE)
				{
					pstdata.angle.x = jypack.data.x * 180 / 32768.0;
					pstdata.angle.y = jypack.data.y * 180 / 32768.0;
					pstdata.angle.z = jypack.data.z * 180 / 32768.0;	
					
				}
				u2len = 0;
			}
			jysta = JYRCVSTA_HEADER;				
			break;
				
		default:
			jysta = JYRCVSTA_HEADER;			
			break;
	}
	
	
	
	
}










