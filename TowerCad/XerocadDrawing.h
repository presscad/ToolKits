#pragma once

#include "IXeroCad.h"
#include "Buffer.h"
#include "HashTable.h"

#ifndef __DECLARE_GRID_DATA_STRU__
#define __DECLARE_GRID_DATA_STRU__
typedef struct tagGRID_DATA_STRU
{
	int data_type;	//0.�����ı�1.ʵʱ�ı�2.ʵʱ����3.��ͼ����
	long type_id;
	char sContent[MAX_PATH];
	char sContentPrefix[16];
	char sContentSuffix[16];
	double min_x,min_y,max_x,max_y;
	double fTextHigh,fHighToWide;
	//1.TopLeft    2.TopCenter    3.TopRight
	//4.MiddleLeft 5.MiddleCenter 6.MiddleRight
	//7.BottomLeft 8.BottomCenter 9.BottomRight
	int align_type;	
	tagGRID_DATA_STRU(){
		data_type=0;
		type_id=0;
		strcpy(sContent,"");
		strcpy(sContentPrefix,"");
		strcpy(sContentSuffix,"");
		min_x=min_y=max_x=max_y=0;
		fTextHigh=fHighToWide=0;
		align_type=0;
	}
}GRID_DATA_STRU;
#endif

bool AttachDataToBuffer(CHashList<GRID_DATA_STRU>&hashAttachDataByPointId,CBuffer& buufer);

bool InitXeroDrawingAndDataFromFile(IDrawing *pDrawing,const char* sFileName);

bool  DrawingToBuffer( IDrawing *pDrawing,CBuffer& buffer);