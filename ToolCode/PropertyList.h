#if !defined(AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_)
#define AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyList.h : header file
//

#define PIT_STATIC		0	//PIT = property item type
#define PIT_COMBO		1
#define PIT_EDIT		2
#define PIT_ITEMSET		3	//һ��ͨ��������ѡ����еĲ���������ѡ������һ��ѡ�ֵ
#define PIT_BUTTON		4
#define PIT_GRAYBUTTON	5	//�����Ļ�ɫ��Ŀ��ť��
#define PIT_CHECKBOX	6	//��ѡ�����ڽ���������������ֵ�����Ե���ʾ wht 11-04-27

#define BDT_FILEPATH	0	//BDT = button display type
#define BDT_COMMON		1
#define BDT_COMMONEDIT	2
#define BDT_POPMENU		3
#define BDT_POPMENUEDIT	4

#define CDT_EDIT		0	//CDT = combbox display type
#define CDT_LIST		1
#define CDT_COLOR		2

#define IDC_PROPCMBBOX		712
#define IDC_PROPEDITBOX		713
#define IDC_PROPBTNCTRL		714
#define IDC_PROPCHKBOX		715

#define ID_FIRST_POPMENU        WM_USER+101
/////////////////////////////////////////////////////////////////////////////
#include <afxtempl.h>
#include "ColorSelectComboBox.h"

//Feel free to modify this class to what ever info you want in the ListCtrl
class CItemInfo
{
	int m_nCheck;		//��PIT_CHECKBOX��ʹ�øñ��� wht 11-04-27
public:
	static const BYTE TYPE_STATIC		=0;	//PIT = property item type
	static const BYTE TYPE_COMBO		=1;
	static const BYTE TYPE_EDIT		=2;
	static const BYTE TYPE_ITEMSET	=3;	//һ��ͨ��������ѡ����еĲ���������ѡ������һ��ѡ�ֵ
	static const BYTE TYPE_BUTTON		=4;
	static const BYTE TYPE_GRAYBUTTON	=5;	//�����Ļ�ɫ��Ŀ��ť��
	static const BYTE TYPE_CHECKBOX	=6;	//��ѡ�����ڽ���������������ֵ�����Ե���ʾ wht 11-04-27
public:
	CItemInfo(){
		m_controlType=PIT_STATIC;
		m_buttonType=BDT_COMMON;
		m_cmbType=CDT_LIST;
		m_strPropValue = _T("");
		m_bMutiObjsProp=FALSE;
		m_sButtonName=_T("...");
		m_nCheck=0;
	}
	CString m_strPropName;
	CString m_strPropValue;
	CString m_strPropHelp;
	BOOL m_bMutiObjsProp;
	int m_controlType;	
	int m_buttonType;	//0-file path, 1-big button, 2-common button  //wht 08-04-10
	int m_cmbType;		//0��ʾCBS_DROPDOWN 1:��ʾCBS_DROPDOWNLIST 2:��ʾCorlorSelComboBox
	CString m_cmbItems;	//helper function used if a control needs to initalize it self with a list(combobox,listbox)
	CString m_sButtonName;	//��ť����Ĭ��Ϊ"..." //wht 11-04-13
	
	//0.δѡ�� 1.ѡ�� 2.��ȷ����״̬��������ť����BS_3STATE��BS_AUTO3STATE���ʱ��Ч wht 11-04-27
	void SetCheck(int nCheck) {m_nCheck=nCheck;}	//���ø�ѡ��״̬
	int GetCheck() {return m_nCheck;}				//�õ���ѡ��״̬ 
};
class CPropertyList;
DWORD GetBinaryWord(int base0Index);
class CPropTreeItem : public CObject
{
	CObList m_listChild;
	BOOL m_bHideSelf;
	BOOL m_bReadOnly;
public:
	//Ĭ�ϵ�ǰ���������ֻ��״̬
	static BOOL m_bDefaultReadOnlyState;
	CPropTreeItem();
	~CPropTreeItem();
public:
	BOOL m_bHideChildren;  
	DWORD m_dwPropGroup;	//������������
	CPropTreeItem* m_pParent;
	CItemInfo* m_lpNodeInfo;
	long m_idProp;
	void SetReadOnly(BOOL bTrue=TRUE){m_bReadOnly=bTrue;}
	int GetTailIndex();
	BOOL IsHideSelf(){return m_bHideSelf;}
	BOOL IsReadOnly(){return m_bReadOnly;}
	int m_iIndex; //CListBox index
	int m_nIndent; 
	friend class CPropertyList;
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyList window

class CPropertyList : public CListBox
{
// Construction
	BOOL m_bLocked;		//����������
	WORD m_wInteralLockRef;	//����m_bLocked�ⲿ���ܽ��в�������ʸ��ģ����Ӵ�ֻ���ڲ��Զ��������������һ������ wjh-2014.9.22
	BOOL m_bEnableTextChangeEnvent;	//���ڲ�����SetWindowTextʱ�����ÿؼ����ݸ����¼�
public:
	int m_iPropGroup;	//��ǰ��ʾ�����Թ�������
	CPropertyList();
	CPropTreeItem root;
// Attributes
public:
	BOOL (*FireCheckBoxClick)(CPropertyList* pPropList,CPropTreeItem* pItem);
	BOOL (*FireButtonClick)(CPropertyList* pPropList,CPropTreeItem* pItem);
	BOOL (*FireValueModify)(CPropertyList* pPropList,CPropTreeItem* pItem,CString&);
	BOOL (*FireStatusModify)(CPropertyList* pPropList,CPropTreeItem* pItem);
	BOOL (*FireHelpPrompt)(CPropertyList* pPropList,CPropTreeItem* pItem);
	BOOL (*FirePopMenuClick)(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu);
	BOOL (*FirePickColor)(CPropertyList* pPropList,CPropTreeItem* pItem,COLORREF &clr);
	void SetCheckBoxClickFunc(BOOL (*func)(CPropertyList* pPropList,CPropTreeItem* pItem)){FireCheckBoxClick = func;}
	void SetButtonClickFunc(BOOL (*func)(CPropertyList* pPropList,CPropTreeItem* pItem)){FireButtonClick = func;}
	void SetModifyValueFunc(BOOL (*func)(CPropertyList* pPropList,CPropTreeItem* pItem,CString &valueStr)){FireValueModify = func;}
	void SetModifyStatusFunc(BOOL (*func)(CPropertyList* pPropList,CPropTreeItem* pItem)){FireStatusModify = func;}
	void SetPropHelpPromptFunc(BOOL (*func)(CPropertyList* pPropList,CPropTreeItem* pItem)){FireHelpPrompt = func;}
	void SetPopMenuClickFunc(BOOL (*func)(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)){FirePopMenuClick = func;}
	void SetPickColorFunc(BOOL (*func)(CPropertyList* pPropList,CPropTreeItem* pItem,COLORREF &clr)){FirePickColor=func;}
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyList)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	
// Implementation
public:
	virtual ~CPropertyList();
	BOOL Lock(){
		if(m_bLocked)
			return FALSE;
		else
			m_bLocked = TRUE;
		return TRUE;
	}
	BOOL IsLocked(){return m_wInteralLockRef>0||m_bLocked;}
	void Unlock(){m_bLocked = FALSE;}
public:	//wjh�¼�
	CToolTipCtrl *m_ToolTips;	//������ʾ
	CPropTreeItem* GetRootItem(void){return &root;}
	void InternalUpdateTreeItemIndex();
	//Insert item and return new parent node.
	//the bUpdate is here for performance reasons, when you insert say 100 node each having 10 children(1000 listview items)
	//the bUpdate should be set to FALSE(default) but when you want to insert an item, and you want to user to see it right away
	//set bUpdate to TRUE.(see the use of bUpdate in the HowToInsertItemsAfterTheGridHasBeenInitialized function in the CMySuperGridCtrl)
	CPropTreeItem* InsertItem(CPropTreeItem *pParent, CItemInfo* lpInfo, int iInsertIndex, BOOL bUpdate=0);
	//walk all over the place setting the hide/show flag of the nodes.
	//it also deletes items from the listviewctrl.
	void HideChildren(CPropTreeItem *pItem,int nItem);
	//collapse all children from pItem
	void Collapse(CPropTreeItem *pItem);
	//expand one folder and return the last index of the expanded folder
	int Expand(CPropTreeItem* pSelItem, int nIndex);
	void CleanPropTreeItem(CPropTreeItem *pItem);
private:
	void DrawTreeItemSymbol(CDC* pDC, CPropTreeItem* pSelItem, int nListItem, const CRect& rcBounds);
	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchange();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDblclk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
protected:
	afx_msg void OnKillfocusCmbBox();
	afx_msg void OnKillfocusEditBox();
	afx_msg void OnKillfocusBtn();
	afx_msg void OnSelchangeCmbBox();
	afx_msg void OnChangeEditBox();
	afx_msg void OnButton();
	afx_msg void OnCheckBox();
	afx_msg void OnClickPopMenu(UINT nID);

	DECLARE_MESSAGE_MAP()

	//see if user clicked the [+] [-] sign, also handles LVS_EX_CHECKBOXES.
	BOOL HitTestOnSign(CPoint point, LVHITTESTINFO& ht);
	void InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo);
	void DisplayButton(CRect region, CPropTreeItem* pItem);
	void DisplayCheckBox(CRect region, CPropTreeItem *pItem);	//��ʾCheckBox wht 11-04-27
	void DisplayPopMenu(CPropTreeItem* pItem);
	void InternalAppendItem(CPropTreeItem *pParent, CPropTreeItem *pItem);
	BOOL m_bPropValueModified;
	CPropTreeItem* InternalFindItemByPropId(long prop_id,CPropTreeItem *pItem,CPropTreeItem **ppParentItem);
	BOOL DeletePropTreeItem(CPropTreeItem *pItem, CPropTreeItem *pParentItem);
public:
	void SetDividerScale(double fScale) { m_fDividerScale=fScale; }
	BOOL SetItemReadOnly(long prop_id,BOOL bReadOnly=TRUE);
	void SetAllSonItemsReadOnly(CPropTreeItem *pParentItem,BOOL bReadOnly=TRUE);
	void SetPromptString(CString sPrompt);
	void* m_pObj;
	void HideInputCtrl();
	BOOL DeleteItemByPropId(long prop_id);
	void DeleteAllSonItems(CPropTreeItem *pParentItem);
	CPropTreeItem* FindItemByPropId(long prop_id,CPropTreeItem **ppParentItem);
	BOOL SetItemPropHelpStr(long prop_id, const char *sFormatStr,...);
	BOOL SetItemPropValue(long prop_id, const char *sFormatStr,...);
	BOOL SetItemPropValue(long prop_id, CString &sValueText);
	BOOL GetItemPropValue(long prop_id, CString &sValueText);
	BOOL SetItemPropName(long prop_id, CString &sPropName);
	BOOL SetItemRedrawState(long prop_id);
	void FireToolTip(CPoint point);
	void CleanTree();
	void CleanCallBackFunc();
	void Redraw();
	void SelectItem(int index,BOOL bDisplayCtrl=FALSE);
	CPropTreeItem* GetSelectedItem();
	CPropTreeItem* m_pCurEditItem;
	int m_nObjClassTypeId;
	HWND m_hPromptWnd;
	BOOL m_bEscKeyDown;
	//CComboBox m_cmbBox1,m_cmbBox2;
	CColorSelectComboBox m_cmbBox1,m_cmbBox2,m_cmbBox3;
	CEdit m_editBox;
	CButton m_btnCtrl,m_chkBtnCtrl;
	CMenu m_popMenu;
	CFont m_SSerif8Font;	
	int m_curSel,m_prevSel;
	int m_nDivTop;
	int m_nDivBtm;
	int m_nOldDivX;
	int m_nLastBox;
	BOOL m_bTracking;
	BOOL m_bDivIsSet;
	HCURSOR m_hCursorArrow;
	HCURSOR m_hCursorSize;
private:
	int m_nDivider;
	double m_fDividerScale;	//�ָ�����������Ʒָ��ߵ�λ��,Ĭ��ֵΪ0.6 wht 10-12-02
	BOOL m_bEnterKeyDown;	//���»س�������OnSelchangeʱ��Ӧ����OnButton() wht 11-07-16
	void HideEditBox();	
	void HideCmbBox();
};

//���ڵ�ǰ���������п�������ʱҪ���м�������
class CLockPropertyList
{
	CPropertyList *m_pPropList;
public:
	CLockPropertyList(CPropertyList *pPropList)
	{
		m_pPropList=pPropList;
		pPropList->Lock();
	}
	~CLockPropertyList()
	{
		m_pPropList->Unlock();
	}
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_)
