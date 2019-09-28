// ListViewComparer.h: interface for the CListViewComparer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTVIEWCOMPARER_H__DEA88421_796C_41D4_9436_5FCA43C5C239__INCLUDED_)
#define AFX_LISTVIEWCOMPARER_H__DEA88421_796C_41D4_9436_5FCA43C5C239__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum COL_SORT_TYPE{
	SORTTYPE_STRING = 0,	//���ַ�������
	SORTTYPE_PARTNO = 1,	//����������
	SORTTYPE_SPEC = 2,		//���������
	SORTTYPE_INTEGER = 3,	//����������
	SORTTYPE_HEX_NUM = 4,	//��ʮ������������
};


//�����б������
class CListViewComparer{
	int m_nMaxColumn;
public:
	int (CALLBACK* FireCompareItemFunc)(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	CListCtrl *m_pListCtrl;
	int m_iColumn;
	BOOL *sortOrderAscending;
	COL_SORT_TYPE *sortTypeArr;	//��¼ÿһ�ж�Ӧ���������ͣ�Ĭ�ϰ��ַ�������
	CListViewComparer();
	int SetMaxColumnCount(int max_column);
	void SetColumnSortType(int iCol,COL_SORT_TYPE sortType);
	COL_SORT_TYPE GetColumnSortType(int iCol);
	virtual ~CListViewComparer();
};
int CALLBACK CompareItemProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

#endif // !defined(AFX_LISTVIEWCOMPARER_H__DEA88421_796C_41D4_9436_5FCA43C5C239__INCLUDED_)
