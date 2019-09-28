#include "stdafx.h"
#include "XhTable.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//-------------------��Ԫ��洢��ά����--------------
CGridMap::CGridMap()
{
	m_nRow = m_nColumn = 0;
	grid_map = NULL;	//��յ�Ԫ��洢��ָ��
}
CGridMap::~CGridMap()
{
	delete []grid_map;	//��յ�Ԫ��洢����ռ�ڴ�
}

void CGridMap::Destroy()
{
	m_nRow = m_nColumn = 0;
	delete []grid_map;
	grid_map=NULL;
}
BOOL CGridMap::Create(int nRow,int nColumn)
{
	if(grid_map==NULL)
		grid_map = new CGrid[nRow*nColumn];
	else
		return FALSE;
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nColumn;j++)
		{
			grid_map[i*nColumn+j].iRow		= i;
			grid_map[i*nColumn+j].iColumn	= j;
		}
	}
	m_nRow = nRow;			//��ֵ������
	m_nColumn = nColumn;	//��ֵ������
	return TRUE;
}

BOOL CGridMap::InsertRow(int iRow)
{
	int i,j;
	if(iRow<0||iRow>=m_nRow)
		return FALSE;
	else
	{
		CGrid *new_map = new CGrid[m_nRow*m_nColumn+m_nColumn];
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				if(i<iRow)
					new_map[i*m_nColumn+j] = grid_map[i*m_nColumn+j];
				else
					new_map[(i+1)*m_nColumn+j] = grid_map[i*m_nColumn+j];
			}
		}
		delete []grid_map;
		m_nRow++;
		grid_map = new_map;
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				grid_map[i*m_nColumn+j].iRow	= i;
				grid_map[i*m_nColumn+j].iColumn	= j;
			}
		}
	}
	return TRUE;
}
BOOL CGridMap::DelRow(int iRow)
{
	int i,j;
	if(iRow<0||iRow>=m_nRow)
		return FALSE;
	else
	{
		CGrid *new_map = new CGrid[m_nRow*m_nColumn-m_nColumn];
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				if(i<iRow)
					new_map[i*m_nColumn+j] = grid_map[i*m_nColumn+j];
				else if(i==iRow)
					continue;
				else
					new_map[(i-1)*m_nColumn+j] = grid_map[i*m_nColumn+j];
			}
		}
		delete []grid_map;
		m_nRow--;
		grid_map = new_map;
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				grid_map[i*m_nColumn+j].iRow	= i;
				grid_map[i*m_nColumn+j].iColumn	= j;
			}
		}
	}
	return TRUE;
}
BOOL CGridMap::InsertColumn(int iColumn)
{
	int i,j;
	if(iColumn<0||iColumn>=m_nColumn)
		return FALSE;
	else
	{
		CGrid *new_map = new CGrid[m_nRow*(m_nColumn+1)];
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				if(j<iColumn)
					new_map[i*(m_nColumn+1)+j] = grid_map[i*m_nColumn+j];
				else if(j==iColumn)
					continue;
				else
					new_map[i*(m_nColumn+1)+j+1] = grid_map[i*m_nColumn+j];
			}
		}
		delete []grid_map;
		grid_map = new_map;
		m_nColumn++;
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				grid_map[i*m_nColumn+j].iRow	= i;
				grid_map[i*m_nColumn+j].iColumn	= j;
			}
		}
	}
	return TRUE;
}
BOOL CGridMap::DelColumn(int iColumn)
{
	int i,j;
	if(iColumn<0||iColumn>=m_nColumn)
		return FALSE;
	else
	{
		CGrid *new_map = new CGrid[m_nRow*(m_nColumn-1)];
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				if(j<iColumn)
					new_map[i*(m_nColumn-1)+j] = grid_map[i*m_nColumn+j];
				else if(j==iColumn)
					continue;
				else
					new_map[i*(m_nColumn-1)+j-1] = grid_map[i*m_nColumn+j];
			}
		}
		delete []grid_map;
		grid_map = new_map;
		m_nColumn--;
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				grid_map[i*m_nColumn+j].iRow	= i;
				grid_map[i*m_nColumn+j].iColumn	= j;
			}
		}
	}
	return TRUE;
}
	//�������кŻ��ָ����Ԫ��ָ��
CGrid* CGridMap::GetGridAt(int iRow,int iColumn)
{
	int i = iRow*(m_nColumn)+iColumn;
	if(i<0||i>=m_nRow*m_nColumn)
		return NULL;
	else
		return &grid_map[iRow*(m_nColumn)+iColumn];
}
	//��䵥Ԫ������
BOOL CGridMap::SetGrid(int iRow, int iColumn,const CGrid &grid)
{
	if(iRow*m_nColumn+iColumn<0||iRow*m_nColumn+iColumn>=m_nRow*m_nColumn)
		return FALSE;
	CGrid *pGrid = GetGridAt(iRow,iColumn);
	if(pGrid)
	{
		pGrid->Copy(grid);	//ȫ����ֵ��ָ����Ԫ��
		return TRUE;
	}
	else
		return FALSE;
}
////////////////////////////////////////////////////////////////////////
CFormatTbl::CFormatTbl()
{
	ID = 0;		//��ʶ
	locate_style = TOP_LEFT;
	rows_arr_high = columns_arr_wide = NULL;
}
CFormatTbl::~CFormatTbl()
{
	if(columns_arr_wide!=NULL)
		delete []columns_arr_wide;
	if(rows_arr_high!=NULL)
		delete []rows_arr_high;
}
BOOL CFormatTbl::CreateCells(int nRow,int nColumn)
{
	if(nRow<1||nColumn<1)
		return FALSE;
	if(!grid_map.Create(nRow,nColumn))
		return FALSE;
	rows_arr_high = new double[nRow];
	columns_arr_wide = new double[nColumn];
	int i;
	for(i=0;i<nRow;i++)
		rows_arr_high[i] = 20.0;
	for(i=0;i<nColumn;i++)
		columns_arr_wide[i] = 80.0;
	return TRUE;
}
//iRow��iColumn������ڵ���1���ֱ�С�ڹ������������������
BOOL CFormatTbl::SetGrid(int iRow, int iColumn,char *string,BOOL bForceToString/*=TRUE*/)
{
	if(!IsValidGridIndex(iRow,iColumn))
		return FALSE;
	return grid_map.GetGridAt(iRow,iColumn)->data.SetGridString(string,bForceToString);
}
BOOL CFormatTbl::SetRowHigh(int iRow,double high)
{
	if(iRow<0||iRow>=grid_map.GetRowsCount())
		return FALSE;
	rows_arr_high[iRow] = high;
	return TRUE;
}
BOOL CFormatTbl::SetColumnWide(int iColumn,double wide)
{
	if(iColumn<0||iColumn>=grid_map.GetColumnsCount())
		return FALSE;
	columns_arr_wide[iColumn] = wide;
	return TRUE;
}
void CFormatTbl::SetBasePos(f2dPoint &pos)
{
	base_pos = pos;
}
void CFormatTbl::SetLocateStyle(TBL_LOCATE_STYLE eStyle)
{
	locate_style = eStyle;
}

	//iRow��iColumn������ڵ���1���ֱ�С�ڹ������������������
BOOL CFormatTbl::GetGridString(int iRow, int iColumn,char *string)
{
	if(!IsValidGridIndex(iRow,iColumn))
		return FALSE;
	CGrid *pGrid = grid_map.GetGridAt(iRow,iColumn);
	if(pGrid==NULL||pGrid->data.sVal==NULL||pGrid->data.vType!=GRID_VT_STRING)
		return FALSE;
	else 
		strcpy(string,pGrid->data.sVal);
	return TRUE;
}
	//���ָ�����߾����ʽ����Ӧ��Ե��λ�þ���
double CFormatTbl::getRowLinePos(BOOL bUpToDown,int iRow)
{
	double dist = 0;
	if(bUpToDown)	//���϶��˵�ָ���о���
	{
		for(int i=0;i<iRow;i++)
			dist += rows_arr_high[i];
	}
	else			//���µ׶˵�ָ���о���
	{
		for(int i=grid_map.GetRowsCount()-1;i>=iRow;i--)
			dist += rows_arr_high[i];
	}
	return dist;
}
	//���ָ�����߾����ʽ����Ӧ��Ե��λ�þ���
double CFormatTbl::getColumnLinePos(BOOL bLToR,int iColumn)
{
	double dist = 0;
	if(bLToR)	//����˵�ָ���о���
	{
		for(int i=0;i<iColumn;i++)
			dist += columns_arr_wide[i];
	}
	else		//���Ҷ˵�ָ���о���
	{
		for(int i=grid_map.GetColumnsCount()-1;i>=iColumn;i--)
			dist += columns_arr_wide[i];
	}
	return dist;
}
BOOL CFormatTbl::GetRowLineStart(int iRow,f2dPoint &start)
{
	if(iRow<0||iRow>grid_map.GetRowsCount())
		return FALSE;
	switch(locate_style)
	{
	case TOP_LEFT:		//���Ͻ�Ϊ��ά�����
		start.x = base_pos.x;
		start.y = base_pos.y+getRowLinePos(TRUE,iRow);
		break;
	case TOP_RIGHT:		//���Ͻ�Ϊ����
		start.x = base_pos.x - getColumnLinePos(FALSE,0);
		start.y = base_pos.y+getRowLinePos(TRUE,iRow);
		break;
	case BOTTOM_LEFT:	//���½�Ϊ����
		start.x = base_pos.x;
		start.y = base_pos.y-getRowLinePos(FALSE,iRow);
		break;
	case BOTTOM_RIGHT:	//���½�Ϊ����
		start.x = base_pos.x - getColumnLinePos(FALSE,0);
		start.y = base_pos.y-getRowLinePos(FALSE,iRow);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CFormatTbl::GetRowLineEnd(int iRow,f2dPoint &end)
{
	if(iRow<0||iRow>grid_map.GetRowsCount())
		return FALSE;
	switch(locate_style)
	{
	case TOP_LEFT:
		end.x = base_pos.x+getColumnLinePos(TRUE,grid_map.GetColumnsCount());
		end.y = base_pos.y+getRowLinePos(TRUE,iRow);
		break;
	case TOP_RIGHT:
		end.x = base_pos.x;
		end.y = base_pos.y+getRowLinePos(TRUE,iRow);
		break;
	case BOTTOM_LEFT:
		end.x = base_pos.x+getColumnLinePos(TRUE,grid_map.GetColumnsCount());
		end.y = base_pos.y-getRowLinePos(FALSE,iRow);
		break;
	case BOTTOM_RIGHT:
		end.x = base_pos.x;
		end.y = base_pos.y-getRowLinePos(FALSE,iRow);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CFormatTbl::GetColumnLineStart(int iColumn,f2dPoint &start)
{
	if(iColumn<0||iColumn>grid_map.GetColumnsCount())
		return FALSE;
	switch(locate_style)
	{
	case TOP_LEFT:
		start.x = base_pos.x+getColumnLinePos(TRUE,iColumn);
		start.y = base_pos.y;
		break;
	case TOP_RIGHT:
		start.x = base_pos.x - getColumnLinePos(FALSE,iColumn);
		start.y = base_pos.y;
		break;
	case BOTTOM_LEFT:
		start.x = base_pos.x+getColumnLinePos(TRUE,iColumn);
		start.y = base_pos.y-getRowLinePos(FALSE,0);
		break;
	case BOTTOM_RIGHT:
		start.x = base_pos.x - getColumnLinePos(FALSE,iColumn);
		start.y = base_pos.y-getRowLinePos(FALSE,0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CFormatTbl::GetColumnLineEnd(int iColumn,f2dPoint &end)
{
	if(iColumn<0||iColumn>grid_map.GetColumnsCount())
		return FALSE;
	switch(locate_style)
	{
	case TOP_LEFT:
		end.x = base_pos.x+getColumnLinePos(TRUE,iColumn);
		end.y = base_pos.y-getRowLinePos(TRUE,grid_map.GetRowsCount());
		break;
	case TOP_RIGHT:
		end.x = base_pos.x-getColumnLinePos(FALSE,iColumn);
		end.y = base_pos.y-getRowLinePos(TRUE,grid_map.GetRowsCount());
		break;
	case BOTTOM_LEFT:
		end.x = base_pos.x+getColumnLinePos(TRUE,iColumn);
		end.y = base_pos.y;
		break;
	case BOTTOM_RIGHT:
		end.x = base_pos.x-getColumnLinePos(FALSE,iColumn);
		end.y = base_pos.y;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CFormatTbl::GetRowHigh(int iRow, double *high)
{
	if(!IsValidGridIndex(iRow,1))
		return FALSE;
	*high = rows_arr_high[iRow];
	return TRUE;
}
BOOL CFormatTbl::GetColumnWide(int iColumn, double *wide)
{
	if(!IsValidGridIndex(1,iColumn))
		return FALSE;
	*wide = columns_arr_wide[iColumn];
	return TRUE;
}
void CFormatTbl::GetBasePos(f2dPoint &pos)
{
	pos = base_pos;
}
TBL_LOCATE_STYLE CFormatTbl::GetLocateStyle()
{
	return locate_style;
}
	//���ָ����Ԫ���С(�߿�)
BOOL CFormatTbl::GetGridRange(int &start_i,int &start_j,int *end_i, int *end_j)
{
	int i,j,tem_end_i=start_i,tem_end_j=start_j;
	f2dPoint pos;
	//�ж��������Ƿ�Ϸ�
	if(!IsValidGridIndex(start_i,start_j))
		return FALSE;
	CGrid *pGrid = grid_map.GetGridAt(start_i,start_j);
	//�ж�ָ�����Ԫ�Ƿ�Ϊ�鵥Ԫ(������)
	if(pGrid->m_bVGridVirtual||pGrid->m_bHGridVirtual)
		return FALSE;
	//��λ���Ԫ���Ͻǵ�����λ��
	if(!GetColumnLineStart(start_j,pos))
		return FALSE;
	if(!GetRowLineStart(start_i,pos))
		return FALSE;
	//��λ���Ԫ���½ǵ�����λ��
	for(i=start_i+1;i<grid_map.GetRowsCount();i++)		//���㵥Ԫ��߶�
	{
		if(!GetGridAt(i,start_j)->m_bVGridVirtual)//�Ƿ���ͬ����һ��Ԫ��ϲ�
			break;
		tem_end_i = i;
	}
	for(j=start_j+1;j<grid_map.GetColumnsCount();j++)	//���㵥Ԫ����
	{
		if(!GetGridAt(start_i,j)->m_bHGridVirtual)	//�Ƿ���ͬ����һ��Ԫ��ϲ�
			break;
		tem_end_j = j;
	}
	if(tem_end_i>start_i||tem_end_j>start_j)
	{
		if(end_i)
			*end_i = tem_end_i;
		if(end_j)
			*end_j = tem_end_j;
		return TRUE;
	}
	else
		return FALSE;
}
//���Ԫ��������0Ϊ��ʼֵ
BOOL CFormatTbl::GetGridRect(int iRow, int iColumn, f2dRect &rect)
{
	int i,j;
	f2dPoint pos;
	//�ж��������Ƿ�Ϸ�
	if(!IsValidGridIndex(iRow,iColumn))
		return FALSE;
	CGrid *pGrid = grid_map.GetGridAt(iRow,iColumn);
	//�ж�ָ�����Ԫ�Ƿ�Ϊ�鵥Ԫ(������)
	if(pGrid->m_bVGridVirtual||pGrid->m_bHGridVirtual)
		return FALSE;
	//��λ���Ԫ���Ͻǵ�����λ��
	if(!GetColumnLineStart(iColumn,pos))
		return FALSE;
	rect.topLeft.x = pos.x;
	if(!GetRowLineStart(iRow,pos))
		return FALSE;
	rect.topLeft.y = pos.y;
	rect.bottomRight.x = rect.topLeft.x+columns_arr_wide[iColumn];
	rect.bottomRight.y = rect.topLeft.y-rows_arr_high[iRow];
	//��λ���Ԫ���½ǵ�����λ��
	for(i=iRow+1;i<grid_map.GetRowsCount();i++)		//���㵥Ԫ��߶�
	{
		if(!GetGridAt(i,iColumn)->m_bVGridVirtual)//�Ƿ���ͬ����һ��Ԫ��ϲ�
			break;
		rect.bottomRight.y += rows_arr_high[i];
	}
	for(j=iColumn+1;j<grid_map.GetColumnsCount();j++)	//���㵥Ԫ����
	{
		if(!GetGridAt(iRow,j)->m_bHGridVirtual)	//�Ƿ���ͬ����һ��Ԫ��ϲ�
			break;
		rect.bottomRight.x += columns_arr_wide[j];
	}
	return TRUE;
}
BOOL CFormatTbl::GetGridSize(int iRow,int iColumn,
							 double *height, double *wide)
{
	f2dRect rect;
	GetGridRect(iRow,iColumn,rect);
	*height = fabs(rect.topLeft.y-rect.bottomRight.y);
	*wide = fabs(rect.bottomRight.x-rect.topLeft.x);
	return FALSE;
}

//���Ԫ��������0Ϊ��ʼֵ
CGrid* CFormatTbl::GetGridAt(int iRow, int iColumn)
{
	return grid_map.GetGridAt(iRow,iColumn);
}
//���Ԫ��������0Ϊ��ʼֵ
BOOL CFormatTbl::IsValidGridIndex(int iRow, int iColumn)
{
	if(iRow<0||iRow>=grid_map.GetRowsCount()||iColumn<0||iColumn>=grid_map.GetColumnsCount())
		return FALSE;
	else
		return TRUE;
}
int CFormatTbl::GetColumnsCount()
{
	return grid_map.GetColumnsCount();
}
int CFormatTbl::GetRowsCount()
{
	return grid_map.GetRowsCount();
}
double CFormatTbl::GetTableWide()
{
	double wide=0;
	for(int i=0;i<grid_map.GetColumnsCount();i++)
		wide+=columns_arr_wide[i];
	return wide;
}
double CFormatTbl::GetTableHeight()
{
	double height=0;
	for(int i=0;i<grid_map.GetRowsCount();i++)
		height+=rows_arr_high[i];
	return height;
}

void CFormatTbl::Destroy()
{
	grid_map.Destroy();
	locate_style = TOP_LEFT;
	if(columns_arr_wide!=NULL)
		delete []columns_arr_wide;
	if(rows_arr_high!=NULL)
		delete []rows_arr_high;
	rows_arr_high = columns_arr_wide = NULL;
}

BOOL CFormatTbl::InsertRow(int iRow,double high)
{
	int i,n = grid_map.GetRowsCount();
	if(iRow<0||iRow>=n)
		return FALSE;
	else
	{
		double *rows_high = new double[n];
		memcpy(rows_high,rows_arr_high,n*sizeof(double));
		delete[]rows_arr_high;
		rows_arr_high = new double[n+1];
		for(i=0;i<iRow;i++)
			rows_arr_high[i] = rows_high[i];
		rows_arr_high[iRow] = high;
		for(i=iRow;i<n;i++)
			rows_arr_high[i+1] = rows_high[i];
		delete []rows_high;
		return grid_map.InsertRow(iRow);
	}
}
BOOL CFormatTbl::DelRow(int iRow)
{
	int i,n = grid_map.GetRowsCount();
	if(iRow<0||iRow>=n||iRow>=n)
		return FALSE;
	else
	{
		double *rows_high = new double[n];
		memcpy(rows_high,rows_arr_high,n*sizeof(double));
		delete[]rows_arr_high;
		rows_arr_high = new double[n-1];
		for(i=0;i<iRow;i++)
			rows_arr_high[i] = rows_high[i];
		for(i=iRow+1;i<n;i++)
			rows_arr_high[i-1] = rows_high[i];
		delete []rows_high;
		return grid_map.DelRow(iRow);
	}
}
BOOL CFormatTbl::InsertColumn(int iColumn,double wide)
{
	int i,n = grid_map.GetColumnsCount();
	if(iColumn<0||iColumn>=n)
		return FALSE;
	else
	{
		double *columns_wide = new double[n];
		memcpy(columns_wide,columns_arr_wide,n*sizeof(double));
		delete[]columns_arr_wide;
		columns_arr_wide = new double[n+1];
		for(i=0;i<iColumn;i++)
			columns_arr_wide[i] = columns_wide[i];
		columns_arr_wide[iColumn] = wide;
		for(i=iColumn;i<n;i++)
			columns_arr_wide[i+1] = columns_wide[i];
		delete []columns_wide;
		return grid_map.InsertColumn(iColumn);
	}
}
BOOL CFormatTbl::DelColumn(int iColumn)
{
	int i,n = grid_map.GetColumnsCount();
	if(iColumn<0||iColumn>=n||iColumn>=n)
		return FALSE;
	else
	{
		double *columns_wide = new double[n];
		memcpy(columns_wide,columns_arr_wide,n*sizeof(double));
		delete[]columns_arr_wide;
		columns_arr_wide = new double[n-1];
		for(i=0;i<iColumn;i++)
			columns_arr_wide[i] = columns_wide[i];
		for(i=iColumn+1;i<n;i++)
			columns_arr_wide[i-1] = columns_wide[i];
		delete []columns_wide;
		return grid_map.DelColumn(iColumn);
	}
}
	//���ı��ļ������ʽ��
BOOL CFormatTbl::Import(FILE *fp)		//���ı��ļ������ʽ��
{
	char sLineText[200],*sTokey=NULL,separator[]=" =,\r\n";
	int m_nRow = GetRowsCount(),m_nCol = GetColumnsCount();

	while(feof(fp)==0)
	{
		fgets(sLineText,200,fp);
		sTokey = strtok(sLineText,separator);
		if(sTokey)
		{
			if(stricmp(sTokey,"FormatTableID")==0)
			{
				sTokey = strtok(NULL,separator);
				//if(sTokey)
					//ID = atoi(sTokey);
			}
			else if(stricmp(sTokey,"LocateStyle")==0)
			{
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					int key = atoi(sTokey);
					if(key==TOP_LEFT)
						locate_style = TOP_LEFT;
					else if(key==TOP_RIGHT)
						locate_style = TOP_RIGHT;
					else if(key==BOTTOM_LEFT)
						locate_style = BOTTOM_LEFT;
					else if(key==BOTTOM_RIGHT)
						locate_style = BOTTOM_RIGHT;
					else
						locate_style = TOP_LEFT;
				}
			}
			else if(stricmp(sTokey,"Rows")==0)
			{
				int nRow,nCol;
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					nRow = atoi(sTokey);
					sTokey = strtok(NULL,separator);
					if(sTokey)
					{
						if(stricmp(sTokey,"Columns")==0)
							sTokey = strtok(NULL,separator);
						if(sTokey)
							nCol = atoi(sTokey);
						else
							nCol = 1;
					}
					else 
						nCol = 1;
					CreateCells(nRow,nCol);
				}
			}
			else if(stricmp(sTokey,"BasePosX")==0)
			{
				sTokey = strtok(NULL,separator);
				if(sTokey)
					base_pos.x = atof(sTokey);
			}
			else if(stricmp(sTokey,"BasePosY")==0)
			{
				sTokey = strtok(NULL,separator);
				if(sTokey)
					base_pos.y = atof(sTokey);
			}
			else if(sLineText[0]=='#')
				continue;
			else if(sLineText[0]=='\n')
				continue;
			else if(stricmp(sTokey,"Row_i")==0)
			{
				int ii;
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					ii = atoi(sTokey);
					sTokey = strtok(NULL,separator);
					if(sTokey&&stricmp(sTokey,"High")==0)
					{
						sTokey = strtok(NULL,separator);
						double high = atof(sTokey);
						SetRowHigh(ii,high);
					}
				}
			}
			else if(stricmp(sTokey,"Column_j")==0)
			{
				int ii;
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					ii = atoi(sTokey);
					sTokey = strtok(NULL,separator);
					if(sTokey&&stricmp(sTokey,"Wide")==0)
					{
						sTokey = strtok(NULL,separator);
						double wide = atof(sTokey);
						SetColumnWide(ii,wide);
					}
				}
			}
			else if(stricmp(sTokey,"--NewGridStart")==0)
			{
				fgets(sLineText,200,fp);
				sTokey = strtok(sLineText,separator);
				int i=0,j=0;
				if(sTokey&&strcmp(sTokey,"i")==0)
				{
					sTokey = strtok(NULL,separator);
					if(sTokey)
					{
						i = atoi(sTokey);
						sTokey = strtok(NULL,separator);
					}
				}
				if(sTokey&&strcmp(sTokey,"j")==0)
				{
					sTokey = strtok(NULL,separator);
					j = atoi(sTokey);
				}
				grid_map.GetGridAt(i,j)->Import(fp);
			}
			else if(stricmp(sTokey,"--EndFormatTbl")==0)
				break;
		}
	}
	return TRUE;
}
BOOL CFormatTbl::Import(char *filename)
{
	FILE* fp;
	char sLineText[200],*sTokey=NULL,separator[]=" =,\r\n";
	int m_nRow = GetRowsCount(),m_nCol = GetColumnsCount();
	if((fp=fopen(filename,"rt"))==NULL)
		return FALSE;
	while(feof(fp)==0)
	{
		fgets(sLineText,200,fp);
		sTokey = strtok(sLineText,separator);
		if(sTokey)
		{
			if(stricmp(sTokey,"FormatTableID")==0)
			{
				sTokey = strtok(NULL,separator);
				//if(sTokey)
					//ID = atoi(sTokey);
			}
			else if(stricmp(sTokey,"LocateStyle")==0)
			{
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					int key = atoi(sTokey);
					if(key==TOP_LEFT)
						locate_style = TOP_LEFT;
					else if(key==TOP_RIGHT)
						locate_style = TOP_RIGHT;
					else if(key==BOTTOM_LEFT)
						locate_style = BOTTOM_LEFT;
					else if(key==BOTTOM_RIGHT)
						locate_style = BOTTOM_RIGHT;
					else
						locate_style = TOP_LEFT;
				}
			}
			else if(stricmp(sTokey,"Rows")==0)
			{
				int nRow,nCol;
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					nRow = atoi(sTokey);
					sTokey = strtok(NULL,separator);
					if(sTokey)
					{
						if(stricmp(sTokey,"Columns")==0)
							sTokey = strtok(NULL,separator);
						if(sTokey)
							nCol = atoi(sTokey);
						else
							nCol = 1;
					}
					else 
						nCol = 1;
					CreateCells(nRow,nCol);
				}
			}
			else if(stricmp(sTokey,"BasePosX")==0)
			{
				sTokey = strtok(NULL,separator);
				if(sTokey)
					base_pos.x = atof(sTokey);
			}
			else if(stricmp(sTokey,"BasePosY")==0)
			{
				sTokey = strtok(NULL,separator);
				if(sTokey)
					base_pos.y = atof(sTokey);
			}
			else if(sLineText[0]=='#')
				continue;
			else if(sLineText[0]=='\n')
				continue;
			else if(stricmp(sTokey,"Row_i")==0)
			{
				int ii;
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					ii = atoi(sTokey);
					sTokey = strtok(NULL,separator);
					if(sTokey&&stricmp(sTokey,"High")==0)
					{
						sTokey = strtok(NULL,separator);
						double high = atof(sTokey);
						SetRowHigh(ii,high);
					}
				}
			}
			else if(stricmp(sTokey,"Column_j")==0)
			{
				int ii;
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					ii = atoi(sTokey);
					sTokey = strtok(NULL,separator);
					if(sTokey&&stricmp(sTokey,"Wide")==0)
					{
						sTokey = strtok(NULL,separator);
						double wide = atof(sTokey);
						SetColumnWide(ii,wide);
					}
				}
			}
			else if(stricmp(sTokey,"--NewGridStart")==0)
			{
				fgets(sLineText,200,fp);
				sTokey = strtok(sLineText,separator);
				int i=0,j=0;
				if(sTokey&&strcmp(sTokey,"i")==0)
				{
					sTokey = strtok(NULL,separator);
					if(sTokey)
					{
						i = atoi(sTokey);
						sTokey = strtok(NULL,separator);
					}
				}
				if(sTokey&&strcmp(sTokey,"j")==0)
				{
					sTokey = strtok(NULL,separator);
					j = atoi(sTokey);
				}
				grid_map.GetGridAt(i,j)->Import(fp);
			}
		}
	}
	fclose(fp);
	return TRUE;
}
	//���ı��ļ�������ʽ��
BOOL CFormatTbl::Export(FILE *fp)			//���ı��ļ�������ʽ��
{
	int i,j,m_nRow = GetRowsCount(),m_nCol = GetColumnsCount();

	fprintf(fp,"FormatTableID=%d\n",ID);
	fprintf(fp,"LocateStyle=%d\n",locate_style);
	fprintf(fp,"Rows=%d,Columns=%d\n",m_nRow,m_nCol);
	fprintf(fp,"BasePosX=%.f\n",base_pos.x);
	fprintf(fp,"BasePosY=%.f\n",base_pos.y);
	fprintf(fp,"\n#----------Row High Section-----------\n\n");
	for(i=0;i<m_nRow;i++)
		fprintf(fp,"Row_i=%d,High=%.f\n",i,rows_arr_high[i]);
	fprintf(fp,"\n#----------Column Wide Section-----------\n\n");
	for(i=0;i<m_nCol;i++)
		fprintf(fp,"Column_j=%d,Wide=%.f\n",i,columns_arr_wide[i]);
	fprintf(fp,"\n#----------Grid Content Section-----------\n\n");
	for(i=0;i<m_nRow;i++)
	{
		for(j=0;j<m_nCol;j++)
		{
			fprintf(fp,"--NewGridStart\n");
			fprintf(fp,"i=%d,j=%d\n",i,j);
			grid_map.GetGridAt(i,j)->Export(fp);
			fprintf(fp,"\n");
		}
	}

	return TRUE;
}
BOOL CFormatTbl::Export(char *filename)
{
	FILE* fp;
	int i,j,m_nRow = GetRowsCount(),m_nCol = GetColumnsCount();
	if((fp=fopen(filename,"wt"))==NULL)
		return FALSE;
	fprintf(fp,"FormatTableID=%d\n",ID);
	fprintf(fp,"LocateStyle=%d\n",locate_style);
	fprintf(fp,"Rows=%d,Columns=%d\n",m_nRow,m_nCol);
	fprintf(fp,"BasePosX=%.f\n",base_pos.x);
	fprintf(fp,"BasePosY=%.f\n",base_pos.y);
	fprintf(fp,"\n#----------Row High Section-----------\n\n");
	for(i=0;i<m_nRow;i++)
		fprintf(fp,"Row_i=%d,High=%.f\n",i,rows_arr_high[i]);
	fprintf(fp,"\n#----------Column Wide Section-----------\n\n");
	for(i=0;i<m_nCol;i++)
		fprintf(fp,"Column_j=%d,Wide=%.f\n",i,columns_arr_wide[i]);
	fprintf(fp,"\n#----------Grid Content Section-----------\n\n");
	for(i=0;i<m_nRow;i++)
	{
		for(j=0;j<m_nCol;j++)
		{
			fprintf(fp,"--NewGridStart\n");
			fprintf(fp,"i=%d,j=%d\n",i,j);
			grid_map.GetGridAt(i,j)->Export(fp);
			fprintf(fp,"\n");
		}
	}

	fclose(fp);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CFlexTbl Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlexTbl::CFlexTbl()
{
	m_nFormatTblCounts = 0;
	curFormatTblID = 0;
	iCursor = 0;
}

CFlexTbl::~CFlexTbl()
{
	int i,n;
	n = flex_tbl.GetSize();
	for(i=0;i<n;i++)
		delete flex_tbl[i];
	flex_tbl.RemoveAll();
}
void CFlexTbl::RemoveAll()
{
	int i,n;
	n = flex_tbl.GetSize();
	for(i=0;i<n;i++)
		delete flex_tbl[i];
	flex_tbl.RemoveAll();
}

BOOL CFlexTbl::CreateTblFrame(CFormatTbl *pFormatTbl,ATOM_LIST<f2dLine>* pTblLineList)
{
	int i,j,nColumnsCount,nRowsCount;
	f2dPoint start,end;
	f2dLine line;
	if(pTblLineList==NULL)
		return FALSE;
	else
		pTblLineList->Empty();
	//f3dLine��featureֵ��ʾͼԪ�������չ��ʶ�����startPt��featureֵ��ʾ
	//����λ�ã�������������λ�ã�����������3��ʾ��3������-4��ʾ��4������
	//TODO:���ڰ������õ�EntityDataBase.h�任��������һ�д�����������ʱע�� wjh-2016.6.30
	//line.feature = pEDB->EXT_ID;
	if(pFormatTbl->GetColumnsCount()<=0||pFormatTbl->GetRowsCount()<=0)
		return FALSE;
	nColumnsCount = pFormatTbl->GetColumnsCount();
	nRowsCount	  = pFormatTbl->GetRowsCount();
	//�����б����(����)
	for (i=0;i<=nRowsCount;i++)
	{
		pFormatTbl->GetRowLineStart(i,start);
		pFormatTbl->GetRowLineEnd(i,end);
		for(j=0;j<nColumnsCount;j++)
		{	//�������е�����Ԫ
			if(i==0||i==nRowsCount)	//��ĩ�в����ж��鵥Ԫ
				break;
			double wide;
			pFormatTbl->GetColumnWide(j,&wide);	//��õ�j�б���
			CGrid *pGrid = pFormatTbl->GetGridAt(i,j);
			if(j==0&&pGrid->m_bVGridVirtual)
			{		//���о�Ϊ�鵥Ԫ
				start.x += wide;
				end.x = start.x;
			}
			else if(pGrid->m_bVGridVirtual)
			{	//��ǰ��Ϊ�鵥Ԫ
				if(!pFormatTbl->GetGridAt(i,j-1)->m_bVGridVirtual)
				{
					line.startPt.Set(start.x,start.y);
					line.endPt.Set(end.x,end.y);
					line.startPt.feature = i+1;	//������ʾ����Ϊ����
					pTblLineList->append(line);
					start.x = end.x+wide;
					end.x = start.x;
				}
				else
				{
					end.x += wide;
					start.x = end.x;
				}
			}
			else	//��ǰ�в����鵥Ԫ,�޸�����β����
			{
				if(j==0)
					end.x = start.x+wide;
				else
					end.x += wide;
			}
		}
		line.startPt.Set(start.x,start.y);
		line.endPt.Set(end.x,end.y);
		line.startPt.feature = i+1;	//������ʾ����Ϊ����
		pTblLineList->append(line);
	}
	// ���ɱ������(����)
	for(j=0;j<=nColumnsCount;j++)
	{
		pFormatTbl->GetColumnLineStart(j,start);
		pFormatTbl->GetColumnLineEnd(j,end);
		for(i=0;i<nRowsCount;i++)
		{	//�������е�����Ԫ
			if(j==0||j==nColumnsCount)	//��ĩ�в����ж��鵥Ԫ
				break;
			double high;
			pFormatTbl->GetRowHigh(i,&high);
			CGrid *pGrid = pFormatTbl->GetGridAt(i,j);
			if(pGrid==NULL)
				break;
			if(i==0&&pGrid->m_bHGridVirtual)
			{		//���о�Ϊ�鵥Ԫ
				start.y -= high;
				end.y = start.y;
			}
			else if(pGrid->m_bHGridVirtual)
			{		//��ǰ�����鵥Ԫ
				if(!pFormatTbl->GetGridAt(i-1,j)->m_bHGridVirtual)
				{
					line.startPt.Set(start.x,start.y);
					line.endPt.Set(end.x,end.y);
					line.startPt.feature = -(j+1);	//������ʾ����Ϊ����
					pTblLineList->append(line);
					start.y = end.y-high;
					end.y = start.y;
				}
				else
				{
					end.y -= high;
					start.y = end.y;
				}
			}
			else	//��ǰ�в����鵥Ԫ,�޸�����β����
			{
				if(i==0)
					end.y = start.y - high;
				else
					end.y -= high;
			}
		}
		line.startPt.Set(start.x,start.y);
		line.endPt.Set(end.x,end.y);
		line.startPt.feature = -(j+1);	//������ʾ����Ϊ����
		pTblLineList->append(line);
	}
	return TRUE;
}
CFormatTbl *CFlexTbl::GetFirst()
{
	if(flex_tbl.GetSize()<=0)
		return NULL;
	else
	{
		iCursor = 1;
		return flex_tbl[0];
	}
}

CFormatTbl *CFlexTbl::GetNext()
{
	if(iCursor<flex_tbl.GetSize())
		return flex_tbl[iCursor++];
	else
		return NULL;
}
CFormatTbl *CFlexTbl::GetFormatTblByID(long ID)
{
	CFormatTbl *pTable;
	for(pTable=GetFirst();pTable!=NULL;pTable=GetNext())
	{
		if(pTable->ID==ID)
			return pTable;
	}
	return NULL;
}

long CFlexTbl::AddFormatTbl(f2dPoint base_pos,TBL_LOCATE_STYLE mode,int nRow, int nColumn)
{
	CFormatTbl *pFormatTbl;
	pFormatTbl = new CFormatTbl;
	pFormatTbl->ID = ++curFormatTblID;
	pFormatTbl->CreateCells(nRow,nColumn);
	pFormatTbl->SetLocateStyle(mode);
	pFormatTbl->SetBasePos(base_pos);
	flex_tbl.Add(pFormatTbl);
	m_nFormatTblCounts++;
	return pFormatTbl->ID;
}
CFormatTbl *CFlexTbl::AddEmptyFormatTbl()
{
	CFormatTbl *pFormatTbl;
	pFormatTbl = new CFormatTbl;
	pFormatTbl->ID = ++curFormatTblID;
	flex_tbl.Add(pFormatTbl);
	m_nFormatTblCounts++;
	return pFormatTbl;
}
BOOL CFlexTbl::DelFormatTbl(long ID)
{
	for(int i=0;i<m_nFormatTblCounts;i++)
	{
		if(flex_tbl[i]->GetID()==ID)
		{
			flex_tbl.RemoveAt(i);
			return TRUE;
		}
	}
	return FALSE;
}

long CFlexTbl::GetFormatTblCount()
{
	return flex_tbl.GetSize();
}

int CFormatTbl::ReleaseSnapStatus()
{
	int nHits=0;
	long nRows = grid_map.GetRowsCount();
	long nColumns = grid_map.GetColumnsCount();
	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nColumns;j++)
		{
			CGrid *pGrid = grid_map.GetGridAt(i,j);
			if(pGrid->m_bSelectStatus)
			{
				nHits++;
				pGrid->m_bSelectStatus=FALSE;
			}
		}
	}
	return nHits;
}

int CFormatTbl::GetSelectedCounts()
{
	int nHits=0;
	long nRows = grid_map.GetRowsCount();
	long nColumns = grid_map.GetColumnsCount();
	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nColumns;j++)
		{
			CGrid *pGrid = grid_map.GetGridAt(i,j);
			if(pGrid->m_bSelectStatus)
			{
				nHits++;
			}
		}
	}
	return nHits;
}
CGrid *CFormatTbl::GetFirstSelectedGrid(CFormatTbl **pFormatTbl/*=NULL*/,int *pos_i/*=NULL*/,int *pos_j/*=NULL*/)
{
	long nRows = grid_map.GetRowsCount();
	long nColumns = grid_map.GetColumnsCount();
	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nColumns;j++)
		{
			CGrid *pGrid = grid_map.GetGridAt(i,j);
			if(pGrid->m_bSelectStatus)
			{
				if(pFormatTbl)
					*pFormatTbl = this;
				if(pos_i)
					*pos_i = i;
				if(pos_j)
					*pos_j = j;
				return pGrid;
			}
		}
	}
	return NULL;
}

void CFormatTbl::SelectAll()
{
	long nRows = grid_map.GetRowsCount();
	long nColumns = grid_map.GetColumnsCount();
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nColumns;j++)
			grid_map.GetGridAt(i,j)->m_bSelectStatus=TRUE;
}

CGrid * CFormatTbl::SnapGrid(double x, double y, int *iRow, int *iCol)
{
	int i,j,nRow,nCol;
	f2dRect rect;
	nRow = GetRowsCount();
	nCol = GetColumnsCount();
	for(i=0;i<nRow;i++)
	{
		for(j=0;j<nCol;j++)
		{
			GetGridRect(i,j,rect);
			if(rect.PtInRect(f2dPoint(x,y)))
			{
				if(iRow)
					*iRow = i;
				if(iCol)
					*iCol = j;
				return GetGridAt(i,j);
			}
		}
	}
	return NULL;
}

CGrid * CFlexTbl::SnapGrid(double x, double y, int *iRow, int *iCol, long *nFormatTblID)
{
	int i;
	CGrid *pGrid;
	for(i=0;i<flex_tbl.GetSize();i++)
	{
		pGrid = flex_tbl[i]->SnapGrid(x,y,iRow,iCol);
		if(pGrid)
		{
			if(nFormatTblID)
				*nFormatTblID=flex_tbl[i]->ID;
			return pGrid;
		}
	}
	return NULL;
}

CFormatTbl *CFlexTbl::SnapFormatTbl(double x, double y)
{
	f2dRect rect2d;
	int i,n = flex_tbl.GetSize();
	for(i=0;i<n;i++)
	{
		if(flex_tbl[i]->GetTblRect(&rect2d))
		{
			if(rect2d.PtInRect(f2dPoint(x,y)))
				return flex_tbl[i];
		}
	}
	return NULL;
}

BOOL CFormatTbl::GetTblRect(f2dRect *tbl_rect)
{
	if(GetRowsCount()<=0||GetColumnsCount()<=0)
		return FALSE;
	double tbl_wide,tbl_high;
	tbl_wide = GetTableWide();
	tbl_high = GetTableHeight();
	switch(locate_style)
	{
	case TOP_LEFT:		//���Ͻ�
		tbl_rect->topLeft = base_pos;
		tbl_rect->bottomRight.Set(base_pos.x+tbl_wide,base_pos.y-tbl_high);
		break;
	case TOP_RIGHT:		//���Ͻ�
		tbl_rect->topLeft.Set(base_pos.x-tbl_wide,base_pos.y);
		tbl_rect->bottomRight.Set(base_pos.x,base_pos.y-tbl_high);
		break;
	case BOTTOM_LEFT:	//���½�
		tbl_rect->topLeft.Set(base_pos.x,base_pos.y+tbl_high);
		tbl_rect->bottomRight.Set(base_pos.x+tbl_wide,base_pos.y);
		break;
	case BOTTOM_RIGHT:	//���½�
		tbl_rect->topLeft.Set(base_pos.x-tbl_wide,base_pos.y+tbl_high);
		tbl_rect->bottomRight = base_pos;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

long CFormatTbl::GetFormatTblID()
{
	return ID;
}

BOOL CFlexTbl::SnapGridPos(double x, double y, long nExcludeFormatTblID, 
						   f2dPoint *snap_pos,double scope/*=1.0*/)
{
	int i,n = flex_tbl.GetSize();
	double dist;
	f2dPoint tem_snap_pos;
	BOOL bSnaped = FALSE;
	for(i=0;i<n;i++)
	{
		if(flex_tbl[i]->GetID()==nExcludeFormatTblID)
			continue;
		if(!flex_tbl[i]->SnapGridPos(x,y,&tem_snap_pos,scope))
			continue;
		else
		{
			if(!bSnaped)
			{
				dist = fabs(x-tem_snap_pos.x)+fabs(y-tem_snap_pos.y);
				*snap_pos = tem_snap_pos;
				bSnaped = TRUE;
			}
			else
			{
				double tem_dist = fabs(x-tem_snap_pos.x)+fabs(y-tem_snap_pos.y);
				if(dist>tem_dist)
				{
					dist = tem_dist;
					*snap_pos = tem_snap_pos;
				}
			}
		}
	}
	return bSnaped;
}

BOOL CFormatTbl::SnapGridPos(double x, double y, f2dPoint *snap_pos,double scope/*=1.0*/)
{
	int i,nRow,nCol;
	double dist;
	f2dPoint start,tem_snap_pos;
	nRow = GetRowsCount();
	nCol = GetColumnsCount();
	BOOL bSnaped = FALSE;
	dist = scope;
	snap_pos->Set(x,y);
	for(i=0;i<=nRow;i++)
	{
		GetRowLineStart(i,start);
		if(fabs(start.y-y)<dist)
		{
			snap_pos->y = start.y;
			dist = fabs(start.y-y);
			bSnaped = TRUE;
		}
	}
	dist = scope;
	for(i=0;i<=nCol;i++)
	{
		GetColumnLineStart(i,start);
		if(fabs(start.x-x)<dist)
		{
			snap_pos->x = start.x;
			dist = fabs(start.x-x);
			bSnaped = TRUE;
		}
	}
	return bSnaped;
}

int CFlexTbl::ReleaseSnapStatus()
{
	int nHits=0;
	for(int i=0;i<flex_tbl.GetSize();i++)
		nHits+=flex_tbl[i]->ReleaseSnapStatus();
	return nHits;
}
int CFlexTbl::GetSelectedCounts()
{
	int nHits=0;
	for(int i=0;i<flex_tbl.GetSize();i++)
		nHits+=flex_tbl[i]->GetSelectedCounts();
	return nHits;
}
CGrid* CFlexTbl::GetFirstSelectedGrid(CFormatTbl **pFormatTbl/*=NULL*/,int *pos_i/*=NULL*/,int *pos_j/*=NULL*/)
{
	CGrid *pGrid;
	for(int i=0;i<flex_tbl.GetSize();i++)
	{
		pGrid=flex_tbl[i]->GetFirstSelectedGrid(pFormatTbl,pos_i,pos_j);
		if(pGrid)
			return pGrid;
	}
	return NULL;
}

void CFlexTbl::CopyFrom(CFlexTbl *pFlexTbl)
{
	CFormatTbl *pSrcFormatTbl,*pDestFormatTbl;
	for(pSrcFormatTbl=pFlexTbl->GetFirst();pSrcFormatTbl;pSrcFormatTbl=pFlexTbl->GetNext())
	{
		pDestFormatTbl = AddEmptyFormatTbl();
		pDestFormatTbl->CopyFrom(pSrcFormatTbl);
	}
}

void CFormatTbl::CopyFrom(CFormatTbl *pFormatTbl)
{
	int i,j,nRows,nCols;
	double high,wide;
	pFormatTbl->GetBasePos(base_pos);
	locate_style = pFormatTbl->GetLocateStyle();
	nRows = pFormatTbl->GetRowsCount();
	nCols = pFormatTbl->GetColumnsCount();
	CreateCells(nRows,nCols);
	for(i=0;i<nRows;i++)
	{
		pFormatTbl->GetRowHigh(i,&high);
		SetRowHigh(i,high);
	}
	for(j=0;j<nCols;j++)
	{
		pFormatTbl->GetColumnWide(j,&wide);
		SetColumnWide(j,wide);
	}
	for(i=0;i<nRows;i++)
	{
		for(j=0;j<nCols;j++)
		{
			CGrid *pGrid = pFormatTbl->GetGridAt(i,j);
			GetGridAt(i,j)->Copy(*pGrid);
		}
	}
}


//���Ʊ�񵫲�����iRemoveStart��iRemoveEnd֮����� wht 11-03-13
void CFormatTbl::CopyFrom(CFormatTbl *pFormatTbl,int iRemoveStart,int iRemoveEnd)
{
	if(iRemoveStart<0||iRemoveEnd<0||iRemoveEnd<=iRemoveStart)
	{	//���������ı������
		CopyFrom(pFormatTbl);
		return;
	}
	int i,j,nRows,nCols;
	double high,wide;
	pFormatTbl->GetBasePos(base_pos);
	locate_style = pFormatTbl->GetLocateStyle();
	nRows = pFormatTbl->GetRowsCount();
	nCols = pFormatTbl->GetColumnsCount();
	int nRealRows=nRows-(iRemoveEnd-iRemoveStart+1);	//��ʵ����
	CreateCells(nRealRows,nCols);
	int iRowIndex=0;
	for(i=0;i<nRows;i++)
	{	//�����и�
		if(i>=iRemoveStart&&i<=iRemoveEnd)
			continue;	//����ָ����
		pFormatTbl->GetRowHigh(i,&high);
		SetRowHigh(iRowIndex,high);
		iRowIndex++;
	}
	for(j=0;j<nCols;j++)
	{	//�����п�
		pFormatTbl->GetColumnWide(j,&wide);
		SetColumnWide(j,wide);
	}
	iRowIndex=0;
	for(i=0;i<nRows;i++)
	{	//���õ�Ԫ������
		if(i>=iRemoveStart&&i<=iRemoveEnd)
			continue;	//����ָ����
		for(j=0;j<nCols;j++)
		{
			CGrid *pGrid = pFormatTbl->GetGridAt(i,j);
			GetGridAt(iRowIndex,j)->Copy(*pGrid);
		}
		iRowIndex++;
	}
}

/*#ifndef __DRAWING_CONTEXT_
HRESULT CFormatTbl::ReadFormatTbl(IStream *pStream)
{
	WORD w;
	int i,j,nRow,nCol;
	w = locate_style;
	pStream->Read(&w,sizeof(WORD),NULL);
	if(w==TOP_LEFT)
		locate_style = TOP_LEFT;
	else if(w==TOP_RIGHT)
		locate_style = TOP_RIGHT;
	else if(w==BOTTOM_LEFT)
		locate_style = BOTTOM_LEFT;
	else if(w==BOTTOM_RIGHT)
		locate_style = BOTTOM_RIGHT;
	pStream->Read(&w,sizeof(WORD),NULL);
	nRow = w;
	pStream->Read(&w,sizeof(WORD),NULL);
	nCol = w;
	pStream->Read(&base_pos.x,sizeof(double),NULL);
	pStream->Read(&base_pos.y,sizeof(double),NULL);
	CreateCells(nRow,nCol);
	for(i=0;i<nRow;i++)
		pStream->Read(&rows_arr_high[i],sizeof(double),NULL);
	for(i=0;i<nCol;i++)
		pStream->Read(&columns_arr_wide[i],sizeof(double),NULL);
	for(i=0;i<nRow;i++)
	{
		for(j=0;j<nCol;j++)
			grid_map.GetGridAt(i,j)->ReadGridFromStream(pStream);
	}
	return S_OK;
}

HRESULT CFormatTbl::WriteFormatTbl(IStream *pStream)
{
	WORD w;
	int i,j,nRow,nCol;
	w = locate_style;
	pStream->Write(&w,sizeof(WORD),NULL);
	nRow = w = (WORD)GetRowsCount();
	pStream->Write(&w,sizeof(WORD),NULL);
	nCol = w = (WORD)GetColumnsCount();
	pStream->Write(&w,sizeof(WORD),NULL);
	pStream->Write(&base_pos.x,sizeof(double),NULL);
	pStream->Write(&base_pos.y,sizeof(double),NULL);
	for(i=0;i<nRow;i++)
		pStream->Write(&rows_arr_high[i],sizeof(double),NULL);
	for(i=0;i<nCol;i++)
		pStream->Write(&columns_arr_wide[i],sizeof(double),NULL);
	for(i=0;i<nRow;i++)
	{
		for(j=0;j<nCol;j++)
			grid_map.GetGridAt(i,j)->WriteGridToStream(pStream);
	}
	return S_OK;
}
#endif*/
BOOL CFlexTbl::Import(char *filename)
{
	FILE* fp;
	char sLineText[200],*sTokey=NULL,separator[]=" =,\r\n";
	if((fp=fopen(filename,"rt"))==NULL)
		return FALSE;
	int index=0;
	while(feof(fp)==0)
	{
		fgets(sLineText,200,fp);
		sTokey = strtok(sLineText,separator);
		if(stricmp(sTokey,"FormatTableN")==0)
		{
			sTokey = strtok(NULL,separator);
			int n = atoi(sTokey);
			flex_tbl.SetSize(n);
			for(int ii=0;ii<n;ii++)
				flex_tbl[ii] = new CFormatTbl;
			
		}
		else if(stricmp(sTokey,"--StartFormatTbl")==0)
		{
			flex_tbl[index]->Import(fp);
			index++;
		}
		else if(stricmp(sTokey,"--FlexTableEnd")==0)
			break;
		else
			continue;
	}
	fclose(fp);
	return TRUE;
}
BOOL CFlexTbl::Export(char *filename)
{
	FILE* fp;
	int i,n;
	if((fp=fopen(filename,"wt"))==NULL)
		return FALSE;
	n = flex_tbl.GetSize();
	fprintf(fp,"--FlexTableStart\n");
	fprintf(fp,"FormatTableN=%d\n",n);
	for(i=0;i<n;i++)
	{
		fprintf(fp,"--StartFormatTbl %d\n",i);
		flex_tbl[i]->Export(fp);
		fprintf(fp,"--EndFormatTbl %d \n",i);
	}
	fprintf(fp,"--FlexTableEnd\n");

	fclose(fp);
	return TRUE;
}