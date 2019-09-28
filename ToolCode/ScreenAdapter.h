#pragma once
#include "afxwin.h"
//��������.x = Ԥ������.x * ��ʾ�����/Ԥ����ʾ����ȡ�
//��������.y = Ԥ������.y * ��ʾ���߶�/Ԥ����ʾ���߶ȡ�

class CScreenAdapter
{
private:
	//static const int SCREEN_PRESET_WIDTH=1440;
	//static const int SCREEN_PRESET_HEIGHT=900;
	static double ZoomScaleOfKeepScrSizeX;
	static double ZoomScaleOfKeepScrSizeY;
public:
	static bool InitByCurrDC(CDC* pDC);
	static CPoint TransPoint(CPoint &srcPt)
	{
		int nWidth=GetSystemMetrics(SM_CXSCREEN);  //�õ���Ļ��� 
		int nHeight=GetSystemMetrics(SM_CYSCREEN);
		CPoint destPt;
		destPt.x=(int)(srcPt.x*ZoomScaleOfKeepScrSizeX+0.5);
		destPt.y=(int)(srcPt.y*ZoomScaleOfKeepScrSizeY+0.5);
		return destPt;
	}
	static int AdaptWidthX(int width)
	{
		return (int)(width*ZoomScaleOfKeepScrSizeX+0.5);
	}
	static int AdaptHeightY(int height)
	{
		return (int)(height*ZoomScaleOfKeepScrSizeY+0.5);
	}
};