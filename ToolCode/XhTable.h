#ifndef __FORMAT_TABLE_H_
#define __FORMAT_TABLE_H_

#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "TblDef.h"
#include "EntityDataBase.h"

class CFormatTbl;
class CGrid;
/*HIDE_CLASS */class CGridMap
{
	CGrid *grid_map;		//���洢��
	int m_nRow,m_nColumn;	//��������������
public:
	CGridMap();
	~CGridMap();
public:
	void Destroy();
	BOOL Create(int nRow,int nColumn);
	int GetRowsCount(){return m_nRow;}
	int GetColumnsCount(){return m_nColumn;}
	BOOL InsertRow(int iRow);
	BOOL DelRow(int iRow);
	BOOL InsertColumn(int iColumn);
	BOOL DelColumn(int iColumn);

	CGrid* GetGridAt(int iRow,int iColumn);					//�������кŻ��ָ����Ԫ��ָ��
	BOOL SetGrid(int iRow, int iColumn,const CGrid &grid);	//��䵥Ԫ������
};
class CFlexTbl;
class CFormatTbl
{
	long ID;
	friend class CFlexTbl;
public:
	CFormatTbl();
	~CFormatTbl();
private:
	TBL_LOCATE_STYLE locate_style;//���Ͻ�;���½�;���½�;���Ͻ�
	f2dPoint base_pos;			//��(��λ)���ά����
	double *rows_arr_high;			//�и��б�
	double *columns_arr_wide;		//�п��б�
	CGridMap grid_map;			//���洢��
	double getRowLinePos(BOOL bUpToDown,int iRow);		//���ָ�����߾����ʽ����Ӧ��Ե��λ�þ���
	double getColumnLinePos(BOOL bLToR,int iColumn);	//���ָ�����߾����ʽ����Ӧ��Ե��λ�þ���
public:
/*#ifndef __MAPTA_
	HRESULT WriteFormatTbl(IStream *pStream);
	HRESULT ReadFormatTbl(IStream *pStream);
#endif*/
	void CopyFrom(CFormatTbl *pFormatTbl);
	//���Ʊ�񵫲�����iRemoveStart��iRemoveEnd֮����� wht 11-03-13
	void CopyFrom(CFormatTbl *pFormatTbl,int iRemoveStart,int iRemoveEnd);
	BOOL SnapGridPos(double x,double y, f2dPoint *snap_pos,double scope=1.0);
	long GetFormatTblID();
	BOOL GetTblRect(f2dRect *tbl_rect);
	CGrid * SnapGrid(double x,double y,int *iRow=NULL,int *iCol=NULL);
	void SelectAll();
	int ReleaseSnapStatus();
	int GetSelectedCounts();
	CGrid *GetFirstSelectedGrid(CFormatTbl **pFormatTbl=NULL,int *pos_i=NULL,int *pos_j=NULL);
	void Destroy();			//���ٱ��,�ͷ��ڴ�
	int GetColumnsCount();	//��ñ��������
	int GetRowsCount();		//��ñ��������
	BOOL CreateCells(int iRow,int iColumn);						//��ʼ��������ȱʡָ�����������
	long GetID(){return ID;}
	BOOL IsValidGridIndex(int iRow=1,int iColumn=1);		//���������ű�ʶ��Ч���ж�
	CGrid* GetGridAt(int iRow,int iColumn);					//�������кŻ��ָ����Ԫ��ָ��
	BOOL GetGridRect(int iRow,int iColumn,f2dRect &rect);	//���ָ����Ԫ��λ�þ���
	BOOL GetGridSize(int iRow,int iColumn,					//���ָ����Ԫ���С(�߿�)
			double *height=NULL, double *wide=NULL);
	BOOL GetGridRange(int &start_i,int &start_j,int *end_i, int *end_j);		//���ָ����Ԫ���С(�߿�)
			
	//iRow��iColumn������ڵ���0���ֱ�С�ڹ������������������
	BOOL SetGrid(int iRow, int iColumn,char *string,BOOL bForceToString=TRUE);	//��䵥Ԫ������
	BOOL SetRowHigh(int iRow,double high);					//�趨ָ���и�
	BOOL SetColumnWide(int iColumn,double wide);			//�趨ָ���п�
	BOOL GetGridString(int iRow, int iColumn,char *string);	//���ָ����Ԫ��������ַ���
	BOOL GetRowHigh(int iRow,double *high);					//���ָ���и�
	BOOL GetColumnWide(int iColumn,double *wide);			//���ָ���п�
	BOOL InsertRow(int iRow,double high=0);
	BOOL DelRow(int iRow);
	BOOL InsertColumn(int iColumn,double wide=0);
	BOOL DelColumn(int iColumn);

	//iRow��iColumn������ڵ���0���ֱ�С�ڵ��ڹ������������������
	BOOL GetRowLineStart(int iRow,f2dPoint &start);			//���ָ�����ߵ���ʼλ��
	BOOL GetRowLineEnd(int iRow,f2dPoint &end);				//���ָ�����ߵ���ֹλ��
	BOOL GetColumnLineStart(int iColumn,f2dPoint &start);	//���ָ�����ߵ���ʼλ��
	BOOL GetColumnLineEnd(int iColumn,f2dPoint &end);		//���ָ�����ߵ���ֹλ��

	void SetBasePos(f2dPoint &pos);						//�趨��λ���ά����
	void GetBasePos(f2dPoint &pos);						//��ø�ʽ��Ķ�λ���ά����
	void SetLocateStyle(TBL_LOCATE_STYLE eStyle);		//�趨���λ��ʽ
	TBL_LOCATE_STYLE GetLocateStyle();					//��ø�ʽ��Ķ�λ��ʽ
	double GetTableWide();								//������Ÿ�ʽ����ܿ�
	double GetTableHeight();							//������Ÿ�ʽ����ܸ�
	BOOL Import(char *filename);						//���ı��ļ������ʽ��
	BOOL Export(char *filename);						//���ı��ļ�������ʽ��
	BOOL Import(FILE *fp);						//���ı��ļ������ʽ��
	BOOL Export(FILE *fp);						//���ı��ļ�������ʽ��
};

struct TBL_LINE{
	POINT startPt,endPt;
	long feature;
	TBL_LINE(){feature=0;}
};

class CFlexTbl  
{
	int iCursor;
	long curFormatTblID;
	long m_nFormatTblCounts;
public:
	CFlexTbl();
	virtual ~CFlexTbl();
	void RemoveAll();
	CArray<CFormatTbl*,CFormatTbl*&>flex_tbl;
public:
	BOOL Import(char *filename);						//���ı��ļ��������Ա�
	BOOL Export(char *filename);						//���ı��ļ��������Ա�
	void CopyFrom(CFlexTbl *pFlexTbl);
	BOOL SnapGridPos(double x,double y, long nExcludeFormatTblID,
		f2dPoint *snap_pos,double scope=1.0);
	CGrid * SnapGrid(double x,double y,int *iRow=NULL,int *iCol=NULL,long *nFormatTblID=NULL);
	int ReleaseSnapStatus();
	int GetSelectedCounts();
	CGrid *GetFirstSelectedGrid(CFormatTbl **pFormatTbl=NULL,int *pos_i=NULL,int *pos_j=NULL);
	CFormatTbl *GetFirst();
	CFormatTbl *GetNext();
	CFormatTbl *GetFormatTblByID(long ID);
	long AddFormatTbl(f2dPoint base_pos,TBL_LOCATE_STYLE mode,int nRow, int nColumn);
	CFormatTbl *AddEmptyFormatTbl();
	BOOL DelFormatTbl(long ID);
	long GetFormatTblCount();
	BOOL CreateTblFrame(CFormatTbl* pFormatTbl,ATOM_LIST<f2dLine>* pTblLineList);
	CFormatTbl *SnapFormatTbl(double x, double y);

};

#endif