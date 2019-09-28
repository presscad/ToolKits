// JgDrawing.cpp: implementation of the CAngleDrawing class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Query.h"
#ifdef __DRAWING_CONTEXT_
#include "GlobalFunc.h"
#include "SysPara.h"
#endif
#include "SortFunc.h"
#include "DistFillet.h"
#include ".\JgDrawing.h"
#include "PartLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef __TSA_FILE_
//////////////////////////////////////////////////////////////////////
// ȫ�ֺ���
//////////////////////////////////////////////////////////////////////
//����ֵΪ0��ʾλ��X֫��1��ʾλ��Y֫
//-1��ʾ��˨������Ǹ�֫���߲�ƥ��
#ifdef __DRAWING_CONTEXT_
int ProjectJgLsToMap(UCS_STRU &ucs,CLDSBolt *pLs,f3dPoint norm_X,f3dPoint norm_Y,f3dPoint &prj_pos)
{
	f3dPoint basePt;
	basePt = pLs->ucs.origin;
	//�Ǹִ��ڿ��Ͻ��ҿ��ϽǶ���������Сʱ��ʹ��0.707�жϲ�׼ȷ wht 10-11-29
	double ddx=dot_prod(norm_X,pLs->get_norm());
	double ddy=dot_prod(norm_Y,pLs->get_norm());
	if(fabs(ddx)>fabs(ddy)&&fabs(ddx)>0.707)
	{
		Int3dlf(basePt,basePt,pLs->get_norm(),ucs.origin,norm_X);
		coord_trans(basePt,ucs,FALSE);
		prj_pos.Set(basePt.z,-basePt.x,0);
		return 0;
	}
	else if(fabs(ddy)>0.707)
	{
		Int3dlf(basePt,basePt,pLs->get_norm(),ucs.origin,norm_Y);
		coord_trans(basePt,ucs,FALSE);
		prj_pos.Set(basePt.z,basePt.y,0);
		return 1;
	}
	else
		return -1;
}
//������˨�ڻ��ι����ϵ�λ��
BOOL CalLsPosInArcPart(CLDSArcPart* pArcPart,CLsRef *pLsRef,f3dPoint& ls_pos)
{
	if(pArcPart==NULL || pLsRef==NULL)
		return FALSE;
	CLDSBolt* pBolt=pLsRef->GetLsPtr();
	UCS_STRU ucs;
	pArcPart->getUCS(ucs);
	//������˨����λ�õ�Բ�Ľ�
	f3dPoint inters,vec;
	Int3dlf(inters,pBolt->ucs.origin,ucs.axis_z,ucs.origin,ucs.axis_z);
	vec=inters-ucs.origin;
	normalize(vec);
	double angle=CalRotateAngle(ucs.axis_z,ucs.axis_x,vec);
	angle+=pArcPart->start_oddment/pArcPart->baseline.Radius();
	if(angle<0)
		angle+=2*Pi;
	//������˨����
	if(pArcPart->GetClassTypeId()==CLS_ARCANGLE)
	{
		CLDSArcAngle* pArcAngle=(CLDSArcAngle*)pArcPart;
		ls_pos.x=pArcAngle->baseline.Radius()*angle;
		if(fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_x)&&
			fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_y))
		{	//X֫:
			Int3dlf(inters,pBolt->ucs.origin,ucs.axis_z,ucs.origin,ucs.axis_z);
			double dist=DISTANCE(inters,ucs.origin);
			ls_pos.y=-fabs(dist-pArcAngle->baseline.Radius()-pArcAngle->offset_h);
		}
		else	
		{	//Y֫:
			f3dPoint pt=pBolt->ucs.origin;
			coord_trans(pt,ucs,FALSE);
			ls_pos.y=fabs(pt.z-pArcAngle->offset_v);
		}
	}
	else if(pArcPart->GetClassTypeId()==CLS_ARCFLAT)
	{
		CLDSArcFlat* pArcFlat=(CLDSArcFlat*)pArcPart;
		ls_pos.x=pArcFlat->baseline.Radius()*angle;
		inters=pBolt->ucs.origin;
		coord_trans(inters,ucs,FALSE);
		ls_pos.y=inters.z-pArcFlat->offset_v-pArcFlat->wide*0.5;
	}
	else if(pArcPart->GetClassTypeId()==CLS_ARCSLOT)
	{
		CLDSArcSlot* pArcSlot=(CLDSArcSlot*)pArcPart;
		ls_pos.x=pArcSlot->baseline.Radius()*angle;
		ls_pos.z=0;
		if(pArcSlot->m_siLayoutStyle==0 || pArcSlot->m_siLayoutStyle==2)
		{
			if(fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_x)&&
				fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_y))
			{	//��˨λ�ڲ۸ֲ��
				f3dPoint pick,intersUp,intersDown;
				pick=ucs.origin+ucs.axis_z*pArcSlot->offset_v;
				Int3dlf(inters,pBolt->ucs.origin,ucs.axis_z,pick,ucs.axis_z);
				double radius=pArcSlot->baseline.Radius();
				double dist=fabs(DISTANCE(inters,ucs.origin)-radius-pArcSlot->offset_h);
				//�ж���˨����һ��
				if(pArcSlot->m_siLayoutStyle==0)
				{
					intersUp=inters;
					pick=pick-ucs.axis_z*pArcSlot->height;
					Int3dlf(intersDown,pBolt->ucs.origin,ucs.axis_z,pick,ucs.axis_z);
				}
				else
				{
					intersDown=inters;
					pick=pick+ucs.axis_z*pArcSlot->height;
					Int3dlf(intersUp,pBolt->ucs.origin,ucs.axis_z,pick,ucs.axis_z);
				}
				if(DISTANCE(intersUp,pBolt->ucs.origin)<DISTANCE(intersDown,pBolt->ucs.origin))
					ls_pos.y=-pArcSlot->height*0.5-dist;
				else
					ls_pos.y=pArcSlot->height*0.5+dist;
			}
			else	//��˨λ�ڲ۸ֵױ�
			{
				f3dPoint pt=pBolt->ucs.origin;
				coord_trans(pt,ucs,FALSE);
				if(pArcSlot->m_siLayoutStyle==0)
					ls_pos.y=pArcSlot->height*0.5-fabs((pt.z-pArcSlot->offset_v));
				else
					ls_pos.y=fabs((pt.z-pArcSlot->offset_v))-pArcSlot->height*0.5;
			}
		}
		else if(pArcSlot->m_siLayoutStyle==1 || pArcSlot->m_siLayoutStyle==3)
		{
			if(fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_x)&&
				fabs(pBolt->ucs.axis_z*ucs.axis_z)>fabs(pBolt->ucs.axis_z*ucs.axis_y))
			{	//��˨λ�ڲ۸ֵĵױ�
				f3dPoint inters;
				Int3dlf(inters,pBolt->ucs.origin,ucs.axis_z,ucs.origin,ucs.axis_z);
				double dist=DISTANCE(inters,ucs.origin)-pArcSlot->baseline.Radius()-

					pArcSlot->offset_h;
				ls_pos.y=dist-pArcSlot->height*0.5;
			}
			else	//��˨λ�ڲ۸ֲ��
			{
				f3dPoint pt,inters,pick;
				pt=pBolt->ucs.origin;
				coord_trans(pt,ucs,FALSE);
				vec.Set(pt.x,pt.y,0);
				pick=ucs.origin+ucs.axis_z*pArcSlot->offset_v+vec*pArcSlot->baseline.Radius()+pArcSlot->offset_h;
				Int3dlf(inters,pt,ucs.axis_z,pick,ucs.axis_z);
				f3dPoint intersUp,intersDown;
				if(pArcSlot->m_siLayoutStyle==1)
				{
					intersDown=inters;
					pick=pick+vec*pArcSlot->height;
					Int3dlf(intersUp,pt,ucs.axis_z,pick,ucs.axis_z);
				}
				else
				{
					intersDown=inters;
					pick=pick-vec*pArcSlot->height;
					Int3dlf(intersDown,pt,ucs.axis_z,pick,ucs.axis_z);
				}
				if(DISTANCE(intersUp,pt)>DISTANCE(intersDown,pt))
					ls_pos.y=-fabs(pt.z-pArcSlot->offset_v)-pArcSlot->height*0.5;
				else
					ls_pos.y=fabs(pt.z-pArcSlot->offset_v)+pArcSlot->height*0.5;
			}
		}
	}
	return TRUE;
}
#endif
int compareBoltPos(const f3dPoint &centre1,const f3dPoint &centre2)
{
	if(centre1.x>centre2.x)
		return 1;
	else if(centre1.x<centre2.x)
		return -1;
	else
		return 0;
}
static int compareLsMark(const CRodDrawing::LsMarkPtr &ls1,const CRodDrawing::LsMarkPtr &ls2)
{
	if(ls1->realPos.x>ls2->realPos.x)
		return 1;
	else if(ls1->realPos.x<ls2->realPos.x)
		return -1;
	else
		return 0;
}
int compareZhunXianDim(const CZhunXianTextDim &zhun1,const CZhunXianTextDim &zhun2)
{
	if(zhun1.dist<0&&zhun2.dist>0)
		return -1;
	else if(zhun1.dist>0&&zhun2.dist<0)
		return 1;
	else //ͬ�űȽϾ���ֵ
	{
		if(fabs(zhun1.dist)>fabs(zhun2.dist))
			return 1;
		else if(fabs(zhun1.dist)<fabs(zhun2.dist))
			return -1;
		else
			return 0;
	}
}
int compareKaiHeDim(const CKaiHeAngleDim &dim1,const CKaiHeAngleDim &dim2)
{
	if(dim1.dimStartPos.x>dim2.dimStartPos.x)
		return 1;
	else if(dim1.dimStartPos.x<dim2.dimStartPos.x)
		return -1;
	else
		return 0;
}

CJgDrawingPara::CJgDrawingPara()
{
	fDimTextSize =2.5;		
	iDimPrecision =0;			
	fRealToDraw=10;			
	fDimArrowSize=1.5;		
	fTextXFactor=0.7; 
	fPartNoTextSize=3.0;
	iPartNoFrameStyle=3;	
	fPartNoMargin=0.3;
	fPartNoCirD=8.0;
	fPartGuigeTextSize=3.0;
	iMatCharPosType=0;
	cMatCharSeparator=0;
	//�Ǹֹ���ͼ
	iJgZoomSchema=3;			//0.1:1���� 1.ʹ�ù���ͼ���� 2.����ͬ������ 3.����ֱ�����
	bModulateLongJg=TRUE;		//�����Ǹֳ���
	bOneCardMultiPart=FALSE;	//�Ǹ�����һ��������
	iJgGDimStyle=2;				//0.ʼ�˱�ע  1.�м��ע 2.�Զ��ж�
	bMaxExtendAngleLength=TRUE;
	nMaxBoltNumStartDimG=100;	//������ʼ�˱�ע׼��֧�ֵ������˨��
	iLsSpaceDimStyle=2;			//0.X�᷽��	  1.Y�᷽��  2.�Զ��ж� 3.����ע
	nMaxBoltNumAlongX=50;		//��X�᷽���ע֧�ֵ������˨����
	bDimCutAngle=TRUE;			//��ע�Ǹ��н�
	bDimCutAngleMap=TRUE;		//��ע�Ǹ��н�ʾ��ͼ
	bDimPushFlatMap=TRUE;		//��עѹ��ʾ��ͼ
	bJgUseSimpleLsMap=FALSE;	//�Ǹ�ʹ�ü���˨ͼ��
	bDimLsAbsoluteDist=TRUE;	//��ע��˨���Գߴ�
	bMergeLsAbsoluteDist=TRUE;	//�ϲ����ڵȾ���˨���Գߴ�
	bDimRibPlatePartNo=TRUE;	//��ע�Ǹ��߰���
	bDimRibPlateSetUpPos=TRUE;	//��ע�Ǹ��߰尲װλ��
	iCutAngleDimType=0;			//�Ǹ��нǱ�ע��ʽ 
	//���ϽǱ�ע
	bDimKaiHe=TRUE;				//��ע�Ǹֿ��Ͻ�
	bDimKaiheAngleMap=TRUE;		//��ע�Ǹֿ��Ͻ�ʾ��ͼ
	fKaiHeJiaoThreshold=2;		//Ĭ�Ͽ��ϽǱ�ע��ֵΪ2��
	fKaiHeJiaoMinDeita=20;		//���Ͻ������С��϶ wjh 2015.2.6
	bDimKaiheSumLen=TRUE;
	bDimKaiheAngle=TRUE;
	bDimKaiheSegLen=TRUE;
	bDimKaiheScopeMap=FALSE;
	bSplitAngleSketchDrawing=FALSE;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD CRodDrawing::DimSpaceTextAlongX		= 0x00000001;//:00000000 0000000000000000 00000001
DWORD CRodDrawing::DimSpaceTextAlongY		= 0x80000001;//:10000000 0000000000000000 00000001
DWORD CRodDrawing::DimPosTextByOneLayer		= 0x00000002;//:00000000 0000000000000000 00000010
DWORD CRodDrawing::DimPosTextByMultiLayer	= 0x40000002;//:01000000 0000000000000000 00000010
DWORD CRodDrawing::DimAxisGTextAtStart		= 0x00000004;//:00000000 0000000000000000 00000100
DWORD CRodDrawing::DimAxisGTextAtMiddle		= 0x20000004;//:00100000 0000000000000000 00000100
DWORD CRodDrawing::CutAngleDimByImage		= 0x00000008;//:00000000 0000000000000000 00001000
DWORD CRodDrawing::CutAngleDimByText		= 0x10000008;//:00010000 0000000000000000 00001000
DWORD CRodDrawing::BriefDrawing				= 0x00000100;//:00000000 0000000000000001 00000000
DWORD CRodDrawing::OneCardMultiPart			= 0x00000200;//:00000000 0000000000000010 00000000
DWORD CRodDrawing::ModulateLongJg			= 0x00000400;//:00000000 0000000000000100 00000000 
DWORD CRodDrawing::SpecifyDrawscale			= 0x00000800;//:00000000 0000000000001000 00000000 //wht 11-04-20
//�����������ʹ�������н�С�ı���(�����ñ�֤�Ǹ��������ͼ����) wht 11-05-07
DWORD CRodDrawing::UseMinDrawscale			= 0x00001000;//:00000000 0000000000010000 00000000
CJgDrawingPara CRodDrawing::drawPara;
void CRodDrawing::InitJgDrawingPara()
{
#ifdef __DRAWING_CONTEXT_
	sys.fonts.InitSpecialProp(4);
	drawPara.iLsSpaceDimStyle=sys.part_map.angle.iLsSpaceDimStyle;		
	drawPara.nMaxBoltNumAlongX=sys.part_map.angle.nMaxBoltNumAlongX;		
	drawPara.iJgGDimStyle =sys.part_map.angle.iJgGDimStyle;			
	drawPara.iJgZoomSchema=sys.part_map.angle.iJgZoomSchema;			
	drawPara.bDimRibPlateSetUpPos=sys.part_map.angle.bDimRibPlateSetUpPos;	
	//drawPara.fDimTextSize =sys.dim_map.fDimTextSize;
	//drawPara.iDimPrecision =sys.general_set.iDimPrecision;			
	//drawPara.fKaiHeJiaoThreshold =sys.part_map.angle.fKaiHeJiaoThreshold; 
	//drawPara.bMaxExtendAngleLength=sys.part_map.angle.bMaxExtendAngleLength;	
	//drawPara.fRealToDraw=sys.general_set.PartMapScale.fRealToDraw;			
	//drawPara.nMaxBoltNumStartDimG=sys.part_map.angle.nMaxBoltNumStartDimG;
	//drawPara.fPartNoTextSize=sys.dim_map.PartNo.fPartNoTextSize;
	//drawPara.iPartNoFrameStyle=sys.dim_map.PartNo.iPartNoFrameStyle;
	//drawPara.fPartNoCirD=sys.dim_map.PartNo.fPartNoCirD;
	//drawPara.fDimArrowSize=1.5;		
	//drawPara.fTextXFactor=0.7; 

	//����ͼ��ע�����С��ʼ��
	drawPara.fDimTextSize =sys.fonts.angle.fDimTextSize;		
	drawPara.fPartNoTextSize=sys.fonts.angle.fPartNoTextSize;
	drawPara.fPartGuigeTextSize=sys.fonts.angle.fPartGuigeTextSize;

	drawPara.iDimPrecision =sys.general_set.iDimPrecision;			
	drawPara.fRealToDraw=sys.general_set.PartMapScale.fRealToDraw;	
	drawPara.fDimArrowSize=sys.dim_map.fDimArrowSize;		
	drawPara.fTextXFactor=sys.TEXT_X_FACTOR(); 
	drawPara.iPartNoFrameStyle=sys.dim_map.PartNo.iPartNoFrameStyle;	
	drawPara.fPartNoMargin=sys.dim_map.PartNo.fPartNoMargin;
	drawPara.fPartNoCirD=sys.dim_map.PartNo.fPartNoCirD;
	drawPara.iMatCharPosType=sys.general_set.iMatCharPosType;
	drawPara.cMatCharSeparator=sys.general_set.cMatCharSeparator;
	//�Ǹֹ���ͼ
	drawPara.iJgZoomSchema=sys.part_map.angle.iJgZoomSchema;
	drawPara.bModulateLongJg=sys.part_map.angle.bModulateLongJg;		
	drawPara.bOneCardMultiPart=sys.part_map.angle.bOneCardMultiPart;	
	drawPara.iJgGDimStyle=sys.part_map.angle.iJgGDimStyle;			
	drawPara.bMaxExtendAngleLength=sys.part_map.angle.bMaxExtendAngleLength;
	drawPara.nMaxBoltNumStartDimG=sys.part_map.angle.nMaxBoltNumStartDimG;	
	drawPara.iLsSpaceDimStyle=sys.part_map.angle.iLsSpaceDimStyle;		
	drawPara.nMaxBoltNumAlongX=sys.part_map.angle.nMaxBoltNumAlongX;		
	drawPara.bDimCutAngle=sys.part_map.angle.bDimCutAngle;			
	drawPara.bDimCutAngleMap=sys.part_map.angle.bDimCutAngleMap;		
	drawPara.bDimPushFlatMap=sys.part_map.angle.bDimPushFlatMap;		
	drawPara.bJgUseSimpleLsMap=sys.part_map.angle.bJgUseSimpleLsMap;	
	drawPara.bDimLsAbsoluteDist=sys.part_map.angle.bDimLsAbsoluteDist;	
	drawPara.bMergeLsAbsoluteDist=sys.part_map.angle.bMergeLsAbsoluteDist;	
	drawPara.bDimRibPlatePartNo=sys.part_map.angle.bDimRibPlatePartNo;
	drawPara.bDimRibPlateMaterial=sys.part_map.angle.bDimRibPlateMaterial;
	drawPara.bDimRibPlateSetUpPos=sys.part_map.angle.bDimRibPlateSetUpPos;	
	drawPara.iCutAngleDimType=sys.part_map.angle.iCutAngleDimType;
	drawPara.iCutWingDimType=sys.part_map.angle.iCutWingDimType;
	//���ϽǱ�ע
	drawPara.bDimKaiHe=sys.part_map.angle.bDimKaiHe;
	drawPara.bDimKaiheAngleMap=sys.part_map.angle.bDimKaiheAngleMap;
	drawPara.fKaiHeJiaoThreshold=sys.part_map.angle.fKaiHeJiaoThreshold;		
	drawPara.fKaiHeJiaoMinDeita =sys.part_map.angle.fKaiHeJiaoMinDeita; 
	drawPara.bDimKaiheSumLen=sys.part_map.angle.bDimKaiheSumLen;
	drawPara.bDimKaiheAngle=sys.part_map.angle.bDimKaiheAngle;
	drawPara.bDimKaiheSegLen=sys.part_map.angle.bDimKaiheSegLen;
	drawPara.bDimKaiheScopeMap=sys.part_map.angle.bDimKaiheScopeMap;
	drawPara.bSplitAngleSketchDrawing=sys.customize.bSplitAngleSketchDrawing;
#endif
}
CRodDrawing::CRodDrawing()
{
	PRECISION=1.0;
	length = 0;
	wingWide = wingThick = 0;
	m_fLengthScaleCoef=m_fVertScaleCoef=1.0;
	WtoH=0.7;
	minDrawingSpace=3.5;
	leftGDimMargin=5;	//�����Ҫ���ľ�Ԥ���߾�5mm
	rightMargin=5;		//�Ҳ�Ԥ���߾�5mm
	m_nWingAxisGX=m_nWingAxisGY=0;

	dimStyleFlag.SetMaskBitCount(8);
	dimStyleFlag.SetFlagDword(DimSpaceTextAlongX|DimPosTextByOneLayer|CutAngleDimByText|DimAxisGTextAtStart|DimSpaceTextAlongY);
	LsPosArr.SetSize(0,20);
	LsCenterArr.SetSize(0,20);
	LsCirX.SetSize(0,20);
	LsCirY.SetSize(0,20);
	dim_arr.SetSize(0,20);
	pure_txt_arr.SetSize(0,20);
	m_ciResideContext=0;
	InitJgDrawingPara();
}

CRodDrawing::~CRodDrawing()
{

}

//����ʵ��Xֵ(�ӽǸ���ʼ�˼���(��������ͷ)),���������ĳ��ȷ���λ��ֵ
double CRodDrawing::TransCoordX(double realPosX)
{
	int i;
	if(realPosX<=0)
		return 0;
	if(!dimStyleFlag.IsHasFlag(ModulateLongJg)
		&&!dimStyleFlag.IsHasFlag(CRodDrawing::SpecifyDrawscale))	//��������Ǹֳ���
		return realPosX;
	if(LsPosArr.GetSize()<=0)	//����˨CArray����GetSize��GetCount������ͬ,��GetCount��һЩ�ϰ汾�в�֧��,�ʸ�ΪGetSize wjh-2011.9.26
		return realPosX*m_fLengthScaleCoef;
	for(i=0;i<LsPosArr.GetSize();i++)
	{
		if(realPosX<=LsPosArr[i].realCoordX)
		{	//��ֵ����ʵ��λ�ö�Ӧ�Ļ�ͼλ��
			if(i>0)
				return LsPosArr[i-1].drawCoordX+(LsPosArr[i].drawCoordX-LsPosArr[i-1].drawCoordX)*(realPosX-LsPosArr[i-1].realCoordX)/(LsPosArr[i].realCoordX-LsPosArr[i-1].realCoordX);
			else
				return LsPosArr[i].drawCoordX*(realPosX/LsPosArr[i].realCoordX);
		}
	}
	if(i==LsPosArr.GetSize())
	{
		double endRealSpace=length-LsPosArr[i-1].realCoordX;
		double endDrawSpace=length*m_fLengthScaleCoef-LsPosArr[i-1].drawCoordX;
		double draw2real_scale=endDrawSpace/endRealSpace;
		if(draw2real_scale<0)
			draw2real_scale=0.021*drawPara.fDimTextSize;	//0.021=0.7*3/100
		return LsPosArr[i-1].drawCoordX+draw2real_scale*(realPosX-LsPosArr[i-1].realCoordX);
	}
	else
		return 0;
}
//����ʵ��Yֵ,����������Y����λ��ֵ
double CRodDrawing::TransCoordY(double realPosY)
{
	if(!dimStyleFlag.IsHasFlag(ModulateLongJg))
		return realPosY*m_fVertScaleCoef;					//��Ҫ��Ǹֳ��ȵ���
	int i;
	if(realPosY<0)	//λ��X֫��
	{
		if(m_nWingAxisGX<=0)
			return realPosY*m_fVertScaleCoef;
		else if(realPosY>-wingAxisGArrX[0])
			return realPosY*wingAxisCoordArrX[0]/-wingAxisGArrX[0];
		for(i=0;i<m_nWingAxisGX;i++)
		{
			double upLimPosY,upDrawCoordY;
			if(i<m_nWingAxisGX-1)
			{
				upLimPosY=-wingAxisGArrX[i+1];
				upDrawCoordY=wingAxisCoordArrX[i+1];
			}
			else
			{
				upLimPosY=-wingWide;
				upDrawCoordY=-wingWide*m_fVertScaleCoef;
			}
			if(realPosY<=-wingAxisGArrX[i]&&realPosY>upLimPosY)
				return wingAxisCoordArrX[i]+(realPosY+wingAxisGArrX[i])*(upDrawCoordY-wingAxisCoordArrX[i])/(upLimPosY+wingAxisGArrX[i]);
		}
		return realPosY*m_fVertScaleCoef;
	}
	else if(realPosY>0)	//λ��Y֫��
	{
		if(m_nWingAxisGY<=0)
			return realPosY*m_fVertScaleCoef;
		else if(realPosY<wingAxisGArrY[0])
			return realPosY*wingAxisCoordArrY[0]/wingAxisGArrY[0];
		for(i=0;i<m_nWingAxisGY;i++)
		{
			double upLimPosY,upDrawCoordY;
			if(i<m_nWingAxisGY-1)
			{
				upLimPosY=wingAxisGArrY[i+1];
				upDrawCoordY=wingAxisCoordArrY[i+1];
			}
			else
			{
				upLimPosY=wingWide;
				upDrawCoordY=wingWide*m_fVertScaleCoef;
			}
			if(realPosY>=wingAxisGArrY[i]&&realPosY<upLimPosY)
				return wingAxisCoordArrY[i]+(realPosY-wingAxisGArrY[i])*(upDrawCoordY-wingAxisCoordArrY[i])/(upLimPosY-wingAxisGArrY[i]);
		}
		return realPosY*m_fVertScaleCoef;
	}
	else
		return realPosY*m_fVertScaleCoef;
}
bool CRodDrawing::IsHasIntersObj(CZhunXianTextDim &zhunDim)
{
	if(zhunDim.dist>0)	//Y֫
	{
		for(int i=0;i<LsCirY.GetSize();i++)
		{
			if(fabs(LsCirY[i].dimCircle.centre.x-zhunDim.dimPos.x)<4)
				return true;	//2mm�Ǿ���ֵ
		}
	}
	else if(zhunDim.dist<0)	//X֫
	{
		for(int i=0;i<LsCirX.GetSize();i++)
		{
			if(fabs(LsCirX[i].dimCircle.centre.x-zhunDim.dimPos.x)<4)
				return true;	//2mm�Ǿ���ֵ
		}
	}
	double width=strlen(zhunDim.DimText())*CSizeTextDim::dimStyle.textHeight*0.7;
	for(int j=0;j<zhun_dim_arr.GetSize();j++)
	{
		if(zhun_dim_arr[j].dist==zhunDim.dist)
			continue;	//�����Լ��Ƚ�
		else if(zhun_dim_arr[j].dist*zhunDim.dist<0)
			continue;	//���벻ͬ֫�ľ�Ƚ�
		if(fabs(zhun_dim_arr[j].dimPos.x-zhunDim.dimPos.x)<CSizeTextDim::dimStyle.textHeight*2)
			return true;
		//if(zhun_dim_arr[j].lineStart.y<zhunDim.dimPos.y+0.5*width&&zhun_dim_arr[j].lineStart.y>zhunDim.dimPos.y-0.5*width)
		//	return true;
	}
	return false;
}
SCOPE_STRU CRodDrawing::RelayoutDrawing()
{
	int i;
	for(i=0;i<LsCirX.GetSize();i++)
	{
		LsCirX[i].dimCircle.centre.x+=leftGDimMargin;
		LsCirX[i].dimCircle.centre.y=TransCoordY(ftoi(LsCirX[i].dimCircle.centre.y));
	}
	for(i=0;i<LsCirY.GetSize();i++)
	{
		LsCirY[i].dimCircle.centre.x+=leftGDimMargin;
		LsCirY[i].dimCircle.centre.y=TransCoordY(ftoi(LsCirY[i].dimCircle.centre.y));
	}
	//�Ǹֳ��ȷ��������ؼ�ʾ����
	for(i=0;i<edgeLineArr.GetSize();i++)
	{
		edgeLineArr[i].startPt.y=TransCoordY(edgeLineArr[i].startPt.y);
		edgeLineArr[i].endPt.y=TransCoordY(edgeLineArr[i].endPt.y);
		edgeLineArr[i].startPt.x+=leftGDimMargin;
		edgeLineArr[i].endPt.x+=leftGDimMargin;
	}
	//�Ǹֲ�ͼ����������
	for(i=0;i<lineArr.GetSize();i++)
	{
		lineArr[i].startPt.y=TransCoordY(lineArr[i].startPt.y);
		lineArr[i].endPt.y=TransCoordY(lineArr[i].endPt.y);
		lineArr[i].startPt.x+=leftGDimMargin;
		lineArr[i].endPt.x+=leftGDimMargin;
	}
	//�н���֫��ע
	f3dPoint horiVec,vertVec;
	double halfPi=Pi/2;
	SCOPE_STRU usedDimRegion,dimRegion;
	usedDimRegion.fMinX=leftGDimMargin;
	usedDimRegion.fMaxX=TransCoordX(length)+leftGDimMargin;
	usedDimRegion.fMaxY=TransCoordY(wingWide);
	usedDimRegion.fMinY=TransCoordY(-wingWide);
	f3dPoint rgnVert[4];
	for(i=0;i<dim_arr.GetSize();i++)
	{
		if(dim_arr[i].nHits>0)
			continue;	//�Ǹ�����˨�׾��ע
		dim_arr[i].dimStartPos.y=TransCoordY(dim_arr[i].dimStartPos.y);
		dim_arr[i].dimEndPos.y=TransCoordY(dim_arr[i].dimEndPos.y);
		dim_arr[i].dimPos.y=TransCoordY(dim_arr[i].dimPos.y);
		dim_arr[i].ResetPosition();
		dim_arr[i].origin.x+=leftGDimMargin;
		dim_arr[i].GetDimRect(rgnVert);
		for(int j=0;j<4;j++)
			usedDimRegion.VerifyVertex(rgnVert[j]);
	}
	//׼�߼�׼���ע
	CHeapSort<CZhunXianTextDim>::HeapSort(zhun_dim_arr.GetData(),zhun_dim_arr.GetSize(),compareZhunXianDim);
	int counter=0,prevSign=1;
	dimRegion=usedDimRegion;
	double dimOffsetDist=0;
	for(i=0;i<zhun_dim_arr.GetSize();i++)
	{
		if(i>0&&prevSign<0&&zhun_dim_arr[i].dist>0)
		{
			counter=0;	//���������
			dimOffsetDist=0;
		}
		zhun_dim_arr[i].lineStart.y=TransCoordY(ftoi(zhun_dim_arr[i].lineStart.y));	//��׼�����ձ�עֵȡ�����ʴ˴�ΪЭ��ҲӦȡ�������Y����
		zhun_dim_arr[i].lineEnd.y=TransCoordY(ftoi(zhun_dim_arr[i].lineEnd.y));

		zhun_dim_arr[i].dimStartPos.y=TransCoordY(ftoi(zhun_dim_arr[i].dimStartPos.y));
		zhun_dim_arr[i].dimEndPos.y=TransCoordY(ftoi(zhun_dim_arr[i].dimEndPos.y));
		zhun_dim_arr[i].dimPos.y=TransCoordY(ftoi(zhun_dim_arr[i].dimPos.y));
		zhun_dim_arr[i].dimStartPos.x+=(leftGDimMargin-dimOffsetDist);
		zhun_dim_arr[i].dimEndPos.x+=(leftGDimMargin-dimOffsetDist);
		if(dimStyleFlag.IsHasFlag(DimAxisGTextAtStart))
		{
			zhun_dim_arr[i].lineStart.x=leftGDimMargin;
			zhun_dim_arr[i].lineEnd.x+=leftGDimMargin+zhun_dim_arr[i].dimStyle.textHeight*2;
			zhun_dim_arr[i].dimPos.x=dimRegion.fMinX-(counter*2+0.5)*zhun_dim_arr[i].dimStyle.textHeight;
			if(counter==0)
			{	//����׼�ݱ�עλ����Ǹ�ʼ��λ�õ�ƫ�ƾ��� 
				dimOffsetDist=(zhun_dim_arr[i].dimStartPos.x-(zhun_dim_arr[i].dimPos.x+1));
				//������Ǹ�ʼ�������׼��ʼ�˼��ն˱�עλ�ã��ɱ�עλ�÷��Ƴ� wht 12-03-13
				zhun_dim_arr[i].dimStartPos.x=zhun_dim_arr[i].dimPos.x+1;
				zhun_dim_arr[i].dimEndPos.x=zhun_dim_arr[i].dimPos.x+1;
			}
			usedDimRegion.VerifyVertex(zhun_dim_arr[i].dimPos-f3dPoint(zhun_dim_arr[i].dimStyle.textHeight*1.5));
			counter++;
		}
		else
		{
			zhun_dim_arr[i].lineStart.x+=leftGDimMargin-zhun_dim_arr[i].dimStyle.textHeight*2;
			zhun_dim_arr[i].lineEnd.x+=leftGDimMargin+zhun_dim_arr[i].dimStyle.textHeight*2;
			zhun_dim_arr[i].dimPos.x+=leftGDimMargin;
		}
		if(zhun_dim_arr[i].dist>0)
			prevSign=1;
		else
			prevSign=-1;
	}
	if(dimStyleFlag.IsHasFlag(DimAxisGTextAtMiddle))
	{
		double lengthDrawCoordX=TransCoordX(length);
		for(i=0;i<zhun_dim_arr.GetSize();i++)
		{
			//����������׼���ע
			CZhunXianTextDim zhunDim=zhun_dim_arr[i];
			double offset_x=0,offset_y=0;
			int sign_x=1,sign_y=1;
			while(offset_x<100)
			{
				zhunDim.dimPos.x=zhun_dim_arr[i].dimPos.x+offset_x;
				if(zhunDim.dimPos.x<lengthDrawCoordX-CSizeTextDim::dimStyle.textHeight*2)
				{
					sign_x=1;
					offset_y=0;
					while(offset_y<10)
					{
						sign_y=1;
						zhunDim.dimPos.y=zhun_dim_arr[i].dimPos.y+offset_y;
						if(!IsHasIntersObj(zhunDim))
							break;
						sign_y=-1;
						zhunDim.dimPos.y=zhun_dim_arr[i].dimPos.y-offset_y;
						if(!IsHasIntersObj(zhunDim))
							break;
						offset_y+=1;
					}
					if(offset_y!=10)
						break;
				}
				zhunDim.dimPos.x=zhun_dim_arr[i].dimPos.x-offset_x;
				if(zhunDim.dimPos.x>leftGDimMargin+CSizeTextDim::dimStyle.textHeight*2)
				{
					sign_x=-1;
					offset_y=0;
					while(offset_y<10)
					{
						sign_y=1;
						zhunDim.dimPos.y=zhun_dim_arr[i].dimPos.y+offset_y;
						if(!IsHasIntersObj(zhunDim))
							break;
						sign_y=-1;
						zhunDim.dimPos.y=zhun_dim_arr[i].dimPos.y-offset_y;
						if(!IsHasIntersObj(zhunDim))
							break;
						offset_y+=1;
					}
					if(offset_y!=10)
						break;
				}
				offset_x+=1;
			}
			if(offset_x<100)
			{
				zhun_dim_arr[i].dimPos+=f3dPoint(sign_x*offset_x,sign_y*offset_y);
				zhun_dim_arr[i].dimStartPos.x+=sign_x*offset_x;
				zhun_dim_arr[i].dimEndPos.x+=sign_x*offset_x;
			}
			if(zhun_dim_arr[i].lineStart.x>zhun_dim_arr[i].dimStartPos.x)
				zhun_dim_arr[i].lineStart.x=zhun_dim_arr[i].dimStartPos.x;
			if(zhun_dim_arr[i].lineEnd.x<zhun_dim_arr[i].dimStartPos.x)
				zhun_dim_arr[i].lineEnd.x=zhun_dim_arr[i].dimStartPos.x;
		}
	}
	for(i=0;i<zhun_dim_arr.GetSize();i++)
		zhun_dim_arr[i].dist=fabs(zhun_dim_arr[i].dist);
	//�Ǹ�����˨�׾��ע
	dimRegion=usedDimRegion;
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongX))
	{
		for(i=0;i<dim_arr.GetSize();i++)
		{
			if(dim_arr[i].nHits<=0)	//�Ǹ��н���֫��ע
				continue;
			dim_arr[i].dimStartPos.x=TransCoordX(dim_arr[i].dimStartPos.x);
			dim_arr[i].dimStartPos.y=TransCoordY(dim_arr[i].dimStartPos.y);
			dim_arr[i].dimEndPos.x=TransCoordX(dim_arr[i].dimEndPos.x);
			dim_arr[i].dimEndPos.y=TransCoordY(dim_arr[i].dimEndPos.y);
			dim_arr[i].dimPos.x=0.5*(dim_arr[i].dimStartPos.x+dim_arr[i].dimEndPos.x);
			dim_arr[i].dimPos.y=TransCoordY(dim_arr[i].dimPos.y);
			dim_arr[i].ResetPosition();
			if(dim_arr[i].origin.y>0)
				dim_arr[i].origin.y=dimRegion.fMaxY+dim_arr[i].dimStyle.textHeight*0.5;	//�������ָ߼�϶
			else
				dim_arr[i].origin.y=dimRegion.fMinY-dim_arr[i].dimStyle.textHeight*0.5;
			dim_arr[i].origin.x+=leftGDimMargin;
			dim_arr[i].GetDimRect(rgnVert);
			for(int j=0;j<4;j++)
				usedDimRegion.VerifyVertex(rgnVert[j]);
		}
	}
	else
		dim_arr.Empty();//RemoveAll();
	//iOrder: 0.��ʾ����ע�� 1.��ʾ��һ����˨����Գߴ� 2.��ʾ�ڶ�����˨����Գߴ� 3.��˨���Գߴ� wht 11-05-07
	for(int iOrder=0;iOrder<5;iOrder++)
	{	//��������ע�͡��Ǹ�����˨�׾༰��λ���ֱ�ע
		dimRegion=usedDimRegion;
		for(i=0;i<pure_txt_arr.GetSize();i++)
		{
			if(abs(pure_txt_arr[i].iOrder)!=iOrder)
				continue;
			pure_txt_arr[i].dimPos.x=TransCoordX(pure_txt_arr[i].dimPos.x);
			if(pure_txt_arr[i].iOrder==0)
				pure_txt_arr[i].alignType=MiddleLeft;
			else if(pure_txt_arr[i].iOrder==4)
			{	//������˨�ױ��(����+����)
				f3dPoint temPt=pure_txt_arr[i].dimPos;
				int nLen=strlen(pure_txt_arr[i].dimText);
				double height=pure_txt_arr[i].textHeight;
				double width=height*nLen;
				if(pure_txt_arr[i].dimPos.y<0)
				{
					pure_txt_arr[i].dimPos.y=dimRegion.fMinY-width*0.7;
					if(usedDimRegion.fMinY>pure_txt_arr[i].dimPos.y-width*0.7)
						usedDimRegion.fMinY=pure_txt_arr[i].dimPos.y-width*0.7;
				}
				else
				{	
					pure_txt_arr[i].dimPos.y=dimRegion.fMaxY;//+width*0.5;
					if(usedDimRegion.fMaxY<pure_txt_arr[i].dimPos.y+width*0.7)
						usedDimRegion.fMaxY=pure_txt_arr[i].dimPos.y+width*0.7;
				}
				//������˨����ַ�����
				f3dLine line;
				temPt.x+=leftGDimMargin;
				temPt.y=TransCoordY(temPt.y);
				line.startPt.Set(temPt.x,temPt.y);
				temPt.y=pure_txt_arr[i].dimPos.y;
				if(temPt.y>0)
					temPt.y+=+width*0.5;
				line.endPt.Set(temPt.x,temPt.y);
				lineArr.Add(line);
			}
			else if(pure_txt_arr[i].iOrder>0)
			{	
				pure_txt_arr[i].dimPos.y=dimRegion.fMaxY+pure_txt_arr[i].textHeight*0.5;	//�������ָ߼�϶
				if(usedDimRegion.fMaxY<pure_txt_arr[i].dimPos.y+pure_txt_arr[i].GetWidth())
					usedDimRegion.fMaxY=pure_txt_arr[i].dimPos.y+pure_txt_arr[i].GetWidth();
			}
			else
			{
				pure_txt_arr[i].dimPos.y=dimRegion.fMinY-pure_txt_arr[i].textHeight*0.5;	//�������ָ߼�϶
				if(usedDimRegion.fMinY>pure_txt_arr[i].dimPos.y-pure_txt_arr[i].GetWidth())
					usedDimRegion.fMinY=pure_txt_arr[i].dimPos.y-pure_txt_arr[i].GetWidth();
			}
			pure_txt_arr[i].origin.x+=leftGDimMargin;
		}
	}
	return usedDimRegion;
}
SCOPE_STRU CAngleDrawing::RelayoutDrawing()
{
	SCOPE_STRU usedDimRegion=CRodDrawing::RelayoutDrawing();
	int i=0,n=0;
	f3dPoint rgnVert[4];
	//�Ǹֿ��Ͻ�ʾע
	CKaiHeAngleDim kaihe_dim,kaihe_dim2;
	n=GetKaiHeDimCount();
	//�����������򿪺ϽǱ�ע
	CHeapSort<CKaiHeAngleDim>::HeapSort(kaihe_angle_dim_arr.GetData(),n,compareKaiHeDim);
	//������Ͻ��Ŀ��ϽǺϲ�Ϊһ�����Ͻ�
	double MERGE_KAIHE_SPACE =wingWide*2;
	for(i=0;i<n-1;i++)
	{
		GetKaiHeDimAt(i,&kaihe_dim);
		GetKaiHeDimAt(i+1,&kaihe_dim2);
		if(i==0&&kaihe_dim.real_start.x<MERGE_KAIHE_SPACE)
		{
			kaihe_angle_dim_arr[i].real_start.x=0;
			kaihe_angle_dim_arr[i].dimStartPos.x=0;
			kaihe_angle_dim_arr[i].dist=kaihe_angle_dim_arr[i].real_end.x;
		}
		if(i==n-2&&length-kaihe_dim2.real_end.x<MERGE_KAIHE_SPACE)
		{
			kaihe_angle_dim_arr[i+1].real_end.x=length;
			kaihe_angle_dim_arr[i+1].dimEndPos.x=TransCoordX(length);
			kaihe_angle_dim_arr[i+1].dist=length-kaihe_angle_dim_arr[i+1].real_start.x;
		}
		if(kaihe_dim.real_end.x>kaihe_dim2.real_start.x-wingWide*2)	//����Ͻ��ĺϲ�Ϊһ�ο��Ͻ�
		{
			kaihe_angle_dim_arr[i].dimEndPos = kaihe_dim2.dimEndPos;
			kaihe_angle_dim_arr[i].real_end = kaihe_dim2.real_end;
			kaihe_angle_dim_arr[i].end_ls_pos=kaihe_dim2.end_ls_pos;
			kaihe_angle_dim_arr[i].real_end_ls_pos=kaihe_dim2.real_end_ls_pos;
			kaihe_angle_dim_arr[i].dist = kaihe_dim2.real_end.x-kaihe_dim.real_start.x;
			kaihe_angle_dim_arr.RemoveAt(i+1);
			i--;
			n--;
		}
	}
	SCOPE_STRU dimRegion=usedDimRegion;
	for(i=0;i<kaihe_angle_dim_arr.GetSize();i++)
	{
		//TODO:��ʱ�趨 wjh-2015.2.5
		//kaihe_angle_dim_arr[i].dimStyle=DimStyleTable::dimStyle;
#ifdef __DRAWING_CONTEXT_
		kaihe_angle_dim_arr[i].dimStyle.style.gap=0.75;
		kaihe_angle_dim_arr[i].dimStyle.textHeight=3;
#endif
		kaihe_angle_dim_arr[i].dimStartPos.y=TransCoordY(kaihe_angle_dim_arr[i].dimStartPos.y);
		kaihe_angle_dim_arr[i].dimEndPos.y=TransCoordY(kaihe_angle_dim_arr[i].dimEndPos.y);
		kaihe_angle_dim_arr[i].dimPos.y=TransCoordY(kaihe_angle_dim_arr[i].dimPos.y);
		kaihe_angle_dim_arr[i].ResetPosition();
		if(!kaihe_angle_dim_arr[i].bInWingX)	//Y֫���Ͻ�
			kaihe_angle_dim_arr[i].origin.y=dimRegion.fMaxY;
		else						//X֫���Ͻ�
			kaihe_angle_dim_arr[i].origin.y=dimRegion.fMinY;
		kaihe_angle_dim_arr[i].origin.x+=leftGDimMargin;
		kaihe_angle_dim_arr[i].GetDimRect(rgnVert);
		for(int j=0;j<4;j++)
			usedDimRegion.VerifyVertex(rgnVert[j]);
	}
	//�߰�λ�ñ�ע
	for(i=0;i<rib_dim_arr.GetSize();i++)
	{
		int thickoffset=rib_thick_arr[i];
		if(thickoffset>0)
		{	//���Ϊ�߰��Ҳ����������߰��������ұ߻��Ʊ���һ���Դ��� wjh-2017.3.10
			double anchorX=TransCoordX(rib_dim_arr[i].dimStartPos.x-thickoffset);
			rib_dim_arr[i].dimStartPos.x=anchorX+min(thickoffset*0.1,TransCoordX(rib_dim_arr[i].dimStartPos.x)-anchorX);
		}
		else
			rib_dim_arr[i].dimStartPos.x=TransCoordX(rib_dim_arr[i].dimStartPos.x);
		rib_dim_arr[i].dimStartPos.y=TransCoordY(ftoi(rib_dim_arr[i].dimStartPos.y));
		if(thickoffset<0)
		{
			thickoffset*=-1;
			double anchorX=TransCoordX(rib_dim_arr[i].dimEndPos.x+thickoffset);
			rib_dim_arr[i].dimEndPos.x=anchorX-min(thickoffset*0.1,anchorX-TransCoordX(rib_dim_arr[i].dimEndPos.x));
		}
		else
			rib_dim_arr[i].dimEndPos.x=TransCoordX(rib_dim_arr[i].dimEndPos.x);
		rib_dim_arr[i].dimEndPos.y=TransCoordY(ftoi(rib_dim_arr[i].dimEndPos.y));
		rib_dim_arr[i].ResetPosition2();
		rib_dim_arr[i].origin.x+=leftGDimMargin;
		rib_dim_arr[i].GetDimRect(rgnVert);
	}
	//�߰��ű�ע
	dimRegion=usedDimRegion;
	for(i=0;i<part_no_arr.GetSize();i++)
	{
		part_no_arr[i].dataPos.y=TransCoordY(part_no_arr[i].dataPos.y);
		part_no_arr[i].dataPos.x+=leftGDimMargin;
		part_no_arr[i].dimPos.x+=leftGDimMargin;
		if(part_no_arr[i].dimPos.y>0)
			part_no_arr[i].dimPos.y=dimRegion.fMaxY+drawPara.fPartNoTextSize;
		else
			part_no_arr[i].dimPos.y=dimRegion.fMinY-drawPara.fPartNoTextSize;
		part_no_arr[i].GetDimRect(rgnVert);
		for(int j=0;j<4;j++)
			usedDimRegion.VerifyVertex(rgnVert[j]);
	}
	if(drawPara.bSplitAngleSketchDrawing)
	{	//�Ǹֲ�ͼ��Ϊ����������ͼ
		double fFrontViewUpOffset=-dimRegion.fMinY+2;	//���4mm
		double division=edgeLineArr[2].startPt.y;
		double fTopViewDownOffset= dimRegion.fMaxY+2;	//���4mm
		//1.���ȷ������
		f3dLine midline=edgeLineArr[2];
		for(i=0;i<edgeLineArr.GetSize();i++)
		{
			if(i==2)	//�Ǹ��м���
			{
				edgeLineArr[i].startPt.y+=fFrontViewUpOffset;
				edgeLineArr[i].endPt.y  +=fFrontViewUpOffset;
			}
			else if(edgeLineArr[i].startPt.y>division-EPS&&edgeLineArr[i].endPt.y>division-EPS)	//Y֫��
			{
				edgeLineArr[i].startPt.y-=fTopViewDownOffset;
				edgeLineArr[i].endPt.y  -=fTopViewDownOffset;
			}
			else if(edgeLineArr[i].startPt.y<division+EPS&&edgeLineArr[i].endPt.y<division+EPS)	//X֫��
			{
				edgeLineArr[i].startPt.y+=fFrontViewUpOffset;
				edgeLineArr[i].endPt.y  +=fFrontViewUpOffset;
			}
			else
				logerr.Log("����ͼԪ��Խ�м���");
		}
		midline.startPt.y-=fTopViewDownOffset;
		midline.endPt.y  -=fTopViewDownOffset;
		edgeLineArr.Add(midline);
		//�Ǹ�ʾ���ͼ�г������������ȷ�������������ͼԪ
		ARRAY_LIST<f3dLine> addlines;
		for(i=0;i<lineArr.GetSize();i++)
		{
			if(lineArr[i].startPt.y>division-EPS&&lineArr[i].endPt.y>division-EPS)	//Y֫��
			{
				lineArr[i].startPt.y-=fTopViewDownOffset;
				lineArr[i].endPt.y  -=fTopViewDownOffset;
			}
			else if(lineArr[i].startPt.y<division+EPS&&lineArr[i].endPt.y<division+EPS)	//X֫��
			{
				lineArr[i].startPt.y+=fFrontViewUpOffset;
				lineArr[i].endPt.y  +=fFrontViewUpOffset;
			}
			else
			{
				f3dLine line=lineArr[i];
				f3dPoint linevec=line.endPt-line.startPt;
				line.endPt=line.startPt+linevec*((division-line.startPt.y)/linevec.y);
				lineArr[i].startPt=line.endPt;
				if(line.startPt.y>division-EPS)
				{	//line��Y֫����
					lineArr[i].startPt.y+=fFrontViewUpOffset;
					lineArr[i].endPt.y  +=fFrontViewUpOffset;
					line.startPt.y-=fTopViewDownOffset;
					line.endPt.y  -=fTopViewDownOffset;
				}
				else
				{	//line��X֫����
					lineArr[i].startPt.y-=fTopViewDownOffset;
					lineArr[i].endPt.y  -=fTopViewDownOffset;
					line.startPt.y+=fFrontViewUpOffset;
					line.endPt.y  +=fFrontViewUpOffset;
				}
				addlines.append(line);
			}
		}
		for(i=0;i<addlines.GetSize();i++)
			lineArr.Add(addlines[i]);
		//�Ǹ��ϵ���˨��
		for(i=0;i<LsCirX.GetSize();i++)
		{
			if(LsCirX[i].dimCircle.centre.y>division-EPS)	//Y֫��
				LsCirX[i].dimCircle.centre.y-=fTopViewDownOffset;
			else if(LsCirX[i].dimCircle.centre.y<division+EPS)	//X֫��
				LsCirX[i].dimCircle.centre.y+=fFrontViewUpOffset;
			else
				logerr.Log("����ͼԪ��Խ�м���");
		}
		for(i=0;i<LsCirY.GetSize();i++)
		{
			if(LsCirY[i].dimCircle.centre.y>division-EPS)	//Y֫��
				LsCirY[i].dimCircle.centre.y-=fTopViewDownOffset;
			else if(LsCirY[i].dimCircle.centre.y<division+EPS)	//X֫��
				LsCirY[i].dimCircle.centre.y+=fFrontViewUpOffset;
			else
				logerr.Log("����ͼԪ��Խ�м���");
		}
		for(i=0;i<dim_arr.GetSize();i++)
		{
			if(dim_arr[i].dimPos.y>division-EPS)
			{
				dim_arr[i].dimPos.y-=fTopViewDownOffset;
				dim_arr[i].dimStartPos.y-=fTopViewDownOffset;
				dim_arr[i].dimEndPos.y-=fTopViewDownOffset;
			}
			else if(dim_arr[i].dimPos.y<division+EPS)	//X֫��
			{
				dim_arr[i].dimPos.y+=fFrontViewUpOffset;
				dim_arr[i].dimStartPos.y+=fFrontViewUpOffset;
				dim_arr[i].dimEndPos.y+=fFrontViewUpOffset;
			}
			else
				logerr.Log("����ͼԪ��Խ�м���");
		}
		for(i=0;i<pure_txt_arr.GetSize();i++)
		{
			if(pure_txt_arr[i].dimPos.y>division-EPS)
				pure_txt_arr[i].dimPos.y-=fTopViewDownOffset;
			else if(pure_txt_arr[i].dimPos.y<division+EPS)	//X֫��
				pure_txt_arr[i].dimPos.y+=fFrontViewUpOffset;
			else
				logerr.Log("����ͼԪ��Խ�м���");
		}
		for(i=0;i<zhun_dim_arr.GetSize();i++)
		{
			if(zhun_dim_arr[i].dimPos.y>division-EPS)
			{
				zhun_dim_arr[i].dimPos.y-=fTopViewDownOffset;
				zhun_dim_arr[i].dimStartPos.y-=fTopViewDownOffset;
				zhun_dim_arr[i].dimEndPos.y-=fTopViewDownOffset;
				zhun_dim_arr[i].lineStart.y-=fTopViewDownOffset;
				zhun_dim_arr[i].lineEnd.y-=fTopViewDownOffset;
			}
			else if(zhun_dim_arr[i].dimPos.y<division+EPS)	//X֫��
			{
				zhun_dim_arr[i].dimPos.y+=fFrontViewUpOffset;
				zhun_dim_arr[i].dimStartPos.y+=fFrontViewUpOffset;
				zhun_dim_arr[i].dimEndPos.y+=fFrontViewUpOffset;
				zhun_dim_arr[i].lineStart.y+=fFrontViewUpOffset;
				zhun_dim_arr[i].lineEnd.y+=fFrontViewUpOffset;
			}
			else
				logerr.Log("����ͼԪ��Խ�м���");
		}
		//�߰���
		for(i=0;i<part_no_arr.GetSize();i++)
		{
			if(part_no_arr[i].dimPos.y>division-EPS)
			{
				part_no_arr[i].dimPos.y-=fTopViewDownOffset;
				part_no_arr[i].dataPos.y-=fTopViewDownOffset;
			}
			else if(part_no_arr[i].dimPos.y<division+EPS)	//X֫��
			{
				part_no_arr[i].dimPos.y+=fFrontViewUpOffset;
				part_no_arr[i].dataPos.y+=fFrontViewUpOffset;
			}
			else
				logerr.Log("����ͼԪ��Խ�м���");
		}
		for(i=0;i<kaihe_angle_dim_arr.GetSize();i++)
		{
			if(kaihe_angle_dim_arr[i].dimPos.y>division-EPS)
			{
				kaihe_angle_dim_arr[i].origin.y-=fTopViewDownOffset;
				kaihe_angle_dim_arr[i].dimPos.y-=fTopViewDownOffset;
				kaihe_angle_dim_arr[i].dimStartPos.y-=fTopViewDownOffset;
				kaihe_angle_dim_arr[i].dimEndPos.y-=fTopViewDownOffset;
				kaihe_angle_dim_arr[i].start_ls_pos.y-=fTopViewDownOffset;
				kaihe_angle_dim_arr[i].end_ls_pos.y-=fTopViewDownOffset;
			}
			else if(kaihe_angle_dim_arr[i].dimPos.y<division+EPS)	//X֫��
			{
				kaihe_angle_dim_arr[i].origin.y+=fFrontViewUpOffset;
				kaihe_angle_dim_arr[i].dimPos.y+=fFrontViewUpOffset;
				kaihe_angle_dim_arr[i].dimStartPos.y+=fFrontViewUpOffset;
				kaihe_angle_dim_arr[i].dimEndPos.y+=fFrontViewUpOffset;
				kaihe_angle_dim_arr[i].start_ls_pos.y+=fFrontViewUpOffset;
				kaihe_angle_dim_arr[i].end_ls_pos.y+=fFrontViewUpOffset;
			}
			else
				logerr.Log("����ͼԪ��Խ�м���");
		}
	}
	return usedDimRegion;
}

BOOL CRodDrawing::DimLsZhun(f3dPoint ls_prj_pos)
{
	int i,nDimCount;
	CZhunXianTextDim zhun_dim;
#ifdef __DRAWING_CONTEXT_
	zhun_dim.dimStyle.style=DimStyleTable::dimStyle;
	zhun_dim.dimStyle.textHeight = drawPara.fDimTextSize;
#endif
	BOOL bHasSameDim = FALSE;
	nDimCount = zhun_dim_arr.GetSize();
	if(fabs(ls_prj_pos.y)<EPS)
		return FALSE;	//�����Ƿ���˨��������
	for(i=0;i<nDimCount;i++)
	{
		if(ftoi(zhun_dim_arr[i].lineStart.y)==ftoi(ls_prj_pos.y))
		{
			zhun_dim_arr[i].lineStart.x = __min(zhun_dim_arr[i].lineStart.x,ls_prj_pos.x);
			zhun_dim_arr[i].lineEnd.x = __max(zhun_dim_arr[i].lineEnd.x,ls_prj_pos.x);
			bHasSameDim=TRUE;
			break;
		}
	}
	if(!bHasSameDim)
	{
		zhun_dim.lineStart.Set(ls_prj_pos.x,ls_prj_pos.y,0);
		zhun_dim.lineEnd.Set(ls_prj_pos.x,ls_prj_pos.y,0);
		if(!dimStyleFlag.IsHasFlag(BriefDrawing))
		{
			if(dimStyleFlag.IsHasFlag(DimAxisGTextAtStart))
			{
				if(ls_prj_pos.y<0)	//X֫�ľ��ע
				{
					zhun_dim.dimStartPos.Set(0,ls_prj_pos.y,0);
					zhun_dim.dimEndPos.Set(0,0,0);
				}
				else				//Y֫�ľ��ע
				{
					zhun_dim.dimStartPos.Set(0,0,0);
					zhun_dim.dimEndPos.Set(0,ls_prj_pos.y,0);
				}
				zhun_dim.dimPos.Set(-1,ls_prj_pos.y/2.0,0);
			}
			else //if(dimStyleFlag.IsHasFlag(DimAxisGTextAtMiddle))
			{
				zhun_dim.dimStartPos.Set(ls_prj_pos.x+4,ls_prj_pos.y,0);
				zhun_dim.dimEndPos.Set(ls_prj_pos.x+4,0,0);
				zhun_dim.dimPos.Set(ls_prj_pos.x+2,ls_prj_pos.y/2.0,0);
			}
			zhun_dim.angle = Pi/2.0;	//��ֱ��ע
			zhun_dim.dist = ls_prj_pos.y;	//Բ����
		}
		zhun_dim_arr.Add(zhun_dim);
		return TRUE;
	}
	return FALSE;
}
void CRodDrawing::CreateLsFuncTextDim()
{
	double hole_d=0,fDim=0,fDimIncrement=0;
	CXhChar50 dimText;
	CTextOnlyDrawing txt_dim;
	txt_dim.iOrder=4;	//������˨�׵ı��
	txt_dim.textHeight=drawPara.fDimTextSize;
	txt_dim.angle=Pi/2;
	//X֫��˨������
	for(int i=0;i<LsCirX.GetSize();i++)
	{
		if(LsCirX[i].cFuncType<=1)	//cFuncType==1��ʾ�Ŷ�������Ҫ���ֱ�ע
			continue;
		fDim=LsCirX[i].dimCircle.radius*2;
		fDimIncrement=LsCirX[i].dimCircle.feature/10.0;
		hole_d=fDim+fDimIncrement;	//ʵ�ʿ׾�
		dimText.Printf("��%g",hole_d);
		if(LsCirX[i].cFlag==1)	//���
			dimText.Append("���");
		else
			dimText.Append("��");
		//
		txt_dim.dimPos=LsCirX[i].realPos;
		txt_dim.dimText=dimText;
		pure_txt_arr.append(txt_dim);
	}
	//Y֫��˨������
	for(int i=0;i<LsCirY.GetSize();i++)
	{
		if(LsCirY[i].cFuncType<=1)
			continue;
		fDim=LsCirY[i].dimCircle.radius*2;
		fDimIncrement=LsCirY[i].dimCircle.feature/10.0;
		hole_d=fDim+fDimIncrement;	//ʵ�ʿ׾�
		dimText.Printf("��%g",hole_d);
		if(LsCirY[i].cFlag==1)	//���
			dimText.Append("���");
		else
			dimText.Append("��");
		//
		txt_dim.dimPos=LsCirY[i].realPos;
		txt_dim.dimText=dimText;
		pure_txt_arr.append(txt_dim);
	}
}
void CRodDrawing::CreateLsSpaceDimList()
{
	int i;
	CSizeTextDim ls_dim;
	if(drawPara.iDimPrecision==0)	//��ȷ��1.0mm
		PRECISION=1;
	else if(drawPara.iDimPrecision==1)	//��ȷ��0.5mm
		PRECISION=0.5;
	else if(drawPara.iDimPrecision==2)	//��ȷ��0.1mm
		PRECISION=0.1;
	CSizeTextDim::PRECISION = PRECISION;
#ifdef __DRAWING_CONTEXT_
	CSizeTextDim::dimStyle.style=DimStyleTable::dimStyle;
	CSizeTextDim::dimStyle.textHeight=drawPara.fDimTextSize;
#endif
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongX))
		ls_dim.angle = 0;

	CTextOnlyDrawing pure_txt_dimX,pure_txt_dimY,ls_txt_dimX,ls_txt_dimY,prevLsPosDimX,prevLsPosDimY;
	CTextOnlyDrawing::PRECISION = PRECISION;
	//����CAngleDrawing::SpecifyDrawscale��ͨ���ⲿָ����ͼ����ʱĩ��˨��Ǹ�ĩ�˼���ע����
	//֮ǰ�汾���ɽǸֹ��տ�ʱ��Ϊδ����CAngleDrawing::SpecifyDrawscale��������û������ wht 15-02-16
	//double lengthEndDrawPosX= TransCoordX(length);
	pure_txt_dimX.bInXWing = TRUE;
	pure_txt_dimY.bInXWing = FALSE;
	pure_txt_dimX.dist = 0;
	pure_txt_dimY.dist = 0;
	pure_txt_dimX.angle = Pi/2;
	pure_txt_dimY.angle = Pi/2;
	pure_txt_dimX.alignType=MiddleRight;
	pure_txt_dimY.alignType=MiddleLeft;
	pure_txt_dimX.textHeight=drawPara.fDimTextSize;
	pure_txt_dimY.textHeight=drawPara.fDimTextSize;
	pure_txt_dimX.iOrder=-3;	//iOrder==-3��ʾX֫��˨���Գߴ磬��ǰ��-2��ʾ wht 11-05-07 
	pure_txt_dimY.iOrder=3;		//iOrder== 3��ʾY֫��˨���Գߴ磬��ǰ�� 2��ʾ wht 11-05-07
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
	{
		ls_txt_dimX.alignType=MiddleRight;
		ls_txt_dimX.bInXWing=TRUE;
		ls_txt_dimY.alignType=MiddleLeft;
		ls_txt_dimY.bInXWing=FALSE;
		ls_txt_dimX.angle = Pi/2;
		ls_txt_dimY.angle = Pi/2;
		ls_txt_dimX.textHeight = drawPara.fDimTextSize;
		ls_txt_dimY.textHeight = drawPara.fDimTextSize;
		ls_txt_dimX.iOrder=-1;	//iOrder==-1��ʾX֫��һ����˨����Գߴ�
		ls_txt_dimY.iOrder= 1;	//iOrder== 1��ʾY֫��һ����˨����Գߴ�
	}
	
	if(dimStyleFlag.IsHasFlag(BriefDrawing))
		return;
	CXhChar100 sDimText;
	f3dPoint real_ls_start,real_ls_end;
	double prev_dist=0;
	//Բ����λ���׾��ע
	CDistFillet fillet;
	fillet.coordArr.SetSize(LsCirX.GetSize()+1);
	for(i=0;i<LsCirX.GetSize();i++)
		fillet.coordArr[i]=LsCirX[i].realPos.x;
	fillet.coordArr[i]=length;
	bool filletSucces;
	if(fabs(PRECISION-1)<EPS)
		filletSucces=fillet.Fillet(120,FILLET_INTEGER);
	else if(fabs(PRECISION-0.5)<EPS)
		filletSucces=fillet.Fillet(120,FILLET_HALF_INTEGER);
	else
		filletSucces=fillet.Fillet(120,FILLET_ONE_FLOAT);
	if(!filletSucces)
		logerr.Log("%s�˼���X֫ĩβ��˨��λ�þ��Գߴ��ע��������г����쳣�����ܵ��¿�λ���Գߴ��ע��������߳ߴ��ע���Ȳ���ʵ",(char*)sPartNo);
	for(i=0;i<LsCirX.GetSize();i++)
		LsCirX[i].realPos.x=fillet.coordArr[i];
	//����Բ����Ŀ�λ���׾�������Ӧ��ע
	//�ж��׿���˨�ı�ע���� wjh-2011.8.15
	int iFirstLsDimRow=-1;	//Ĭ���׿���˨���ڵ�һ�� 
	if(LsCirX.GetSize()>0)
	{
		int g1=ftoi(fabs(LsCirX[0].realPos.y));
		for(i=1;i<LsCirX.GetSize();i++)
		{
			int g_n=ftoi(fabs(LsCirX[i].realPos.y));
			if(g_n==g1)
				continue;
			if(g_n<g1)
			{
				iFirstLsDimRow=-2;	//��һ��������˨��ͬ�ĵ���˨���������˨λ�ڵ�λ������˨��ڶ���
				break;
			}
			else if(g_n>g1)
			{
				iFirstLsDimRow=-1;	//��һ��������˨��ͬ�ĵ���˨���������˨λ�ڸ�λ������˨���һ��
				break;
			}
		}
	}
	ls_dim.dimStartPos.Set();
	for(i=0;i<LsCirX.GetSize();i++)
	{
		ls_dim.dimEndPos = LsCirX[i].realPos;//dimCircle.centre;
		real_ls_end = LsCirX[i].realPos;
		ls_dim.dist = fillet.distArr[i];
		if(fabs(ls_dim.dist)<PRECISION)	//С��Ԥ��������Ϊ��ͬһ��
			continue;
		
		ls_dim.iTextDimSylte=1;		//�����ǽǸ�X֫��˨�ĳߴ��ע��ʽΪ�����ڱ�ע���²ࡣ	wxc-2016.8.5
		ls_dim.dimPos.Set(0,-wingWide-1,0);
		ls_dim.dimStartPos.y = -wingWide;
		ls_dim.dimEndPos.y = -wingWide;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
		{	//�׾�Y����ֱ��ע
			ls_txt_dimX.dist=ls_dim.dist;
			ls_txt_dimX.dimPos=ls_dim.dimEndPos;
			if(i==0)
			{
				ls_txt_dimX.iOrder=iFirstLsDimRow;
				if(ls_txt_dimX.iOrder==-2)
					ls_txt_dimX.dimPos.y-=10;
			}
			else
			{
				CRodDrawing::LsMark *pPrevLsMark=&LsCirX[i-1];
				int g=ftoi(fabs(LsCirX[i].realPos.y));
				int g_p=ftoi(fabs(LsCirX[i-1].realPos.y));
				if(g==g_p)	//ͬǰһ����˨��ע����
				{
					ls_txt_dimX.dimPos.y=prevLsPosDimX.dimPos.y;
					ls_txt_dimX.iOrder=prevLsPosDimX.iOrder;
				}
				else if(g>g_p)
				{
					ls_txt_dimX.dimPos.y-=10;
					ls_txt_dimX.iOrder=-2; //iOrder==-2��ʾX֫�ڶ�����˨����Գߴ�
				}
				else //if(g<g_p)
					ls_txt_dimX.iOrder=-1; //iOrder==-1��ʾX֫��һ����˨����Գߴ�
			}
			/*if(LsCirX[i].iRow==2)	//�����ڶ�����˨�ı�עλ�� wht 11-05-07
			{
				ls_txt_dimX.dimPos.y-=10;
				ls_txt_dimX.iOrder=-2; //iOrder==-2��ʾX֫�ڶ�����˨����Գߴ�
			}
			else 
				ls_txt_dimX.iOrder=-1; //iOrder==-1��ʾX֫��һ����˨����Գߴ�*/
			LsCirX[i].pDimSpaceText=pure_txt_arr.append(ls_txt_dimX);
			//�˴�����ʹ��ָ�룬��Ϊ�������pure_txt_arr.append(pure_txt_dimX)������
			//�������·����ڴ浼��pPrevLsPosDimXָ����ڴ汻�ͷ� wht 12-03-13
			//pPrevLsPosDimX=&pure_txt_arr[pure_txt_arr.GetSize()-1];
			prevLsPosDimX=pure_txt_arr[pure_txt_arr.GetSize()-1];
		}
		else if(i>1&&dim_arr.GetSize()>0&&dim_arr[dim_arr.GetSize()-1].dist==ls_dim.dist&&ls_dim.dist<150)
		{	//�׾�X����ֱ��עʱ,�����ظ����ֵĿ׾��עҪ���кϲ�
			dim_arr[dim_arr.GetSize()-1].nHits++;
			dim_arr[dim_arr.GetSize()-1].dimEndPos=ls_dim.dimEndPos;
			LsCirX[i].pDimSpaceSize=dim_arr.GetTail();//[dim_arr.GetSize()-1]
		}
		else
		{	//������X��׾��ע
			ls_dim.nHits=1;
			LsCirX[i].pDimSpaceSize=dim_arr.append(ls_dim);
		}
		prev_dist+=ls_dim.dist;
		ls_dim.dimStartPos = ls_dim.dimEndPos;
		real_ls_start = real_ls_end;
	}
	for(i=0;i<LsCirX.GetSize();i++)
	{
		if(fabs(fillet.distArr[i])<PRECISION)	//С��Ԥ��������Ϊ��ͬһ��
			continue;
		//bMergeLsAbsoluteDist==TRUEʱ������ڽ��Ⱦ���˨ֻ�����һ����˨��X(��)����Գߴ� wxc 14-5-6
		if( drawPara.bMergeLsAbsoluteDist&&LsCirX[i].pDimSpaceSize&&LsCirX[i].pDimSpaceSize->nHits>1&&
			LsCirX[i].dimCircle.centre.x<LsCirX[i].pDimSpaceSize->dimEndPos.x-PRECISION)
			continue;	//����ڽ��Ⱦ���˨ֻ�����һ����˨��X(��)����Գߴ�
		pure_txt_dimX.dist=fillet.coordArr[i];
		pure_txt_dimX.dimPos = LsCirX[i].dimCircle.centre;//ls_dim.dimEndPos;
		pure_txt_dimX.dimPos.y=-wingWide-10;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&m_nWingAxisGX>1)
			pure_txt_dimX.dimPos.y-=10;	//�����б�ע��˨��࣬��Ҫ�������Գߴ��עλ�� wht 11-05-07
		pure_txt_arr.append(pure_txt_dimX);
	}
	if(LsCirX.GetSize()>0)
	{
		pure_txt_dimX.dist=fillet.coordArr[LsCirX.GetSize()];	
		ls_dim.dist = fillet.distArr[LsCirX.GetSize()];
		ls_dim.dimEndPos.Set(length,-wingWide);
		ls_dim.dimPos.Set(0,-wingWide-1,0);
		ls_dim.iTextDimSylte=1;		//�����ǽǸ�X֫��˨�ļ���ע
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
		{	//�׾�Y����ֱ��ע
			ls_txt_dimX.dist=ls_dim.dist;
			ls_txt_dimX.dimPos=ls_dim.dimEndPos;
			ls_txt_dimX.iOrder=-1;	//�˾��ڵ�һ�ű�ע
			pure_txt_arr.append(ls_txt_dimX);
		}
		/*else if(dim_arr[dim_arr.GetSize()-1].dist==ls_dim.dist)
		{	//�׾�X����ֱ��עʱ,�����ظ����ֵĿ׾��עҪ���кϲ�
			dim_arr[dim_arr.GetSize()-1].nHits++;
			dim_arr[dim_arr.GetSize()-1].dimEndPos=ls_dim.dimEndPos;
		}*/
		else
		{	//������X��׾��ע
			ls_dim.nHits=1;
			dim_arr.append(ls_dim);
		}
		pure_txt_dimX.dimPos = ls_dim.dimEndPos;
		pure_txt_dimX.dimPos.y=-wingWide-10;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&m_nWingAxisGX>1)
			pure_txt_dimX.dimPos.y-=10;	//�����б�ע��˨��࣬��Ҫ�������Գߴ��עλ�� wht 11-05-07
		pure_txt_arr.append(pure_txt_dimX);
	}

	//Բ����λ���׾��ע
	fillet.coordArr.SetSize(LsCirY.GetSize()+1);
	for(i=0;i<LsCirY.GetSize();i++)
		fillet.coordArr[i]=LsCirY[i].realPos.x;
	fillet.coordArr[i]=length;
	if(fabs(PRECISION-1)<EPS)
		filletSucces=fillet.Fillet(120,FILLET_INTEGER);
	else if(fabs(PRECISION-0.5)<EPS)
		filletSucces=fillet.Fillet(120,FILLET_HALF_INTEGER);
	else
		filletSucces=fillet.Fillet(120,FILLET_ONE_FLOAT);
	if(!filletSucces)
		logerr.Log("%s�˼���Y֫ĩβ��˨��λ�þ��Գߴ��ע��������г����쳣�����ܵ��¿�λ���Գߴ��ע��������߳ߴ��ע���Ȳ���ʵ",(char*)sPartNo);
	for(i=0;i<LsCirY.GetSize();i++)
		LsCirY[i].realPos.x=fillet.coordArr[i];
	ls_dim.dimStartPos.Set(0);
	real_ls_start.Set(0);
	prev_dist=0;
	//�ж��׿���˨�ı�ע���� wjh-2011.8.15
	iFirstLsDimRow=1;	//Ĭ���׿���˨���ڵ�һ��
	if(LsCirY.GetSize()>0)
	{
		int g1=ftoi(LsCirY[0].realPos.y);
		for(i=1;i<LsCirY.GetSize();i++)
		{
			int g_n=ftoi(LsCirY[i].realPos.y);
			if(g_n==g1)
				continue;
			if(g_n<g1)
			{
				iFirstLsDimRow=2;	//��һ��������˨��ͬ�ĵ���˨���������˨λ�ڵ�λ������˨��ڶ���
				break;
			}
			else if(g_n>g1)
			{
				iFirstLsDimRow=1;	//��һ��������˨��ͬ�ĵ���˨���������˨λ�ڸ�λ������˨���һ��
				break;
			}
		}
	}
	for(i=0;i<LsCirY.GetSize();i++)
	{
		real_ls_end = LsCirY[i].realPos;
		ls_dim.dist = fillet.distArr[i];
		//���ܴ��ڸ�����˨���,��Ϊ���������ݵ�ҲӦ�ü��ݣ��ʴ˴�ʹ��fabs() wht 11-06-02
		if(fabs(ls_dim.dist)<PRECISION)	//С��Ԥ��������Ϊ��ͬһ��
			continue;
		prev_dist+=ls_dim.dist;
		ls_dim.iTextDimSylte=0;
		ls_dim.dimPos.Set(0,wingWide+1,0);
		ls_dim.dimStartPos.y = wingWide;
		ls_dim.dimEndPos = LsCirY[i].realPos;//.dimCircle.centre;
		ls_dim.dimEndPos.y = wingWide;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
		{	//�׾�Y����ֱ��ע
			ls_txt_dimY.dist=ls_dim.dist;
			ls_txt_dimY.dimPos=ls_dim.dimEndPos;
			if(i==0)
			{
				ls_txt_dimY.iOrder=iFirstLsDimRow;
				if(ls_txt_dimY.iOrder==2)
					ls_txt_dimY.dimPos.y+=10;
			}
			else
			{
				int g=ftoi(LsCirY[i].realPos.y);
				int g_p=ftoi(LsCirY[i-1].realPos.y);
				if(g==g_p)	//ͬǰһ����˨��ע����
				{
					ls_txt_dimY.dimPos.y=prevLsPosDimY.dimPos.y;
					ls_txt_dimY.iOrder=prevLsPosDimY.iOrder;
				}
				else if(g>g_p)
				{
					ls_txt_dimY.dimPos.y+=10;
					ls_txt_dimY.iOrder=2; //iOrder==2��ʾY֫�ڶ�����˨����Գߴ�
				}
				else //if(g<g_p)
					ls_txt_dimY.iOrder=1; //iOrder==-1��ʾX֫��һ����˨����Գߴ�
			}
			/*if(LsCirY[i].iRow==2)	//�����ڶ�����˨�ı�עλ�� wht 11-05-07
			{
				ls_txt_dimY.dimPos.y+=10;
				ls_txt_dimY.iOrder=2; //iOrder==2��ʾY֫�ڶ�����˨����Գߴ�
			}
			else 
				ls_txt_dimY.iOrder=1; //iOrder==1��ʾY֫��һ����˨����Գߴ�*/
			LsCirY[i].pDimSpaceText=pure_txt_arr.append(ls_txt_dimY);
			//�˴�����ʹ��ָ�룬��Ϊ�������pure_txt_arr.append(pure_txt_dimX)������
			//�������·����ڴ浼��pPrevLsPosDimXָ����ڴ汻�ͷ� wht 12-03-13
			//pPrevLsPosDimY=&pure_txt_arr[pure_txt_arr.GetSize()-1];
			prevLsPosDimY=pure_txt_arr[pure_txt_arr.GetSize()-1];
		}
		//�˴�Ӧ��֤dim_arr.GetSize()>0 wht 11-06-02
		else if(i>1&&dim_arr.GetSize()>0&&dim_arr[dim_arr.GetSize()-1].dist==ls_dim.dist&&ls_dim.dist<150)
		{	//�׾�X����ֱ��עʱ,�����ظ����ֵĿ׾��עҪ���кϲ�
			dim_arr[dim_arr.GetSize()-1].nHits++;
			dim_arr[dim_arr.GetSize()-1].dimEndPos=ls_dim.dimEndPos;
			LsCirY[i].pDimSpaceSize=dim_arr.GetTail();
		}
		else
		{	//������X��׾��ע
			ls_dim.nHits=1;
			LsCirY[i].pDimSpaceSize=dim_arr.append(ls_dim);
		}

		ls_dim.dimStartPos = ls_dim.dimEndPos;
		real_ls_start = real_ls_end;
	}
	for(i=0;i<LsCirY.GetSize();i++)
	{
		if(fabs(fillet.distArr[i])<PRECISION)	//С��Ԥ��������Ϊ��ͬһ��
			continue;
		//bMergeLsAbsoluteDist==TRUEʱ������ڽ��Ⱦ���˨ֻ�����һ����˨��X(��)����Գߴ�  wxc 14-5-5
		if( drawPara.bMergeLsAbsoluteDist&&
			LsCirY[i].pDimSpaceSize&&LsCirY[i].pDimSpaceSize->nHits>1&&
			LsCirY[i].dimCircle.centre.x<LsCirY[i].pDimSpaceSize->dimEndPos.x-PRECISION)
			continue;	//����ڽ��Ⱦ���˨ֻ�����һ����˨��X(��)����Գߴ�
		pure_txt_dimY.dist=fillet.coordArr[i];
		pure_txt_dimY.dimPos = LsCirY[i].dimCircle.centre;//ls_dim.dimEndPos;
		pure_txt_dimY.dimPos.y=-wingWide-10;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&m_nWingAxisGX>1)
			pure_txt_dimX.dimPos.y-=10;	//�����б�ע��˨��࣬��Ҫ�������Գߴ��עλ�� wht 11-05-07
		pure_txt_arr.append(pure_txt_dimY);
	}
	if(LsCirY.GetSize()>0)
	{
		pure_txt_dimY.dist=fillet.coordArr[LsCirY.GetSize()];
		ls_dim.iTextDimSylte=0;
		ls_dim.dist = fillet.distArr[LsCirY.GetSize()];
		ls_dim.dimPos.Set(0,wingWide+1,0);
		ls_dim.dimStartPos.y = wingWide;
		ls_dim.dimEndPos.Set(length,wingWide);
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
		{	//�׾�Y����ֱ��ע
			ls_txt_dimY.dist=ls_dim.dist;
			ls_txt_dimY.dimPos=ls_dim.dimEndPos;
			ls_txt_dimY.iOrder=1;	//�˾��ڵ�һ�ű�ע
			pure_txt_arr.append(ls_txt_dimY);
		}
		/*else if(dim_arr[dim_arr.GetSize()-1].dist==ls_dim.dist)
		{	//�׾�X����ֱ��עʱ,�����ظ����ֵĿ׾��עҪ���кϲ�
			dim_arr[dim_arr.GetSize()-1].nHits++;
			dim_arr[dim_arr.GetSize()-1].dimEndPos=ls_dim.dimEndPos;
		}*/
		else
		{	//������X��׾��ע
			ls_dim.nHits=1;
			dim_arr.append(ls_dim);
		}
		pure_txt_dimY.dimPos = ls_dim.dimEndPos;
		pure_txt_dimY.dimPos.y=wingWide+20;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&m_nWingAxisGY>1)
			pure_txt_dimY.dimPos.y+=10;	//�����б�ע��˨��࣬��Ҫ�������Գߴ��עλ�� wht 11-05-07
		pure_txt_arr.append(pure_txt_dimY);
	}
}
void CRodDrawing::CreateJgLsCircleList()
{
	int i;
	for(i=0;i<LsCirX.GetSize();i++)
	{
		LsCirX[i].dimCircle.centre.x=TransCoordX(LsCirX[i].realPos.x);
		DimLsZhun(LsCirX[i].dimCircle.centre);
	}
	for(i=0;i<LsCirY.GetSize();i++)
	{
		LsCirY[i].dimCircle.centre.x=TransCoordX(LsCirY[i].realPos.x);
		DimLsZhun(LsCirY[i].dimCircle.centre);
	}
#ifdef lkalsdjlsadfk
	CSizeTextDim ls_dim;
	if(drawPara.iDimPrecision==0)	//��ȷ��1.0mm
		PRECISION=1;
	else if(drawPara.iDimPrecision==1)	//��ȷ��0.5mm
		PRECISION=0.5;
	else if(drawPara.iDimPrecision==2)	//��ȷ��0.1mm
		PRECISION=0.1;
	CSizeTextDim::PRECISION = PRECISION;
	CSizeTextDim::dimStyle.style=DimStyleTable::dimStyle;
	CSizeTextDim::dimStyle.textHeight=drawPara.fDimTextSize;
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongX))
		ls_dim.angle = 0;

	CTextOnlyDrawing pure_txt_dimX,pure_txt_dimY,ls_txt_dimX,ls_txt_dimY,prevLsPosDimX,prevLsPosDimY;
	CTextOnlyDrawing::PRECISION = PRECISION;
	double lengthEndDrawPosX= TransCoordX(length);
	pure_txt_dimX.bInXWing = TRUE;
	pure_txt_dimY.bInXWing = FALSE;
	pure_txt_dimX.dist = 0;
	pure_txt_dimY.dist = 0;
	pure_txt_dimX.angle = Pi/2;
	pure_txt_dimY.angle = Pi/2;
	pure_txt_dimX.alignType=MiddleRight;
	pure_txt_dimY.alignType=MiddleLeft;
	pure_txt_dimX.textHeight=drawPara.fDimTextSize;
	pure_txt_dimY.textHeight=drawPara.fDimTextSize;
	pure_txt_dimX.iOrder=-3;	//iOrder==-3��ʾX֫��˨���Գߴ磬��ǰ��-2��ʾ wht 11-05-07 
	pure_txt_dimY.iOrder=3;		//iOrder== 3��ʾY֫��˨���Գߴ磬��ǰ�� 2��ʾ wht 11-05-07
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
	{
		ls_txt_dimX.alignType=MiddleRight;
		ls_txt_dimX.bInXWing=TRUE;
		ls_txt_dimY.alignType=MiddleLeft;
		ls_txt_dimY.bInXWing=FALSE;
		ls_txt_dimX.angle = Pi/2;
		ls_txt_dimY.angle = Pi/2;
		ls_txt_dimX.textHeight = drawPara.fDimTextSize;
		ls_txt_dimY.textHeight = drawPara.fDimTextSize;
		ls_txt_dimX.iOrder=-1;	//iOrder==-1��ʾX֫��һ����˨����Գߴ�
		ls_txt_dimY.iOrder= 1;	//iOrder== 1��ʾY֫��һ����˨����Գߴ�
	}
	
	if(dimStyleFlag.IsHasFlag(BriefDrawing))
		return;
	CXhChar100 sDimText;
	f3dPoint real_ls_start,real_ls_end;
	double prev_dist=0;
	ls_dim.dimStartPos.Set();
	//Բ����λ���׾��ע
	CDistFillet fillet;
	fillet.coordArr.SetSize(LsCirX.GetSize()+1);
	for(i=0;i<LsCirX.GetSize();i++)
		fillet.coordArr[i]=LsCirX[i].realPos.x;
	fillet.coordArr[i]=length;
	if(fabs(PRECISION-1)<EPS)
		fillet.Fillet(120,FILLET_INTEGER);
	else if(fabs(PRECISION-0.5)<EPS)
		fillet.Fillet(120,FILLET_HALF_INTEGER);
	else
		fillet.Fillet(120,FILLET_ONE_FLOAT);
	//����Բ����Ŀ�λ���׾�������Ӧ��ע
	//�ж��׿���˨�ı�ע���� wjh-2011.8.15
	int iFirstLsDimRow=-1;	//Ĭ���׿���˨���ڵ�һ�� 
	if(LsCirX.GetSize()>0)
	{
		int g1=ftoi(fabs(LsCirX[0].realPos.y));
		for(i=1;i<LsCirX.GetSize();i++)
		{
			int g_n=ftoi(fabs(LsCirX[i].realPos.y));
			if(g_n==g1)
				continue;
			if(g_n<g1)
			{
				iFirstLsDimRow=-2;	//��һ��������˨��ͬ�ĵ���˨���������˨λ�ڵ�λ������˨��ڶ���
				break;
			}
			else if(g_n>g1)
			{
				iFirstLsDimRow=-1;	//��һ��������˨��ͬ�ĵ���˨���������˨λ�ڸ�λ������˨���һ��
				break;
			}
		}
	}
	for(i=0;i<LsCirX.GetSize();i++)
	{
		ls_dim.dimEndPos = LsCirX[i].dimCircle.centre;
		real_ls_end = LsCirX[i].realPos;
		ls_dim.dist = fillet.distArr[i];
		if(fabs(ls_dim.dist)<PRECISION)	//С��Ԥ��������Ϊ��ͬһ��
			continue;
		pure_txt_dimX.dist=fillet.coordArr[i];
		
		ls_dim.dimPos.Set(0,-wingWide-1,0);
		ls_dim.dimStartPos.y = -wingWide;
		ls_dim.dimEndPos.y = -wingWide;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
		{	//�׾�Y����ֱ��ע
			ls_txt_dimX.dist=ls_dim.dist;
			ls_txt_dimX.dimPos=ls_dim.dimEndPos;
			if(i==0)
			{
				ls_txt_dimX.iOrder=iFirstLsDimRow;
				if(ls_txt_dimX.iOrder==-2)
					ls_txt_dimX.dimPos.y-=10;
			}
			else
			{
				CRodDrawing::LsMark *pPrevLsMark=&LsCirX[i-1];
				int g=ftoi(fabs(LsCirX[i].realPos.y));
				int g_p=ftoi(fabs(LsCirX[i-1].realPos.y));
				if(g==g_p)	//ͬǰһ����˨��ע����
				{
					ls_txt_dimX.dimPos.y=prevLsPosDimX.dimPos.y;
					ls_txt_dimX.iOrder=prevLsPosDimX.iOrder;
				}
				else if(g>g_p)
				{
					ls_txt_dimX.dimPos.y-=10;
					ls_txt_dimX.iOrder=-2; //iOrder==-2��ʾX֫�ڶ�����˨����Գߴ�
				}
				else //if(g<g_p)
					ls_txt_dimX.iOrder=-1; //iOrder==-1��ʾX֫��һ����˨����Գߴ�
			}
			/*if(LsCirX[i].iRow==2)	//�����ڶ�����˨�ı�עλ�� wht 11-05-07
			{
				ls_txt_dimX.dimPos.y-=10;
				ls_txt_dimX.iOrder=-2; //iOrder==-2��ʾX֫�ڶ�����˨����Գߴ�
			}
			else 
				ls_txt_dimX.iOrder=-1; //iOrder==-1��ʾX֫��һ����˨����Գߴ�*/
			pure_txt_arr.append(ls_txt_dimX);
			//�˴�����ʹ��ָ�룬��Ϊ�������pure_txt_arr.append(pure_txt_dimX)������
			//�������·����ڴ浼��pPrevLsPosDimXָ����ڴ汻�ͷ� wht 12-03-13
			//pPrevLsPosDimX=&pure_txt_arr[pure_txt_arr.GetSize()-1];
			prevLsPosDimX=pure_txt_arr[pure_txt_arr.GetSize()-1];
		}
		else if(i>0&&dim_arr.GetSize()>0&&dim_arr[dim_arr.GetSize()-1].dist==ls_dim.dist)
		{	//�׾�X����ֱ��עʱ,�����ظ����ֵĿ׾��עҪ���кϲ�
			dim_arr[dim_arr.GetSize()-1].nHits++;
			dim_arr[dim_arr.GetSize()-1].dimEndPos=ls_dim.dimEndPos;
		}
		else
		{	//������X��׾��ע
			ls_dim.nHits=1;
			dim_arr.append(ls_dim);
		}
		pure_txt_dimX.dimPos = ls_dim.dimEndPos;
		pure_txt_dimX.dimPos.y=-wingWide-10;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&m_nWingAxisGX>1)
			pure_txt_dimX.dimPos.y-=10;	//�����б�ע��˨��࣬��Ҫ�������Գߴ��עλ�� wht 11-05-07
		prev_dist+=ls_dim.dist;
		ls_dim.dimStartPos = ls_dim.dimEndPos;
		real_ls_start = real_ls_end;
		pure_txt_arr.append(pure_txt_dimX);
	}
	if(LsCirX.GetSize()>0)
	{
		pure_txt_dimX.dist=fillet.coordArr[LsCirX.GetSize()];	
		ls_dim.dist = fillet.distArr[LsCirX.GetSize()];
		ls_dim.dimEndPos.Set(lengthEndDrawPosX,-wingWide);
		ls_dim.dimPos.Set(0,-wingWide-1,0);
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
		{	//�׾�Y����ֱ��ע
			ls_txt_dimX.dist=ls_dim.dist;
			ls_txt_dimX.dimPos=ls_dim.dimEndPos;
			ls_txt_dimX.iOrder=-1;	//�˾��ڵ�һ�ű�ע
			pure_txt_arr.append(ls_txt_dimX);
		}
		else if(dim_arr[dim_arr.GetSize()-1].dist==ls_dim.dist)
		{	//�׾�X����ֱ��עʱ,�����ظ����ֵĿ׾��עҪ���кϲ�
			dim_arr[dim_arr.GetSize()-1].nHits++;
			dim_arr[dim_arr.GetSize()-1].dimEndPos=ls_dim.dimEndPos;
		}
		else
		{	//������X��׾��ע
			ls_dim.nHits=1;
			dim_arr.append(ls_dim);
		}
		pure_txt_dimX.dimPos = ls_dim.dimEndPos;
		pure_txt_dimX.dimPos.y=-wingWide-10;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&m_nWingAxisGX>1)
			pure_txt_dimX.dimPos.y-=10;	//�����б�ע��˨��࣬��Ҫ�������Գߴ��עλ�� wht 11-05-07
		pure_txt_arr.append(pure_txt_dimX);
	}

	//Բ����λ���׾��ע
	fillet.coordArr.SetSize(LsCirY.GetSize()+1);
	for(i=0;i<LsCirY.GetSize();i++)
		fillet.coordArr[i]=LsCirY[i].realPos.x;
	fillet.coordArr[i]=length;
	if(fabs(PRECISION-1)<EPS)
		fillet.Fillet(120,FILLET_INTEGER);
	else if(fabs(PRECISION-0.5)<EPS)
		fillet.Fillet(120,FILLET_HALF_INTEGER);
	else
		fillet.Fillet(120,FILLET_ONE_FLOAT);
	ls_dim.dimStartPos.Set(0);
	real_ls_start.Set(0);
	prev_dist=0;
	//�ж��׿���˨�ı�ע���� wjh-2011.8.15
	iFirstLsDimRow=1;	//Ĭ���׿���˨���ڵ�һ��
	if(LsCirY.GetSize()>0)
	{
		int g1=ftoi(LsCirY[0].realPos.y);
		for(i=1;i<LsCirY.GetSize();i++)
		{
			int g_n=ftoi(LsCirY[i].realPos.y);
			if(g_n==g1)
				continue;
			if(g_n<g1)
			{
				iFirstLsDimRow=2;	//��һ��������˨��ͬ�ĵ���˨���������˨λ�ڵ�λ������˨��ڶ���
				break;
			}
			else if(g_n>g1)
			{
				iFirstLsDimRow=1;	//��һ��������˨��ͬ�ĵ���˨���������˨λ�ڸ�λ������˨���һ��
				break;
			}
		}
	}
	for(i=0;i<LsCirY.GetSize();i++)
	{
		real_ls_end = LsCirY[i].realPos;
		ls_dim.dist = fillet.distArr[i];
		//���ܴ��ڸ�����˨���,��Ϊ���������ݵ�ҲӦ�ü��ݣ��ʴ˴�ʹ��fabs() wht 11-06-02
		if(fabs(ls_dim.dist)<PRECISION)	//С��Ԥ��������Ϊ��ͬһ��
			continue;
		pure_txt_dimY.dist=fillet.coordArr[i];
		prev_dist+=ls_dim.dist;
		ls_dim.dimPos.Set(0,wingWide+1,0);
		ls_dim.dimStartPos.y = wingWide;
		ls_dim.dimEndPos = LsCirY[i].dimCircle.centre;
		ls_dim.dimEndPos.y = wingWide;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
		{	//�׾�Y����ֱ��ע
			ls_txt_dimY.dist=ls_dim.dist;
			ls_txt_dimY.dimPos=ls_dim.dimEndPos;
			if(i==0)
			{
				ls_txt_dimY.iOrder=iFirstLsDimRow;
				if(ls_txt_dimY.iOrder==2)
					ls_txt_dimY.dimPos.y+=10;
			}
			else
			{
				int g=ftoi(LsCirY[i].realPos.y);
				int g_p=ftoi(LsCirY[i-1].realPos.y);
				if(g==g_p)	//ͬǰһ����˨��ע����
				{
					ls_txt_dimY.dimPos.y=prevLsPosDimY.dimPos.y;
					ls_txt_dimY.iOrder=prevLsPosDimY.iOrder;
				}
				else if(g>g_p)
				{
					ls_txt_dimY.dimPos.y+=10;
					ls_txt_dimY.iOrder=2; //iOrder==2��ʾY֫�ڶ�����˨����Գߴ�
				}
				else //if(g<g_p)
					ls_txt_dimY.iOrder=1; //iOrder==-1��ʾX֫��һ����˨����Գߴ�
			}
			/*if(LsCirY[i].iRow==2)	//�����ڶ�����˨�ı�עλ�� wht 11-05-07
			{
				ls_txt_dimY.dimPos.y+=10;
				ls_txt_dimY.iOrder=2; //iOrder==2��ʾY֫�ڶ�����˨����Գߴ�
			}
			else 
				ls_txt_dimY.iOrder=1; //iOrder==1��ʾY֫��һ����˨����Գߴ�*/
			pure_txt_arr.append(ls_txt_dimY);
			//�˴�����ʹ��ָ�룬��Ϊ�������pure_txt_arr.append(pure_txt_dimX)������
			//�������·����ڴ浼��pPrevLsPosDimXָ����ڴ汻�ͷ� wht 12-03-13
			//pPrevLsPosDimY=&pure_txt_arr[pure_txt_arr.GetSize()-1];
			prevLsPosDimY=pure_txt_arr[pure_txt_arr.GetSize()-1];
		}
		//�˴�Ӧ��֤dim_arr.GetSize()>0 wht 11-06-02
		else if(i>0&&dim_arr.GetSize()>0&&dim_arr[dim_arr.GetSize()-1].dist==ls_dim.dist)
		{	//�׾�X����ֱ��עʱ,�����ظ����ֵĿ׾��עҪ���кϲ�
			dim_arr[dim_arr.GetSize()-1].nHits++;
			dim_arr[dim_arr.GetSize()-1].dimEndPos=ls_dim.dimEndPos;
		}
		else
		{	//������X��׾��ע
			ls_dim.nHits=1;
			dim_arr.append(ls_dim);
		}

		ls_dim.dimStartPos = ls_dim.dimEndPos;
		real_ls_start = real_ls_end;
		pure_txt_dimY.dimPos = ls_dim.dimEndPos;
		pure_txt_dimY.dimPos.y=wingWide+20;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&m_nWingAxisGY>1)
			pure_txt_dimY.dimPos.y+=10;	//�����б�ע��˨��࣬��Ҫ�������Գߴ��עλ�� wht 11-05-07
		pure_txt_arr.append(pure_txt_dimY);
	}
	if(LsCirY.GetSize()>0)
	{
		pure_txt_dimY.dist=fillet.coordArr[LsCirY.GetSize()];
		ls_dim.dist = fillet.distArr[LsCirY.GetSize()];
		ls_dim.dimPos.Set(0,wingWide+1,0);
		ls_dim.dimStartPos.y = wingWide;
		ls_dim.dimEndPos.Set(lengthEndDrawPosX,wingWide);
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
		{	//�׾�Y����ֱ��ע
			ls_txt_dimY.dist=ls_dim.dist;
			ls_txt_dimY.dimPos=ls_dim.dimEndPos;
			ls_txt_dimY.iOrder=1;	//�˾��ڵ�һ�ű�ע
			pure_txt_arr.append(ls_txt_dimY);
		}
		else if(dim_arr[dim_arr.GetSize()-1].dist==ls_dim.dist)
		{	//�׾�X����ֱ��עʱ,�����ظ����ֵĿ׾��עҪ���кϲ�
			dim_arr[dim_arr.GetSize()-1].nHits++;
			dim_arr[dim_arr.GetSize()-1].dimEndPos=ls_dim.dimEndPos;
		}
		else
		{	//������X��׾��ע
			ls_dim.nHits=1;
			dim_arr.append(ls_dim);
		}
		pure_txt_dimY.dimPos = ls_dim.dimEndPos;
		pure_txt_dimY.dimPos.y=wingWide+20;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&m_nWingAxisGY>1)
			pure_txt_dimY.dimPos.y+=10;	//�����б�ע��˨��࣬��Ҫ�������Գߴ��עλ�� wht 11-05-07
		pure_txt_arr.append(pure_txt_dimY);
	}
#endif
}
//TODO:�Ǻ��߼���
BOOL CAngleDrawing::GetMidFlatPos(f3dPoint &startPos,f3dPoint &endPos,f3dPoint &midPos,int wing_x0_y1)
{
	if(wing_x0_y1==0)
	{
		for(int i=0;i<LsCirX.GetSize();i++)
		{
			if(LsCirX[i].cFuncType==0xFF)
			{
				midPos=LsCirX[i].dimCircle.centre;
				return TRUE;
			}
		}
	}
	else if(wing_x0_y1==1)
	{
		for(int i=0;i<LsCirY.GetSize();i++)
		{
			if(LsCirY[i].cFuncType==0xFF)
			{
				midPos=LsCirY[i].dimCircle.centre;
				return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL CAngleDrawing::FindNearLsMark(int* piWingX0Y1,double realCoordX,BOOL bLeft,LsMark* pOutBoltMark)
{
	BOOL bFindInWingX=FALSE,bFindInWingY=FALSE;
	LsMark xWingXOutBolt,xWingYOutBolt;
	if(LsCirX.GetSize()>0)
	{
		for(int i=0;i<LsCirX.GetSize();i++)
		{
			if(realCoordX<=LsCirX[i].realPos.x)
			{
				if(bLeft && i>0)
				{
					xWingXOutBolt.realPos=LsCirX[i-1].realPos;
					//xWingXOutBolt.wingX0Y1=wingX0Y1;
					xWingXOutBolt.dimCircle.centre=LsCirX[i-1].dimCircle.centre;
					xWingXOutBolt.dimCircle.radius=LsCirX[i-1].dimCircle.radius;
					bFindInWingX=TRUE;
					break;
				}
				if(!bLeft)
				{
					xWingXOutBolt.realPos=LsCirX[i].realPos;
					//xWingXOutBolt.wingX0Y1=wingX0Y1;
					xWingXOutBolt.dimCircle.centre=LsCirX[i].dimCircle.centre;
					xWingXOutBolt.dimCircle.radius=LsCirX[i].dimCircle.radius;
					bFindInWingX=TRUE;
					break;
				}
			}
		}
		if(bLeft&&!bFindInWingX)	
		{	//ȡ���һ����˨��Ϊ���������˨
			xWingXOutBolt.realPos=LsCirX[LsCirX.GetSize()-1].realPos;
			//xWingXOutBolt.wingX0Y1=wingX0Y1;
			xWingXOutBolt.dimCircle.centre=LsCirX[LsCirX.GetSize()-1].dimCircle.centre;
			xWingXOutBolt.dimCircle.radius=LsCirX[LsCirX.GetSize()-1].dimCircle.radius;
			bFindInWingX=TRUE;
		}
	}
	if(LsCirY.GetSize()>0)
	{
		for(int i=0;i<LsCirY.GetSize();i++)
		{
			if(realCoordX<=LsCirY[i].realPos.x)
			{
				if(bLeft && i>0)
				{
					xWingYOutBolt.realPos=LsCirY[i-1].realPos;
					//xWingYOutBolt.wingX0Y1=wingX0Y1;
					xWingYOutBolt.dimCircle.centre=LsCirY[i-1].dimCircle.centre;
					xWingYOutBolt.dimCircle.radius=LsCirY[i-1].dimCircle.radius;
					bFindInWingY=TRUE;
					break;
				}
				if(!bLeft)
				{
					xWingYOutBolt.realPos=LsCirY[i].realPos;
					//xWingYOutBolt.wingX0Y1=wingX0Y1;
					xWingYOutBolt.dimCircle.centre=LsCirY[i].dimCircle.centre;
					xWingYOutBolt.dimCircle.radius=LsCirY[i].dimCircle.radius;
					bFindInWingY=TRUE;
					break;
				}
			}
		}
		if(bLeft&&!bFindInWingY)	
		{	//ȡ���һ����˨��Ϊ���������˨
			xWingYOutBolt.realPos=LsCirY[LsCirY.GetSize()-1].realPos;
			//xWingYOutBolt.wingX0Y1=wingX0Y1;
			xWingYOutBolt.dimCircle.centre=LsCirY[LsCirY.GetSize()-1].dimCircle.centre;
			xWingYOutBolt.dimCircle.radius=LsCirY[LsCirY.GetSize()-1].dimCircle.radius;
			bFindInWingY=TRUE;
		}
	}
	if(bFindInWingX&&!bFindInWingY)
	{
		*pOutBoltMark=xWingXOutBolt;
		*piWingX0Y1=0;
	}
	else if(!bFindInWingX&&bFindInWingY)
	{
		*pOutBoltMark=xWingYOutBolt;
		*piWingX0Y1=1;
	}
	else if(bFindInWingX&&bFindInWingY)
	{
		if(bLeft)	//���X����ϴ��߾����߰����
		{
			*pOutBoltMark=xWingXOutBolt.realPos.x>xWingYOutBolt.realPos.x?xWingXOutBolt:xWingYOutBolt;
			*piWingX0Y1	 =xWingXOutBolt.realPos.x>xWingYOutBolt.realPos.x?0:1;
		}
		else		//�Ҳ�X�����С�߾����߰����
		{
			*pOutBoltMark=xWingXOutBolt.realPos.x<xWingYOutBolt.realPos.x?xWingXOutBolt:xWingYOutBolt;
			*piWingX0Y1	 =xWingXOutBolt.realPos.x<xWingYOutBolt.realPos.x?0:1;
		}
	}
	else
		return FALSE;
	return TRUE;
}
//
double CAngleDrawing::GetBoltMaxY(int wingX0Y1)
{
	double fMaxY=0;
	if(wingX0Y1==0)
	{
		for(int i=0;i<LsCirX.GetSize();i++)
		{
			if(fabs(LsCirX[i].realPos.y)>fMaxY)
				fMaxY=fabs(LsCirX[i].realPos.y);
		}
		return fMaxY*-1;
	}
	else
	{
		for(int i=0;i<LsCirY.GetSize();i++)
		{
			if(fabs(LsCirY[i].realPos.y)>fMaxY)
				fMaxY=fabs(LsCirY[i].realPos.y);
		}
		return fMaxY;
	}
}
BOOL CAngleDrawing::IsLsMarkPosOverlap(f3dPoint ls_pos)
{
	for(int i=0;i<rib_mark_arr.GetNodeNum();i++)
	{
		if(ls_pos.IsEqual(rib_mark_arr[i].realPos))
			return TRUE;
	}
	return FALSE;
}
//���ɽǸ��߰�Ķ�λ��Ϣ
void CAngleDrawing::CreateJgRibPlateDimList(ATOM_LIST<RIB_PLATE_INFO> &rib_plate_list)
{
	if(!drawPara.bDimRibPlateSetUpPos)
		return;
	//ͳ�ƽǸ��Ϻ��ӵĽǸ��߰���Ϣ
	LsMark mark,ls_mark,*pNowMark=NULL;
	//TODO:����wingX0Y1��ʶ���㷨 wjh-2017.3.10
	int iPreferWingX0Y1=LsCirX.GetSize()>LsCirY.GetSize()?0:1;
	double fMaxY=GetBoltMaxY(mark.wingX0Y1);
	for(RIB_PLATE_INFO *pRibPlateInfo=rib_plate_list.GetFirst();pRibPlateInfo;pRibPlateInfo=rib_plate_list.GetNext())
	{	
		//��¼�߰�������Ҳ�㣨���ڻ����߰������ߣ�
		mark.realPos.Set(pRibPlateInfo->fPosZ-pRibPlateInfo->fThick/2,fMaxY,0);
		mark.dimCircle.centre=mark.realPos;
		mark.dimCircle.radius=-1;
		pNowMark=rib_mark_arr.append(mark);
		LsCenterArr.append(pNowMark);
		//
		mark.realPos.Set(pRibPlateInfo->fPosZ+pRibPlateInfo->fThick/2,fMaxY,0);
		mark.dimCircle.centre=mark.realPos;
		mark.dimCircle.radius=-1;
		pNowMark=rib_mark_arr.append(mark);
		LsCenterArr.append(pNowMark);
		//��¼�߰��м�λ�ü�������˨�����ڱ�ע�߰�λ�ã�
		mark.realPos.Set(pRibPlateInfo->fPosZ,fMaxY,0);
		mark.realPos.feature=(int)pRibPlateInfo->fThick;	//��¼�߰���
		mark.dimCircle.centre=mark.realPos;
		mark.dimCircle.radius=0;
		pNowMark=rib_mark_arr.append(mark);
		if(FindNearLsMark(&mark.wingX0Y1,mark.realPos.x,TRUE,&ls_mark)&&!IsLsMarkPosOverlap(ls_mark.realPos))
			rib_mark_arr.append(ls_mark);
		if(FindNearLsMark(&mark.wingX0Y1,mark.realPos.x,FALSE,&ls_mark)&&!IsLsMarkPosOverlap(ls_mark.realPos))
			rib_mark_arr.append(ls_mark);
		pNowMark->wingX0Y1=mark.wingX0Y1;
	}
	CHeapSort<LsMarkPtr>::HeapSort(rib_mark_arr.m_pIndexTbl,rib_mark_arr.GetSize(),compareLsMark);
	CHeapSort<LsMarkPtr>::HeapSort(LsCenterArr.m_pData,LsCenterArr.GetSize(),compareLsMark);
	if(dimStyleFlag.IsHasFlag(BriefDrawing))	//���ƽǸּ�ͼ
		return;
	CSizeTextDim::PRECISION = 0.5;
#ifdef __DRAWING_CONTEXT_
	CSizeTextDim::dimStyle.style=DimStyleTable::dimStyle;
	CSizeTextDim::dimStyle.textHeight=drawPara.fDimTextSize;
#endif
	CSizeTextDim size_dim;
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongX))
		size_dim.angle = 0;
	//����Ǹ��߰�λ�ã����ɱ�ע
	ARRAYPTR_LIST<LsMark> tem_mark_Arr;
	for(int i=0;i<rib_mark_arr.GetSize();i++)
	{
		if(rib_mark_arr[i].dimCircle.radius==-1)
			continue;
		tem_mark_Arr.append(rib_mark_arr[i]);
	}
	f3dPoint leftLs,leftPt,rightPt,rightLs;
	double fDist1=0,fDist2=0;
	int nMarkNum=tem_mark_Arr.GetSize();
	for(int i=0;i<nMarkNum;i++)
	{
		if(tem_mark_Arr[i].dimCircle.radius!=0)
			continue;
		leftPt.Set(tem_mark_Arr[i].realPos.x-tem_mark_Arr[i].realPos.feature/2,fMaxY,0);
		rightPt.Set(tem_mark_Arr[i].realPos.x+tem_mark_Arr[i].realPos.feature/2,fMaxY,0);
		if(i==0)
		{	//ʼ���߰�
			leftLs.Set(0,fMaxY,0);
			if(tem_mark_Arr[i+1].dimCircle.radius==0)
				rightLs.Set(tem_mark_Arr[i+1].realPos.x-tem_mark_Arr[i+1].realPos.feature/2,fMaxY,0);
			else
				rightLs.Set(tem_mark_Arr[i+1].realPos.x,fMaxY,0);
			fDist1=leftPt.x-leftLs.x;
			fDist2=rightLs.x-rightPt.x;
		}
		else if(i==nMarkNum-1)
		{	//�ն��߰�
			rightLs.Set(length,fMaxY,0);
			if(tem_mark_Arr[i-1].dimCircle.radius==0)
				leftLs.Set(tem_mark_Arr[i-1].realPos.x+tem_mark_Arr[i-1].realPos.feature/2,fMaxY,0);
			else
				leftLs.Set(tem_mark_Arr[i-1].realPos.x,fMaxY,0);
			fDist1=leftPt.x-leftLs.x;
			fDist2=rightLs.x-rightPt.x;
		}
		else if(tem_mark_Arr[i-1].dimCircle.radius>0 && tem_mark_Arr[i+1].dimCircle.radius>0)
		{	//�߰�λ��������˨֮��(����ʵ�ʱ�������Բ�����߰�λ��)
			leftLs.Set(tem_mark_Arr[i-1].realPos.x,fMaxY,0);
			rightLs.Set(tem_mark_Arr[i+1].realPos.x,fMaxY,0);
			double fSumDist=tem_mark_Arr[i+1].dimCircle.centre.x-tem_mark_Arr[i-1].dimCircle.centre.x;
			double fLeftDist=tem_mark_Arr[i].realPos.x-tem_mark_Arr[i-1].dimCircle.centre.x;
			double fScale=fLeftDist/fSumDist;
			fDist1=(rightLs.x-leftLs.x)*fScale-tem_mark_Arr[i].realPos.feature/2;
			fDist2=(rightLs.x-leftLs.x)*(1-fScale)-tem_mark_Arr[i].realPos.feature/2;
		}
		else
		{	//�߰岻λ��������˨֮��
			if(tem_mark_Arr[i-1].dimCircle.radius==0)
				leftLs.Set(tem_mark_Arr[i-1].realPos.x+tem_mark_Arr[i-1].realPos.feature/2,fMaxY,0);
			else
				leftLs.Set(tem_mark_Arr[i-1].realPos.x,fMaxY,0);
			if(tem_mark_Arr[i+1].dimCircle.radius==0)
				rightLs.Set(tem_mark_Arr[i+1].realPos.x-tem_mark_Arr[i+1].realPos.feature/2,fMaxY,0);
			else
				rightLs.Set(tem_mark_Arr[i+1].realPos.x,fMaxY,0);
			fDist1=leftPt.x-leftLs.x;
			fDist2=rightLs.x-rightPt.x;
		}
		if(tem_mark_Arr[i].wingX0Y1==0)	//X֫
			size_dim.iTextDimSylte=1;
		else							//Y֫
			size_dim.iTextDimSylte=0;
		size_dim.dimStartPos=leftLs;
		size_dim.dimEndPos=leftPt;
		size_dim.dimPos=(leftLs+leftPt)*0.5;	
		size_dim.dist=fDist1;
		rib_thick_arr.append(-tem_mark_Arr[i].realPos.feature/2);//��¼��Ӧ�߰����Ա���ڻ������߰������߱���һ���Դ��� wjh-2017.3.10
		rib_dim_arr.append(size_dim);
		size_dim.dimStartPos=rightPt;
		size_dim.dimEndPos=rightLs;
		size_dim.dimPos=(rightPt+rightLs)*0.5;
		size_dim.dist=fDist2;
		rib_thick_arr.append(tem_mark_Arr[i].realPos.feature/2);
		rib_dim_arr.append(size_dim);
	}
}
//���ɽǸ��Ϻ��ӵĽǸ��߰��������
void CAngleDrawing::CreateJgRibPlateEdge(ATOM_LIST<RIB_PLATE_INFO> &rib_plate_list)
{
	f3dLine left_line,right_line;
	//���Ǹ��߰嵱����˨��������
	for(RIB_PLATE_INFO *rib_plate_info=rib_plate_list.GetFirst();rib_plate_info;rib_plate_info=rib_plate_list.GetNext())
	{	//���ƽǸ��߰�������
		left_line.startPt.Set(0,wingWide);
		double anchorX=TransCoordX(rib_plate_info->fPosZ);
		left_line.startPt.x=anchorX-min(rib_plate_info->fThick*0.05,anchorX-TransCoordX(rib_plate_info->fPosZ-rib_plate_info->fThick*0.5));
		left_line.endPt.Set(left_line.startPt.x,-wingWide);
		left_line.feature=1;		//�ڲ���߰�϶���������
		lineArr.Add(left_line);	//�����
		right_line.startPt.Set(0,wingWide);
		right_line.startPt.x=anchorX+min(rib_plate_info->fThick*0.05,TransCoordX(rib_plate_info->fPosZ+rib_plate_info->fThick*0.5)-anchorX);
		right_line.endPt.Set(right_line.startPt.x,-wingWide);
		right_line.feature=1;		//�ڲ���߰�϶���������
		lineArr.Add(right_line);	//�ұ���
		//����߰���
		int wingX0Y1=LsCirX.GetSize()>LsCirY.GetSize()?0:1;
		CXhChar50 sMat,sThick,sPartNo(rib_plate_info->sPlateNo);
		if(drawPara.bDimRibPlateMaterial)
		{
			if(rib_plate_info->m_cMaterial!=CSteelMatLibrary::Q235BriefMark())
				QuerySteelMatMark(rib_plate_info->m_cMaterial,sMat);
			sThick.Printf("%s-%d",(char*)sMat,(int)rib_plate_info->fThick);
		}
		else
			sThick.Printf("-%d",(int)rib_plate_info->fThick);
		CTextFrame data_point;
		data_point.InitTextFrame(drawPara.fPartNoTextSize,0,-1,drawPara.fPartNoMargin);
		data_point.BuildFrame(sPartNo,sThick,0,drawPara.iPartNoFrameStyle);
		if(wingX0Y1==0)
			data_point.dataPos=(left_line.endPt+right_line.endPt)*0.5;
		else
			data_point.dataPos=(left_line.startPt+right_line.startPt)*0.5;
		data_point.dimPos=data_point.dataPos;
		part_no_arr.Add(data_point);
	}
}

//-----VVV-----��ͨ�Ǹ�(������֫���нǵĽǸ�)�Ļ���-----VVV------
double CRodDrawing::TransCutWingOrAngleDrawDist(double dist)
{
	if(dist<=0)
		return 0;
	CXhChar16 dimtxt("%.0f",dist);
	double drawlen=0.7*drawPara.fDimTextSize*dimtxt.GetLength();
	do{
		double drawdist=TransCoordY(dist);
		//TODO:Ϊʲô����drawdist>0?
		if(drawdist>0&&drawdist<drawlen)
			dist+=2;
		else
			break;
	}while(true);
	return dist;
}
	// --------- 1.2. ��֫ -----------
	/*cut_wing_para[0]��ʾʼ����֫cut_wing_para[1]��ʾ�ն���֫
	...[][0]��ʾ�Ǹ�������ȡ����
	...[][1]��ʾ��һ֫��ȡ���
	...[][2]��ʾ��֫������ȡ����
	*/
	//double cut_wing_para[2][3];
	//������֫����֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
	//int cut_wing[2];
    // --------- 1.3. �н� -----------
	/*
	cut_angle[0]��ʾ���X֫
	cut_angle[1]��ʾ���Y֫
	cut_angle[2]��ʾ�յ�X֫
	cut_angle[3]��ʾ�յ�Y֫
	...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
	*/
	//double cut_angle[4][2];
void CAngleDrawing::CreateJgProfile(double cut_wing_para_jg[2][3],long cut_wing[2],double cut_angle_jg[4][2],
							double wingWide,double wingThick,BOOL bIncStartDim/*=TRUE*/,BOOL bIncEndDim/*=TRUE*/)
{
	f3dLine line;
	CSizeTextDim jg_dim;
	//jg_dim.dimStyle.style=DimStyleTable::dimStyle;
	//jg_dim.dimStyle.textHeight = drawPara.fDimTextSize; 

	if(dimStyleFlag.IsHasFlag(BriefDrawing)&&wingThick<10)
		wingThick=10;
	//��ʼ���Ǹָ�����
	double lengthEndDrawPosX=TransCoordX(length);
	if(bIncStartDim)
	{
		edgeLineArr[0].startPt.Set(0,wingWide,0);
		edgeLineArr[1].startPt.Set(0,wingThick,0);
		edgeLineArr[2].startPt.Set(0,0,0);
		edgeLineArr[3].startPt.Set(0,-wingThick,0);
		edgeLineArr[4].startPt.Set(0,-wingWide,0);
	}
	if(bIncStartDim||bIncEndDim)
	{	//�˴��������bIncStartDim,��Ϊһ���ȵ���bIncStartDim,��ʱ����ն˲���ֵ�ڻ����н�ʱ���߽����޷�����
		edgeLineArr[0].endPt.Set(lengthEndDrawPosX,wingWide,0);
		edgeLineArr[1].endPt.Set(lengthEndDrawPosX,wingThick,0);
		edgeLineArr[2].endPt.Set(lengthEndDrawPosX,0,0);
		edgeLineArr[3].endPt.Set(lengthEndDrawPosX,-wingThick,0);
		edgeLineArr[4].endPt.Set(lengthEndDrawPosX,-wingWide,0);
	}
	//���н���֫���ƾ�����зŴ��Ա�����н���֫��ײ
	double cut_wing_para[2][3];
	double cut_angle[4][2];
	memcpy(cut_wing_para,cut_wing_para_jg,6*sizeof(double));
	memcpy(cut_angle,cut_angle_jg,8*sizeof(double));
	int i=0;
	for(i=0;i<2;i++)
	{
		if(cut_wing[i]<=0)
			continue;
		cut_wing_para[i][0]=TransCutWingOrAngleDrawDist(cut_wing_para[i][0]);
		double zoomscale=cut_wing_para[i][0]/cut_wing_para_jg[i][0];
		cut_wing_para[i][1]=TransCutWingOrAngleDrawDist(cut_wing_para[i][1]);
		if(cut_wing_para_jg[2]>cut_wing_para_jg[0])
		{
			cut_wing_para[i][2]=cut_wing_para_jg[i][2]*zoomscale;
			if(cut_wing_para[i][2]>=lengthEndDrawPosX*0.5)
			{//����cut_wing_para[i][0]�����zoomscale����ʱ�������µ���cut_wing_para[i][2]��ֵ  wxc-2016.8.4
				double dist=cut_wing_para_jg[i][2];
				cut_wing_para[i][2]=TransCutWingOrAngleDrawDist(dist);
			}
			//�����ٴ���TransDrawDistX���л��Ƴ���ת������������������볤��ֵС��������ͬһ���ֵ���������ӹ��������� wjh-2015.4.27
			//if(i==0)
			//	cut_wing_para[i][2]=TransDrawDistX(cut_wing_para_jg[i][2]);
			//else
			//	cut_wing_para[i][2]=TransDrawDistX(cut_wing_para_jg[i][2],length-cut_wing_para_jg[i][2]);
		}
		//ʹ��TransCoordY����X������Ƴ���,��֤X��Y���򳤶�ͬ�Ȼ��ƣ�
		//Y������Ƴ�����RelayoutDrawingʱ��ͳһ����,�˴����ô��� wht 15-09-19
		cut_wing_para[i][0]=TransCoordY(cut_wing_para[i][0]);
		cut_wing_para[i][2]=TransCoordY(cut_wing_para[i][2]);
	}
	for(i=0;i<4;i++)
	{
		cut_angle[i][0]=TransCutWingOrAngleDrawDist(cut_angle[i][0]);
		cut_angle[i][1]=TransCutWingOrAngleDrawDist(cut_angle[i][1]);
		//ʹ��TransCoordY����X������Ƴ���,��֤X��Y���򳤶�ͬ�Ȼ��ƣ�
		//Y������Ƴ�����RelayoutDrawingʱ��ͳһ����,�˴����ô��� wht 15-09-19
		cut_angle[i][0]=TransCoordY(cut_angle[i][0]);
	}
	if(bIncStartDim)
	{
		//������ʼ������֫
		if(cut_wing[0]>0)
		{
			if(cut_wing[0]==1)	//X֫Ϊ��֫��֫
			{
				edgeLineArr[2].startPt.Set(cut_wing_para[0][0]);
				edgeLineArr[3].startPt.Set(cut_wing_para[0][0],-wingThick);
				edgeLineArr[4].startPt.Set(cut_wing_para[0][2],-wingWide);
				line.startPt = edgeLineArr[4].startPt;
				line.endPt	 = edgeLineArr[3].startPt;
				lineArr.Add(line);

				line.startPt = line.endPt;
				line.endPt.y = 0;
				lineArr.Add(line);

				line.startPt = line.endPt;
				line.endPt.Set(0,cut_wing_para[0][1],0);
				lineArr.Add(line);
				f3dLine line2;
				line2.endPt.Set(100,0,0);	//�Ǹ�Z�᷽��ACAD��X�᷽��
				Int3dpl(line,line2,edgeLineArr[2].startPt);
				//�Ǹ��ϵ��������ڸ���ͼ�е�ͶӰ
				line2.startPt.Set(0,wingThick,0);
				line2.endPt.Set(100,wingThick,0);
				Int3dpl(line,line2,edgeLineArr[1].startPt);
				if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
				{
					bool dim0=true,dim2=true;
					if(cut_wing_para_jg[0][2]==cut_wing_para_jg[0][0])
					{	//��֫������������ȡ������ͬ��ֻ���עһ���ߴ� wjh-2013.10.24
						double dimdist=TransCoordX(cut_wing_para_jg[0][0]);
						CXhChar16 text("%.0f",cut_wing_para_jg[0][0]);
						if(dimdist<(drawPara.fDimTextSize*drawPara.fTextXFactor*text.GetLength()
							+drawPara.fDimArrowSize))
							dim0=false;	//������ȡ���ȹ��̣���ע��֫����
						else
							dim2=false;	//������ȡ���ȹ�������ע�Ǹ�����
					}
					if(dim2)
					{//��֫������ȡ���ȱ�ע
					jg_dim.angle = 0;
					jg_dim.dist = cut_wing_para_jg[0][2];
					jg_dim.dimStartPos.Set(0,-wingWide);
					jg_dim.dimEndPos = edgeLineArr[4].startPt;
					jg_dim.dimPos.Set(0,-wingWide-1,0);
					jg_dim.nHits=-1;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					}
					if(dim0)
					{//�Ǹ�������ȡ���ȱ�ע
					jg_dim.dist = cut_wing_para_jg[0][0];
					jg_dim.dimStartPos.Set(0,-wingThick);
					jg_dim.dimEndPos.Set(cut_wing_para[0][0],-wingThick);
					jg_dim.dimPos.Set(0,-wingThick-1,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					}
					//��һ֫��ȡ��ȱ�ע
					if(cut_wing_para_jg[0][1]>EPS)
					{
						jg_dim.angle = Pi/2.0;
						jg_dim.dist = cut_wing_para_jg[0][1];
						jg_dim.dimStartPos.Set(0,0,0);
						jg_dim.dimEndPos.Set(0,cut_wing_para[0][1],0);
						jg_dim.dimPos.Set(-1,0,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
					}
				}
				if(cut_angle_jg[1][0]>0)	//Y֫�н�
				{
					edgeLineArr[0].startPt.x=cut_angle[1][0];
					line.startPt=line.endPt;
					line.endPt.Set(0,wingWide-cut_angle[1][1],0);
					lineArr.Add(line);
					line.startPt = line.endPt;
					line.endPt.Set(edgeLineArr[0].startPt.x,wingWide,0);
					lineArr.Add(line);
					if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
					{
						//Y֫�нǳߴ��ע֮��ֱ���߷���
						jg_dim.angle = Pi/2;
						jg_dim.dist = cut_angle_jg[1][1];
						jg_dim.dimStartPos.Set(0,wingWide,0);
						jg_dim.dimEndPos.Set(0,wingWide-cut_angle[1][1],0);
						jg_dim.dimPos.Set(-1,0,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
						//Y֫�нǳߴ��ע֮���߷���
						jg_dim.angle = 0;
						jg_dim.dist = cut_angle_jg[1][0];
						jg_dim.dimStartPos.Set(cut_angle[1][0],wingWide,0);
						jg_dim.dimEndPos.Set(0,wingWide,0);
						jg_dim.dimPos.Set(0,wingWide+1,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
					}
				}
				else
				{
					line.startPt = line.endPt;
					line.endPt.Set(0,wingWide,0);
					lineArr.Add(line);
				}
			}
			else							//Y֫Ϊ��֫��֫
			{
				edgeLineArr[2].startPt.Set(cut_wing_para[0][0]);
				edgeLineArr[1].startPt.Set(cut_wing_para[0][0],wingThick);
				edgeLineArr[0].startPt.Set(cut_wing_para[0][2],wingWide);

				line.startPt = edgeLineArr[0].startPt;
				line.endPt = edgeLineArr[1].startPt;
				lineArr.Add(line);

				line.startPt = line.endPt;
				line.endPt.y = 0;
				lineArr.Add(line);

				line.startPt = line.endPt;
				line.endPt.Set(0,-cut_wing_para[0][1],0);
				lineArr.Add(line);

				f3dLine line2;
				line2.endPt.Set(100,0,0);	//�Ǹ�Z�᷽��ACAD��X�᷽��
				Int3dpl(line,line2,edgeLineArr[2].startPt);
				//�Ǹ��ϵ��������ڸ���ͼ�е�ͶӰ
				line2.startPt.Set(0,-wingThick,0);
				line2.endPt.Set(100,-wingThick,0);
				Int3dpl(line,line2,edgeLineArr[3].startPt);

				if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
				{
					bool dim0=true,dim2=true;
					if(cut_wing_para_jg[0][2]==cut_wing_para_jg[0][0])
					{	//��֫������������ȡ������ͬ��ֻ���עһ���ߴ� wjh-2013.10.24
						double dimdist=TransCoordX(cut_wing_para_jg[0][0]);
						CXhChar16 text("%.0f",cut_wing_para_jg[0][0]);
						if(dimdist<(drawPara.fDimTextSize*drawPara.fTextXFactor*text.GetLength()
							+drawPara.fDimArrowSize))
							dim0=false;	//������ȡ���ȹ��̣���ע��֫����
						else
							dim2=false;	//������ȡ���ȹ�������ע�Ǹ�����
					}
					if(dim2)
					{//��֫������ȡ���ȱ�ע
					jg_dim.angle = 0;
					jg_dim.dist = cut_wing_para_jg[0][2];
					jg_dim.dimStartPos.Set(0,wingWide);
					jg_dim.dimEndPos = edgeLineArr[0].startPt;
					jg_dim.dimPos.Set(0,wingWide+1,0);
					jg_dim.nHits=-1;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					}
					if(dim0)
					{//�Ǹ�������ȡ���ȱ�ע
					jg_dim.dimStartPos.Set(0,wingThick);
					jg_dim.dimEndPos.Set(cut_wing_para[0][0],wingThick,0);
					jg_dim.dist = cut_wing_para_jg[0][0];
					jg_dim.dimPos.Set(0,wingThick+1,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					}
					//��һ֫��ȡ��ȱ�ע
					if(cut_wing_para_jg[0][1]>EPS)
					{
						jg_dim.angle = Pi/2.0;
						jg_dim.dist = cut_wing_para_jg[0][1];
						jg_dim.dimStartPos.Set(0,0,0);
						jg_dim.dimEndPos.Set(0,-cut_wing_para[0][1],0);
						jg_dim.dimPos.Set(-1,0,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
					}
				}
				if(cut_angle_jg[0][0]>0)	//X֫�н�
				{
					edgeLineArr[4].startPt.x=cut_angle[0][0];
					line.startPt = line.endPt;
					line.endPt.Set(0,cut_angle[0][1]-wingWide,0);
					lineArr.Add(line);

					line.startPt = line.endPt;
					line.endPt.Set(edgeLineArr[4].startPt.x,-wingWide,0);
					lineArr.Add(line);
					if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
					{
						//X֫�нǳߴ��ע֮��ֱ���߷���
						jg_dim.angle = Pi/2.0;
						jg_dim.dist = cut_angle_jg[0][1];
						jg_dim.dimStartPos.Set(0,-wingWide,0);
						jg_dim.dimEndPos.Set(0,-wingWide+cut_angle[0][1],0);
						jg_dim.dimPos.Set(-1,0,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
						//X֫�нǳߴ��ע֮���߷���
						jg_dim.angle = 0;
						jg_dim.dist = cut_angle_jg[0][0];
						jg_dim.dimStartPos.Set(cut_angle[0][0],-wingWide);
						jg_dim.dimEndPos.Set(0,-wingWide,0);
						jg_dim.dimPos.Set(0,-wingWide-1,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
					}
				}
				else
				{
					line.startPt = line.endPt;
					line.endPt.Set(0,-wingWide,0);
					lineArr.Add(line);
				}
			}

		}
		//���κ��г�����
		else if(cut_angle_jg[0][0]<EPS&&cut_angle_jg[1][0]<EPS)
		{
			line.startPt.Set(0,wingWide,0);
			line.endPt.Set(0,-wingWide,0);
			lineArr.Add(line);
		}
		else	//������ʼ��ֻ���н�
		{
			if(cut_angle_jg[0][0]>0)	//X֫�н�
			{
				edgeLineArr[4].startPt.Set(cut_angle[0][0],-wingWide);
				line.startPt = edgeLineArr[4].startPt;
				line.endPt.Set(0,-wingWide+cut_angle[0][1],0);
				lineArr.Add(line);
				line.startPt = line.endPt;
				line.endPt.Set();
				lineArr.Add(line);
				if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
				{
					//X֫�н���֮��ֱ���߷����ע
					jg_dim.angle = Pi/2;
					jg_dim.dist = cut_angle_jg[0][1];
					jg_dim.dimStartPos.Set(0,-wingWide+cut_angle[0][1],0);
					jg_dim.dimEndPos.Set(0,-wingWide,0);
					jg_dim.dimPos.Set(-1,0,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					//X֫�н���֮���߷����ע
					jg_dim.angle = 0;
					jg_dim.dist = cut_angle_jg[0][0];
					jg_dim.dimStartPos.Set(0,-wingWide,0);
					jg_dim.dimEndPos.Set(cut_angle[0][0],-wingWide);
					jg_dim.dimPos.Set(0,-wingWide-1,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
				}
			}
			else
			{
				line.startPt = edgeLineArr[4].startPt;
				line.endPt.Set();
				lineArr.Add(line);
			}
			if(cut_angle_jg[1][0]>0)	//Y֫�н�
			{
				edgeLineArr[0].startPt.Set(cut_angle[1][0],wingWide);
				line.startPt.Set();
				line.endPt.Set(0,wingWide-cut_angle[1][1],0);
				lineArr.Add(line);
				line.startPt = line.endPt;
				line.endPt.Set(edgeLineArr[0].startPt.x,wingWide,0);
				lineArr.Add(line);
				if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
				{
					//Y֫�н���֮��ֱ���߷����ע
					jg_dim.angle = Pi/2;
					jg_dim.dist = cut_angle_jg[1][1];
					jg_dim.dimStartPos.Set(0,wingWide-cut_angle[1][1],0);
					jg_dim.dimEndPos.Set(0,wingWide,0);
					jg_dim.dimPos.Set(-1,0,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					//Y֫�н���֮���߷����ע
					jg_dim.angle = 0;
					jg_dim.dist = cut_angle_jg[1][0];
					jg_dim.dimStartPos.Set(0,wingWide,0);
					jg_dim.dimEndPos.Set(cut_angle[1][0],wingWide);
					jg_dim.dimPos.Set(0,wingWide+1,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
				}
			}
			else
			{
				line.startPt = edgeLineArr[0].startPt;
				line.endPt.Set();
				lineArr.Add(line);
			}
		}
	}
	if(bIncEndDim)
	{
		//������ֹ����֫
		if(cut_wing[1]>0)
		{
			if(cut_wing[1]==1)	//X֫Ϊ��֫��֫
			{
				edgeLineArr[2].endPt.Set(lengthEndDrawPosX-cut_wing_para[1][0]);
				edgeLineArr[3].endPt.Set(lengthEndDrawPosX-cut_wing_para[1][0],-wingThick);
				edgeLineArr[4].endPt.Set(lengthEndDrawPosX-cut_wing_para[1][2],-wingWide);
				line.startPt = edgeLineArr[4].endPt;
				line.endPt = edgeLineArr[3].endPt;
				lineArr.Add(line);

				line.startPt = line.endPt;
				line.endPt.y = 0;
				lineArr.Add(line);

				line.startPt = line.endPt;
				line.endPt.Set(lengthEndDrawPosX,cut_wing_para[1][1],0);
				lineArr.Add(line);
				f3dLine line2;
				//��ʾ�Ǹ�Z�᷽��/���ߣ�ACADX�᷽��ֱ�ߣ�
				line2.endPt.Set(100,0,0);
				Int3dpl(line,line2,edgeLineArr[2].endPt);
				//�Ǹ��������ڸ���ͼ�е�ͶӰ
				line2.startPt.Set(0,wingThick,0);
				line2.endPt.Set(100,wingThick,0);
				Int3dpl(line,line2,edgeLineArr[1].endPt);
				if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
				{
					bool dim0=true,dim2=true;
					if(cut_wing_para_jg[1][2]==cut_wing_para_jg[1][0])
					{	//��֫������������ȡ������ͬ��ֻ���עһ���ߴ� wjh-2013.10.24
						double dimdist=TransCoordX(cut_wing_para_jg[1][0]);
						CXhChar16 text("%.0f",cut_wing_para_jg[1][0]);
						if(dimdist<(drawPara.fDimTextSize*drawPara.fTextXFactor*text.GetLength()
							+drawPara.fDimArrowSize))
							dim0=false;	//������ȡ���ȹ��̣���ע��֫����
						else
							dim2=false;	//������ȡ���ȹ�������ע�Ǹ�����
					}
					if(dim2)
					{//��֫������ȡ���ȱ�ע
					jg_dim.angle = 0;
					jg_dim.dist = cut_wing_para_jg[1][2];
					jg_dim.dimStartPos.Set(lengthEndDrawPosX,-wingWide,0);
					jg_dim.dimEndPos = edgeLineArr[4].endPt;
					jg_dim.dimPos.Set(lengthEndDrawPosX,-wingWide-1,0);
					jg_dim.nHits=-1;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					}
					if(dim0)
					{//�Ǹ�������ȡ���ȱ�ע
					jg_dim.dist = cut_wing_para_jg[1][0];
					jg_dim.dimStartPos.Set(lengthEndDrawPosX,-wingThick,0);
					jg_dim.dimEndPos.Set(lengthEndDrawPosX-cut_wing_para[1][0],-wingThick,0);
					jg_dim.dimPos.Set(lengthEndDrawPosX,-wingThick-1,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					}
					//��һ֫��ȡ��ȱ�ע
					if(cut_wing_para_jg[1][1]>EPS)
					{
						jg_dim.angle = Pi/2.0;
						jg_dim.dist = cut_wing_para_jg[1][1];
						jg_dim.dimStartPos.Set(lengthEndDrawPosX,0,0);
						jg_dim.dimEndPos.Set(lengthEndDrawPosX,cut_wing_para[1][1],0);
						jg_dim.dimPos.Set(lengthEndDrawPosX+1,0,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
					}
				}
				if(cut_angle_jg[3][1]>0)	//Y֫�н�
				{
					edgeLineArr[0].endPt.x=lengthEndDrawPosX-cut_angle[3][0];
					line.startPt = line.endPt;
					line.endPt.Set(lengthEndDrawPosX,wingWide-cut_angle[3][1],0);
					lineArr.Add(line);
					line.startPt = line.endPt;
					line.endPt.Set(edgeLineArr[0].endPt.x,wingWide,0);
					lineArr.Add(line);
					if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
					{
						//Y֫�нǳߴ��ע֮��ֱ���߷���
						jg_dim.angle = Pi/2;
						jg_dim.dist = cut_angle_jg[3][1];
						jg_dim.dimStartPos.Set(lengthEndDrawPosX,wingWide,0);
						jg_dim.dimEndPos.Set(lengthEndDrawPosX,wingWide-cut_angle[3][1],0);
						jg_dim.dimPos.Set(lengthEndDrawPosX+1,0,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
						//Y֫�нǳߴ��ע֮���߷���
						jg_dim.angle = 0;
						jg_dim.dist = cut_angle_jg[3][0];
						jg_dim.dimStartPos.Set(lengthEndDrawPosX-cut_angle[3][0],wingWide);
						jg_dim.dimEndPos.Set(lengthEndDrawPosX,wingWide,0);
						jg_dim.dimPos.Set(lengthEndDrawPosX,wingWide+1,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
					}
				}
				else
				{
					line.startPt = line.endPt;
					line.endPt.Set(lengthEndDrawPosX,wingWide,0);
					lineArr.Add(line);
				}
			}
			else							//Y֫Ϊ��֫��֫
			{
				edgeLineArr[2].endPt.Set(lengthEndDrawPosX-cut_wing_para[1][0]);
				edgeLineArr[1].endPt.Set(lengthEndDrawPosX-cut_wing_para[1][0],wingThick);
				edgeLineArr[0].endPt.Set(lengthEndDrawPosX-cut_wing_para[1][2],wingWide);
				line.startPt = edgeLineArr[0].endPt;
				line.endPt = edgeLineArr[1].endPt;
				lineArr.Add(line);

				line.startPt = line.endPt;
				line.endPt.y = 0;
				lineArr.Add(line);

				line.startPt = line.endPt;
				line.endPt.Set(lengthEndDrawPosX,-cut_wing_para[1][1],0);
				lineArr.Add(line);
				f3dLine line2;
				//��ʾ�Ǹ�Z�᷽��/���ߣ�ACADX�᷽��ֱ�ߣ�
				line2.endPt.Set(100,0,0);
				Int3dpl(line,line2,edgeLineArr[2].endPt);
				//�Ǹ��������ڸ���ͼ�е�ͶӰ
				line2.startPt.Set(0,-wingThick,0);
				line2.endPt.Set(100,-wingThick,0);
				Int3dpl(line,line2,edgeLineArr[3].endPt);
				if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)
				{	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
					bool dim0=true,dim2=true;
					if(cut_wing_para_jg[1][2]==cut_wing_para_jg[1][0])
					{	//��֫������������ȡ������ͬ��ֻ���עһ���ߴ� wjh-2013.10.24
						double dimdist=TransCoordX(cut_wing_para_jg[1][0]);
						CXhChar16 text("%.0f",cut_wing_para_jg[1][0]);
						if(dimdist<(drawPara.fDimTextSize*drawPara.fTextXFactor*text.GetLength()
							+drawPara.fDimArrowSize))
							dim0=false;	//������ȡ���ȹ��̣���ע��֫����
						else
							dim2=false;	//������ȡ���ȹ�������ע�Ǹ�����
					}
					if(dim2)
					{//��֫������ȡ���ȱ�ע
					jg_dim.angle = 0;
					jg_dim.dist = cut_wing_para_jg[1][2];
					jg_dim.dimStartPos.Set(lengthEndDrawPosX,wingWide,0);
					jg_dim.dimEndPos = edgeLineArr[0].endPt;
					jg_dim.dimPos.Set(lengthEndDrawPosX,wingWide+1,0);
					jg_dim.nHits=-1;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					}
					if(dim0)
					{//�Ǹ�������ȡ���ȱ�ע
					jg_dim.dimStartPos.Set(lengthEndDrawPosX,wingThick,0);
					jg_dim.dimEndPos.Set(lengthEndDrawPosX-cut_wing_para[1][0],wingThick,0);
					jg_dim.dist = cut_wing_para_jg[1][0];
					jg_dim.dimPos.Set(lengthEndDrawPosX,wingThick+1,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					}
					//��һ֫��ȡ��ȱ�ע
					if(cut_wing_para_jg[1][1]>EPS)
					{
						jg_dim.dimStartPos.Set(lengthEndDrawPosX,0,0);
						jg_dim.dimEndPos.Set(lengthEndDrawPosX,-cut_wing_para[1][1],0);
						jg_dim.dist = cut_wing_para_jg[1][1];
						jg_dim.dimPos.Set(lengthEndDrawPosX+1,0,0);
						jg_dim.angle = Pi/2.0;
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
					}
				}
				if(cut_angle_jg[2][0]>0)	//X֫�н�
				{
					edgeLineArr[4].endPt.x=lengthEndDrawPosX-cut_angle[2][0];
					line.startPt = line.endPt;
					line.endPt.Set(lengthEndDrawPosX,cut_angle[2][1]-wingWide,0);
					lineArr.Add(line);
					line.startPt = line.endPt;
					line.endPt.Set(edgeLineArr[4].endPt.x,-wingWide,0);
					lineArr.Add(line);
					if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
					{
						//X֫�нǳߴ��ע֮��ֱ���߷���
						jg_dim.angle = Pi/2.0;
						jg_dim.dist = cut_angle_jg[2][1];
						jg_dim.dimStartPos.Set(lengthEndDrawPosX,-wingWide,0);
						jg_dim.dimEndPos.Set(lengthEndDrawPosX,-wingWide+cut_angle[2][1],0);
						jg_dim.dimPos.Set(lengthEndDrawPosX+1,0,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
						//X֫�нǳߴ��ע֮���߷���
						jg_dim.angle = 0;
						jg_dim.dist = cut_angle_jg[2][0];
						jg_dim.dimStartPos.Set(lengthEndDrawPosX-cut_angle[2][0],-wingWide,0);
						jg_dim.dimEndPos.Set(lengthEndDrawPosX,-wingWide,0);
						jg_dim.dimPos.Set(lengthEndDrawPosX,-wingWide-1,0);
						jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
						dim_arr.append(jg_dim);
					}
				}
				else
				{
					line.startPt = line.endPt;
					line.endPt.Set(lengthEndDrawPosX,-wingWide,0);
					lineArr.Add(line);
				}
			}
		}
		//���κ��г�����
		else if(cut_angle_jg[2][0]<EPS&&cut_angle_jg[3][0]<EPS)
		{
			line.startPt.Set(lengthEndDrawPosX,wingWide,0);
			line.endPt.Set(lengthEndDrawPosX,-wingWide,0);
			lineArr.Add(line);
		}
		else	//������ֹ���н�
		{
			if(cut_angle_jg[2][0]>0)	//X֫�н�
			{
				edgeLineArr[4].endPt.Set(lengthEndDrawPosX-cut_angle[2][0],-wingWide);
				line.startPt.Set(lengthEndDrawPosX,0,0);
				line.endPt.Set(lengthEndDrawPosX,-wingWide+cut_angle[2][1],0);
				lineArr.Add(line);
				line.startPt = line.endPt;
				line.startPt = edgeLineArr[4].endPt;
				lineArr.Add(line);

				if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
				{
					//X֫�н���֮��ֱ���߷����ע
					jg_dim.angle = Pi/2;
					jg_dim.dist = cut_angle_jg[2][1];
					jg_dim.dimStartPos.Set(lengthEndDrawPosX,-wingWide+cut_angle[2][1],0);
					jg_dim.dimEndPos.Set(lengthEndDrawPosX,-wingWide,0);
					jg_dim.dimPos.Set(lengthEndDrawPosX+1,0,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					//X֫�н���֮���߷����ע
					jg_dim.angle = 0;
					jg_dim.dist = cut_angle_jg[2][0];
					jg_dim.dimStartPos.Set(lengthEndDrawPosX,-wingWide,0);
					jg_dim.dimEndPos.Set(lengthEndDrawPosX-cut_angle[2][0],-wingWide,0);
					jg_dim.dimPos.Set(lengthEndDrawPosX+1,-wingWide,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
				}
			}
			else
			{
				line.startPt = edgeLineArr[4].endPt;
				line.endPt.Set(lengthEndDrawPosX,0,0);
				lineArr.Add(line);
			}
			if(cut_angle_jg[3][0]>0)	//Y֫�н�
			{
				edgeLineArr[0].endPt.Set(lengthEndDrawPosX-cut_angle[3][0],wingWide);
				line.startPt.Set(lengthEndDrawPosX,0,0);
				line.endPt.Set(lengthEndDrawPosX,wingWide-cut_angle[3][1],0);
				lineArr.Add(line);
				line.startPt = line.endPt;
				line.startPt = edgeLineArr[0].endPt;
				lineArr.Add(line);
				if(dimStyleFlag.IsHasFlag(CutAngleDimByText))//drawPara.bDimCutAngle)	//��ͨ������Ҫ,�㽭ʢ�ﲻ��Ҫ
				{
					//Y֫�н���֮��ֱ���߷����ע
					jg_dim.angle = Pi/2;
					jg_dim.dist = cut_angle_jg[3][1];
					jg_dim.dimStartPos.Set(lengthEndDrawPosX,wingWide-cut_angle[3][1],0);
					jg_dim.dimEndPos.Set(lengthEndDrawPosX,wingWide,0);
					jg_dim.dimPos.Set(lengthEndDrawPosX+1,0,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
					//Y֫�н���֮���߷����ע
					jg_dim.angle = 0;
					jg_dim.dist = cut_angle_jg[3][0];
					jg_dim.dimStartPos.Set(lengthEndDrawPosX,wingWide,0);
					jg_dim.dimEndPos.Set(lengthEndDrawPosX-cut_angle[3][0],wingWide,0);
					jg_dim.dimPos.Set(lengthEndDrawPosX,wingWide+1,0);
					jg_dim.nHits=0;	//Ϊ0��ʾΪ�н���֫��ע
					dim_arr.append(jg_dim);
				}
			}
			else
			{
				line.startPt = edgeLineArr[0].endPt;
				line.endPt.Set(lengthEndDrawPosX,0,0);
				lineArr.Add(line);
			}
		}
	}
}
int CRodDrawing::GetLineCount()
{
	return lineArr.GetSize();
}

int CRodDrawing::GetZEdgeCount()
{
	return edgeLineArr.GetSize();
}

BOOL CRodDrawing::GetZEdge(f3dLine &line,int index)
{
	if(index<0||index>=edgeLineArr.GetSize())
		return FALSE;
	line.startPt = edgeLineArr[index].startPt;
	line.endPt = edgeLineArr[index].endPt;
	line.feature = edgeLineArr[index].feature;	//����
	return TRUE;
}

BOOL CRodDrawing::GetLineAt(f3dLine &line,int index)
{
	if(index>=0&&index<lineArr.GetSize())
	{
		line.startPt = lineArr[index].startPt;
		line.endPt = lineArr[index].endPt;
		line.feature=lineArr[index].feature;
		return TRUE;
	}
	else
		return FALSE;
}

int CRodDrawing::GetXLsCount()
{
	return LsCirX.GetSize();
}

int CRodDrawing::GetYLsCount()
{
	return LsCirY.GetSize();
}

BOOL CRodDrawing::GetXLsCircle(f3dCircle &Cir,int index)
{
	if(index<0||index>LsCirX.GetSize())//.GetUpperBound())
		return FALSE;
	Cir = LsCirX[index].dimCircle;
	Cir.pen.style=LsCirX[index].cFuncType;
	return TRUE;
}

BOOL CRodDrawing::GetYLsCircle(f3dCircle &Cir,int index)
{
	if(index<0||index>LsCirY.GetSize())//GetUpperBound())
		return FALSE;
	Cir = LsCirY[index].dimCircle;
	Cir.pen.style=LsCirY[index].cFuncType;
	return TRUE;
}

int CRodDrawing::GetDimCount()
{
	return dim_arr.GetSize();
}
int CRodDrawing::GetZhunDimCount()
{
	return zhun_dim_arr.GetSize();
}

BOOL CRodDrawing::GetDimAt(int index,CSizeTextDim *pDim)
{
	if(index<0||index>dim_arr.GetSize())//GetUpperBound())
		return FALSE;
	memcpy(pDim,&dim_arr[index],sizeof(CSizeTextDim));
	return TRUE;
}

BOOL CRodDrawing::GetZhunDimAt(int index,CZhunXianTextDim *pDim)
{
	if(index<0||index>zhun_dim_arr.GetUpperBound())
		return FALSE;
	memcpy(pDim,&zhun_dim_arr[index],sizeof(CZhunXianTextDim ));
	return TRUE;
}
int CAngleDrawing::GetKaiHeDimCount()
{
	return kaihe_angle_dim_arr.GetSize();
}
BOOL CAngleDrawing::GetKaiHeDimAt(int index,CKaiHeAngleDim *pDim)
{
	if(index<0||index>kaihe_angle_dim_arr.GetUpperBound())
		return FALSE;
	memcpy(pDim,&kaihe_angle_dim_arr[index],sizeof(CKaiHeAngleDim));
	return TRUE;
}
int CAngleDrawing::GetPartNoCount()
{
	return part_no_arr.GetSize();
}
BOOL CAngleDrawing::GetPartNoDimAt(int index,CTextFrame *pDim)
{
	if(index<0||index>part_no_arr.GetUpperBound())
		return FALSE;
	memcpy(pDim,&part_no_arr[index],sizeof(CTextFrame));
	return TRUE;
}
int CAngleDrawing::GetRibPlateDimCount()
{
	return rib_dim_arr.GetSize();
}
BOOL CAngleDrawing::GetRibPlateDimAt(int index,CSizeTextDim* pDim)
{
	if(index<0||index>rib_dim_arr.GetSize())
		return FALSE;
	memcpy(pDim,&rib_dim_arr[index],sizeof(CSizeTextDim));
	return TRUE;
}
//from Node.cpp
extern BOOL IsMaxInAll(double dest_val,double other1,double other2,double other3);
//����Ǹֿ��Ͻ�
#ifdef  __DRAWING_CONTEXT_
BOOL CAngleDrawing::ProcessKaiHeAngle(CLDSLineAngle *pJg,UCS_STRU &angleUcs,BOOL bUcsIncStartOdd)
{
	CLDSNode *pNode;
	NODESET nodeset;
	f3dLine line;
	CKaiHeAngleDim kaihe_dim;
	double wingWide,wingThick,wing_angle;
	char sKaiHeAngle[50];

	//-1��ʾ�����Ͻǣ�0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻ�
	int kaihe_base_wing=pJg->GetKaiHeWingX0Y1();
	wing_angle = pJg->GetRadWingAngle();
	JGZJ jgzj;
	if(pJg->m_bEnableTeG)
	{
		if(kaihe_base_wing==0)
			jgzj=pJg->xWingXZhunJu;
		else
			jgzj=pJg->xWingYZhunJu;
	}
	else
		getjgzj(jgzj,pJg->GetWidth());
	if((pJg->layer(0)=='S'||pJg->layer(0)=='L')&&pJg->layer(1)=='Z'||kaihe_base_wing==-1)
		return FALSE;	//�Ȳ���������һ�㲻���Ͻ�,��Ϊָ�������Ͻ�ʱҲ����Ҫ����
	//����97��ͼ�涨���е��޸� WJH-2004/09/28 
	//ʹ�á����Ͻ���ֵ�������Ƿ���Ҫ��ע���Ͻ���ϢĬ��Ϊ2��wht 11-05-06
	if(fabs(pJg->GetDecWingAngle()-90)>drawPara.fKaiHeJiaoThreshold)	//�н�ƫ�����2��Ҫ���Ͻ�
	{
		if(wing_angle>Pi/2)
			sprintf(sKaiHeAngle,"+%.1f��",DEGTORAD_COEF*(wing_angle-Pi/2));
		else
			sprintf(sKaiHeAngle,"-%.1f��",DEGTORAD_COEF*(Pi/2-wing_angle));
	}
	else
		return FALSE;
	if(pJg->pStart==NULL||pJg->pEnd==NULL)
		return FALSE;

	line.startPt = pJg->pStart->Position(false);
	line.endPt = pJg->pEnd->Position(false);
	for(pNode=Ta.Node.GetFirst();pNode!=NULL;pNode=Ta.Node.GetNext())
	{
		int ret=line.PtInLine(pNode->Position(false));
		if(ret==0)
			continue;
		else if((ret<0&&pNode->hFatherPart==pJg->handle)||ret>0)
			nodeset.append(pNode);	//λ�ڽǸ��ϵĽڵ�
	}
	wingWide = pJg->GetWidth();
	wingThick= pJg->GetThick();
	double length=pJg->GetLength();
	ARRAY_LIST<double>arrBoltsPosZ(0,20);
	f3dPoint work_wing_norm=pJg->get_norm_x_wing();
	f3dPoint other_wing_norm=pJg->get_norm_y_wing();
	if(kaihe_base_wing==1)
	{
		work_wing_norm=pJg->get_norm_y_wing();
		other_wing_norm=pJg->get_norm_x_wing();
	}
	for(CLsRef* pLsRef=pJg->GetFirstLsRef();pLsRef;pLsRef=pJg->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		if(fabs(pBolt->get_norm()*work_wing_norm)<fabs(pBolt->get_norm()*other_wing_norm))
			continue;	//�ǿ��Ͻ�֫�ϵ���˨
		GEPOINT inters;
		Int3dlf(inters,pBolt->ucs.origin,pBolt->get_norm(),pJg->Start(),work_wing_norm);
		coord_trans(inters,angleUcs,FALSE);
		arrBoltsPosZ.append(inters.z);
	}
	CQuickSort<double>::QuickSort(arrBoltsPosZ.m_pData,arrBoltsPosZ.GetSize(),compare_double);
	for(pNode=nodeset.GetFirst();pNode!=NULL;pNode=nodeset.GetNext())
	{
		if(pNode==pJg->pStart)
			kaihe_dim.dimEndPos = pJg->Start();
		else if(pNode==pJg->pEnd)
			kaihe_dim.dimEndPos = pJg->End();
		else
			kaihe_dim.dimEndPos = pJg->GetDatumPosBer(pNode);//->pos;
		coord_trans(kaihe_dim.dimEndPos,angleUcs,FALSE);
		kaihe_dim.dimStartPos = kaihe_dim.dimEndPos;
		//���ϽǱ�ע�м�ڵ�λ��(�������);
		//���������ϵangleUcs�����Ǹ�ʼ����ͷ��Ϣ(����ϵԭ��Ϊ�Ǹ�ʼ��ʵ�����)��bUcsIncStartOdd����Ӧ����ΪTRUE wht 15-04-23
		if(!CalKaiHeStartEndPos(pJg,pNode,kaihe_dim.dimStartPos,kaihe_dim.dimEndPos,angleUcs,FALSE,bUcsIncStartOdd))
			continue;
		else if(fabs(kaihe_dim.dimStartPos.z-kaihe_dim.dimEndPos.z)<EPS2)
			continue;
		//
		double *pStartBoltZ=NULL;
		for(int i=0;i<arrBoltsPosZ.GetSize();i++)
		{
			if(pStartBoltZ==NULL&&arrBoltsPosZ[i]>kaihe_dim.dimStartPos.z)
			{
				pStartBoltZ=&arrBoltsPosZ[i];
				break;
			}
		}
		kaihe_dim.dimStartPos.z-=sys.part_map.angle.fKaiHeJiaoMinDeita;
		kaihe_dim.dimEndPos.z+=sys.part_map.angle.fKaiHeJiaoMinDeita;
		double incre_start=0,incre_end=0;
		if(pStartBoltZ!=NULL)
		{
			double space_start=*pStartBoltZ-kaihe_dim.dimStartPos.z;
			double space_end  =kaihe_dim.dimEndPos.z-*pStartBoltZ;
			incre_start=ceil(0.1*space_start)*10-space_start;
			incre_end=ceil(0.1*space_end)*10-space_end;
			kaihe_dim.dimStartPos.z-=incre_start;
			kaihe_dim.dimEndPos.z  +=incre_end;
		}
		if(kaihe_dim.dimStartPos.z<0)
			kaihe_dim.dimStartPos.z=0;
		if(kaihe_dim.dimEndPos.z>length)
			kaihe_dim.dimEndPos.z=length;
		kaihe_dim.dimStartPos.Set(kaihe_dim.dimStartPos.z);
		kaihe_dim.dimEndPos.Set(kaihe_dim.dimEndPos.z);
		if(pNode==pJg->pStart)
		{
			kaihe_dim.dimStartPos.y=0;
			kaihe_dim.dimStartPos.Set(0,0);
			kaihe_dim.dimEndPos.Set(kaihe_dim.dimEndPos.x,0);
			kaihe_dim.dist = kaihe_dim.dimEndPos.x-kaihe_dim.dimStartPos.x;
			kaihe_dim.start0_end1_else2 = 0;
		}
		else if(pNode==pJg->pEnd)
		{
			kaihe_dim.dimStartPos.Set(kaihe_dim.dimStartPos.x,0);
			kaihe_dim.dimEndPos.Set(length,0);
			kaihe_dim.dist = kaihe_dim.dimEndPos.x-kaihe_dim.dimStartPos.x;
			kaihe_dim.start0_end1_else2 = 1;
		}
		else
		{
			kaihe_dim.dimStartPos.Set(kaihe_dim.dimStartPos.x,0);
			kaihe_dim.dimEndPos.Set(kaihe_dim.dimEndPos.x,0);
			kaihe_dim.dist = kaihe_dim.dimEndPos.x-kaihe_dim.dimStartPos.x;
			kaihe_dim.start0_end1_else2 = 2;	//�������˿��Ͻ� wht 09-11-16
		}
		kaihe_dim.dimPos=(kaihe_dim.dimStartPos+kaihe_dim.dimEndPos)*0.5;
		if(drawPara.bDimKaiHe)	//�㽭ʢ���Լ�Ҫ��ȥ�����ϽǱ�ע
		{
			kaihe_dim.dimText.Copy(sKaiHeAngle);
			kaihe_dim.real_start = kaihe_dim.dimStartPos;
			kaihe_dim.real_end   = kaihe_dim.dimEndPos;
			kaihe_dim.real_dim   = kaihe_dim.dimPos;
			kaihe_dim.dimStartPos.x =TransCoordX(kaihe_dim.real_start.x);
			kaihe_dim.dimEndPos.x =TransCoordX(kaihe_dim.real_end.x);
			kaihe_dim.dimPos.x =TransCoordX(kaihe_dim.real_dim.x);
			if(kaihe_base_wing==0)
			{	//X֫���Ͻ�
				kaihe_dim.dimPos.y=-1;
				kaihe_dim.bInWingX=TRUE;
			}
			else
			{
				kaihe_dim.dimPos.y=1;	//Y֫���Ͻ�
				kaihe_dim.bInWingX=FALSE;
			}
			kaihe_angle_dim_arr.Add(kaihe_dim);
		}
	}
	return TRUE;
}
#endif
BOOL CAngleDrawing::ProcessKaiHeAngle(CProcessAngle *pJg)
{
	if (pJg==NULL)
		return FALSE;
	for (KAI_HE_JIAO *pKaiHeJiao=pJg->kaiHeJiaoList.GetFirst();pKaiHeJiao;pKaiHeJiao=pJg->kaiHeJiaoList.GetNext())
	{
		CKaiHeAngleDim kai_he_jiao;
		kai_he_jiao.real_dim.x=pKaiHeJiao->position;
		if(pJg->kaihe_base_wing_x0_y1==0)
		{
			kai_he_jiao.real_dim.y=-1;
			kai_he_jiao.bInWingX=TRUE;
		}
		else
		{
			kai_he_jiao.real_dim.y=1;
			kai_he_jiao.bInWingX=FALSE;
		}
		kai_he_jiao.real_start.Set(pKaiHeJiao->position-pKaiHeJiao->startLength,0,0);
		kai_he_jiao.real_end.Set(pKaiHeJiao->position+pKaiHeJiao->endLength,0,0);
		kai_he_jiao.dist=pKaiHeJiao->startLength+pKaiHeJiao->endLength;
		kai_he_jiao.dimStartPos.x =TransCoordX(kai_he_jiao.real_start.x);
		kai_he_jiao.dimEndPos.x =TransCoordX(kai_he_jiao.real_end.x);
		kai_he_jiao.dimPos.x =TransCoordX(kai_he_jiao.real_dim.x);
		kaihe_angle_dim_arr.Add(kai_he_jiao);
	}
	return TRUE;
}

int CRodDrawing::GetPureTxtCount()
{
	return pure_txt_arr.GetSize();
}
BOOL CRodDrawing::GetPureTxtDimAt(int index,CTextOnlyDrawing *pDim)
{
	if(index<0||index>pure_txt_arr.GetSize())//GetUpperBound())
		return FALSE;
	memcpy(pDim,&pure_txt_arr[index],sizeof(CTextOnlyDrawing));
	return TRUE;
}

f2dRect CRodDrawing::GetDrawRegion(double PART_MAP_SCALE)
{
	int i,n;
	f2dRect rect;
	CSizeTextDim jg_dim;
	CKaiHeAngleDim kaihe_dim;
	CZhunXianTextDim zhun_dim;
	CTextOnlyDrawing pure_txt;
	SCOPE_STRU scope;
	n=GetDimCount();
	for(i=0;i<n;i++)
	{
		GetDimAt(i,&jg_dim);
		scope.VerifyVertex(jg_dim.dimStartPos);
		scope.VerifyVertex(jg_dim.dimEndPos);
		scope.VerifyVertex(jg_dim.dimPos);
	}
	n=GetZhunDimCount();
	for(i=0;i<n;i++)
	{
		GetZhunDimAt(i,&zhun_dim);
		scope.VerifyVertex(zhun_dim.dimStartPos);
		scope.VerifyVertex(zhun_dim.dimEndPos);
		scope.VerifyVertex(zhun_dim.dimPos);
	}
	n = GetPureTxtCount();
	for(i=0;i<n;i++)
	{
		GetPureTxtDimAt(i,&pure_txt);
		if(pure_txt.bInXWing)	//X֫��
			scope.VerifyVertex(pure_txt.dimPos-f3dPoint(0,6*drawPara.fPartGuigeTextSize*PART_MAP_SCALE));
		else
			scope.VerifyVertex(pure_txt.dimPos+f3dPoint(0,3*drawPara.fPartGuigeTextSize*PART_MAP_SCALE));
	}
	n=GetKaiHeDimCount();
	BOOL bFindInWingX=FALSE,bFindInWingY=FALSE;
	for(i=0;i<n;i++)
	{
		GetKaiHeDimAt(i,&kaihe_dim);
		if(kaihe_dim.bInWingX)
			bFindInWingX=TRUE;
		if(!kaihe_dim.bInWingX)
			bFindInWingY=TRUE;
		if(bFindInWingX&&bFindInWingY)
			break;
	}
	if(bFindInWingX)
		scope.fMinY-=(2+2*drawPara.fDimTextSize)*PART_MAP_SCALE;
	if(bFindInWingY)
		scope.fMaxY+=(1+1*drawPara.fDimTextSize)*PART_MAP_SCALE;
	for(i=0;i<edgeLineArr.GetSize();i++)
	{
		scope.VerifyVertex(edgeLineArr[i].startPt);
		scope.VerifyVertex(edgeLineArr[i].endPt);
	}
	rect.topLeft.Set(scope.fMinX,scope.fMaxY);
	rect.bottomRight.Set(scope.fMaxX,scope.fMinY);
	return rect;
}
//�����жνǸֵ���˨ͳ����һ��,
void CRodDrawing::StatBoltsInfo(ATOM_LIST<BOLT_INFO> &bolt_list,long mid_flat_pos_z/*=0*/)
{
	int i;
	int nMinAxisGX=10000,nMinAxisGY=10000; //��¼X��Y֫��С��׼��
	for(BOLT_INFO *bolt_info=bolt_list.GetFirst();bolt_info;bolt_info=bolt_list.GetNext())
	{
		LsMark mark,*pNowMark;
		mark.cFuncType=bolt_info->cFuncType;
		mark.cFlag=bolt_info->cFlag;
		mark.realPos.Set(bolt_info->posX,bolt_info->posY);
		if(bolt_info->posY< 0)
			mark.wingX0Y1=0;
		else
			mark.wingX0Y1=1;
		double g=fabs(bolt_info->posY);
		mark.dimCircle.centre=mark.realPos;
		mark.dimCircle.radius = bolt_info->bolt_d/2.0;
		mark.dimCircle.feature=ftoi(bolt_info->hole_d_increment*10);
		mark.dimCircle.ID=bolt_info->hiberId.HiberDownId(2);
		if(mark.wingX0Y1==0)
		{	//X֫�ϵ���˨
			mark.dimCircle.centre.y=mark.realPos.y = -g;
			for(i=0;i<m_nWingAxisGX;i++)
			{
				if(wingAxisGArrX[i]==ftoi(g))
					break;
			}
			if(i==m_nWingAxisGX && m_nWingAxisGX<MAX_G_COUNT)
			{
				wingAxisGArrX[i]=ftoi(g);
				m_nWingAxisGX++;
			}
			if(wingAxisGArrX[i]<nMinAxisGX)
				nMinAxisGX=wingAxisGArrX[i];	//��¼X֫��С��׼��
			if(bolt_info->waistLen>0)
			{	//��Բ��
				mark.dimCircle.pen.width=(int)bolt_info->waistLen;		//ʹ�û��ʿ�ȱ�����Բ�׳���
				f3dPoint waist_vec=bolt_info->waistVec;					//ʹ����˨�׷��߷��򱣴���Բ����
				mark.dimCircle.norm.Set(waist_vec.z,waist_vec.x,waist_vec.y);
			}
			pNowMark=LsCirX.append(mark);//Add(mark);
		}
		else if(mark.wingX0Y1==1)
		{	//Y֫�ϵ���˨
			mark.dimCircle.centre.y=mark.realPos.y = g;
			for(i=0;i<m_nWingAxisGY;i++)
			{
				if(wingAxisGArrY[i]==ftoi(g))
					break;
			}
			if(i==m_nWingAxisGY && m_nWingAxisGY<MAX_G_COUNT)
			{
				wingAxisGArrY[i]=ftoi(g);
				m_nWingAxisGY++;
			}
			if(wingAxisGArrY[i]<nMinAxisGY)
				nMinAxisGY=wingAxisGArrY[i];	//��¼X֫��С��׼��
			if(bolt_info->waistLen>0)
			{	//��Բ��
				mark.dimCircle.pen.width=(int)bolt_info->waistLen;			//ʹ�û��ʿ�ȱ�����Բ�׳���
				f3dPoint waist_vec=bolt_info->waistVec;						//ʹ����˨�׷��߷��򱣴���Բ����
				mark.dimCircle.norm.Set(waist_vec.z,waist_vec.x,waist_vec.y);
			}
			pNowMark=LsCirY.append(mark);
		}
		//f3dPoint boltCenter=mark.realPos;
		//boltCenter.feature=mark.wingX0Y1;
		LsCenterArr.append(pNowMark);//boltCenter);
	}
	if(mid_flat_pos_z!=0)
	{	//�����м�ѹ����Ҫ��ע�м�ѹ��λ��
		LsMark mark,*pNowMark;
		mark.realPos.Set(abs(mid_flat_pos_z),0,0);
		mark.wingX0Y1 = mid_flat_pos_z>0?1:0;
		mark.dimCircle.centre=mark.realPos;
		mark.dimCircle.radius=0;
		pNowMark=mid_flat_pos_z>0?LsCirY.append(mark):LsCirX.append(mark);
		pNowMark->cFuncType=0xFF;	//ѹ��λ��
		LsCenterArr.append(pNowMark);
	}
	//����˨�����ȷ���������������
	CHeapSort<LsMarkPtr>::HeapSort(LsCirX.m_pIndexTbl,LsCirX.GetSize(),compareLsMark);
	CHeapSort<LsMarkPtr>::HeapSort(LsCirY.m_pIndexTbl,LsCirY.GetSize(),compareLsMark);
	CHeapSort<LsMarkPtr>::HeapSort(LsCenterArr.m_pData,LsCenterArr.GetSize(),compareLsMark);//compareBoltPos);
	CQuickSort<int>::QuickSort(wingAxisGArrX,m_nWingAxisGX,compare_int);
	CQuickSort<int>::QuickSort(wingAxisGArrY,m_nWingAxisGY,compare_int);
	//�Ǹ���˨Ϊ˫�Ż���Ų���ʱִ�����´��� wht 11-05-07
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&(m_nWingAxisGX>1||m_nWingAxisGY>1))
	{	//Y�������ֵ������С׼�����˨���ڵڶ��б�ע��˨���
		for(i=0;i<LsCirX.GetSize();i++)
		{
			if(fabs(LsCirX[i].realPos.y)>(nMinAxisGX+1))
				LsCirX[i].iRow=2;
			else 
				LsCirX[i].iRow=1;
		}
		for(i=0;i<LsCirY.GetSize();i++)
		{
			if(fabs(LsCirY[i].realPos.y)>(nMinAxisGY+1))
				LsCirY[i].iRow=2;
			else 
				LsCirY[i].iRow=1;
		}
	}
}
double BOLT_SYMBOL_RADIUS = 1.2;	//������˨ͼ���İ뾶
double MIN_BOLTDRAWING_SPACE = 1.2;
static void ExtendLsSpaceDim(CRodDrawing::LsGroup* pGroup, CRodDrawing::LsPosMark LsPosArr[],int nLsNum)
{
	int i;
	if(pGroup->groupTypeStart0End1Mid2==0)	//ʼ��������
	{
		for(i=0;i<pGroup->m_nLsNum;i++)
		{
			if(pGroup->lsMarkArr[i]->needExtendDist<=0)
				continue;
			for(int j=i;j<pGroup->m_nLsNum;j++)
			{
				if( pGroup->lsMarkArr[j]->wingX0Y1Both2<2&&pGroup->lsMarkArr[i]->wingX0Y1Both2<2&&
					pGroup->lsMarkArr[j]->wingX0Y1Both2!=pGroup->lsMarkArr[i]->wingX0Y1Both2)
					continue;//pGroup->lsMarkArr[j]->drawCoordX+=MIN_BOLTDRAWING_SPACE;
				else
					pGroup->lsMarkArr[j]->drawCoordX+=pGroup->lsMarkArr[i]->needExtendDist;
			}
		}
	}
	else if(pGroup->groupTypeStart0End1Mid2==1)	//�ն�������
	{
		if(pGroup->lsMarkArr[pGroup->m_nLsNum-1]->forewardMoveDist>0)
			pGroup->lsMarkArr[pGroup->m_nLsNum-1]->drawCoordX-=pGroup->lsMarkArr[pGroup->m_nLsNum-1]->forewardMoveDist;
		pGroup->lsMarkArr[pGroup->m_nLsNum-1]->needExtendDist+=pGroup->lsMarkArr[pGroup->m_nLsNum-1]->forewardMoveDist;
		for(i=pGroup->m_nLsNum-1;i>=0;i--)
		{
			if(pGroup->lsMarkArr[i]->needExtendDist<=0)
				continue;
			for(int j=0;j<i;j++)
			{
				if( pGroup->lsMarkArr[j]->wingX0Y1Both2<2&&pGroup->lsMarkArr[i]->wingX0Y1Both2<2&&
					pGroup->lsMarkArr[j]->wingX0Y1Both2!=pGroup->lsMarkArr[i]->wingX0Y1Both2)
					continue;//pGroup->lsMarkArr[j]->drawCoordX+=MIN_BOLTDRAWING_SPACE;
				else
					pGroup->lsMarkArr[j]->drawCoordX-=pGroup->lsMarkArr[i]->needExtendDist;

			}
		}
	}
	else
	{
		int midIndex=pGroup->m_nLsNum/2;	//�趨һ�м��λΪ��׼����
		for(i=0;i<pGroup->m_nLsNum;i++)
		{
			if(pGroup->lsMarkArr[i]->needExtendDist<=0&&pGroup->lsMarkArr[i]->forewardMoveDist<=0)
				continue;
			if(i<=midIndex)	//������
			{
				for(int j=0;j<i;j++)
					pGroup->lsMarkArr[j]->drawCoordX-=pGroup->lsMarkArr[i]->needExtendDist;
			}
			else if(i>midIndex)	//������
			{
				for(int j=i;j<pGroup->m_nLsNum;j++)
					pGroup->lsMarkArr[j]->drawCoordX+=pGroup->lsMarkArr[i]->needExtendDist;
			}
		}
	}
}
//��ͳ����һ�����˨(����LsCenterArr��)���з�����ȷ����ͼʱ�����ŷ���
//�˺���������ȷ������߾�֮�����
void CRodDrawing::MakeZoomPlanByBoltsInfo()
{
	LsPosArr.Empty();//RemoveAll();
	int i,nLsPosX=0,nLsPosY=0;
	LsPosMark mark;
	double MIN_WING_STAGGER_SPACE=BOLT_SYMBOL_RADIUS*0.5;	//��ͬ֫��λ��˨�׼����С���Ƽ�� wjh-2016.8.6
	if(!drawPara.bMergeLsAbsoluteDist)
		mark.minPrevDimSpace=MIN_BOLTDRAWING_SPACE=drawPara.fDimTextSize+0.2;
	else	//һ��X���겻ͬ������˨������ƫ��Ϊ�����˨ͼ���뾶 wjh-2016.8.6
		mark.minPrevDimSpace=MIN_BOLTDRAWING_SPACE=BOLT_SYMBOL_RADIUS;	//1.2;
	double clearance=0.1;	//���������ֱ�ע�����С��϶
	double lsPosArrX[1000],lsPosArrY[1000];
	for(i=0;i<LsCenterArr.GetSize();i++)
	{
		if(fabs(LsCenterArr[i]->realPos.x)<EPS||fabs(LsCenterArr[i]->realPos.x-length)<EPS)
			continue;
		if(LsCenterArr[i]->wingX0Y1==0)//feature==0)	//X֫��˨��
		{
			if(nLsPosX>0&&fabs(lsPosArrX[nLsPosX-1]-LsCenterArr[i]->realPos.x)<PRECISION)
				continue;	//���ȷ���λ�ù��ڽӽ���Ϊ��ͬһ��λ
			lsPosArrX[nLsPosX] = LsCenterArr[i]->realPos.x;
			nLsPosX++;
			if(LsPosArr.GetSize()==0||fabs(LsPosArr[LsPosArr.GetSize()-1].realCoordX-LsCenterArr[i]->realPos.x)>PRECISION)
			{
				mark.realCoordX=LsCenterArr[i]->realPos.x;
				mark.drawCoordX=mark.realCoordX*m_fLengthScaleCoef;
				mark.wingX0Y1Both2=0;
				mark.pDimSpaceSizeX=mark.pDimSpaceSizeY=NULL;
				mark.pDimSpaceTextX=mark.pDimSpaceTextY=NULL;
				mark.pDimSpaceSizeX=LsCenterArr[i]->pDimSpaceSize;
				mark.pDimSpaceTextX=LsCenterArr[i]->pDimSpaceText;
				LsPosArr.append(mark);
			}
			else if(LsPosArr[LsPosArr.GetSize()-1].wingX0Y1Both2==1)
			{
				LsPosArr[LsPosArr.GetSize()-1].wingX0Y1Both2=2;
				LsPosArr[LsPosArr.GetSize()-1].pDimSpaceSizeX=LsCenterArr[i]->pDimSpaceSize;
				LsPosArr[LsPosArr.GetSize()-1].pDimSpaceTextX=LsCenterArr[i]->pDimSpaceText;
			}
		}
		else if(LsCenterArr[i]->wingX0Y1==1)//.feature==1)	//Y֫��˨��
		{
			if(nLsPosY>0&&fabs(lsPosArrY[nLsPosY-1]-LsCenterArr[i]->realPos.x)<PRECISION)
				continue;	//���ȷ���λ�ù��ڽӽ���Ϊ��ͬһ��λ
			lsPosArrY[nLsPosY] = LsCenterArr[i]->realPos.x;
			nLsPosY++;
			if(LsPosArr.GetSize()==0||fabs(LsPosArr[LsPosArr.GetSize()-1].realCoordX-LsCenterArr[i]->realPos.x)>PRECISION)
			{
				mark.realCoordX=LsCenterArr[i]->realPos.x;
				mark.drawCoordX=mark.realCoordX*m_fLengthScaleCoef;
				mark.wingX0Y1Both2=1;
				mark.pDimSpaceSizeX=mark.pDimSpaceSizeY=NULL;
				mark.pDimSpaceTextX=mark.pDimSpaceTextY=NULL;
				mark.pDimSpaceSizeY=LsCenterArr[i]->pDimSpaceSize;
				mark.pDimSpaceTextY=LsCenterArr[i]->pDimSpaceText;
				LsPosArr.append(mark);
			}
			else if(LsPosArr[LsPosArr.GetSize()-1].wingX0Y1Both2==0)
			{
				LsPosArr[LsPosArr.GetSize()-1].wingX0Y1Both2=2;
				LsPosArr[LsPosArr.GetSize()-1].pDimSpaceSizeY=LsCenterArr[i]->pDimSpaceSize;
				LsPosArr[LsPosArr.GetSize()-1].pDimSpaceTextY=LsCenterArr[i]->pDimSpaceText;
			}
		}
		else	//����������(�����������)
		{
			if(LsPosArr.GetSize()==0||fabs(LsPosArr[LsPosArr.GetSize()-1].realCoordX-LsCenterArr[i]->realPos.x)>PRECISION)
			{
				mark.realCoordX=LsCenterArr[i]->realPos.x;
				mark.drawCoordX=mark.realCoordX*m_fLengthScaleCoef;
				mark.wingX0Y1Both2=2;
				LsPosArr.append(mark);
			}
		}
	}
	if(dimStyleFlag.IsHasFlag(CRodDrawing::BriefDrawing))
		return;	//���ƽǸּ�ͼʱ����Ҫ������˨���Ƽ�� wht 11-07-15
	//�Զ�����ͳ����˨��
	ATOM_LIST<CRodDrawing::LsGroup>groupList;
	CRodDrawing::LsGroup *pCurLsGroup=NULL;
	CTextOnlyDrawing dim;
	if(drawPara.iDimPrecision==0)		//��ȷ��1.0mm
		CTextOnlyDrawing::PRECISION=1;
	else if(drawPara.iDimPrecision==1)	//��ȷ��0.5mm
		CTextOnlyDrawing::PRECISION=0.5;
	else if(drawPara.iDimPrecision==2)	//��ȷ��0.1mm
		CTextOnlyDrawing::PRECISION=0.1;
	dim.textHeight=drawPara.fDimTextSize;
	dim.iOrder=1;	//������Ϊ��0����������Ϊ��ע���ַ����Զ������ע�ַ���
	for(i=1;i<LsPosArr.GetSize();i++)
	{
		dim.dist=LsPosArr[i].realCoordX-LsPosArr[i-1].realCoordX;
		if(dim.dist<120)
		{
			if(pCurLsGroup==NULL)
			{
				pCurLsGroup=groupList.append();
				pCurLsGroup->lsMarkArr[0]=&LsPosArr[i-1];
				pCurLsGroup->m_nLsNum=1;
				if(i==1&&LsPosArr[i-1].realCoordX<120)
					pCurLsGroup->groupTypeStart0End1Mid2=0;
				else
					pCurLsGroup->groupTypeStart0End1Mid2=2;
			}
			if(i==LsPosArr.GetSize()-1&&length-LsPosArr[i].realCoordX<120)
				pCurLsGroup->groupTypeStart0End1Mid2=1;
			pCurLsGroup->lsMarkArr[pCurLsGroup->m_nLsNum]=&LsPosArr[i];
			pCurLsGroup->m_nLsNum++;
		}
		else
			pCurLsGroup=NULL;
	}
	//����������Ҫ������Ƽ��Ŀ׾��ע
	CRodDrawing::LsPosMark *pPrevLsPosMarkInX=NULL,*pPrevLsPosMarkInY=NULL;
	for(i=0;i<LsPosArr.GetSize();i++)
	{
		LsPosArr[i].needExtendDist=0;
		LsPosArr[i].forewardMoveDist=0;
		if(LsPosArr[i].wingX0Y1Both2==0||LsPosArr[i].wingX0Y1Both2==2)
		{	//X֫��˨
			if(pPrevLsPosMarkInX!=NULL)
			{
				dim.dist=LsPosArr[i].realCoordX-pPrevLsPosMarkInX->realCoordX;
				if(dimStyleFlag.IsHasFlag(DimSpaceTextAlongX))
				{
					if(LsPosArr[i].pDimSpaceSizeX)
					{
						double drawwidth = 0.7*drawPara.fDimTextSize*strlen(LsPosArr[i].pDimSpaceSizeX->DimText())+clearance;
						LsPosArr[i].minPrevDimSpace=drawwidth/LsPosArr[i].pDimSpaceSizeX->nHits;//+clearance;
						if(LsPosArr[i].minPrevDimSpace<MIN_BOLTDRAWING_SPACE)
							LsPosArr[i].minPrevDimSpace=MIN_BOLTDRAWING_SPACE;
					}
					else
						LsPosArr[i].minPrevDimSpace=dim.GetWidth()+clearance;
				}
				else
					LsPosArr[i].minPrevDimSpace=dim.GetHeight()+clearance;
				if(LsPosArr[i].minPrevDimSpace>LsPosArr[i].drawCoordX-pPrevLsPosMarkInX->drawCoordX)
					LsPosArr[i].needExtendDist=LsPosArr[i].minPrevDimSpace-(LsPosArr[i].drawCoordX-pPrevLsPosMarkInX->drawCoordX);
			}
			else
			{
				dim.dist=LsPosArr[i].realCoordX;
				if(dimStyleFlag.IsHasFlag(DimSpaceTextAlongX))
					LsPosArr[i].minPrevDimSpace=dim.GetWidth()+clearance;
				else
					LsPosArr[i].minPrevDimSpace=dim.GetHeight()+clearance;
				if(LsPosArr[i].minPrevDimSpace>LsPosArr[i].drawCoordX)
					LsPosArr[i].needExtendDist=LsPosArr[i].minPrevDimSpace-LsPosArr[i].drawCoordX;
			}
			pPrevLsPosMarkInX=&LsPosArr[i];
		}
		if(LsPosArr[i].wingX0Y1Both2==1||LsPosArr[i].wingX0Y1Both2==2)
		{	//Y֫��˨
			double minPrevDimSpaceX=LsPosArr[i].minPrevDimSpace;
			if(pPrevLsPosMarkInY!=NULL)
			{
				dim.dist=LsPosArr[i].realCoordX-pPrevLsPosMarkInY->realCoordX;
				if(dimStyleFlag.IsHasFlag(DimSpaceTextAlongX))
				{
					if(LsPosArr[i].pDimSpaceSizeY)
					{
						double drawwidth = 0.7*drawPara.fDimTextSize*strlen(LsPosArr[i].pDimSpaceSizeY->DimText())+clearance;
						LsPosArr[i].minPrevDimSpace=drawwidth/LsPosArr[i].pDimSpaceSizeY->nHits;//+clearance;
						if(LsPosArr[i].minPrevDimSpace<MIN_BOLTDRAWING_SPACE)
							LsPosArr[i].minPrevDimSpace=MIN_BOLTDRAWING_SPACE;
					}
					else
						LsPosArr[i].minPrevDimSpace=dim.GetWidth()+clearance;
				}
				else
					LsPosArr[i].minPrevDimSpace=dim.GetHeight()+clearance;
				if(LsPosArr[i].minPrevDimSpace>LsPosArr[i].drawCoordX-pPrevLsPosMarkInY->drawCoordX)
				{
					double dist=LsPosArr[i].minPrevDimSpace-(LsPosArr[i].drawCoordX-pPrevLsPosMarkInY->drawCoordX);
					if(dist>LsPosArr[i].needExtendDist)	//����X��Y֫����������Ҫ��һ�µ����
						LsPosArr[i].needExtendDist=dist;
				}
			}
			else
			{
				dim.dist=LsPosArr[i].realCoordX;
				if(dimStyleFlag.IsHasFlag(DimSpaceTextAlongX))
					LsPosArr[i].minPrevDimSpace=dim.GetWidth()+clearance;
				else
					LsPosArr[i].minPrevDimSpace=dim.GetHeight()+clearance;
				if(LsPosArr[i].minPrevDimSpace>LsPosArr[i].drawCoordX)
				{
					double dist=LsPosArr[i].minPrevDimSpace-LsPosArr[i].drawCoordX;
					if(dist>LsPosArr[i].needExtendDist)	//����X��Y֫����������Ҫ��һ�µ����
						LsPosArr[i].needExtendDist=dist;
				}
			}
			//ͬλ˫֫��˨ʱȡ��С��ǰ����С��ע���루��X֫Ϊ60,Y֫Ϊ840������˴������� wjh-2013.11.19
			if(LsPosArr[i].wingX0Y1Both2==2&&minPrevDimSpaceX<LsPosArr[i].minPrevDimSpace)
				LsPosArr[i].minPrevDimSpace=minPrevDimSpaceX;
			pPrevLsPosMarkInY=&LsPosArr[i];
		}
	}
	if(pPrevLsPosMarkInX)
	{
		double minSpace;
		dim.dist=length-pPrevLsPosMarkInX->realCoordX;
		if(dimStyleFlag.IsHasFlag(DimSpaceTextAlongX))
			minSpace=dim.GetWidth()+clearance;
		else
			minSpace=dim.GetHeight()+clearance;
		if(minSpace>length*m_fLengthScaleCoef-pPrevLsPosMarkInX->drawCoordX)
			pPrevLsPosMarkInX->forewardMoveDist=minSpace-(length*m_fLengthScaleCoef-pPrevLsPosMarkInX->drawCoordX);
	}
	if(pPrevLsPosMarkInY)
	{
		double minSpace;
		dim.dist=length-pPrevLsPosMarkInY->realCoordX;
		if(dimStyleFlag.IsHasFlag(DimSpaceTextAlongX))
			minSpace=dim.GetWidth()+clearance;
		else
			minSpace=dim.GetHeight()+clearance;
		if(minSpace>length*m_fLengthScaleCoef-pPrevLsPosMarkInY->drawCoordX)
			pPrevLsPosMarkInY->forewardMoveDist=minSpace-(length*m_fLengthScaleCoef-pPrevLsPosMarkInY->drawCoordX);
	}
	for(LsGroup *pLsGroup=groupList.GetFirst();pLsGroup;pLsGroup=groupList.GetNext())
		ExtendLsSpaceDim(pLsGroup,LsPosArr.m_pData,LsPosArr.GetSize());
	//��̫��ʱ,�������λ�ᵼ����˨��֮�����С,�ڴ���Ҫ��������ɨ�����һ������������ɨ�����һ��
	//ɨ����˨����ص��׾���������
	pPrevLsPosMarkInX=pPrevLsPosMarkInY=NULL;
	for(i=0;i<LsPosArr.GetSize();i++)
	{
		double drawSpace;
		CRodDrawing::LsPosMark *pTempPrevLsPosMarkX=NULL;
		CRodDrawing::LsPosMark *pTempPrevLsPosMarkY=NULL;
		if(i<LsPosArr.GetSize()-1)
		{
			double localMinBoltDrawSpace=MIN_BOLTDRAWING_SPACE;
			if(LsPosArr[i].wingX0Y1Both2+LsPosArr[i+1].wingX0Y1Both2==1)
				localMinBoltDrawSpace=MIN_WING_STAGGER_SPACE;	//�ڽ���˨������ͬ֫ʱ��С��� wjh-2016.8.6
			if(LsPosArr[i].drawCoordX>LsPosArr[i+1].drawCoordX-localMinBoltDrawSpace)
				LsPosArr[i+1].drawCoordX=LsPosArr[i].drawCoordX+localMinBoltDrawSpace;
		}
		if(LsPosArr[i].wingX0Y1Both2==0||LsPosArr[i].wingX0Y1Both2==2)
		{	//X֫��˨
			if(pPrevLsPosMarkInX!=NULL)
				drawSpace=LsPosArr[i].drawCoordX-pPrevLsPosMarkInX->drawCoordX;
			else
				drawSpace=LsPosArr[i].drawCoordX;
			if(LsPosArr[i].minPrevDimSpace>drawSpace)	//������
			{
				for(int j=i;j<LsPosArr.GetSize()-1;j++)
				{
					if(j==i)
					{
						LsPosArr[j].drawCoordX+=LsPosArr[i].minPrevDimSpace-drawSpace;
						pTempPrevLsPosMarkX=&LsPosArr[j];
						if(LsPosArr[j].wingX0Y1Both2==2)
							pTempPrevLsPosMarkY=&LsPosArr[j];
					}
					else
					{
						bool bCanBreak=false;
						if(LsPosArr[j].wingX0Y1Both2==0||LsPosArr[j].wingX0Y1Both2==2)
						{
							if(LsPosArr[j].drawCoordX>=pTempPrevLsPosMarkX->drawCoordX+LsPosArr[j].minPrevDimSpace)
								bCanBreak=true;	//��ǰ����㹻���ü�����λ
							else
								LsPosArr[j].drawCoordX+=LsPosArr[j].minPrevDimSpace-(LsPosArr[j].drawCoordX-pTempPrevLsPosMarkX->drawCoordX);
							pTempPrevLsPosMarkX=&LsPosArr[j];
						}
						if(LsPosArr[j].wingX0Y1Both2==1||LsPosArr[j].wingX0Y1Both2==2)
						{
							if(pTempPrevLsPosMarkY!=NULL)
							{
								if(LsPosArr[j].drawCoordX>=pTempPrevLsPosMarkY->drawCoordX+LsPosArr[j].minPrevDimSpace)
								{
									if(bCanBreak)	//��ǰ����㹻���ü�����λ
										break;
								}
								else
								{
									LsPosArr[j].drawCoordX+=LsPosArr[j].minPrevDimSpace-(LsPosArr[j].drawCoordX-pTempPrevLsPosMarkY->drawCoordX);
									bCanBreak=false;
								}
							}
							else
							{
								if(LsPosArr[j].drawCoordX>=pTempPrevLsPosMarkX->drawCoordX+MIN_WING_STAGGER_SPACE)//drawPara.fDimTextSize*0.5+clearance)
								{	//��ͬ֫��λ�׼��С��С������ԭ����"����ָ�+��϶" --> MIN_WING_STAGGER_SPACE wjh-2016.8.6
									if(bCanBreak)	//��ǰ����㹻���ü�����λ
										break;
								}
								else
								{
									LsPosArr[j].drawCoordX+=MIN_WING_STAGGER_SPACE-(LsPosArr[j].drawCoordX-pTempPrevLsPosMarkX->drawCoordX);
									bCanBreak=false;
								}
							}
							pTempPrevLsPosMarkY=&LsPosArr[j];
						}
						if(bCanBreak)	//��ǰ����㹻���ü�����λ
							break;
					}
				}
			}
			pPrevLsPosMarkInX=&LsPosArr[i];
		}
		if(LsPosArr[i].wingX0Y1Both2==1||LsPosArr[i].wingX0Y1Both2==2)
		{	//Y֫��˨
			if(pPrevLsPosMarkInY!=NULL)
				drawSpace=LsPosArr[i].drawCoordX-pPrevLsPosMarkInY->drawCoordX;
			else
				drawSpace=LsPosArr[i].drawCoordX;
			if(LsPosArr[i].minPrevDimSpace>drawSpace)	//������
			{
				for(int j=i;j<LsPosArr.GetSize()-1;j++)
				{
					if(j==i)
					{
						LsPosArr[j].drawCoordX+=LsPosArr[i].minPrevDimSpace-drawSpace;
						pTempPrevLsPosMarkY=&LsPosArr[j];
						if(LsPosArr[j].wingX0Y1Both2==2)
							pTempPrevLsPosMarkX=&LsPosArr[j];
					}
					else
					{
						bool bCanBreak=false;
						if(LsPosArr[j].wingX0Y1Both2==1||LsPosArr[j].wingX0Y1Both2==2)
						{
							if(LsPosArr[j].drawCoordX>=pTempPrevLsPosMarkY->drawCoordX+LsPosArr[j].minPrevDimSpace)
								bCanBreak=true;	//��ǰ����㹻���ü�����λ
							else
								LsPosArr[j].drawCoordX+=LsPosArr[j].minPrevDimSpace-(LsPosArr[j].drawCoordX-pTempPrevLsPosMarkY->drawCoordX);
							pTempPrevLsPosMarkY=&LsPosArr[j];
						}
						if(LsPosArr[j].wingX0Y1Both2==0||LsPosArr[j].wingX0Y1Both2==2)
						{
							if(pTempPrevLsPosMarkX!=NULL)
							{
								if(LsPosArr[j].drawCoordX>=pTempPrevLsPosMarkX->drawCoordX+LsPosArr[j].minPrevDimSpace)
								{
									if(bCanBreak)
										break;	//��ǰ����㹻���ü�����λ
								}
								else
								{
									LsPosArr[j].drawCoordX+=LsPosArr[j].minPrevDimSpace-(LsPosArr[j].drawCoordX-pTempPrevLsPosMarkX->drawCoordX);
									bCanBreak=false;
								}
							}
							else
							{
								if(LsPosArr[j].drawCoordX>=pTempPrevLsPosMarkY->drawCoordX+MIN_WING_STAGGER_SPACE)
								{	//��ͬ֫��λ�׼��С��С������ԭ����"����ָ�+��϶" --> MIN_WING_STAGGER_SPACE wjh-2016.8.6
									if(bCanBreak)
										break;	//��ǰ����㹻���ü�����λ
								}
								else
								{
									LsPosArr[j].drawCoordX+=MIN_WING_STAGGER_SPACE-(LsPosArr[j].drawCoordX-pTempPrevLsPosMarkY->drawCoordX);
									bCanBreak=false;
								}
							}
							pTempPrevLsPosMarkX=&LsPosArr[j];
						}
						if(bCanBreak)
							break;
					}
				}
			}
			pPrevLsPosMarkInY=&LsPosArr[i];
		}
	}
	//ɨ����˨����ص��׾���������
	CRodDrawing::LsPosMark *pAfterLsPosMarkInX=NULL;
	CRodDrawing::LsPosMark *pAfterLsPosMarkInY=NULL;
	for(i=LsPosArr.GetSize()-1;i>0;i--)
	{
		double drawSpace,minPrevDimSpace=0;
		CRodDrawing::LsPosMark *pTempAfterLsPosMarkX=NULL;
		CRodDrawing::LsPosMark *pTempAfterLsPosMarkY=NULL;
		double localMinBoltDrawSpace=MIN_BOLTDRAWING_SPACE;
		if(LsPosArr[i].wingX0Y1Both2+LsPosArr[i-1].wingX0Y1Both2==1)
			localMinBoltDrawSpace=MIN_WING_STAGGER_SPACE;	//�ڽ���˨������ͬ֫ʱ��С��� wjh-2016.8.6
		if(LsPosArr[i].drawCoordX<LsPosArr[i-1].drawCoordX+localMinBoltDrawSpace)
			LsPosArr[i-1].drawCoordX=LsPosArr[i].drawCoordX-localMinBoltDrawSpace;
		if(LsPosArr[i].wingX0Y1Both2==0||LsPosArr[i].wingX0Y1Both2==2)
		{	//X֫��˨
			if(pAfterLsPosMarkInX!=NULL)
			{
				drawSpace=pAfterLsPosMarkInX->drawCoordX-LsPosArr[i].drawCoordX;
				minPrevDimSpace=pAfterLsPosMarkInX->minPrevDimSpace;
			}
			else
			{
				drawSpace=length*m_fLengthScaleCoef-LsPosArr[i].drawCoordX;
				dim.dist=length-LsPosArr[i].realCoordX;
				minPrevDimSpace=dim.GetWidth();
			}
			if(minPrevDimSpace>drawSpace)	//������
			{
				for(int j=i;j>=0;j--)
				{
					if(j==i)
					{
						LsPosArr[j].drawCoordX-=minPrevDimSpace-drawSpace;
						pTempAfterLsPosMarkX=&LsPosArr[j];
						if(LsPosArr[j].wingX0Y1Both2==2)
							pTempAfterLsPosMarkY=&LsPosArr[j];
					}
					else
					{
						bool bCanBreak=false;
						if(LsPosArr[j].wingX0Y1Both2==0||LsPosArr[j].wingX0Y1Both2==2)
						{
							if(pTempAfterLsPosMarkX->drawCoordX>=LsPosArr[j].drawCoordX+pTempAfterLsPosMarkX->minPrevDimSpace)
								bCanBreak=true;	//��ǰ����㹻���ü�����λ
							else
								LsPosArr[j].drawCoordX-=pTempAfterLsPosMarkX->minPrevDimSpace-(pTempAfterLsPosMarkX->drawCoordX-LsPosArr[j].drawCoordX);
							pTempAfterLsPosMarkX=&LsPosArr[j];
						}
						if(LsPosArr[j].wingX0Y1Both2==1||LsPosArr[j].wingX0Y1Both2==2)
						{
							if(pTempAfterLsPosMarkY!=NULL)
							{
								if(pTempAfterLsPosMarkY->drawCoordX>=LsPosArr[j].drawCoordX+pTempAfterLsPosMarkY->minPrevDimSpace)
								{
									if(bCanBreak)
										break;
								}
								else
								{
									LsPosArr[j].drawCoordX-=pTempAfterLsPosMarkY->minPrevDimSpace-(pTempAfterLsPosMarkY->drawCoordX-LsPosArr[j].drawCoordX);
									bCanBreak=false;
								}
							}
							else
							{
								if(pTempAfterLsPosMarkX->drawCoordX>=LsPosArr[j].drawCoordX+MIN_WING_STAGGER_SPACE)
								{	//��ͬ֫��λ�׼��С��С������ԭ����"����ָ�+��϶" --> MIN_WING_STAGGER_SPACE wjh-2016.8.6
									if(bCanBreak)
										break;	//��ǰ����㹻���ü�����λ
								}
								else
								{
									LsPosArr[j].drawCoordX-=MIN_WING_STAGGER_SPACE-(pTempAfterLsPosMarkX->drawCoordX-LsPosArr[j].drawCoordX);
									bCanBreak=false;
								}
							}
							pTempAfterLsPosMarkY=&LsPosArr[j];
						}
						if(bCanBreak)
							break;
					}
				}
			}
			pAfterLsPosMarkInX=&LsPosArr[i];
		}
		if(LsPosArr[i].wingX0Y1Both2==1||LsPosArr[i].wingX0Y1Both2==2)
		{	//Y֫��˨
			if(pAfterLsPosMarkInY!=NULL)
			{
				drawSpace=pAfterLsPosMarkInY->drawCoordX-LsPosArr[i].drawCoordX;
				minPrevDimSpace=pAfterLsPosMarkInY->minPrevDimSpace;
			}
			else
			{
				drawSpace=length*m_fLengthScaleCoef-LsPosArr[i].drawCoordX;
				dim.dist=length-LsPosArr[i].realCoordX;
				minPrevDimSpace=dim.GetWidth();
			}
			if(minPrevDimSpace>drawSpace)	//������
			{
				for(int j=i;j>=0;j--)
				{
					if(j==i)
					{
						LsPosArr[j].drawCoordX-=minPrevDimSpace-drawSpace;
						pTempAfterLsPosMarkY=&LsPosArr[j];
						if(LsPosArr[j].wingX0Y1Both2==2)
							pTempAfterLsPosMarkX=&LsPosArr[j];
					}
					else
					{
						bool bCanBreak=false;
						if(LsPosArr[j].wingX0Y1Both2==1||LsPosArr[j].wingX0Y1Both2==2)
						{
							if(pTempAfterLsPosMarkY->drawCoordX>=LsPosArr[j].drawCoordX+pTempAfterLsPosMarkY->minPrevDimSpace)
								bCanBreak=true;	//��ǰ����㹻���ü�����λ
							else
								LsPosArr[j].drawCoordX-=pTempAfterLsPosMarkY->minPrevDimSpace-(pTempAfterLsPosMarkY->drawCoordX-LsPosArr[j].drawCoordX);
							pTempAfterLsPosMarkY=&LsPosArr[j];
						}
						if(LsPosArr[j].wingX0Y1Both2==0||LsPosArr[j].wingX0Y1Both2==2)
						{
							if(pTempAfterLsPosMarkX!=NULL)
							{
								if(pTempAfterLsPosMarkX->drawCoordX>=LsPosArr[j].drawCoordX+pTempAfterLsPosMarkX->minPrevDimSpace)
								{
									if(bCanBreak)
										break;
								}
								else
								{
									LsPosArr[j].drawCoordX-=pTempAfterLsPosMarkX->minPrevDimSpace-(pTempAfterLsPosMarkX->drawCoordX-LsPosArr[j].drawCoordX);
									bCanBreak=false;
								}
							}
							else
							{
								if(pTempAfterLsPosMarkY->drawCoordX>=LsPosArr[j].drawCoordX+MIN_WING_STAGGER_SPACE)
								{	//��ͬ֫��λ�׼��С��С������ԭ����"����ָ�+��϶" --> MIN_WING_STAGGER_SPACE wjh-2016.8.6
									if(bCanBreak)
										break;	//��ǰ����㹻���ü�����λ
								}
								else
								{
									LsPosArr[j].drawCoordX-=MIN_WING_STAGGER_SPACE-(pTempAfterLsPosMarkY->drawCoordX-LsPosArr[j].drawCoordX);
									bCanBreak=false;
								}
							}
							pTempAfterLsPosMarkX=&LsPosArr[j];
						}
						if(bCanBreak)
							break;
					}
				}
			}
			pAfterLsPosMarkInY=&LsPosArr[i];
		}
	}
	if(LsPosArr.GetSize()>0 && LsPosArr[0].drawCoordX-LsPosArr[0].minPrevDimSpace<0)
	{	//������ͼ����������ʱ���Զ������ӳ���ͼ���򣨼���������) wjh-2016.8.6
		double offset_right=-LsPosArr[0].drawCoordX+LsPosArr[0].minPrevDimSpace;
		for(i=0;i<LsPosArr.GetSize();i++)
			LsPosArr[i].drawCoordX+=offset_right;
		m_fLengthScaleCoef+=offset_right/length;
	}
}
void CRodDrawing::InitWingAxisCoordY()
{
	int i;
	double minRowSpace=3.0,MIN_ROW_SPACE=3.0,firstRowSpace=1.3*drawPara.fDimTextSize;
	//��������߳�ʼ����ֵ
	for(i=0;i<m_nWingAxisGX;i++)
		wingAxisCoordArrX[i]=-wingAxisGArrX[i]*m_fVertScaleCoef;
	for(i=0;i<m_nWingAxisGY;i++)
		wingAxisCoordArrY[i]=wingAxisGArrY[i]*m_fVertScaleCoef;
	if(dimStyleFlag.IsHasFlag(BriefDrawing))	//���ƽǸּ�ͼʱӦ��֤һ����С����ֵ
		return;	//��ͼ�в��ÿ������߹���������
	if(fabs(wingAxisCoordArrX[0])<firstRowSpace)
		wingAxisCoordArrX[0]=-firstRowSpace;
	if(fabs(wingAxisCoordArrY[0])<firstRowSpace)
		wingAxisCoordArrY[0]=firstRowSpace;
	//��������ɨ���ų���������
	for(i=1;i<m_nWingAxisGX;i++)
	{
		if(fabs(wingAxisCoordArrX[i])-fabs(wingAxisCoordArrX[i-1])<minRowSpace)
			wingAxisCoordArrX[i]=wingAxisCoordArrX[i-1]-minRowSpace;
	}
	for(i=1;i<m_nWingAxisGY;i++)
	{
		if(fabs(wingAxisCoordArrY[i])-fabs(wingAxisCoordArrY[i-1])<minRowSpace)
			wingAxisCoordArrY[i]=wingAxisCoordArrY[i-1]+minRowSpace;
	}
	//�����������߳���֫��Χ����Ӧ�ٴ�������ɨ�����
	double minCalRowSpaceX=(wingWide*m_fVertScaleCoef-firstRowSpace)/(m_nWingAxisGX);	//���һ��'1'��Ϊ�����ڲ���ռ���������߻��ƾ���
	double minCalRowSpaceY=(wingWide*m_fVertScaleCoef-firstRowSpace)/(m_nWingAxisGY);	//���һ��'1'��Ϊ�����ڲ���ռ���������߻��ƾ���
	double scale=minCalRowSpaceX/minCalRowSpaceY;
	if(scale<1.15&&scale>0.85)	//��֫����������15%ȡͬ���� wjh-2014.2.24
		minCalRowSpaceX=minCalRowSpaceY=min(minCalRowSpaceX,minCalRowSpaceY);
	if(minCalRowSpaceX<MIN_ROW_SPACE)
		minRowSpace=minCalRowSpaceX;	//���֫��ʵ�ʻ��Ƹ߶�ʵ��̫�ߣ���Ӧ�Ⱦ���С���ø�����λ�� wjh-2012.3.13
	if(m_nWingAxisGX>0&&wingAxisCoordArrX[m_nWingAxisGX-1]<-wingWide*m_fVertScaleCoef+minRowSpace)
	{
		wingAxisCoordArrX[m_nWingAxisGX-1]=-wingWide*m_fVertScaleCoef+minRowSpace;
		for(i=m_nWingAxisGX-1;i>0;i--)
		{
			if(fabs(wingAxisCoordArrX[i])-fabs(wingAxisCoordArrX[i-1])<minRowSpace)
				wingAxisCoordArrX[i-1]=wingAxisCoordArrX[i]+minRowSpace;
		}
	}
	if(minCalRowSpaceY<MIN_ROW_SPACE)
		minRowSpace=minCalRowSpaceY;	//���֫��ʵ�ʻ��Ƹ߶�ʵ��̫�ߣ���Ӧ�Ⱦ���С���ø�����λ�� wjh-2012.3.13
	if(m_nWingAxisGY>0&&wingAxisCoordArrY[m_nWingAxisGY-1]>wingWide*m_fVertScaleCoef-minRowSpace)
	{
		wingAxisCoordArrY[m_nWingAxisGY-1]=wingWide*m_fVertScaleCoef-minRowSpace;
		for(i=m_nWingAxisGY-1;i>0;i--)
		{
			if(wingAxisCoordArrY[i]-wingAxisCoordArrY[i-1]<minRowSpace)
				wingAxisCoordArrY[i-1]=wingAxisCoordArrY[i]-minRowSpace;
		}
	}
}
void CRodDrawing::CalDrawingScaleCoef(bool bEndCutAngleOrWing,bool bStartCutAngleOrWing)
{
	if(!dimStyleFlag.IsHasFlag(CRodDrawing::SpecifyDrawscale))
	{	//֧��ָ���Ǹֻ�ͼ�������ƣ���Ҫ�������ɶ̽Ǹִ���ͼ wht 11-03-05
		if(!dimStyleFlag.IsHasFlag(ModulateLongJg))
			return;					//��Ҫ��Ǹֳ��ȵ���
		//����X��Y֫���ߵ�����ȷ�������ҪԤ�������ٱ�ע�߾�
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimAxisGTextAtStart))
			leftGDimMargin = 2*drawPara.fDimTextSize*(max(m_nWingAxisGX,m_nWingAxisGY)+1);
		if(dimStyleFlag.IsHasFlag(CRodDrawing::CutAngleDimByText))
		{
			if(bEndCutAngleOrWing)
				rightMargin=3*drawPara.fDimTextSize;	//��ע�н���Ϣʱ�Ҳ�Ԥ���������Ϊ3������߶� wht 10-03-10
			if(bStartCutAngleOrWing)
				leftGDimMargin+=2*drawPara.fDimTextSize;			//ʼ�˴����нǻ���֫ʱ��Ҫ��������϶ wht 12-03-13
		}
		//if(leftGDimMargin<5)
		//	leftGDimMargin=5;	//����ҲҪԤ����5mm
		m_fLengthScaleCoef=(maxDrawingLength-leftGDimMargin-rightMargin)/length;
		if(!drawPara.bMaxExtendAngleLength&&m_fLengthScaleCoef>0.15)//�趨�������(���ȷ���)ѹ���������Է�ֹһЩ�̽Ǹ���׶���ƹ��� wjh-2013.10.24
			m_fLengthScaleCoef=0.15;
		double realDrawingHeight;
		if(dimStyleFlag.IsHasFlag(BriefDrawing))	//���ƽǸּ�ͼʱӦ��֤һ����С����ֵ
			realDrawingHeight=max(0.5*(maxDrawingHeight-6*drawPara.fDimTextSize),0.2*maxDrawingHeight);
		else if(dimStyleFlag.IsHasFlag(OneCardMultiPart))
			realDrawingHeight=0.5*(maxDrawingHeight-20*drawPara.fDimTextSize);
		else 
			realDrawingHeight=0.5*(maxDrawingHeight-25*drawPara.fDimTextSize);
		//�ָ����ù���ʱrealDrawingHeight���ܳ��ָ������豣֤һ����С����ֵ wht 16-06-07
		if(realDrawingHeight<0)
			realDrawingHeight=0.2*maxDrawingHeight;
		if(realDrawingHeight>max(m_nWingAxisGX,m_nWingAxisGY)*1.2*drawPara.fDimTextSize+2.5*drawPara.fDimTextSize)
			realDrawingHeight=max(m_nWingAxisGX,m_nWingAxisGY)*1.2*drawPara.fDimTextSize+2.5*drawPara.fDimTextSize;
		m_fVertScaleCoef=realDrawingHeight/wingWide;
		if(dimStyleFlag.IsHasFlag(UseMinDrawscale))
		{	//����ͬ�����ţ�ʹ���������н�С�����ű��� wht 11-05-07
			double fMinScale=min(m_fVertScaleCoef,m_fLengthScaleCoef);
			m_fLengthScaleCoef=m_fVertScaleCoef=fMinScale;
		}
		InitWingAxisCoordY();
	}
	else 
	{	//ָ�����ű���Ĭ��Ϊ����ͼ���� wht 11-05-07
		if(drawPara.iJgZoomSchema==1)
		{
			SetLengthScaleCoef(1.0/drawPara.fRealToDraw);
			SetVertScaleCoef(1.0/drawPara.fRealToDraw);
		}
		InitWingAxisCoordY();
	}
	MakeZoomPlanByBoltsInfo();
}

//////////////////////////////////////////////////////////////////////////
//
CAngleDrawing::CAngleDrawing()
{
	edgeLineArr.SetSize(5);
	edgeLineArr[1].feature=edgeLineArr[3].feature=1;	//���߻���
	part_no_arr.SetSize(0,20);
}
CAngleDrawing::~CAngleDrawing()
{

}
IMPLEMENT_GET(CAngleDrawing,fDimTextSize)			//���ȳߴ��ע�ı���
{
#ifdef __DRAWING_CONTEXT_
	if(m_ciResideContext==CONTEXT_PARTDRAW)
		return sys.fonts.angle.fDimTextSize;
	else if(m_ciResideContext==CONTEXT_WELDDRAW)
		return sys.fonts.weld.fDimTextSize;
	else if(m_ciResideContext==CONTEXT_STRUDRAW)
		return sys.fonts.segment.fDimTextSize;
	else
		return sys.fonts.basic.fDimTextSize;
#else
	return drawPara.fDimTextSize;
#endif
}
IMPLEMENT_GET(CAngleDrawing,fDimAngleTextSize)	//�Ƕȳߴ��ע�ı���
{
#ifdef __DRAWING_CONTEXT_
	if(m_ciResideContext==CRodDrawing::CONTEXT_PARTDRAW)
		return sys.fonts.angle.fDimAngleTextSize;
	else if(m_ciResideContext==CONTEXT_WELDDRAW)
		return sys.fonts.weld.fDimAngleTextSize;
	else if(m_ciResideContext==CONTEXT_STRUDRAW)
		return sys.fonts.segment.fDimAngleTextSize;
	else
		return sys.fonts.basic.fDimAngleTextSize;
#else
	return drawPara.fDimTextSize;
#endif
}
IMPLEMENT_GET(CAngleDrawing,fDimLabelTextSize)	//����������ָ�		2.5
{
#ifdef __DRAWING_CONTEXT_
	if(m_ciResideContext==CRodDrawing::CONTEXT_PARTDRAW)
		return sys.fonts.angle.fPartNoTextSize;
	else if(m_ciResideContext==CRodDrawing::CONTEXT_WELDDRAW)
		return sys.fonts.weld.fPartNoTextSize;
	else if(m_ciResideContext==CRodDrawing::CONTEXT_STRUDRAW)
		return sys.fonts.segment.fPartNoTextSize;
	else
		return sys.fonts.basic.fPartNoTextSize;
#else
	return drawPara.fPartNoTextSize;
#endif
}
IMPLEMENT_GET(CAngleDrawing,fDimSizeSpecTextSize)	//����������ָ�		2.5
{
#ifdef __DRAWING_CONTEXT_
	if(m_ciResideContext==CONTEXT_PARTDRAW)
		return sys.fonts.angle.fPartGuigeTextSize;
	else if(m_ciResideContext==CONTEXT_WELDDRAW)
		return sys.fonts.weld.fPartGuigeTextSize;
	else if(m_ciResideContext==CONTEXT_STRUDRAW)
		return sys.fonts.segment.fPartGuigeTextSize;
	else
		return sys.fonts.basic.fPartGuigeTextSize;
#else
	return drawPara.fPartGuigeTextSize;
#endif
}

void CRodDrawing::InitDimStyleBySysSetting(int nBoltNum)
{
	int nBolt = nBoltNum;
	if(drawPara.iLsSpaceDimStyle==0)		//��X�᷽��
		dimStyleFlag.SetFlagDword(CRodDrawing::DimSpaceTextAlongX);
	else if(drawPara.iLsSpaceDimStyle==1)	//��Y�᷽��
		dimStyleFlag.SetFlagDword(CRodDrawing::DimSpaceTextAlongY);
	else
	{	//������˨���Զ��ж�		
		if(nBolt>drawPara.nMaxBoltNumAlongX)
			dimStyleFlag.SetFlagDword(CRodDrawing::DimSpaceTextAlongY);
		else
			dimStyleFlag.SetFlagDword(CRodDrawing::DimSpaceTextAlongX);
	}
	if(drawPara.bSplitAngleSketchDrawing)	//�Ǹֲ�ͼ��Ϊ��������ͼ���޷�������ע�ľ�
		dimStyleFlag.SetFlagDword(CRodDrawing::DimAxisGTextAtMiddle);
	else if(drawPara.iJgGDimStyle==0)		//ʼ�˱�ע�ľ�
		dimStyleFlag.SetFlagDword(CRodDrawing::DimAxisGTextAtStart);
	else if(drawPara.iJgGDimStyle==1)	//�м��ע�ľ�
		dimStyleFlag.SetFlagDword(CRodDrawing::DimAxisGTextAtMiddle);
	else
	{	//������˨���Զ��ж�
		if(nBolt<drawPara.nMaxBoltNumStartDimG)
			dimStyleFlag.SetFlagDword(CRodDrawing::DimAxisGTextAtStart);
		else
			dimStyleFlag.SetFlagDword(CRodDrawing::DimAxisGTextAtMiddle);
	}
	//�����Ƿ���Ҫ�����Ǹֳ��� wht 11-03-05
	//if(drawPara.bModulateLongJg)
	//	dimStyleFlag.SetFlagDword(CAngleDrawing::ModulateLongJg);
	//���ýǸ����Ų��� wht 11-05-07
	if(drawPara.iJgZoomSchema==1)
		dimStyleFlag.SetFlagDword(CRodDrawing::SpecifyDrawscale);	//ָ���Ǹ����ű�����Ĭ��Ϊ����ͼ����
	else if(drawPara.iJgZoomSchema==2||drawPara.iJgZoomSchema==3)
	{
		dimStyleFlag.SetFlagDword(CRodDrawing::ModulateLongJg);	//���ݻ�ͼ��������Ǹֳ��� 
		if(drawPara.iJgZoomSchema==2)
			dimStyleFlag.SetFlagDword(CRodDrawing::UseMinDrawscale);//����ͬ�����ţ�ʹ���������н�С�����ű��� 
	}
}

#ifdef __DRAWING_CONTEXT_
void UpdateBoltList(CLDSLineAngle *pLineAngle,UCS_STRU angleDrawUcs,ATOM_LIST<BOLT_INFO> &bolt_list)
{
	for(CLsRef *pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
	{
		f3dCircle cir;
		ProjectJgLsToMap(angleDrawUcs,pLsRef->GetLsPtr(),pLineAngle->get_norm_x_wing(),pLineAngle->get_norm_y_wing(),cir.centre);
		double g =pLineAngle->GetLsG(pLsRef);
		if(fabs(pLineAngle->get_norm_x_wing()*(*pLsRef)->get_norm())>
			fabs(pLineAngle->get_norm_y_wing()*(*pLsRef)->get_norm()))	//��˨�ڽǸ�X֫��
			cir.centre.y=-(float)g;					//��ȷ���λ�ã�float������ֵ��ʾλ�ڣ�֫����ֵ��ʾλ�ڣ�֫��
		else															//��˨�ڽǸ�Y֫��
			cir.centre.y=(float)g;					//��ȷ���λ�ã�float������ֵ��ʾλ�ڣ�֫����ֵ��ʾλ�ڣ�֫��
		//�Բ���Ҫ���п��ϽǵĶ̽Ǹֽ�����˨λ������   wxc-2016.10.28
		if((pLineAngle->pStart==NULL || pLineAngle->pEnd==NULL)&&pLineAngle->GetKaiHeWingX0Y1()<0)
			pLineAngle->EmendShortJgLsPosY(pLsRef->GetLsPtr(),cir.centre.y);
		BOLT_INFO bolt_info;
		bolt_info.posX=(float)cir.centre.x;
		bolt_info.posY=(float)cir.centre.y;
		bolt_info.bolt_d=(short)(*pLsRef)->get_d();
		bolt_info.waistVec=(*pLsRef)->ucs.axis_x;
		bolt_info.waistLen=pLsRef->waistLen;
		bolt_info.hole_d_increment=(float)(*pLsRef)->hole_d_increment;	
		bolt_info.m_dwFlag=(*pLsRef)->m_dwFlag;			//��˨������λ
		bolt_info.cFuncType=(*pLsRef)->FilterFuncType();		//��˨��������
		bolt_info.cFlag=pLsRef->cFlag;					//��˨�׹�������
		vector_trans(bolt_info.waistVec,angleDrawUcs,FALSE);
		bolt_list.append(bolt_info);
	}
}
void UpdateRibPlateList(CLDSLineAngle *pLineAngle,UCS_STRU angleDrawUcs,ATOM_LIST<RIB_PLATE_INFO> &rib_plate_list)
{
	//���Ǹ��߰嵱����˨�������� wht 10-05-13
	f3dPoint angle_vec=pLineAngle->End()-pLineAngle->Start();
	normalize(angle_vec);
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{	
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate *pRibPlate=(CLDSGeneralPlate*)pPart;
		if(pRibPlate->m_hPartWeldParent!=pLineAngle->handle)
			continue;
		BOOL bIsRibPlate=(pRibPlate->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pRibPlate)->m_iParamType==TYPE_LINEANGLERIBPLATE);
		if(!bIsRibPlate)
			bIsRibPlate=fabs(angle_vec*pRibPlate->ucs.axis_z)>EPS_COS;
		if(!bIsRibPlate)
			continue;
		f3dPoint realPos=pRibPlate->ucs.origin+(pRibPlate->GetNormOffset()+pRibPlate->GetThick()*0.5)*pRibPlate->ucs.axis_z;
		coord_trans(realPos,angleDrawUcs,FALSE);
		RIB_PLATE_INFO rib_plate_info;
		rib_plate_info.fPosZ=(float)realPos.z;
		rib_plate_info.sPlateNo.Copy(pRibPlate->GetPartNo());
		rib_plate_info.m_cMaterial=pRibPlate->cMaterial;
		rib_plate_info.fThick=(float)pRibPlate->GetThick();
		rib_plate_list.append(rib_plate_info);
	}
}
BOOL CLineAngleDrawing::CreateLineAngleDrawing(CLDSLineAngle *pLineAngle,double max_drawing_length,double max_drawing_height,
									   double text_height/*=2.5*/,double w_to_h/*=0.7*/,double min_space/*=3.5*/)
{
	if(pLineAngle==NULL)
		return FALSE;
	sPartNo.Copy(pLineAngle->GetPartNo());
	int nBoltNum=pLineAngle->GetLsCount();
	InitDimStyleBySysSetting(nBoltNum);
	drawPara.fDimTextSize=text_height;
	WtoH=w_to_h;
	minDrawingSpace=min_space;			//�׾���С����ֵ
	maxDrawingLength=max_drawing_length;//��ͼ�������ÿ��
	maxDrawingHeight=max_drawing_height;//��ͼ�������ø߶�
	//���ɻ�ͼ����ϵ
	UCS_STRU angleDrawUcs;
	pLineAngle->getUCS(angleDrawUcs);	
	angleDrawUcs.origin-=angleDrawUcs.axis_z*pLineAngle->startOdd();
	wingWide=pLineAngle->GetWidth();				//�Ǹ�֫��
	wingThick=pLineAngle->GetThick();				//�Ǹ�֫��
	length=pLineAngle->GetLength();					//�Ǹֳ���
	m_nWingAxisGX=m_nWingAxisGY=0;					//���������˨����
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&!pLineAngle->IsCutAngleOrWing())
	{
		rightMargin=1.5;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimAxisGTextAtMiddle))
			leftGDimMargin=1.5;
	}
	else
	{
		leftGDimMargin=5;
		rightMargin=5;
	}
	LsCirX.SetSize(0,pLineAngle->GetLsCount()*2/3);
	LsCirY.SetSize(0,pLineAngle->GetLsCount()*2/3);
	ATOM_LIST<RIB_PLATE_INFO> rib_plate_list;
	if(drawPara.bDimRibPlateSetUpPos)
		UpdateRibPlateList(pLineAngle,angleDrawUcs,rib_plate_list);

	bool bEndCutAngleOrWing=false,bStartCutAngleOrWing=false;
	THANDLE handle=pLineAngle->handle;
	//��˨��Ϣ
	ATOM_LIST<BOLT_INFO> bolt_list;
	UpdateBoltList(pLineAngle,angleDrawUcs,bolt_list);
	bStartCutAngleOrWing=pLineAngle->IsStartCutAngleOrWing();
	bEndCutAngleOrWing = pLineAngle->IsEndCutAngleOrWing();
	wingWide =pLineAngle->GetWidth();
	wingThick=pLineAngle->GetThick();
	//�м�ѹ��λ�ñ�ע����δ����
	long nMidFlatPosZ=0;
	/*if((pLineAngle->mid_push_flat_x1_y2==1||pLineAngle->mid_push_flat_x1_y2==2)&&
		pLineAngle->mid_push_flat_length>0)
	{
		f3dPoint mid_push_flat_pos;
		CLDSNode *pMidPushFlatNode=pLineAngle->BelongModel()->FromNodeHandle(pLineAngle->mid_push_flat_node_handle);
		if(pMidPushFlatNode)
		{
			SnapPerp(&mid_push_flat_pos,pLineAngle->Start(),pLineAngle->End(),pMidPushFlatNode->Position(false));
			coord_trans(mid_push_flat_pos,angleDrawUcs,FALSE);
			nMidFlatPosZ=(int)mid_push_flat_pos.z;
			if(pLineAngle->mid_push_flat_x1_y2==1)
				nMidFlatPosZ*=-1;
		}
	}*/
	StatBoltsInfo(bolt_list,nMidFlatPosZ);//ͳ�ƽǸ��ϵ���˨
	CreateLsSpaceDimList();
	CreateJgRibPlateDimList(rib_plate_list);
	CreateLsFuncTextDim();		//������˨�׵��ļ����
	CalDrawingScaleCoef(bEndCutAngleOrWing,bStartCutAngleOrWing);		//���������Ǹ����λ������ŷ���
	CreateJgRibPlateEdge(rib_plate_list);	//���ɽǸ��߰�
	CreateJgProfile(pLineAngle->cut_wing_para,pLineAngle->cut_wing,pLineAngle->cut_angle,wingWide,wingThick,TRUE,TRUE);	//���ɽǸ����β�ͼ
	CreateJgLsCircleList();							//��������˨��صı�ע
	ProcessKaiHeAngle(pLineAngle,angleDrawUcs,TRUE);		//�����Ͻ�
	RelayoutDrawing();
	return TRUE;
}
#endif

BOOL CLineAngleDrawing::CreateLineAngleDrawing(CProcessAngle *pAngle,double max_drawing_length,double max_drawing_height,
	double text_height/*=2.5*/,double w_to_h/*=0.7*/,double min_space/*=3.5*/)
{
	if(pAngle==NULL)
		return FALSE;

	sPartNo.Copy(pAngle->GetPartNo());
	int nBoltNum=pAngle->GetLsCount();
	InitDimStyleBySysSetting(nBoltNum);
	drawPara.fDimTextSize=text_height;
	WtoH=w_to_h;
	minDrawingSpace=min_space;			//�׾���С����ֵ
	maxDrawingLength=max_drawing_length;//��ͼ�������ÿ��
	maxDrawingHeight=max_drawing_height;//��ͼ�������ø߶�
	//���ɻ�ͼ����ϵ
	UCS_STRU angleDrawUcs;
	wingWide=pAngle->m_fWidth;				//�Ǹ�֫��
	wingThick=pAngle->m_fThick;				//�Ǹ�֫��
	length=pAngle->m_wLength;				//�Ǹֳ���
	m_nWingAxisGX=m_nWingAxisGY=0;			//���������˨����
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY)&&!(pAngle->IsStartCutAngleOrWing()||pAngle->IsEndCutAngleOrWing()))
	{
		rightMargin=1.5;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimAxisGTextAtMiddle))
			leftGDimMargin=1.5;
	}
	else
	{
		leftGDimMargin=5;
		rightMargin=5;
	}
	LsCirX.SetSize(0,pAngle->GetLsCount()*2/3);
	LsCirY.SetSize(0,pAngle->GetLsCount()*2/3);

	f3dPoint norm_x_wing,norm_y_wing;
	bool bEndCutAngleOrWing=false,bStartCutAngleOrWing=false;
	double cut_wing_para[2][3],cut_angle[4][2];
	ATOM_LIST<BOLT_INFO> bolt_list;
	//�н���֫
	for (int i=0;i<2;i++)
	{
		for (int j=0;j<3;j++)
			cut_wing_para[i][j]=pAngle->cut_wing_para[i][j];
	}
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<2;j++)
			cut_angle[i][j]=pAngle->cut_angle[i][j];
	}
	//memcpy(cut_wing_para,pAngle->cut_wing_para,6*sizeof(float));
	//memcpy(cut_angle,pAngle->cut_angle,8*sizeof(float));
	//��˨��Ϣ
	BOLT_INFO *pBoltInfo;
	for (pBoltInfo =pAngle->m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=pAngle->m_xBoltInfoList.GetNext())
		bolt_list.append(*pBoltInfo);
	bStartCutAngleOrWing=pAngle->IsStartCutAngleOrWing();
	bEndCutAngleOrWing = pAngle->IsEndCutAngleOrWing();
	StatBoltsInfo(bolt_list);//ͳ�ƽǸ��ϵ���˨
	CreateLsSpaceDimList();
	CreateJgRibPlateDimList(pAngle->rib_plate_list);
	CalDrawingScaleCoef(bEndCutAngleOrWing,bStartCutAngleOrWing);				//���������Ǹ����λ������ŷ���
	CreateJgRibPlateEdge(pAngle->rib_plate_list);	//���ɽǸ��߰�
	CreateJgProfile(cut_wing_para,pAngle->cut_wing,cut_angle,wingWide,wingThick,TRUE,TRUE);	//���ɽǸ����β�ͼ
	CreateJgLsCircleList();							//��������˨��صı�ע
	ProcessKaiHeAngle(pAngle);			//�����Ͻ�
	RelayoutDrawing();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CSlotDrawing
CSlotDrawing::CSlotDrawing()
{
	edgeLineArr.SetSize(8);
	edgeLineArr[1].feature=edgeLineArr[3].feature
		=edgeLineArr[4].feature=edgeLineArr[6].feature=1;	//���߻���
}
CSlotDrawing::~CSlotDrawing()
{

}
void CSlotDrawing::CreateProfile()
{
	if(dimStyleFlag.IsHasFlag(BriefDrawing)&&wingThick<10)
		wingThick=10;
	//��ʼ���Ǹָ�����
	double lengthEndDrawPosX=TransCoordX(length);
	edgeLineArr[0].startPt.Set(0,0.5*wingWide+wingWide2,0);
	edgeLineArr[1].startPt.Set(0,0.5*wingWide+wingThick,0);
	edgeLineArr[2].startPt.Set(0,0.5*wingWide,0);
	edgeLineArr[3].startPt.Set(0,0.5*wingWide-wingThick,0);
	edgeLineArr[4].startPt.Set(0,-0.5*wingWide+wingThick,0);
	edgeLineArr[5].startPt.Set(0,-0.5*wingWide,0);
	edgeLineArr[6].startPt.Set(0,-0.5*wingWide-wingThick,0);
	edgeLineArr[7].startPt.Set(0,-0.5*wingWide-wingWide2,0);
	for(int i=0;i<8;i++)
		edgeLineArr[i].endPt.Set(lengthEndDrawPosX,edgeLineArr[i].startPt.y,0);
	//
	lineArr.Add(f3dLine(edgeLineArr[0].startPt,edgeLineArr[7].startPt));
	lineArr.Add(f3dLine(edgeLineArr[0].endPt,edgeLineArr[7].endPt));
}
#ifdef  __DRAWING_CONTEXT_
void UpdateBoltList(CLDSPart *pPart,UCS_STRU dcs,ATOM_LIST<BOLT_INFO> &bolt_list)
{
	if( pPart==NULL||
	   (pPart->GetClassTypeId()!=CLS_LINESLOT&&pPart->GetClassTypeId()!=CLS_ARCSLOT&&
		pPart->GetClassTypeId()!=CLS_LINEFLAT&&pPart->GetClassTypeId()!=CLS_ARCFLAT))
		return;
	for(CLsRef *pLsRef=pPart->GetLsRefList()->GetFirst();pLsRef;pLsRef=pPart->GetLsRefList()->GetNext())
	{
		CLDSBolt *pBolt=pLsRef->GetLsPtr();
		f3dPoint basePt=pBolt->ucs.origin;
		BOLT_INFO bolt_info;
		if(pPart->IsArcPart())
		{
			CalLsPosInArcPart((CLDSArcPart*)pPart,pLsRef,basePt);
			bolt_info.posX=(float)basePt.x;
			bolt_info.posY=(float)basePt.y;
		}
		else
		{
			Int3dlf(basePt,basePt,pBolt->get_norm(),dcs.origin,pPart->ucs.axis_y);
			coord_trans(basePt,dcs,FALSE);
			if(pPart->GetClassTypeId()==CLS_LINEFLAT)
			{
				bolt_info.posX=(float)basePt.x;
				bolt_info.posY=(float)basePt.y;
			}
			else 
			{
				bolt_info.posX=(float)basePt.z;
				bolt_info.posY=(float)basePt.x;
			}
		}
		bolt_info.bolt_d=(short)(*pLsRef)->get_d();
		bolt_info.waistVec=pBolt->ucs.axis_x;
		bolt_info.waistLen=pLsRef->waistLen;
		bolt_info.hole_d_increment=(float)pBolt->hole_d_increment;	
		bolt_info.m_dwFlag=pBolt->m_dwFlag;			//��˨������λ
		bolt_info.cFuncType=pBolt->FilterFuncType();		//��˨��������
		vector_trans(bolt_info.waistVec,dcs,FALSE);
		bolt_list.append(bolt_info);
	}
}
BOOL CSlotDrawing::CreateSlotDrawing(CLDSPart *pSlot,double max_drawing_length,double max_drawing_height,
	double text_height/*=2.5*/,double w_to_h/*=0.7*/,double min_space/*=3.5*/)
{
	if(pSlot==NULL||(pSlot->GetClassTypeId()!=CLS_ARCSLOT&&pSlot->GetClassTypeId()!=CLS_LINESLOT))
		return FALSE;
	sPartNo.Copy(pSlot->GetPartNo());
	int nBoltNum=pSlot->GetLsCount();
	InitDimStyleBySysSetting(nBoltNum);
	drawPara.fDimTextSize=text_height;
	WtoH=w_to_h;
	minDrawingSpace=min_space;			//�׾���С����ֵ
	maxDrawingLength=max_drawing_length;//��ͼ�������ÿ��
	maxDrawingHeight=max_drawing_height;//��ͼ�������ø߶�
	//���ɻ�ͼ����ϵ
	UCS_STRU drawUcs;
	if(pSlot->GetClassTypeId()==CLS_LINESLOT)
	{
		drawUcs=((CLDSLineSlot*)pSlot)->BuildUCS();	
		drawUcs.origin-=drawUcs.axis_z*((CLDSLineSlot*)pSlot)->startOdd();
	}
	else //if(pSlot->GetClassTypeId()==CLS_ARCSLOT)
	{
		((CLDSArcSlot*)pSlot)->getUCS(drawUcs);
		drawUcs.origin-=drawUcs.axis_z*((CLDSArcSlot*)pSlot)->start_oddment;
	}
	wingWide=pSlot->GetHeight();	//֫��			
	wingWide2=pSlot->GetWidth();	//֫��2
	wingThick=pSlot->GetThick();	//֫��
	length=pSlot->GetLength();		//����	
	m_nWingAxisGX=m_nWingAxisGY=0;		//���������˨����
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
	{
		rightMargin=1.5;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimAxisGTextAtMiddle))
			leftGDimMargin=1.5;
	}
	else
	{
		leftGDimMargin=5;
		rightMargin=5;
	}
	LsCirX.SetSize(0,pSlot->GetLsCount()*2/3);
	LsCirY.SetSize(0,pSlot->GetLsCount()*2/3);
	//��˨��Ϣ
	ATOM_LIST<BOLT_INFO> bolt_list;
	UpdateBoltList(pSlot,drawUcs,bolt_list);
	StatBoltsInfo(bolt_list);				//ͳ�ƽǸ��ϵ���˨
	CreateLsSpaceDimList();
	CalDrawingScaleCoef(FALSE,FALSE);		//�����������λ������ŷ���
	CreateProfile();						//���ɽǸ����β�ͼ
	CreateJgLsCircleList();					//��������˨��صı�ע
	RelayoutDrawing();
	return TRUE;
}
#endif
//////////////////////////////////////////////////////////////////////////
// CFlatDrawing
CFlatDrawing::CFlatDrawing()
{
	edgeLineArr.SetSize(2);
}
CFlatDrawing::~CFlatDrawing()
{

}
void CFlatDrawing::CreateProfile()
{
	if(dimStyleFlag.IsHasFlag(BriefDrawing)&&wingThick<10)
		wingThick=10;
	//��ʼ���Ǹָ�����
	double lengthEndDrawPosX=TransCoordX(length);
	edgeLineArr[0].startPt.Set(0,0.5*wingWide,0);
	edgeLineArr[0].endPt.Set(lengthEndDrawPosX,0.5*wingWide,0);
	edgeLineArr[1].startPt.Set(0,-0.5*wingWide,0);
	edgeLineArr[1].endPt.Set(lengthEndDrawPosX,-0.5*wingWide,0);
	//
	lineArr.Add(f3dLine(edgeLineArr[0].startPt,edgeLineArr[1].startPt));
	lineArr.Add(f3dLine(edgeLineArr[0].endPt,edgeLineArr[1].endPt));
}
#ifdef  __DRAWING_CONTEXT_
BOOL CFlatDrawing::CreateFlatDrawing(CLDSPart *pFlat,double max_drawing_length,double max_drawing_height,
	double text_height/*=2.5*/,double w_to_h/*=0.7*/,double min_space/*=3.5*/)
{
	if(pFlat==NULL||(pFlat->GetClassTypeId()!=CLS_ARCFLAT&&pFlat->GetClassTypeId()!=CLS_LINEFLAT))
		return FALSE;
	sPartNo.Copy(pFlat->GetPartNo());
	int nBoltNum=pFlat->GetLsCount();
	InitDimStyleBySysSetting(nBoltNum);
	drawPara.fDimTextSize=text_height;
	WtoH=w_to_h;
	minDrawingSpace=min_space;			//�׾���С����ֵ
	maxDrawingLength=max_drawing_length;//��ͼ�������ÿ��
	maxDrawingHeight=max_drawing_height;//��ͼ�������ø߶�
	//���ɻ�ͼ����ϵ
	UCS_STRU drawUcs;
	if(pFlat->GetClassTypeId()==CLS_LINEFLAT)
	{
		drawUcs=((CLDSLineFlat*)pFlat)->BuildUCS();	
		drawUcs.origin-=drawUcs.axis_x*((CLDSLineFlat*)pFlat)->startOdd();
	}
	else //if(pSlot->GetClassTypeId()==CLS_ARCSLOT)
	{
		((CLDSArcFlat*)pFlat)->getUCS(drawUcs);
		drawUcs.origin-=drawUcs.axis_z*((CLDSArcFlat*)pFlat)->start_oddment;
	}
	wingWide=pFlat->GetWidth();		//֫��
	wingThick=pFlat->GetThick();	//֫��
	length=pFlat->GetLength();		//����	
	m_nWingAxisGX=m_nWingAxisGY=0;		//���������˨����
	if(dimStyleFlag.IsHasFlag(CRodDrawing::DimSpaceTextAlongY))
	{
		rightMargin=1.5;
		if(dimStyleFlag.IsHasFlag(CRodDrawing::DimAxisGTextAtMiddle))
			leftGDimMargin=1.5;
	}
	else
	{
		leftGDimMargin=5;
		rightMargin=5;
	}
	LsCirX.SetSize(0,pFlat->GetLsCount()*2/3);
	LsCirY.SetSize(0,pFlat->GetLsCount()*2/3);
	//��˨��Ϣ
	ATOM_LIST<BOLT_INFO> bolt_list;
	UpdateBoltList(pFlat,drawUcs,bolt_list);
	StatBoltsInfo(bolt_list);				//ͳ�ƽǸ��ϵ���˨
	CreateLsSpaceDimList();
	CalDrawingScaleCoef(FALSE,FALSE);		//�����������λ������ŷ���
	CreateProfile();						//���ɽǸ����β�ͼ
	CreateJgLsCircleList();					//��������˨��صı�ע
	RelayoutDrawing();
	return TRUE;
}
#endif
#endif