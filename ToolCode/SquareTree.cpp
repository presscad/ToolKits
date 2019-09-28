#include "stdafx.h"
#include "SquareTree.h"
#include "f_alg_fun.h"
#include "ArrayList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

POLYGONEX::POLYGONEX()
{

}
POLYGONEX::~POLYGONEX()
{

}
void POLYGONEX::CalLocalCS()
{
	if(!lcs.axis_z.IsZero())
		return;
	lcs.axis_z=zAxis;
	lcs.origin=vertexes[0];
	for(int i=1;lcs.axis_x.IsZero()&&i<count;i++)
		lcs.axis_x=vertexes[i]-vertexes[i-1];
	normalize(lcs.axis_x);
	lcs.axis_y=lcs.axis_z^lcs.axis_x;
}
double POLYGONEX::CalPerimeter()
{
	double fPerimeter=0;
	for(int i=0;i<count;i++)
		fPerimeter+=DISTANCE(vertexes[i],vertexes[(i+1)%count]);
	return fPerimeter;
}
bool IsLineIntersInternal(const double* line1Start,const double* line1End,const double* line2Start,const double* line2End)
{
	GEPOINT v1(line1End[0]-line1Start[0],line1End[1]-line1Start[1]);
	GEPOINT v2(line2End[0]-line2Start[0],line2End[1]-line2Start[1]);
	GEPOINT v(line2Start[0]-line1Start[0],line2Start[1]-line1Start[1]);
	bool startOnRight,endOnRight;
	if(v1.x*v.y-v1.y*v.x<=0) 
		startOnRight=true;	//line2.Start����1�ߵ��Ҳ�
	else
		startOnRight=false;	//����ĳ���ߵ����
	v.Set(line2End[0]-line1Start[0],line2End[1]-line1Start[1]);
	if(v1.x*v.y-v1.y*v.x<=0) 
		endOnRight=true;	//line2.End����1�ߵ��Ҳ�
	else
		endOnRight=false;	//����ĳ���ߵ����
	if(startOnRight==endOnRight)
		return false;	//2��ֱ�߶�λ��1��ֱ�߶ε�ͬһ��,����Ч����
	v.Set(line1Start[0]-line2Start[0],line1Start[1]-line2Start[1]);
	if(v2.x*v.y-v2.y*v.x<=0) 
		startOnRight=true;	//line1.Start����2�ߵ��Ҳ�
	else
		startOnRight=false;	//����ĳ���ߵ����
	v.Set(line1End[0]-line2Start[0],line1End[1]-line2Start[1]);
	if(v2.x*v.y-v2.y*v.x<=0) 
		endOnRight=true;	//line1.End����2�ߵ��Ҳ�
	else
		endOnRight=false;	//����ĳ���ߵ����
	if(startOnRight==endOnRight)
		return false;	//1��ֱ�߶�λ��2��ֱ�߶ε�ͬһ��,����Ч����
	return true;
}
//-1:�����ڶ����������;1.�����������������ص�;2.�����������������ص�(����������ڷ�����);3.�����ڶ����������
int POLYGONEX::IncludeState(GEPOINT *xVertexes, int count,const double* x_y,double sqare_width)
{	
	int i;
	DYN_ARRAY<f3dPoint> vertexArr(count);
	for(i=0;i<count;i++)
		vertexArr[i].Set(xVertexes[i].x,xVertexes[i].y);
	f3dPoint sqareArr[4]={GEPOINT(x_y[0],x_y[1]),
						 GEPOINT(x_y[0]+sqare_width,x_y[1]),
						 GEPOINT(x_y[0]+sqare_width,x_y[1]+sqare_width),
						 GEPOINT(x_y[0],x_y[1]+sqare_width)};
	POLYGONEX polygon1,polygon2;
	polygon1.CreatePolygonRgn(vertexArr,count);	//������Ķ��������
	polygon2.CreatePolygonRgn(sqareArr,4);		//����Ķ��������
	int ret1=polygon1.PtInRgn2(sqareArr[0]);
	int ret2=polygon1.PtInRgn2(sqareArr[1]);
	int ret3=polygon1.PtInRgn2(sqareArr[2]);
	int ret4=polygon1.PtInRgn2(sqareArr[3]);
	if(ret1==0||ret2==0||ret3==0||ret4==0)
		return -1;	//�����ڶ�����������
	else if(ret1==3&&ret2==3&&ret3==3&&ret4==3)
	{	//�����ڶ����������(�������������ص�|�������������ص�)
		//1.�ų������λ�ڷ�����һ�ߵ����
		bool outside=true,inside=true;
		for(i=0;i<count&&outside;i++)	//�жϵ��Ƿ�ȫ�ڷ�����ߵ����
			outside=(xVertexes[i].x<=x_y[0]-EPS);
		if(outside)
			return 1;	//SQUARENODE::COVER_NONE
		for(i=0,outside=true;i<count&&outside;i++)	//�жϵ��Ƿ�ȫ�ڷ����ұߵ��Ҳ�
			outside=(xVertexes[i].x>=x_y[0]+sqare_width+EPS);
		if(outside)
			return 1;	//SQUARENODE::COVER_NONE
		for(i=0,outside=true;i<count&&outside;i++)	//�жϵ��Ƿ�ȫ�ڷ���ױߵ��²�
			outside=(xVertexes[i].y<=x_y[1]-EPS);
		if(outside)
			return 1;	//SQUARENODE::COVER_NONE
		for(i=0,outside=true;i<count&&outside;i++)	//�жϵ��Ƿ�ȫ�ڷ��񶥱ߵ��ϲ�
			outside=(xVertexes[i].y>=x_y[1]+sqare_width+EPS);
		if(outside)
			return 1;	//SQUARENODE::COVER_NONE
		//2.�ų������λ�ڷ��������
		for(i=0,inside=true;i<count&&inside;i++)	//�жϵ��Ƿ�ȫ�ڷ��񶥱ߵ��ϲ�
		{
			double deitax=xVertexes[i].x-x_y[0];
			double deitay=xVertexes[i].y-x_y[1];
			if(deitax<-EPS||deitax>sqare_width+EPS || deitay<-EPS||deitay>sqare_width+EPS)
				inside=false;
		}
		if(inside)	//����ȫ�����˶����
			return 2;	//SQUARENODE::COVER_WITH
		//3.ͨ���������һ���뷽����ıߴ�����Ч�ڽ���,�ж�Ϊ�и��� wjh-2017.11.23
		bool findInters=false;
		for(i=0;i<count&&!findInters;i++)	//�жϵ��Ƿ�ȫ�ڷ��񶥱ߵ��ϲ�
		{
			for(int j=0;j<4&&!findInters;j++)
				findInters=IsLineIntersInternal(xVertexes[i],xVertexes[(i+1)%count],sqareArr[j],sqareArr[(j+1)%4]);
		}
		if(!findInters)	//δ�ҵ��κα��ཻ���
			return 1;	//SQUARENODE::COVER_NONE
		else //if(findInters)
			return 2;	//SQUARENODE::COVER_WITH
	}
	else if((ret1==1||ret1==2)&&(ret2==1||ret2==2)&&(ret3==1||ret3==2)&&(ret4==1||ret4==2))
		return 3;		//SQUARENODE::COVER_IN
	else
		return 2;		//SQUARENODE::COVER_WITH
}
#ifdef __ALFA_TEST_VERSION_
#include "DxfFile.h"
void AddSquareNodeGezi(SQUARENODE* pSquareNode,CDxfFile* pDxfile)
{
	for(int i=0;i<4;i++)
	{
		SQUARENODE* pSonNode=(SQUARENODE*)pSquareNode->GetSquareNode(i);
		bool* pbLeafNode=(bool*)pSonNode+4;
		if(*pbLeafNode)
		{
			GEPOINT start(pSonNode->square.X,pSonNode->square.Y);
			double width=pSonNode->square.fHalfWidth*2;
			if(pSonNode->square.state==0||pSonNode->square.state==SQUARENODE::COVER_IN||pSonNode->square.state==SQUARENODE::COVER_WITH)
			{
				pDxfile->NewLine(start,GEPOINT(start.x+width,start.y));
				pDxfile->NewLine(GEPOINT(start.x+width,start.y),GEPOINT(start.x+width,start.y+width));
				pDxfile->NewLine(GEPOINT(start.x+width,start.y+width),GEPOINT(start.x,start.y+width));
				pDxfile->NewLine(GEPOINT(start.x,start.y+width),GEPOINT(start.x,start.y));
			}
			if(pSonNode->square.state==SQUARENODE::COVER_IN)
				pDxfile->NewCircle(GEPOINT(start.x+width*0.5,start.y+width*0.5),width*0.5);
		}
		else
			AddSquareNodeGezi(pSonNode,pDxfile);
	}
}
#endif
double POLYGONEX::OverlappedWith(GEPOINT *vertexes2, int count2,SQUARENODE* pRootNode/*=NULL*/,double DECISION/*=2*/)
{
	int i;
	SCOPE_STRU scope;
	for(i=0;i<count;i++)
		scope.VerifyVertex(vertexes[i]);
	DYN_ARRAY<GEPOINT> planarVertexs(0),planarVertexs2(count2);
	GEPOINT* currVertexesArr=vertexes;
	CalLocalCS();	//���㵱ǰ����εľֲ�����ϵ�����Դ�����ΪϵΪ���ռ�����������ε��ص�
	bool needTransCS=false;
	if(scope.depth()>EPS)
	{
		//�����ֲ�����ϵ��������ϵͳһ���ֲ�ƽ������ϵ��
		scope.ClearScope();
		planarVertexs.Resize(count);
		for(i=0;i<count;i++)
		{
			planarVertexs[i]=lcs.TransPToCS(vertexes[i]);
			scope.VerifyVertex(planarVertexs[i]);
		}
		currVertexesArr=planarVertexs;
		needTransCS=true;
	}
	//�������vertexes2Ҳת������ǰ�������ƽ����
	for(i=0;i<count2;i++)
	{
		if(needTransCS)
			planarVertexs2[i]=lcs.TransPToCS(vertexes2[i]);
		else
			planarVertexs2[i]=vertexes2[i];
		scope.VerifyVertex(planarVertexs2[i]);
	}
	//2.��scope���X��Yƽ���ڽ����ķָ�ɨ���ص���
	//2.1 �������ڵ㣨��ȫ���������Ϊ�ĸ�����
	GEPOINT center;
	SQUARENODE root;
	double width=max(scope.wide(),scope.high());
	scope.centre(&center.x,&center.y,&center.z);
	SQUARERGN square(center.x-width/2,center.y-width/2,width);
	SQUARENODE* pRoot=(pRootNode)?pRootNode:(&root);
	pRoot->AddSonNode(square);
	DetectSquareCoverState(currVertexesArr,planarVertexs2,count2,pRoot,DECISION);
#ifdef __ALFA_TEST_VERSION_	//���Դ���
	double intersarea=pRoot->SumStateWeight(SQUARENODE::COVER_WITH);
	double coverarea=pRoot->SumStateWeight(SQUARENODE::COVER_IN);
	bool create=false;
	if(create)
	{
		CDxfFile dxfile;
		dxfile.OpenFile("d:\\test.dxf");
		for(i=0;i<count;i++)
			dxfile.NewLine(currVertexesArr[i],currVertexesArr[(i+1)%count]);
		AddSquareNodeGezi(pRoot,&dxfile);
		dxfile.CloseFile();
	}
#endif
	return pRoot->SumStateWeight(SQUARENODE::COVER_IN);
}
int POLYGONEX::DetectSquareCoverState(GEPOINT *vertexes1,GEPOINT *vertexes2, int count2,SQUARENODE* pSquareNode,double DECISION/*=1*/)
{
	int rslt1,rslt2;
	double XY[2]={pSquareNode->square.X,pSquareNode->square.Y};
	rslt1=IncludeState(vertexes1,count ,XY,pSquareNode->square.fHalfWidth*2);
	rslt2=IncludeState(vertexes2,count2,XY,pSquareNode->square.fHalfWidth*2);
	if(rslt1==rslt2)
	{
		if(rslt1==SQUARENODE::COVER_IN)
			return pSquareNode->square.state=SQUARENODE::COVER_IN;
		else if(rslt1==SQUARENODE::COVER_NONE)
			return pSquareNode->square.state=SQUARENODE::COVER_NONE;
		else if(pSquareNode->square.fHalfWidth<DECISION)
			return pSquareNode->square.state=SQUARENODE::COVER_WITH;
		else //if(rslt1==SQUARENODE::COVER_WITH)
		{
			pSquareNode->square.state=SQUARENODE::COVER_WITH;
			pSquareNode->AddSonNode(pSquareNode->square);
		}
	}
	else if(rslt1==SQUARENODE::COVER_NONE||rslt2==SQUARENODE::COVER_NONE)
		return pSquareNode->square.state=SQUARENODE::COVER_NONE;
	else if(pSquareNode->square.fHalfWidth<DECISION)
		return pSquareNode->square.state=SQUARENODE::COVER_WITH;	//����ϸ��
	else
	{
		pSquareNode->square.state=SQUARENODE::COVER_WITH;
		pSquareNode->AddSonNode(pSquareNode->square);
	}
	//�����Է������ϸ��
	int state=SQUARENODE::COVER_NONE;
	for(int i=0;i<4;i++)
	{
		if(i==0 || i==2)
			XY[0]=pSquareNode->square.X;
		else
			XY[0]=pSquareNode->square.X+pSquareNode->square.fHalfWidth;
		if(i==0 || i==1)
			XY[1]=pSquareNode->square.Y;
		else
			XY[1]=pSquareNode->square.Y+pSquareNode->square.fHalfWidth;
		SQUARERGN square(XY[0],XY[1],pSquareNode->square.fHalfWidth);
		SQUARENODE* pSonNode=(SQUARENODE*)pSquareNode->GetSquareNode(i);
		pSonNode->square=square;
		int rslt=DetectSquareCoverState(vertexes1,vertexes2,count2,pSonNode,DECISION);
		if(i==0)
			state=rslt;
		else if(state!=rslt)//����ȫ��ͬ������Ϊ���ָ��ǣ���Ҫϸ��
			state=SQUARENODE::COVER_WITH;
	}
	return state;
}
