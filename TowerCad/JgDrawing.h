// JgDrawing.h: interface for the CJgDrawing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JGDRAWING_H__7471C63E_4F7B_494A_B082_15571080C7D3__INCLUDED_)
#define AFX_JGDRAWING_H__7471C63E_4F7B_494A_B082_15571080C7D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef __DRAWING_CONTEXT_
#include "Tower.h"
#include "TowerType.h"
#else 
#include "ArrayList.h"
#endif
#include "XhCharString.h"
#include "DrawDim.h"
#include "ArrayPtrList.h"
#include "ProcessPart.h"

class CJgDrawingPara
{
public:
	double	fDimTextSize;		//���ȳߴ��ע�ı���
	int iDimPrecision;			//�ߴ羫ȷ��
	double fRealToDraw;			//������ͼ�����ߣ�ʵ�ʳߴ�/��ͼ�ߴ磬��1:20ʱ��fRealToDraw=20
	double	fDimArrowSize;		//�ߴ��ע��ͷ��
	double fTextXFactor;
	double	fPartNoTextSize;	//����������ָ�
	int		iPartNoFrameStyle;	//��ſ����� //0.ԲȦ 1.��Բ���ľ��ο� 2.���ο�	3.�Զ��ж�
	double	fPartNoMargin;		//����������ſ�֮��ļ�϶ֵ 
	double	fPartNoCirD;		//�������Ȧֱ��
	double fPartGuigeTextSize;	//����������ָ�
	int iMatCharPosType;
	char cMatCharSeparator;

	BOOL bModulateLongJg;		//�����Ǹֳ��� ��δʹ�ã���iJgZoomSchema����ñ��� wht 11-05-07
	int iJgZoomSchema;			//�Ǹֻ��Ʒ�����0.1:1���� 1.ʹ�ù���ͼ���� 2.����ͬ������ 3.����ֱ�����
	BOOL bMaxExtendAngleLength;	//����޶�����Ǹֻ��Ƴ���
	BOOL bOneCardMultiPart;		//�Ǹ�����һ��������
	int  iJgGDimStyle;			//0.ʼ�˱�ע  1.�м��ע 2.�Զ��ж�
	int  nMaxBoltNumStartDimG;	//������ʼ�˱�ע׼��֧�ֵ������˨��
	int  iLsSpaceDimStyle;		//0.X�᷽��	  1.Y�᷽��  2.�Զ��ж� 3.����ע  4.�ޱ�ע����(X�᷽��)  4.����ߴ��ߣ��ޱ�ע����(X�᷽��)��Ҫ���ڽ���(��)�����ʼ���ä������
	int  nMaxBoltNumAlongX;		//��X�᷽���ע֧�ֵ������˨����
	BOOL bDimCutAngle;			//��ע�Ǹ��н�
	BOOL bDimCutAngleMap;		//��ע�Ǹ��н�ʾ��ͼ
	BOOL bDimPushFlatMap;		//��עѹ��ʾ��ͼ
	BOOL bJgUseSimpleLsMap;		//�Ǹ�ʹ�ü���˨ͼ��
	BOOL bDimLsAbsoluteDist;	//��ע��˨���Գߴ�
	BOOL bMergeLsAbsoluteDist;	//�ϲ����ڵȾ���˨���Գߴ� �����������:��ʱҲ��Ҫ�� wjh-2014.6.9
	BOOL bDimRibPlatePartNo;	//��ע�Ǹ��߰���
	BOOL bDimRibPlateMaterial;	//��ע�Ǹ��߰����
	BOOL bDimRibPlateSetUpPos;	//��ע�Ǹ��߰尲װλ��
	//�нǱ�ע��ʽһ
	//�нǱ�ע��ʽ�� B:��ͷ�ߴ� L:֫�߳ߴ� C:��߳ߴ� 
	//BXL �н�  CXL ��֫ BXC �н�  �д��=�н�+��֫
	int	 iCutAngleDimType;		//�нǱ�ע��ʽ 0.��ʽһ  1.��ʽ�� wht 10-11-01
	int iCutWingDimType;		//��֫������ע����
	//
	BOOL bDimKaiHe;				//��ע�Ǹֿ��Ͻ�
	BOOL bDimKaiheAngleMap;		//��ע�Ǹֿ��Ͻ�ʾ��ͼ
	double fKaiHeJiaoThreshold; //���ϽǱ�ע��ֵ(��) wht 11-05-06
	double fKaiHeJiaoMinDeita;	//���Ͻ������С��϶ wjh 2015.2.6
	//�������ϽǱ�ע���� wht 12-03-13
	BOOL bDimKaiheSumLen;		//��ע���������ܳ�
	BOOL bDimKaiheAngle;		//��ע���϶���	
	BOOL bDimKaiheSegLen;		//��ע��������ֶγ�
	BOOL bDimKaiheScopeMap;		//��ע���������ʶ��
	//
	BOOL bSplitAngleSketchDrawing;
	CJgDrawingPara();
};

#ifndef __TSA_FILE_
typedef struct tagPART_JG_CARD_INFO
{
	CXhChar16 sSeg;
	CXhChar16 sPartNo;			//����
	CXhChar16 sLength;			//����
	CXhChar16 sDesMaterial;		//��Ʋ���
	CXhChar16 sReplMaterial;	//���ò���
	CXhChar16 sDesGuiGe;		//��ƹ��
	CXhChar16 sReplGuiGe;		//���ù��
	CXhChar16 sPartNum;			//������
	CXhChar16 sSumPartNum;		//�ܼӹ���=������*����
	CXhChar16 sPartWeight;		//����
	CXhChar16 sSumPartWeight;	//����
	CXhChar16 sM12HoleNum;		//M12��˨����
	CXhChar16 sM16HoleNum;		//M16��˨����
	CXhChar16 sM18HoleNum;		//M18��˨����
	CXhChar16 sM20HoleNum;		//M20��˨����
	CXhChar16 sM22HoleNum;		//M22��˨����
	CXhChar16 sM24HoleNum;		//M24��˨����
	CXhChar16 sMXHoleNum;		//������˨����
	CXhChar16 sSumHoleNum;		//�ܿ���
	CXhChar16 sChongKongNum;	//�����
	CXhChar16 sZuanKongNum;		//�����
	CXhChar16 sCutRoot;			//�ٸ�
	CXhChar16 sCutBer;			//����
	CXhChar16 sCutRootLen;		//�ٸ�����	wht 10-11-17
	CXhChar16 sCutBerLen;		//��������
	CXhChar16 sHuoQuFst;		//�״λ���
	CXhChar16 sHuoQuSec;		//���λ���
	CXhChar16 sCutAngSX;		//ʼ��X֫�н�	
	CXhChar16 sCutAngSY;		//ʼ��Y֫�н�
	CXhChar16 sCutAngEX;		//�ն�X֫�н�
	CXhChar16 sCutAngEY;		//�ն�Y֫�н�
	CXhChar50 sCutAngParaSX;	//ʼ��X֫�нǲ�����ʾ
	CXhChar50 sCutAngParaSY;	//ʼ��Y֫�нǲ�����ʾ
	CXhChar50 sCutAngParaEX;	//�ն�X֫�нǲ�����ʾ
	CXhChar50 sCutAngParaEY;	//�ն�Y֫�нǲ�����ʾ
	CXhChar50 sPushFlat;		//ѹ�� ʵʱ���ݱ�ע��ʽΪ:ʼ-Y/220��-Y/230 16���ַ������ù��޸�ΪCXhChar50 wht10-11-03
	CXhChar16 sPushFlatParaSX;	//ʼ��X֫ѹ�������ʾ wht 10-11-03
	CXhChar16 sPushFlatParaSY;	//ʼ��Y֫ѹ�������ʾ
	CXhChar16 sPushFlatParaEX;	//�ն�X֫ѹ�������ʾ
	CXhChar16 sPushFlatParaEY;	//�ն�Y֫ѹ�������ʾ
	CXhChar16 sPushFlatParaMX;	//�м�X֫ѹ�������ʾ
	CXhChar16 sPushFlatParaMY;	//�м�Y֫ѹ�������ʾ
	CXhChar16 sWeld;			//����
	CXhChar16 sKaiJiao;			//����
	CXhChar16 sHeJiao;			//�Ͻ�
	CXhChar16 sWingAngle;		//���ϽǾ������
	CXhChar50 sNotes;			//��ע
	int  m_nCutBerR;			//����Բ���뾶mm,һ���������ĵĽǸ��ڻ�ֵr+2
	int m_nPageNum,m_nPageIndex;//�� ҳ,�� ҳ
	int m_nTaNum;				//����  wht 09-12-31
	CXhChar50 sPrjName;			//��������
	CXhChar50 sTaType;			//����
	CXhChar50 sTaAlias;			//����
	CXhChar50 sTaStampNo;		//��ӡ��
	CXhChar50 sOperator;		//����Ա���Ʊ��ˣ�
	CXhChar50 sAuditor;			//�����
	CXhChar50 sCritic;			//������
	BOOL m_bPunchShear;			//���
	BOOL m_bGasCutting;			//����
	BOOL m_bOneShot;			//һ�����
	//
	tagPART_JG_CARD_INFO() {
		m_bPunchShear = FALSE;
		m_bGasCutting = FALSE;
		m_bOneShot = FALSE;
		m_nCutBerR = m_nPageIndex = m_nPageNum = m_nTaNum = 0;
	}
}PART_JG_CARD_INFO;
typedef struct tagSPECIAL_PROCESS_STAT_CARD_INFO
{
	CXhChar200 sProcessName;
	CXhChar200 sSketchMap;
	int  m_nPageNum,m_nPageIndex; 
}SPECIAL_PROCESS_STAT_CARD_INFO;
//����ֵΪ0��ʾλ��X֫��1��ʾλ��Y֫
//-1��ʾ��˨������Ǹ�֫���߲�ƥ��
#ifdef __DRAWING_CONTEXT_
int ProjectJgLsToMap(UCS_STRU &ucs,CLDSBolt *pLs,f3dPoint norm_X,f3dPoint norm_Y,f3dPoint &prj_pos);
//��PaperPatternDrawing.cpp�ƶ����˴� wht 15-03-18
BOOL CalLsPosInArcPart(CLDSArcPart* pArcPart,CLsRef *pLsRef,f3dPoint& ls_pos);
#endif
int compareBoltPos(const f3dPoint &centre1,const f3dPoint &centre2);
class CRodDrawing  
{
	//��ͳ����һ�����˨(����boltCenterArr��)���з�����ȷ����ͼʱ�����ŷ���
	//�˺���������ȷ������߾�֮�����
	void MakeZoomPlanByBoltsInfo();
	bool IsHasIntersObj(CZhunXianTextDim &zhunDim);
public:
	static const int MAX_G_COUNT = 15;
	struct LsPosMark
	{
		int wingX0Y1Both2;		//��˨��λ����Ӧ��֫
		double realCoordX;		//ʵ��X�������
		double drawCoordX;		//��ͼʱ�Ļ���λ��X�������

		double minPrevDimSpace;	//����ͬ֫ǰһ��ע��С���ƾ���
		double needExtendDist;	//��Ҫ����ļ��ֵ
		double forewardMoveDist;//ֻ���ڼ�¼���һ���׾�ĩ�˱�ע���벻��ʱ��Ҫǰ�ƵĻ��ƾ���
		CSizeTextDim *pDimSpaceSizeX, *pDimSpaceSizeY;
		CTextOnlyDrawing *pDimSpaceTextX, *pDimSpaceTextY;
	};
	typedef struct LsMark
	{
		int wingX0Y1;			//��˨��λ����Ӧ��֫
		int iRow;				//��˨�����������ţ����ڷ��б�ע��˨ wht 05-07
		BYTE cFuncType;			//��˨�׹������� wxc 15-2-13
		BYTE cFlag;				//��˨�׼ӹ�����
		f3dPoint realPos;
		f3dCircle dimCircle;
		CSizeTextDim* pDimSpaceSize;
		CTextOnlyDrawing* pDimSpaceText;
		LsMark(){cFlag=0;cFuncType=0;iRow=1;pDimSpaceSize=NULL;pDimSpaceText=NULL;}
	}*LsMarkPtr;
	struct LsGroup
	{
		int m_nLsNum;
		LsPosMark* lsMarkArr[500];
		int groupTypeStart0End1Mid2;
	};
	static const BYTE CONTEXT_PARTDRAW = 0;	//0.����ͼ����
	static const BYTE CONTEXT_WELDDRAW = 1;	//1.�麸ͼ����
	static const BYTE CONTEXT_STRUDRAW = 2;	//2.�ṹͼ����
	BYTE m_ciResideContext;	//0.����ͼ����;1.�麸ͼ����;2.�ṹͼ����
	static CJgDrawingPara drawPara;//�滻ԭ��CDrawSysPara��Ϣ
	static void InitJgDrawingPara();//��ʼ��CJgDrawingPara
protected:
	CXhChar50 sPartNo;		//CRodDrawing::CreateLsSpaceDimList()�������á�wjh-2015.11.30
	double PRECISION;		//ֻ��ȡ1,0.5,0.1����ֵ
	double leftGDimMargin;	//
	double rightMargin;
	double minDrawingSpace;	//��˨ͼ�������С�����һ������˨ͼ������ʾֱ������
	double WtoH;			//�ַ��߿��
	double maxDrawingLength;
	double maxDrawingHeight;
	double length;			//�Ǹ�ʵ�ʳ���
	double wingWide;		//�Ǹ�֫��
	double wingThick;		//�Ǹ�֫��
	int m_nWingAxisGX;		//X֫��������
	int m_nWingAxisGY;		//Y֫��������
	int wingAxisGArrX[MAX_G_COUNT];	//X֫���ߵ��ľ�����
	int wingAxisGArrY[MAX_G_COUNT];	//Y֫���ߵ��ľ�����
	double wingAxisCoordArrX[MAX_G_COUNT];	//X֫���ߵĻ���λ��Y����
	double wingAxisCoordArrY[MAX_G_COUNT];	//Y֫���ߵĻ���λ��Y����
	double m_fLengthScaleCoef;	//���ǿ׾��ע�����ű���
	double m_fVertScaleCoef;	//֫����(Y��)�����ű���
	//UCS_STRU angleDrawUcs;
	//�Ǹ�������
	CArray <f3dLine,f3dLine&>edgeLineArr;	//���ȷ������
	CArray<f3dLine,f3dLine&>lineArr;//�Ǹ�ʾ���ͼ�г������������ȷ�������������ͼԪ
	//�Ǹ��ϵ���˨��
	ARRAY_LIST<LsMarkPtr>LsCenterArr;	//�Ǹ���������˨��
	ARRAY_LIST<LsPosMark>LsPosArr;	//��˨��λ(���ȷ���)��(˫֫�ۼ�)
	ARRAYPTR_LIST<LsMark>LsCirX;				//��Ӧ��ʵ����˨,���ڻ�ͼʱ��ע��˨ͼ��
	ARRAYPTR_LIST<LsMark>LsCirY;				//��Ӧ��ʵ����˨,���ڻ�ͼʱ��ע��˨ͼ��
	//CArray<f3dPoint,f3dPoint&>LsCenterArr;	//�Ǹ���������˨��
	//CArray<LsPosMark,LsPosMark&>LsPosArr;	//��˨��λ(���ȷ���)��(˫֫�ۼ�)
	//CArray<LsMark,LsMark&>LsCirX;			//��Ӧ��ʵ����˨,���ڻ�ͼʱ��ע��˨ͼ��
	//CArray<LsMark,LsMark&>LsCirY;			//��Ӧ��ʵ����˨,���ڻ�ͼʱ��ע��˨ͼ��
	//��ͼ�еĸ����ע
	//CArray<CSizeTextDim, CSizeTextDim&>dim_arr;
	//CArray<CTextOnlyDrawing,CTextOnlyDrawing&>pure_txt_arr;
	ARRAYPTR_LIST<CSizeTextDim>dim_arr;
	ARRAYPTR_LIST<CTextOnlyDrawing>pure_txt_arr;
	CArray<CZhunXianTextDim,CZhunXianTextDim&>zhun_dim_arr;
protected:
	//�����жνǸֵ���˨ͳ����һ��,
	void StatBoltsInfo(ATOM_LIST<BOLT_INFO> &bolt_list,long mid_flat_pos_z=0);
	void CreateLsSpaceDimList();
	void InitWingAxisCoordY();
	void CalDrawingScaleCoef(bool bEndCutAngleOrWing,bool bStartCutAngleOrWing);
	double TransCoordX(double realPosX);
	double TransCoordY(double realPosY);
	BOOL DimLsZhun(f3dPoint ls_prj_pos);
	double TransCutWingOrAngleDrawDist(double dist);
	void CreateJgLsCircleList();
	virtual SCOPE_STRU RelayoutDrawing();
	void InitDimStyleBySysSetting(int nBoltNum);
	void CreateLsFuncTextDim();
	//��ע����趨
public:
	MASKFLAG dimStyleFlag;
	static DWORD DimSpaceTextAlongX;	//0x00 0000 01:00000000 0000000000000000 00000001
	static DWORD DimSpaceTextAlongY;	//0x80 0000 01:10000000 0000000000000000 00000001
	static DWORD DimPosTextByOneLayer;	//0x00 0000 02:00000000 0000000000000000 00000010
	static DWORD DimPosTextByMultiLayer;//0x40 0000 02:01000000 0000000000000000 00000010
	static DWORD DimAxisGTextAtStart;	//0x00 0000 04:00000000 0000000000000000 00000100
	static DWORD DimAxisGTextAtMiddle;	//0x20 0000 04:00100000 0000000000000000 00000100
	static DWORD CutAngleDimByImage;	//0x00 0000 08:00000000 0000000000000000 00001000
	static DWORD CutAngleDimByText;		//0x10 0000 08:00010000 0000000000000000 00001000
	static DWORD BriefDrawing;			//0x00 0001 00:00000000 0000000000000001 00000000 ���ƽǸּ�ͼ
	static DWORD OneCardMultiPart;		//0x00 0002 00:00000000 0000000000000010 00000000 һ�����
	static DWORD ModulateLongJg;		//0x00 0004 00:00000000 0000000000000100 00000000 �����Ǹֳ���
	static DWORD SpecifyDrawscale;		//0x00 0008 00:00000000 0000000000001000 00000000 ָ����ͼ����
	//�����������ʹ�������н�С�ı���(�����ñ�֤�Ǹ��������ͼ����) wht 11-05-07
	static DWORD UseMinDrawscale;		//0x00 0010 00:00000000 0000000000010000 00000000 
public:
	f2dRect GetDrawRegion(double PART_MAP_SCALE);
	CRodDrawing();
	virtual ~CRodDrawing();
	int GetZEdgeCount();
	int  GetLineCount();
	int GetDimCount();
	int GetZhunDimCount();
	int GetXLsCount();
	int GetYLsCount();
	int GetPureTxtCount();
	double GetWingWide(){return wingWide;}
	double GetWingThick(){return wingThick;}
	BOOL GetZEdge(f3dLine &line,int index);
	BOOL GetLineAt(f3dLine &line,int index);
	BOOL GetXLsCircle(f3dCircle &Cir,int index);
	BOOL GetYLsCircle(f3dCircle &Cir,int index);
	BOOL GetDimAt(int index,CSizeTextDim *pDim);
	BOOL GetZhunDimAt(int index,CZhunXianTextDim *pDim);
	BOOL GetPureTxtDimAt(int index,CTextOnlyDrawing *pDim);
	void SetLengthScaleCoef(double scale){ m_fLengthScaleCoef=scale; }	//���ǿ׾��ע�����ű���
	void SetVertScaleCoef(double scale){ m_fVertScaleCoef=scale;  }		//֫����(Y��)�����ű���

	virtual int GetRibPlateDimCount(){return 0;} 
	virtual int GetKaiHeDimCount(){return 0;}
	virtual int GetPartNoCount(){return 0;}
	virtual BOOL GetRibPlateDimAt(int index,CSizeTextDim* pDim){return FALSE;}
	virtual BOOL GetKaiHeDimAt(int index,CKaiHeAngleDim *pDim){return FALSE;}
	virtual BOOL GetPartNoDimAt(int index,CTextFrame *pDim){return FALSE;}
	virtual BOOL IsPolyAngleDrawing(){return FALSE;}
};

class CAngleDrawing : public CRodDrawing
{
protected:
	ARRAY_LIST<int> rib_thick_arr;		//�߰���
	ARRAYPTR_LIST<CSizeTextDim> rib_dim_arr;
	ARRAYPTR_LIST<LsMark> rib_mark_arr;		//�߰�λ�ñ��
	CArray<CTextFrame, CTextFrame&>part_no_arr;	//�߰���
	CArray<CKaiHeAngleDim,CKaiHeAngleDim&>kaihe_angle_dim_arr;
protected:
	double GetBoltMaxY(int wingX0Y1);
	BOOL FindNearLsMark(int *piWingX0Y1,double realCoordX,BOOL bLeft,LsMark* pOutBoltMark);
	BOOL IsLsMarkPosOverlap(f3dPoint ls_pos);
	void CreateJgRibPlateDimList(ATOM_LIST<RIB_PLATE_INFO> &rib_plate_list);
	void CreateJgRibPlateEdge(ATOM_LIST<RIB_PLATE_INFO> &rib_plate_list);
	void CreateJgProfile(double cut_wing_para[2][3],long cut_wing[2],double cut_angle[4][2],double wingWide,
		double wingThick,BOOL bIncStartDim/*=TRUE*/,BOOL bIncEndDim/*=TRUE*/);
#ifdef  __DRAWING_CONTEXT_
	BOOL ProcessKaiHeAngle(CLDSLineAngle *pJg,UCS_STRU &angleUcs,BOOL bUcsIncStartOdd);
#endif
	BOOL ProcessKaiHeAngle(CProcessAngle *pJg);
	virtual SCOPE_STRU RelayoutDrawing();
public:
	CAngleDrawing();
	virtual ~CAngleDrawing();
	int GetRibPlateDimCount();
	int GetKaiHeDimCount();
	int GetPartNoCount();
	BOOL GetRibPlateDimAt(int index,CSizeTextDim* pDim);
	BOOL GetKaiHeDimAt(int index,CKaiHeAngleDim *pDim);
	BOOL GetPartNoDimAt(int index,CTextFrame *pDim);
	BOOL GetMidFlatPos(f3dPoint &startPos,f3dPoint &endPos,f3dPoint &midPos,int wing_x0_y1);
public:	//��ע��ʽ���ָ��趨
	DECLARE_READONLY_PROPERTY(double,fDimTextSize);			//���ȳߴ��ע�ı���
	DECLARE_READONLY_PROPERTY(double,fDimAngleTextSize);	//�Ƕȳߴ��ע�ı���
	DECLARE_READONLY_PROPERTY(double,fDimLabelTextSize);	//����������ָ�
	DECLARE_READONLY_PROPERTY(double,fDimSizeSpecTextSize);	//����������ָ�
};

class CLineAngleDrawing : public CAngleDrawing
{
public:
#ifdef __DRAWING_CONTEXT_
	BOOL CreateLineAngleDrawing(CLDSLineAngle *pJg,double maxDrawingLength,double maxDrawingHeight,
		double textHeight=2.5,double WtoH=0.7,double minSpace=3.5);
#endif
	BOOL CreateLineAngleDrawing(CProcessAngle *pAngle,double max_drawing_length,double max_drawing_height,
		double text_height=2.5,double w_to_h=0.7,double min_space=3.5);
};

struct HUOQU_LINE
{
	f3dPoint startPt, endPt;
	int huoqu_type;		//0.X֫����  1.X֫����  2.Y֫����  3.Y֫����
	double huoqu_angle;
};
struct ANGLE_DIM_STRU
{
	f3dPoint vertex;
	f3dPoint startPos;
	f3dPoint endPos;
	f3dPoint dimPos;
	double angle;
	double fRadius;
	CXhChar16 sDimText;
	ANGLE_DIM_STRU() { angle = 0; fRadius = 0; sDimText.Copy("<>"); }
};
class CPolyAngleDrawing : public CAngleDrawing
{
	CArray<HUOQU_LINE, HUOQU_LINE&>huoqu_line;//������
#ifdef  __DRAWING_CONTEXT_
	CArray<CLDSLineAngle, CLDSLineAngle&>segArr; //�ֶνǸ�
#endif
	ARRAYPTR_LIST<CTextOnlyDrawing> huoqu_txt_arr;	//������Ϣ
	CArray<ANGLE_DIM_STRU, ANGLE_DIM_STRU&>angle_dim_arr;
protected:
	virtual SCOPE_STRU RelayoutDrawing();
public:
	CPolyAngleDrawing() { ; }
	virtual ~CPolyAngleDrawing() { ; }
	virtual BOOL IsPolyAngleDrawing() { return TRUE; }
#ifdef  __DRAWING_CONTEXT_
	BOOL CreatePolyAngleDrawing(CLDSPolyPart *pHuoQuJg, double maxDrawingLength, double maxDrawingHeight,
		double textHeight = 2.5, double WtoH = 0.7, double minSpace = 3.5);
	BOOL CreatePolyAngleBendDrawing(CLDSPolyPart *pHuoQuJg);
#endif
	int GetHuoQuLineCount() { return (int)huoqu_line.GetSize(); }
	BOOL GetHuoQuLine(f3dLine &line, int index);
#ifdef  __DRAWING_CONTEXT_
	int GetSegAngleCount() { return (int)segArr.GetSize(); }
	CLDSLineAngle* GetSegAngle(int index) { return &segArr[index]; }
#endif
	HUOQU_LINE GetHuoQuLine(int index) { return huoqu_line[index]; }
	int GetHuoQuTextCount() { return huoqu_txt_arr.GetNodeNum(); }
	CTextOnlyDrawing* GetHuoQuTxtDimAt(int index) { return &huoqu_txt_arr[index]; }
	int GetAngleDimCount() { return angle_dim_arr.GetSize(); }
	void GetAngleDimAt(int index, ANGLE_DIM_STRU* pAngleDim) { memcpy(pAngleDim, &angle_dim_arr[index], sizeof(ANGLE_DIM_STRU)); }
};

class CSlotDrawing : public CRodDrawing
{
	double wingWide2;		//֫��2
	void CreateProfile();
public:
	CSlotDrawing();
	~CSlotDrawing();
#ifdef __DRAWING_CONTEXT_
	BOOL CreateSlotDrawing(CLDSPart *pSlot,double maxDrawingLength,double maxDrawingHeight,
		double textHeight=2.5,double WtoH=0.7,double minSpace=3.5);
#endif
};

class CFlatDrawing : public CRodDrawing
{
	void CreateProfile();
public:
	CFlatDrawing();
	~CFlatDrawing();
#ifdef __DRAWING_CONTEXT_
	BOOL CreateFlatDrawing(CLDSPart *pFlat,double maxDrawingLength,double maxDrawingHeight,
		double textHeight=2.5,double WtoH=0.7,double minSpace=3.5);
#endif
};
#endif
#endif // !defined(AFX_JGDRAWING_H__7471C63E_4F7B_494A_B082_15571080C7D3__INCLUDED_)
