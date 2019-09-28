#pragma once
#include "IXeroCad.h"
#include "HashTable.h"

IDbPoint *AppendDbPoint(IDrawing *pDrawing,GEPOINT pos,
						HIBERID hiberId=0,short style=PS_SOLID,
						COLORREF crColor=RGB(0,0,0),BYTE width=5);
IDbLine *AppendDbLine(IDrawing *pDrawing,GEPOINT start,GEPOINT end,
					  HIBERID hiberId=0,short style=PS_SOLID,
					  COLORREF crColor=RGB(0,0,0),BYTE width=1);
IDbArcline *AppendDbArcLine(IDrawing *pDrawing,double* center,double r,
						    double startAngle,double endAngle,double* work_norm,
						    HIBERID hiberId=0,short style=PS_SOLID,
						    COLORREF crColor=RGB(0,0,0),BYTE width=1);
IDbArcline *AppendDbArcLine(IDrawing *pDrawing,
							HIBERID hiberId=0,short style=PS_SOLID,
							COLORREF crColor=RGB(0,0,0),BYTE width=1);
IDbPolyline *AppendDbPolyline(IDrawing *pDrawing,GEPOINT *ptArr,int nArrSize,
							HIBERID hiberId=0,short style=PS_SOLID,
							COLORREF crColor=RGB(0,0,0),BYTE width=1);
IDbCircle *AppendDbCircle(IDrawing *pDrawing,f3dPoint center,f3dPoint norm,
						  double radius,HIBERID hiberId=0,short style=PS_SOLID,
						  COLORREF crColor=RGB(0,0,0),BYTE width=1);
IDbText *AppendDbText(IDrawing *pDrawing,f3dPoint pos,const char* text,
					  double rotation,double height,int aligment=IDbText::AlignMiddleLeft,
					  HIBERID hiberId=0,short style=PS_SOLID,
					  COLORREF crColor=RGB(0,0,0),BYTE width=1,GEPOINT norm=GEPOINT(0,0,1));
IDbMText *AppendDbMText(IDrawing *pDrawing,f3dPoint pos,const char* text,
						double rotation,double height,double width,
						int aligment=IDbText::AlignMiddleLeft,short flowDirection=IDbMText::kLtoR,
						HIBERID hiberId=0,short style=PS_SOLID,
						COLORREF crColor=RGB(0,0,0),BYTE pen_width=1);
IDbRect *AppendDbRect(IDrawing *pDrawing,f3dPoint topLeft,f3dPoint btmRight,
					  HIBERID hiberId=0,short style=PS_SOLID,
					  COLORREF crColor=RGB(0,0,0),BYTE width=1);
IDbAlignedDimension* AppendDbAlignedDim(IDrawing *pDrawing,f3dPoint start,f3dPoint end,
										f3dPoint dimPos,const char* dimText,long dimStyleId=0,double fTextH=2,
										GEPOINT upperDirection=GEPOINT(0,0,-1),HIBERID hiberId=0,short style=PS_SOLID,
										COLORREF crColor=RGB(0,0,0),BYTE width=1);
IDbRotatedDimension* AppendDbRotatedDim(IDrawing *pDrawing,f3dPoint start,f3dPoint end,
										f3dPoint dimPos,const char* dimText,double rotation,long dimStyleId=0,
										GEPOINT upperDirection=GEPOINT(0,0,-1),HIBERID hiberId=0,short style=PS_SOLID,
										COLORREF crColor=RGB(0,0,0),BYTE width=1);
IDbArrow* AppendDbArrow(IDrawing *pDrawing,f3dPoint origin,
						f3dPoint direct,double len,double hederLen,
						HIBERID hiberId=0,short style=PS_SOLID,
						COLORREF crColor=RGB(0,0,0),BYTE width=1);
IDbDiametricDimension *AppendDbDiaDim(IDrawing *pDrawing,f3dPoint chordPoint,
									  f3dPoint farChordPoint,double leaderLength,
									  HIBERID hiberId=0,short style=PS_SOLID,
									  COLORREF crColor=RGB(0,0,0),BYTE width=1);


typedef struct tagDRAWING_DIM_STYLE
{
	CXhChar16 dimStyleName;	//������ʽ����
	long m_idDimStyle;		//������ʽ
	XERO_DIMSTYLEVAR dimStyleVar;
	tagDRAWING_DIM_STYLE(){m_idDimStyle=0;}
	void UpdateDimStyleTextHeight(IEntityDatabase* pDataBase,double fTextHeight);
	void UpdateDimStyleDimGap(IEntityDatabase* pDataBase,double fDimGap);
	void UpdateDimStyleDimExtend(IEntityDatabase* pDataBase,double fDimExtend);
	void UpdateDimStyleDimBlk(IEntityDatabase* pDataBase,char ciDimBlk);
	void UpdateDimStyleArrowSize(IEntityDatabase* pDataBase,double fArrowSize);
	void UpdateDimStyleTextStyleId(IEntityDatabase* pDataBase,long idTextStyle);
	void UpdateDimStyle(IEntityDatabase* pDataBase,XERO_DIMSTYLEVAR var);
}DRAWING_DIM_STYLE;
class XeroDimStyleTable
{
public:
	static DRAWING_DIM_STYLE dimStyle	;
	static DRAWING_DIM_STYLE dimStyleHoleSpace;	//��ע��˨���
	static DRAWING_DIM_STYLE dimStyleHoleSpace2;//��ע��˨���(�ı�λ�ڳߴ����²�)
	static DRAWING_DIM_STYLE dimStyle3;			//��ǿ�ƽ��ߴ���ڳߴ�����ڲ�
	static DRAWING_DIM_STYLE dimAngleStyle;
	static DRAWING_DIM_STYLE dimRadiusStyle;

	static void InitDimStyle(IEntityDatabase* pDataBase);
	static void InitDimStyle2(IEntityDatabase* pDataBase);
	static void UpdateDimStyle(IEntityDatabase* pDataBase,double text_size,double arrow_size=1,
							   double dim_extend=1,double dim_gap=0.5,char ciDimBlk=0);
};