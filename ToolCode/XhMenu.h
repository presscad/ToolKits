#pragma once
#ifndef __XHMENU_H_
#define __XHMENU_H_ 

#include "afxwin.h"

class CXhMenuData 
{ 
public: 
	CXhMenuData () { menuIconNormal = -1; menuIconSelected = -1; menuIconDisabled = -1;nID=0;}; 
	char menuText[32]; 
	UINT menuIconNormal; 
	UINT menuIconSelected; 
	UINT menuIconDisabled; 
	UINT nID; 
}; 

typedef enum {Normal,TextOnly} HIGHLIGHTSTYLE; 
class CXhMenu : public CMenu	 
{ 
	// Construction 
public: 
	CXhMenu();  
	// Attributes 
protected: 
	CTypedPtrArray<CPtrArray, CXhMenuData*> m_MenuList;	 
	//�˵������ 
	CTypedPtrArray<CPtrArray, CXhMenu*>	m_SubMenus;	 
	//�˵����� 
public:  
	// ClassWizard generated virtual function overrides 
	//{{AFX_VIRTUAL() 
	//}}AFX_VIRTUAL  
	// Implementation 
public: 
	virtual ~CXhMenu();	// Virtual Destructor  
	// Drawing:  
	virtual void DrawItem( LPDRAWITEMSTRUCT);	 
	//�Ի��ƺ��� 
	virtual void MeasureItem( LPMEASUREITEMSTRUCT ); 
	//�˵����С���� 

	void SetTextColor (COLORREF ); 
	//�����ı���ɫ 
	void SetBackColor (COLORREF); 
	//���ò˵����� 
	void SetHighlightColor (COLORREF);	 
	//���ø���ɫ�� 
	void SetIconSize (int, int);	 
	//����ͼ���С 
	void SetHighlightStyle (HIGHLIGHTSTYLE ); 
	//���ø������ 
	void SetHighlightTextColor (COLORREF);	 
	//���ø���״̬���ı�ɫ�� 

	virtual BOOL AppendODMenu(LPCTSTR lpstrText,  
		UINT nFlags = MF_OWNERDRAW, 
		UINT nID = 0, 
		UINT nIconNormal = -1,  
		UINT nIconSelected = -1, 
		UINT nIconDisabled = -1);	 
	//��Ӳ˵��� 
	virtual BOOL ModifyODMenu(LPCTSTR lpstrText, 
		UINT	nID = 0, 
		UINT	nIconNormal = -1, 
		UINT	nIconSelected = -1, 
		UINT	nIconDisabled = -1);	 
	//���Ĳ˵��� 
	virtual BOOL LoadMenu(LPCTSTR lpszResourceName); 
	//����˵� 
	virtual BOOL LoadMenu(int nResource); 
	virtual BOOL DestroyMenu(); 

	// Generated message map functions 
protected: 
	COLORREF m_crText; 
	COLORREF m_clrBack; 
	COLORREF m_clrText; 
	COLORREF m_clrHilight; 
	COLORREF m_clrHilightText; 
	LOGFONT m_lf; 
	CFont m_fontMenu; 
	UINT m_iMenuHeight; 
	BOOL m_bLBtnDown; 
	CBrush m_brBackground,m_brSelect; 
	CPen m_penBack; 
	int m_iconX,m_iconY; 
	HIGHLIGHTSTYLE m_hilightStyle;  
	//{{AFX_MSG(CXhMenu) 
	// NOTE - the ClassWizard will add and remove member functions here. 
	//}}AFX_MSG 
};  
/////////////////////////////////////////////////////////////////////////// 
//  
//{{AFX_INSERT_LOCATION}} 
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.  
#endif // __XHMENU_H_ 