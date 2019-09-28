// XhTableCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XhTableCtrl.h"


// CXhTableCtrl

IMPLEMENT_DYNAMIC(CXhTableCtrl, CStatic)

CXhTableCtrl::CXhTableCtrl()
{

}

CXhTableCtrl::~CXhTableCtrl()
{
}


BEGIN_MESSAGE_MAP(CXhTableCtrl, CStatic)
	//{{AFX_MSG_MAP(CUserDefStaticText)
	ON_WM_PAINT()
	//ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CXhTableCtrl ��Ϣ�������
void DrawLine(CDC *pDC,f2dPoint line_start,f2dPoint line_end)
{
	pDC->MoveTo((int)line_start.x,(int)line_start.y);
	pDC->LineTo((int)line_end.x,(int)line_end.y);
}
void DrawText(CDC *pDC,LPRECT lpRect,const char* sText,BYTE alignType,BYTE flowDirection)
{
	pDC->DrawText(sText,-1,lpRect,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING);
}
void DrawTable(CXhTableCtrl *pTblCtrl,CDC *pDC,CFormatTbl *pTable,const double text_margin=1.5)
{
	int nColumnsCount = pTable->GetColumnsCount();
	int nRowsCount	  = pTable->GetRowsCount();
	if(nColumnsCount<=0||nRowsCount<=0)
		return;
	//1.���ñ���ɫ���ָ�����ɫ
	COLORREF crLine=RGB(208,215,229);
	COLORREF crBackground=RGB(245,245,245);	//RGB(199,237,204);
	RECT rectFull;
	pTblCtrl->GetClientRect(&rectFull);
	pDC->SetBkColor(crBackground);
	pDC->SetBkMode(OPAQUE);
	pDC->FillSolidRect(&rectFull,crBackground);
	//2.���Ʊ��ָ���
	CPen psPen(PS_SOLID, 1, crLine);
	CPen* pOldPen = pDC->SelectObject(&psPen);
	//2.1 �����б����(����)
	int i=0,j=0;
	CRect client_rect;
	pTblCtrl->GetWindowRect(client_rect);
	pTblCtrl->GetParent()->ScreenToClient(client_rect);
	f2dPoint start,end,line_start,line_end;
	for (i=0;i<=nRowsCount;i++)
	{
		pTable->GetRowLineStart(i,start);
		pTable->GetRowLineEnd(i,end);
		line_start.Set(start.x,-start.y);
		line_end.Set(end.x,-end.y);
		for(j=0;j<nColumnsCount;j++)
		{	//�������е�����Ԫ
			if(i==0||i==nRowsCount)	//��ĩ�в����ж��鵥Ԫ
				break;
			double wide;
			pTable->GetColumnWide(j,&wide);	//��õ�j�б���
			if(j==0&&pTable->GetGridAt(i,j)->m_bVGridVirtual)
			{		//���о�Ϊ�鵥Ԫ
				line_start.x += wide;
				line_end.x = line_start.x;
			}
			else if(pTable->GetGridAt(i,j)->m_bVGridVirtual==TRUE)
			{	//��ǰ��Ϊ�鵥Ԫ
				if(!pTable->GetGridAt(i,j-1)->m_bVGridVirtual)
				{
					DrawLine(pDC,line_start,line_end);
					line_start.x = line_end.x+wide;
					line_end.x = line_start.x;
				}
				else
				{
					line_end.x += wide;
					line_start.x = line_end.x;
				}
			}
			else	//��ǰ�в����鵥Ԫ,�޸�����β����
			{
				if(j==0)
					line_end.x = line_start.x+wide;
				else
					line_end.x += wide;
			}
		}
		DrawLine(pDC,line_start,line_end);
	}
	//2.2 ���ɱ������(����)
	for(j=0;j<=nColumnsCount;j++)
	{
		pTable->GetColumnLineStart(j,start);
		pTable->GetColumnLineEnd(j,end);
		line_start.Set(start.x,-start.y);
		line_end.Set(end.x,-end.y);
		for(i=0;i<nRowsCount;i++)
		{	//�������е�����Ԫ
			if(j==0||j==nColumnsCount)	//��ĩ�в����ж��鵥Ԫ
				break;
			double high;
			pTable->GetRowHigh(i,&high);
			if(i==0&&pTable->GetGridAt(i,j)->m_bHGridVirtual)
			{		//���о�Ϊ�鵥Ԫ
				line_start.y += high;
				line_end.y = line_start.y;
			}
			else if(pTable->GetGridAt(i,j)->m_bHGridVirtual)
			{		//��ǰ�����鵥Ԫ
				if(!pTable->GetGridAt(i-1,j)->m_bHGridVirtual)
				{
					DrawLine(pDC,line_start,line_end);
					line_start.y = line_end.y+high;
					line_end.y = line_start.y;
				}
				else
				{
					line_end.y += high;
					line_start.y = line_end.y;
				}
			}
			else	//��ǰ�в����鵥Ԫ,�޸�����β����
			{
				if(i==0)
					line_end.y = line_start.y + high;
				else
					line_end.y += high;
			}
		}
		if(fabs(line_start.y-line_end.y)<eps)	//��ĩ���غ�
			continue;
		DrawLine(pDC,line_start,line_end);
	}
	pDC->SelectObject(pOldPen);
	psPen.DeleteObject();
	//3.���Ʊ����⼰����
	f3dPoint text_base;
	f2dRect rect,grid_rect;
	//��д�����⼰����
	for(i=0;i<nRowsCount;i++)
	{
		for(j=0;j<nColumnsCount;j++)
		{
			double width;
			CGrid *pGrid = pTable->GetGridAt(i,j);
			if(pGrid->m_bHGridVirtual||pGrid->m_bVGridVirtual)//����Ԫ
				continue;
			pTable->GetGridRect(i,j,grid_rect);
			CRect rect;
			rect.top=(int)grid_rect.topLeft.y-2;
			rect.bottom=(int)grid_rect.bottomRight.y+2;
			rect.left=(int)grid_rect.topLeft.x+2;
			rect.right=(int)grid_rect.bottomRight.x-2;
			//if(pGrid->text_angle==0)
			//	width = fabs(grid_rect.bottomRight.x-grid_rect.topLeft.x);
			//else
				width = fabs(grid_rect.bottomRight.y-grid_rect.topLeft.y);
			if(pGrid->grid_mode==PIC_BLOCK_GRID)	//���Ԫ�ڲ���ͼ��
			{
				f3dPoint insert_pos;
				insert_pos.x = (grid_rect.topLeft.x+grid_rect.bottomRight.x)/2.0;
				insert_pos.y = (grid_rect.topLeft.y+grid_rect.bottomRight.y)/2.0;
				//InsertBlock(pBlockTableRecord,insert_pos,pGrid->data.sVal,1.0);
				continue;
			}
			DrawText(pDC,rect,pGrid->data.sVal,0,0);
			if(pGrid->feature%10==0)	//���������
			{
				
			}
			else if(pGrid->feature%10==1)//���������
			{
				
			}
			else if(pGrid->feature%10==4)//���������
			{
				
			}
			else if(pGrid->feature%10==2)//���½Ƕ���
			{
				
			}
			else if(pGrid->feature%10==3)//���ϽǶ���
			{
				
			}
		}
	}
	
}

void CXhTableCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawTable(this,&dc,&m_xTable);
}