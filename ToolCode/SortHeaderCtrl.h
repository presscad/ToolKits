#ifndef SORTHEADERCTRL_H
#define SORTHEADERCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSortHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	int m_R;    
	int m_G;    
	int m_B;    
	int m_Gradient; // �����屳��������ϵ��    
	double m_heightCoef;  //��ͷ�߶ȣ����Ǳ���,    
	int m_fontHeight; //����߶�    
	int m_fontWidth;   //������    
	COLORREF m_colorText;
	CStringArray m_titleArr;    
	CString m_alignmentFormat; //��ʾ�������͵���������,0��ʾ����룬1��ʾ�м���룬2��ʾ�Ҷ���
	BOOL m_bEnableSort;
	BOOL m_bCustomDrawTitle;
public:
	CSortHeaderCtrl();

// Attributes
public:

// Operations
public:
	int GetCurrSortColumn(){return m_iSortColumn;}
	BOOL IsSortCurrColAscending(){return m_bSortAscending;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortHeaderCtrl)
	public:
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortHeaderCtrl();

	void SetSortArrow( const int iColumn, const BOOL bAscending );

	// Generated message map functions
protected:
	afx_msg void OnPaint();    
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	LRESULT OnLayout( WPARAM wParam, LPARAM lParam );
	int m_iSortColumn;
	BOOL m_bSortAscending;

	//{{AFX_MSG(CSortHeaderCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // SORTHEADERCTRL_H
