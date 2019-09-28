#pragma once
#include "ArrayList.h"
#include "DrawingToolKits.h"
#include "NumberToolKits.h "
#include "I_DrawSolid.h"
#include "ProcessPart.h"
#include "LocalDrawing.h"
#include "DbEntContext.h"

class CLDSGeneralPlate;
class AcDbBlockTableRecord;
struct DRAW_PARAM{
	const static int TYPE_PATTERN	= 0;	//����ͼ
	const static int TYPE_DETAIL	= 1;	//���ͼ
	BYTE cDrawingType;		//��ͼ����
	double fDrawScale;		//���ű���
	double fPartLabelTextHeight;//��ע�ָ�
	double fDimSizeTextHeight;	//�ߴ��ָ�
	double fDimAngleTextHeight;	//�Ƕ��ָ�
	DRAW_PARAM(BYTE cType=TYPE_DETAIL,double draw_scale=0.2,
		double labelTextHeight=20,double dimTextHeight=20,
		double dimAngleTextHeight=20){
			cDrawingType=cType;
			fDrawScale=draw_scale;
			fPartLabelTextHeight=labelTextHeight;
			fDimAngleTextHeight=dimAngleTextHeight;
			fDimSizeTextHeight=dimTextHeight;
	}
};

class IPlateDrawing : public CXeroDrawing
{
public:
	const static DWORD TYPE_COMMON		= 0;//
	const static DWORD TYPE_TUBE_END	= 1;//�ְ�
	double fTextHeight;
	static DRAW_PARAM m_drawParam;
	static void SetDrawParam(DRAW_PARAM &draw_param){m_drawParam=draw_param;}
protected:
	const static int TYPE_PATTERN	= 0;	//����ͼ
	const static int TYPE_DETAIL	= 1;	//���ͼ
	DWORD m_dwDrawType;//��ͼ����
	DWORD m_dwClassType;//�ְ����
	CDrawingEntsContext m_xEntsContext;
	CProcessPlate *m_pProcessPlate;
	CLDSGeneralPlate *m_pPlate;
public:
	IPlateDrawing(){
		m_pProcessPlate=NULL;
		m_pPlate=NULL;
		m_dwClassType=TYPE_TUBE_END;
		m_dwDrawType=TYPE_DETAIL;
	}
	virtual void Init(CProcessPlate *pProcessPlate){;}
	virtual void Init(CLDSGeneralPlate *pPlate){;}
	virtual void Draw(IDrawing *pDrawing){;}
	virtual void Draw(IDrawingComponent *pComponet){;}
	virtual void Draw(IDrawingCommand *pCommand){;}
	virtual void Draw(AcDbBlockTableRecord *pBlockTbl){;}
	const DWORD GetClassType(){return m_dwClassType;}
};

class CTubeEndPlateDrawing : public IPlateDrawing
{
private:
	//�ְ�
	double L,M,N,S,W,H,m_fThick;
	CXhChar16 m_sPartLabel;
	char m_cMaterial;
	struct HOLE_INFO{
		int d;
		double fIncrement;
		double fPosX,fPosY;
		HOLE_INFO(){d=0;fIncrement=fPosX=fPosY=0;}
	};
	ARRAY_LIST<HOLE_INFO> boltHoleList;//��˨��
	HOLE_INFO *m_pHoleBottomRight;//��ע�����ĵ�
	////�ֹ�
	double m_fTubeD,m_fTubeThick,m_fTubeLengthExposed,m_fHf;
	CXhChar16 m_sTubeMaterial;
public:
	typedef struct HOLE_GROUP{
		double fCoord;
		ARRAY_LIST<HOLE_INFO> boltInfoPtrList;
		void Add(HOLE_INFO *pHole)
		{
			HOLE_INFO hole_info;
			hole_info.d=pHole->d;
			hole_info.fIncrement=pHole->fIncrement;
			hole_info.fPosX=pHole->fPosX;
			hole_info.fPosY=pHole->fPosY;

			HOLE_INFO *phole=NULL;
			for (phole=boltInfoPtrList.GetFirst();phole;phole=boltInfoPtrList.GetNext())
			{
				if(hole_info.fPosX<phole->fPosX&& hole_info.fPosY<phole->fPosY)
					break;
			}
			if(!phole)
				boltInfoPtrList.append(hole_info);
			else
				boltInfoPtrList.insert(hole_info,boltInfoPtrList.GetCurrentIndex());
		}
		HOLE_GROUP(){
			fCoord=0;
			boltInfoPtrList;
		}
	}*HOLE_GROUP_PTR;
public:
	CTubeEndPlateDrawing(void);
	~CTubeEndPlateDrawing(void);
	void Init(CProcessPlate *pProcessPlate);
	void Init(CLDSGeneralPlate *pPlate);
	void Draw(IDrawing *pDrawing);
	void Draw(IDrawingComponent *pComponet);
	void Draw(IDrawingCommand *pCommand);
	void Draw(AcDbBlockTableRecord *pBlockTbl);
private:
	void InitBottomRightHole();
	//���Ʋ�����ͼ
	void CreatePatternDrawing();
	//���Ʋ�����ͼ
	void CreateDetailDrawing();
	void DrawSketch();//������ͼ
	void DrawTube();////�ֹ�
	void DrawDimSize();//��ע�ߴ�
	void DrawPartDetail();//��ע�ı�
	void SideTubeDrawing();//����ͼ
	void AppendPartLabelAndMaterial(f3dPoint dim_pos);//��Ӳ��ʺ��ͺű�ע
};


class CPlateDrawer
{
private:
	IPlateDrawing *m_pPlateDrawing;
	static IPlateDrawing* CreatePlateDrawing(DWORD cls_type_id);
	static bool DestroyPlateDrawing(IPlateDrawing *pPlateDrawing);
public:
	CPlateDrawer();
	~CPlateDrawer(void);
	void Init(CProcessPlate *pProcessPlate);
	void Init(CLDSGeneralPlate *pPlate);
	IPlateDrawing *PlateDrawing(){return m_pPlateDrawing;}
	void SetDrawParam(DRAW_PARAM &draw_param){m_pPlateDrawing->SetDrawParam(draw_param);}
	void Draw(IDrawing *pDrawing);
	void Draw(IDrawingComponent *pComponet);
	void Draw(IDrawingCommand *pCommand);
	void Draw(AcDbBlockTableRecord *pBlockTbl);
};
