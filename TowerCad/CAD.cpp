#include "stdafx.h"
#include "f_alg_fun.h"
#include "CAD.h"


double CSizeTextDim::PRECISION=1.0;
CDimStyle CSizeTextDim::dimStyle;
double CSizeTextDim::GetWidth()
{
	f3dPoint horiVec(cos(angle),sin(angle));
	f3dPoint start_perp,end_perp;
	SnapPerp(&start_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimStartPos);
	SnapPerp(&end_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimEndPos);
	return DISTANCE(start_perp,end_perp);
}
double CSizeTextDim::GetHeight(BOOL bIncShortDimLineOnly/*=FALSE*/)
{
	f3dPoint horiVec(cos(angle),sin(angle));
	double start_dist=0,end_dist=0,max_dist=dimStyle.style.arrowSize*0.5;
	f3dPoint start_perp,end_perp;
	SnapPerp(&start_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimStartPos,&start_dist);
	SnapPerp(&end_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimEndPos,&end_dist);
	if(start_dist>max_dist)	
		max_dist=start_dist;
	if(end_dist>start_dist)
		max_dist=end_dist;
	return (max_dist+dimStyle.style.gap+dimStyle.textHeight);
}
const char* CSizeTextDim::DimText()
{
	if(PRECISION==0.1)
		dimText.Printf("%.1f",dist);
	else if(PRECISION==0.5) 
	{
		double dimdist=fto_halfi(dist);
		if(dimdist==(int)dimdist)
			dimText.Printf("%.0f",dimdist);
		else
			dimText.Printf("%.1f",dimdist);
	}
	else
		dimText.Printf("%.0f",dist);
	if(nHits>1)
	{	//���������ע�ϲ�
		CXhChar16 temp;
		temp.Printf("X%d",nHits);
		dimText.Append(temp);
	}
	return dimText;
}

CSizeTextDim::CSizeTextDim()
{
	nHits=1;
	iOrder=0;
	angle=dist=0;
	bShortenDimStartLine=FALSE;
	bShortenDimEndLine=FALSE;
	bAllowRelayout=TRUE;
}

double CSizeTextDim::Fillet()	//����ע����dist����precision����Բ����
{
	if(PRECISION==0.1)
		dist=ftoi(dist*10)/10;
	else if(PRECISION==0.5) 
		dist=fto_halfi(dist);
	else
		dist=ftoi(dist);
	return dist;
}

const double halfPi=Pi/2;
f3dPoint GetRapidVec(double angle)
{
	if(angle==0)
		return f3dPoint(1,0,0);
	else if(angle==halfPi)
	{
		return f3dPoint(0,1,0);
	}
	else
		return f3dPoint(cos(angle),sin(angle));

}
//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
void CSizeTextDim::GetDimRect(f3dPoint rgnVertArr[4],BOOL bIncShortDimLineOnly/*=FALSE*/,double draw_scale/*=1.0*/) const
{
	f3dPoint horiVec=GetRapidVec(angle);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	f3dPoint startPerp,endPerp,verifyVec;
	SnapPerp(&startPerp,dimStartPos,dimStartPos+vertVec,dimPos);
	SnapPerp(&endPerp,dimEndPos,dimEndPos+vertVec,dimPos);
	verifyVec=dimPos-dimStartPos;
	double text_height=dimStyle.textHeight*draw_scale; //����������������߿� wht 10-09-16
	if(verifyVec*vertVec>0)
	{	//��ע�߻����ڱ�ע���ϲ�
		double startDist=(startPerp-dimStartPos)*vertVec;
		double endDist=(endPerp-dimEndPos)*vertVec;
		double maxDist=max(startDist,endDist);
		rgnVertArr[0]=startPerp-vertVec*maxDist+origin;
		rgnVertArr[1]=endPerp-vertVec*maxDist+origin;
		rgnVertArr[2]=endPerp+vertVec*text_height*1.5+origin;
		rgnVertArr[3]=startPerp+vertVec*text_height*1.5+origin;
	}
	else
	{	//��ע�߻����ڱ�ע���²�
		double startDist=(dimStartPos-startPerp)*vertVec;
		double endDist=(dimEndPos-endPerp)*vertVec;
		double maxDist=max(startDist,endDist);
		double upDist=max(maxDist,text_height*1.5);
		rgnVertArr[0]=startPerp-vertVec*text_height*0.5+origin;
		rgnVertArr[1]=endPerp-vertVec*text_height*0.5+origin;
		rgnVertArr[2]=endPerp+vertVec*upDist+origin;
		rgnVertArr[3]=startPerp+vertVec*upDist+origin;
	}
}

static f3dPoint GetDatumPos(DIMALIGN_TYPE alignType,f3dPoint rgnVertArr[4])
{
	f3dPoint origin;
	switch(alignType)
	{
	case TopLeft:	//����
		origin=rgnVertArr[3];
		break;	
	case TopCenter:	//����
		origin=(rgnVertArr[2]+rgnVertArr[3])*0.5;
		break;
	case TopRight:	//����
		origin=rgnVertArr[2];
		break;
	case MiddleLeft://����
		origin=(rgnVertArr[0]+rgnVertArr[3])*0.5;
		break;
	case MiddleCenter://����
		origin=(rgnVertArr[0]+rgnVertArr[1]+rgnVertArr[2]+rgnVertArr[3])*0.25;
		break;
	case MiddleRight://����
		origin=(rgnVertArr[1]+rgnVertArr[2])*0.5;
		break;
	case BottomLeft://����
		origin=rgnVertArr[0];
		break;
	case BottomCenter://����
		origin=(rgnVertArr[0]+rgnVertArr[1])*0.5;
		break;
	case BottomRight:	//����
		origin=rgnVertArr[1];
		break;
	default:
		break;
	};
	return origin;
}


void CSizeTextDim::ResetPosition(DIMALIGN_TYPE align/*=AlignDefault*/)
{
	double half_dim_width=0.5*GetWidth();	//��ע�����ȵ�һ��
	double dim_height=GetHeight();			//��ע����߶�
	double up_height=dimStyle.style.gap+dimStyle.textHeight;//�ϲ�߶�(����߶����ע�������ע��֮��ļ�϶ֵ֮��)
	double down_height=dim_height-up_height;//�²�߶�

	f3dPoint horiVec=GetRapidVec(angle);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	f3dPoint verifyVec=dimPos-dimStartPos;
	double verify=verifyVec*vertVec;
	if(verify>0)	//��ע���ڱ�ע��ʼ���϶�,
		dimPos=dimStartPos+vertVec*0.5*dimStyle.textHeight+horiVec;
	else		//��ע���ڱ�ע��ʼ���¶�,
		dimPos=dimStartPos-vertVec*(1.5*dimStyle.textHeight+dimStyle.style.gap)+horiVec;
	if(align==AlignDefault)
	{
		if(verify>0)
			alignType=BottomCenter;
		else
			alignType=TopCenter;
	}
	else
		alignType=align;
	f3dPoint rgnVertArr[4];
	GetDimRect(rgnVertArr);
	origin=GetDatumPos(alignType,rgnVertArr);
	dimPos-=origin;
	dimStartPos-=origin;
	dimEndPos-=origin;
}


//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
void CKaiHeAngleDim::GetDimRect(f3dPoint rgnVertArr[4])
{
	f3dPoint horiVec(1,0,0);
	f3dPoint vertVec(0,1,0);
	SCOPE_STRU scope;
	scope.VerifyVertex(dimStartPos+origin);
	scope.VerifyVertex(dimEndPos+origin);
	if(bInWingX)
		scope.fMinY=dimPos.y+origin.y-1.5*dimStyle.textHeight;
	else
		scope.fMaxY=dimPos.y+origin.y+1.5*dimStyle.textHeight;
	rgnVertArr[0].Set(scope.fMinX,scope.fMinY);	//����
	rgnVertArr[1].Set(scope.fMaxX,scope.fMinY);	//����
	rgnVertArr[2].Set(scope.fMaxX,scope.fMaxY);	//����
	rgnVertArr[3].Set(scope.fMinX,scope.fMaxY);	//����
}

void CKaiHeAngleDim::ResetPosition()
{
	f3dPoint horiVec=GetRapidVec(0);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	f3dPoint verifyVec=dimPos-dimStartPos;
	if(bInWingX)
	{
		alignType=TopCenter;
		dimPos=dimStartPos-vertVec*(3.5*dimStyle.textHeight+dimStyle.style.gap)+horiVec;
	}
	else
	{
		alignType=BottomCenter;
		dimPos=dimStartPos+vertVec*3.5*dimStyle.textHeight+horiVec;
	}
	f3dPoint rgnVertArr[4];
	GetDimRect(rgnVertArr);
	origin=GetDatumPos(alignType,rgnVertArr);
	dimPos-=origin;
	dimStartPos-=origin;
	dimEndPos-=origin;
}

double CTextOnlyDrawing::GetHeight()
{
	return textHeight;
}
double CTextOnlyDrawing::GetWidth()
{
	int text_len=strlen(GetPrecisionDimText());
	return 0.7*textHeight*text_len;
}

//precisionֻ��Ϊ"0","0.5","0.0"
double CTextOnlyDrawing::PRECISION=1.0;
const char* CTextOnlyDrawing::GetPrecisionDimText()
{
	if(bNeedConvertDistToText)
	{	//���Գߴ�
		if(PRECISION==0.1)
			dimText.Printf("%.1f",dist);
		else if(PRECISION==0.5) 
		{
			double dimdist=fto_halfi(dist);
			if(dimdist==(int)dimdist)
				dimText.Printf("%.0f",dimdist);
			else
				dimText.Printf("%.1f",dimdist);
		}
		else
			dimText.Printf("%.0f",dist);
	}
	return dimText;
}

//////////////////////////////////////////////////////////////////////
// tagDIM_FRAMETEXT Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTextFrame::CTextFrame(){
	frameStyle=0;
	margin = 0.2;
	textHeight=2.5;
	WtoH=0.7;
	frameHeight=frameWidth=7;
	dataPos.Set();	//����λ��
	dimPos.Set();	//��עλ��
	hPart=0;		//�����������
	cls_id=0;		//������������
	iCurLayer=0;	//�ı�����ͼ��
	textAlignHori=ACDB::kTextCenter;
	textAlignVert=ACDB::kTextVertMid;
}



TMADRAWING_DIM_STYLE DimStyleTable::dimStyle;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle2;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle3;
TMADRAWING_DIM_STYLE DimStyleTable::dimAngleStyle;
TMADRAWING_DIM_STYLE DimStyleTable::dimRadiusStyle;








