// XhTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "XhTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXhTreeCtrl
IMPLEMENT_DYNCREATE(CXhTreeCtrl, CTreeCtrl)

CXhTreeCtrl::CXhTreeCtrl()
{
	m_bDragging = FALSE;
	m_bLMouseDown = FALSE;
	FireIsValidDragS = NULL;
	FireIsValidDragD = NULL;
	FireCompareDataLevel = NULL;
	FireVisitItem = NULL;
	FireInitDragSourceData = NULL;
	DropTreeNodeFunc=NULL;
	m_bSelectPending = FALSE; 
	m_hClickedItem = NULL; 
	m_hFirstSelectedItem = NULL; 
	m_bSelectionComplete = TRUE;
}

CXhTreeCtrl::~CXhTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CXhTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CXhTreeCtrl)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT_EX(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_NOTIFY_REFLECT_EX(NM_SETFOCUS, OnSetfocus)
	ON_NOTIFY_REFLECT_EX(NM_KILLFOCUS, OnKillfocus)
	ON_NOTIFY_REFLECT_EX(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_TIMER() 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXhTreeCtrl message handlers

DROPEFFECT CXhTreeCtrl::OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
{
	return (DROPEFFECT)-1; 
}
DROPEFFECT CXhTreeCtrl::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return OnDragOver(pWnd, pDataObject, dwKeyState, point);
}
DROPEFFECT CXhTreeCtrl::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	if(DragOverFunc)
		return DragOverFunc(this,pDataObject,dwKeyState,point);
	else
		return DROPEFFECT_NONE;
}
BOOL CXhTreeCtrl::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	if(DropFunc)
		return DropFunc(this,pDataObject,dropEffect,point);
	else
		return FALSE;
}

void CXhTreeCtrl::OnDestroy() 
{
	m_dropTarget.Revoke();
	CTreeCtrl::OnDestroy();
}

void CXhTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	//�����������ҷ�����������
	if((GetTickCount()-m_dwDragStart)<DRAG_DELAY )
		return;
	BOOL bLButtonDown = (::GetKeyState(VK_LBUTTON) < 0);
	if(!bLButtonDown)
		return;
	//֧�����ؼ�֮�����ק���� wht 15-08-19
	if(FireInitDragSourceData&&FireInitDragSourceData(this,&m_xDataSource))
		m_xDataSource.DoDragDrop(DROPEFFECT_LINK,CRect(10,10,10,10));
	//���ûص������ж��Ϸ�Դ�Ƿ�Ϸ�,���FireIsValidDragSΪ�գ���Ϊ�����Ϸ�
	//if(FireIsValidDragS==NULL||(FireIsValidDragS&&FireIsValidDragS(this,pNMTreeView->itemNew.hItem)))
	if(FireIsValidDragS&&FireIsValidDragS(this,pNMTreeView->itemNew.hItem))
		m_hItemDragS = pNMTreeView->itemNew.hItem;
	else
		return;
	m_hItemDragD = NULL;
	//�õ������϶�ʱ��ʾ��ͼ���б�
	m_pDragImage = CreateDragImage(m_hItemDragS);
	if( !m_pDragImage )
		return;
	m_bDragging = true;
	m_pDragImage->BeginDrag(0,CPoint(8,8));
	CPoint pt = pNMTreeView->ptDrag;
	ClientToScreen(&pt);
	m_pDragImage->DragEnter (this,pt);  //"this"����ҷ���������ڸô���
	SetCapture();

	m_nScrollTimerID = SetTimer(2,40,NULL);
}

void CXhTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM  hItem;
	UINT       flags;
	//���������ж�ʱ���Ƿ����,���������ɾ��,ɾ�����ٶ�ʱ
	if(m_nHoverTimerID)
	{
		KillTimer(m_nHoverTimerID);
		m_nHoverTimerID = 0;
	}
	m_nHoverTimerID = SetTimer(1,800,NULL);  //��ʱΪ 0.8 �����Զ�չ��
	m_ptHover = point;
	if(m_bDragging && nFlags&MK_LBUTTON)
	{
		CPoint  pt = point;
		CImageList::DragMove(pt);
		//��꾭��ʱ������ʾ
		CImageList::DragShowNolock(FALSE);  //������꾭��ʱ�����ѿ��ĺۼ�
		if((hItem = HitTest(point,&flags))!=NULL)
		{
			SelectDropTarget(hItem);
			m_hItemDragD = hItem;
		}
		CImageList::DragShowNolock(TRUE);
		
		CRect rect;
		GetClientRect(&rect);
		/*//����Ŀ����ҷ�����Եʱ������Ŀ���ڸ���
		if(point.x<rect.left+20)
			m_hItemDragD = NULL;*/
		//����Ŀ����ק������ʱ��Ŀ����ק������ΪԴ��ק�� wht 15-08-20
		//����Ϊ���ڵ㣬���ܳ���ѡ����ܸı������,��δ�ҵ�����ԭ��
		if(!PtInRect(rect,point))
			m_hItemDragD = m_hItemDragS;
	}
	if(m_hClickedItem)
	{
		CSize sizeMoved = m_ptClick-point;
		if(abs(sizeMoved.cx)>GetSystemMetrics(SM_CXDRAG)||abs(sizeMoved.cy)>GetSystemMetrics(SM_CYDRAG))
		{
			m_bSelectPending=FALSE;			
			// Notify parent that he may begin drag operation
			// Since we have taken over OnLButtonDown(), the default handler doesn't
			// do the normal work when clicking an item, so we must provide our own
			// TVN_BEGINDRAG notification for the parent!
			CWnd* pWnd = GetParent();
			if(pWnd && !(GetStyle() & TVS_DISABLEDRAGDROP))
			{
				NM_TREEVIEW tv;
				tv.hdr.hwndFrom = GetSafeHwnd();
				tv.hdr.idFrom = GetWindowLong(GetSafeHwnd(), GWL_ID);
				tv.hdr.code = TVN_BEGINDRAG;
				tv.itemNew.hItem = m_hClickedItem;
				tv.itemNew.state = GetItemState( m_hClickedItem, 0xffffffff );
				tv.itemNew.lParam = GetItemData( m_hClickedItem );
				tv.ptDrag.x = point.x;
				tv.ptDrag.y = point.y;
				pWnd->SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
			}
			m_hClickedItem=NULL;
		}
	}
	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CXhTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonUp(nFlags, point);
	if(m_bSelectPending)
	{
		// A select has been waiting to be performed here
		if(m_hClickedItem!=NULL)
			SelectMultiple( m_hClickedItem, nFlags, point );
		m_bSelectPending=FALSE;
	}
	m_hClickedItem=NULL;
	if(m_bDragging)
	{
		m_bDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();
		delete m_pDragImage;

		//�Ϸ�Դ
		HTREEITEM hItemDragS=GetRootItem();
		while(hItemDragS)
		{
			if(GetItemState(hItemDragS,TVIS_SELECTED)&TVIS_SELECTED)
			{
				if(FireIsValidDragS==NULL||(FireIsValidDragS&&FireIsValidDragS(this,hItemDragS)))
					m_hItemDragS = hItemDragS;
				else
				{
					hItemDragS=GetNextVisibleItem(hItemDragS);
					continue;
				}
			}
			else
			{
				hItemDragS=GetNextVisibleItem(hItemDragS);
				continue;
			}
			hItemDragS=GetNextVisibleItem(hItemDragS);
			SelectDropTarget(NULL);
			if(m_hItemDragS==m_hItemDragD)
			{
				KillTimer(m_nScrollTimerID);
				return;
			}
			//�Ϸ�ԴΪ��
			if(m_hItemDragS==NULL)
				continue;	
			//���ûص������ж��Ϸ�Ŀ��ڵ��Ƿ�Ϸ�
			if(FireIsValidDragD&&!FireIsValidDragD(this,m_hItemDragS,m_hItemDragD))
				return;
			//�����ϷŵĽڵ�Ϊ�գ�ȡ���Ϸ�
			//������Ϸţ��������뵽���ڵ��£��˴��ݲ���Ҫ�����Ϸ�
			if(m_hItemDragD==NULL)
				return;
			if(DropTreeNodeFunc==NULL||DropTreeNodeFunc(this,m_hItemDragS,m_hItemDragD))
			{
				HTREEITEM  hParentItem = m_hItemDragD;
				while((hParentItem=GetParentItem(hParentItem))!=NULL)
				{	//�����ڵ����߼���Ľڵ㸴�Ƶ��ӽڵ���
					if(hParentItem==m_hItemDragS)
					{
						HTREEITEM  hNewTemp = CopyBranch( m_hItemDragS,NULL,TVI_LAST );
						HTREEITEM  hNew = CopyBranch( hNewTemp,m_hItemDragD,TVI_LAST );
						DeleteItem( hNewTemp );
						SelectItem( hNew );
						KillTimer(m_nScrollTimerID);
						continue;
					}
				}
			
				HTREEITEM  hParentItemS = GetParentItem(m_hItemDragS);
				HTREEITEM  hParentItemD = GetParentItem(m_hItemDragD);
				//Դ�ڵ���Ŀ��ڵ�ĸ��ڵ�򸸽ڵ�ĸ��ڵ�Ϊ�ֵܽڵ� �������ͬ
				int iDepthS=0,iDepthD=0;
				hParentItem=hParentItemS;
				while((hParentItem=GetParentItem(hParentItem))!=NULL)
					iDepthS++;
				hParentItem=hParentItemD;
				while((hParentItem=GetParentItem(hParentItem))!=NULL)
					iDepthD++;
				int nRet=0;
				//���ûص������Ƚ�Դ�ڵ���Ŀ��ڵ��������ͼ���
				if(FireCompareDataLevel)
					nRet=FireCompareDataLevel(this,m_hItemDragS,m_hItemDragD);
				if(iDepthS==iDepthD&&nRet<=0)
				{	//�����϶��ڵ��������϶��ڵ�����λ��
					HTREEITEM hNewItem=NULL;
					HTREEITEM hPrevItem=GetPrevSiblingItem(m_hItemDragD);
					if(hPrevItem==NULL)	//���������� ---�����ϷŵĽڵ�Ϊ��һ���ڵ�
						hNewItem=CopyBranch(m_hItemDragS,hParentItemD,TVI_FIRST);
					else if(hPrevItem&&hPrevItem!=m_hItemDragS) //����������
						hNewItem=CopyBranch(m_hItemDragS,hParentItemD,hPrevItem);
					else	//����������
						hNewItem=CopyBranch(m_hItemDragS,hParentItemD,m_hItemDragD);
					DeleteItem(m_hItemDragS);
					SelectItem(hNewItem);
					KillTimer(m_nScrollTimerID);
					continue;
				}
				Expand(m_hItemDragD,TVE_EXPAND);
				HTREEITEM  hNewItem = CopyBranch(m_hItemDragS,m_hItemDragD,TVI_LAST);
				DeleteItem(m_hItemDragS);
				SelectItem(hNewItem);
				KillTimer(m_nScrollTimerID);
			}
		}
	}
}

void CXhTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//����������ҷ
	m_dwDragStart = GetTickCount();
	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = HitTest(point, &nHitFlags);
	// Must invoke label editing explicitly. The base class OnLButtonDown would normally
	// do this, but we can't call it here because of the multiple selection...
	if(!(nFlags&(MK_CONTROL|MK_SHIFT))&&(nHitFlags&TVHT_ONITEMLABEL))
	{
		if(hClickedItem == GetSelectedItem())
		{
			// Clear multple selection before label editing
			//ClearSelection();
			if(GetFocus()!=this)
				SetFocus();
			//SelectItem(hClickedItem);
			//return;
		}
	}
	if(nHitFlags&TVHT_ONITEM)
	{
		if(GetFocus()!=this)
			SetFocus();
		m_hClickedItem = hClickedItem;
		// Is the clicked item already selected ?
		BOOL bIsClickedItemSelected = GetItemState( hClickedItem, TVIS_SELECTED ) & TVIS_SELECTED;
		if (bIsClickedItemSelected)
		{
			// Maybe user wants to drag/drop multiple items!
			// So, wait until OnLButtonUp() to do the selection stuff. 
			m_bSelectPending=TRUE;
		}
		else
		{
			SelectMultiple( hClickedItem, nFlags, point );
			m_bSelectPending=FALSE;
		}
		m_ptClick=point;
	}
	//else
		CTreeCtrl::OnLButtonDown( nFlags, point );
}

//������Ŀ
HTREEITEM CXhTreeCtrl::CopyItem(HTREEITEM hItem, HTREEITEM hNewParent, HTREEITEM hAfter)
{
	CString          sText;
	HTREEITEM        hNewItem;
	TV_INSERTSTRUCT  tvstruct;
	//�õ�Դ��Ŀ����Ϣ
	tvstruct.item.hItem = hItem;
	tvstruct.item.mask  = TVIF_CHILDREN|TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIS_BOLD;
	GetItem(&tvstruct.item);
	sText=GetItemText(hItem);
	tvstruct.item.cchTextMax=sText.GetLength();
	tvstruct.item.pszText   =sText.LockBuffer();
	//����Ŀ���뵽���ʵ�λ��
	tvstruct.hParent = hNewParent;
	tvstruct.hInsertAfter = hAfter;
	tvstruct.item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT;
	hNewItem = InsertItem(&tvstruct);
	sText.ReleaseBuffer ();
	//���ƿ�����Ŀ���ݺ���Ŀ״̬
	SetItemData(hNewItem,GetItemData(hItem));
	SetItemState(hNewItem,GetItemState(hItem,TVIS_STATEIMAGEMASK),TVIS_STATEIMAGEMASK);
	return hNewItem;
}

//������֧
HTREEITEM CXhTreeCtrl::CopyBranch(HTREEITEM hBranch, HTREEITEM hNewParent, HTREEITEM hAfter)
{
	HTREEITEM  hChild;
	HTREEITEM  hNewItem = CopyItem(hBranch,hNewParent,hAfter);
	hChild = GetChildItem(hBranch);

	while(hChild!=NULL)
	{
		CopyBranch(hChild,hNewItem,hAfter);
		hChild = GetNextSiblingItem(hChild);
	}
	return  hNewItem;
}

void CXhTreeCtrl::OnTimer(UINT nIDEvent) 
{
	//������нڵ�
	if( nIDEvent==m_nHoverTimerID )
	{
		KillTimer(m_nHoverTimerID);
		m_nHoverTimerID = 0;
		HTREEITEM  trItem = 0;
		UINT  uFlag = 0;
		trItem = HitTest(m_ptHover,&uFlag);
		if(trItem&&m_bDragging)
		{
			SelectItem(trItem);
			Expand(trItem,TVE_EXPAND);
		}
	}
	//������ҷ�����еĹ�������
	else if(m_bDragging&&nIDEvent==m_nScrollTimerID )
	{
		m_nTimerTicks++;
		CPoint pt;
		GetCursorPos(&pt);
		CRect rect;
		GetClientRect(&rect);
		ClientToScreen(&rect);

		const int MARGIN=2;
		HTREEITEM  hItem = GetFirstVisibleItem();
		if(pt.y<rect.top+MARGIN)
		{	//���Ϲ���
			int slow_scroll=6-(rect.top+MARGIN-pt.y)/20;
			if(0==(m_nTimerTicks%((slow_scroll>0)?slow_scroll:1)))
			{
				CImageList::DragShowNolock(false);
				SendMessage(WM_VSCROLL,SB_LINEUP);
				SelectDropTarget(hItem);
				m_hItemDragD = hItem;
				CImageList::DragShowNolock(true);
			}
		}
		else if(pt.y>rect.bottom-MARGIN)
		{	//���¹���
			int slow_scroll=6-(pt.y-rect.bottom+MARGIN)/20;
			if(0==(m_nTimerTicks%((slow_scroll>0)?slow_scroll:1)))
			{
				CImageList::DragShowNolock(false);
				SendMessage(WM_VSCROLL,SB_LINEDOWN);
				int nCount = GetVisibleCount();
				for(int i=0;i<nCount-1;i++)
					hItem = GetNextVisibleItem(hItem);
				if(hItem)
					SelectDropTarget(hItem);
				m_hItemDragD = hItem;
				CImageList::DragShowNolock(true);
			}
		}
	}
	else
		CTreeCtrl::OnTimer(nIDEvent);
}

//����ָ���ڵ�������ӽڵ�
void CXhTreeCtrl::TraverseItem(HTREEITEM hItem)
{
	HTREEITEM  hChild;
	if(FireVisitItem==NULL||(FireVisitItem&&FireVisitItem(this,hItem)))
		return;
	hChild = GetChildItem(hItem);
	while(hChild!=NULL)
	{
		TraverseItem(hChild);
		hChild = GetNextSiblingItem(hChild);
	}
}

void CXhTreeCtrl::SelectMultiple( HTREEITEM hClickedItem, UINT nFlags, CPoint point )
{
	// Start preparing an NM_TREEVIEW struct to send a notification after selection is done
	NM_TREEVIEW tv;
	memset(&tv.itemOld, 0, sizeof(tv.itemOld));
	CWnd* pWnd = GetParent();
	HTREEITEM hOldItem = GetSelectedItem();
	if (hOldItem)
	{
		tv.itemOld.hItem = hOldItem;
		tv.itemOld.state = GetItemState( hOldItem, 0xffffffff );
		tv.itemOld.lParam = GetItemData( hOldItem );
		tv.itemOld.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;
	}
	// Flag signaling that selection process is NOT complete.
	// (Will prohibit TVN_SELCHANGED from being sent to parent)
	m_bSelectionComplete = FALSE;
	// Action depends on whether the user holds down the Shift or Ctrl key
	if ( nFlags & MK_SHIFT )
	{
		// Select from first selected item to the clicked item
		if ( !m_hFirstSelectedItem )
			m_hFirstSelectedItem = GetSelectedItem();
		SelectItems( m_hFirstSelectedItem, hClickedItem );
	}
	else if ( nFlags & MK_CONTROL )
	{
		// Find which item is currently selected
		HTREEITEM hSelectedItem = GetSelectedItem();
		// Is the clicked item already selected ?
		BOOL bIsClickedItemSelected = GetItemState( hClickedItem, TVIS_SELECTED ) & TVIS_SELECTED;
		BOOL bIsSelectedItemSelected = FALSE;
		if ( hSelectedItem )
			bIsSelectedItemSelected = GetItemState( hSelectedItem, TVIS_SELECTED ) & TVIS_SELECTED;
		// Must synthesize a TVN_SELCHANGING notification
		if (pWnd)
		{
			tv.hdr.hwndFrom = GetSafeHwnd();
			tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );
			tv.hdr.code = TVN_SELCHANGING;
			tv.itemNew.hItem = hClickedItem;
			tv.itemNew.state = GetItemState( hClickedItem, 0xffffffff );
			tv.itemNew.lParam = GetItemData( hClickedItem );
			tv.itemOld.hItem = NULL;
			tv.itemOld.mask = 0;
			tv.action = TVC_BYMOUSE;
			tv.ptDrag.x = point.x;
			tv.ptDrag.y = point.y;
			pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv );
		}
		
		// If the previously selected item was selected, re-select it
		if (bIsSelectedItemSelected)
			SetItemState( hSelectedItem, TVIS_SELECTED, TVIS_SELECTED );
		
		// We want the newly selected item to toggle its selected state,
		// so unselect now if it was already selected before
		if (bIsClickedItemSelected)
			SetItemState(hClickedItem, 0, TVIS_SELECTED);
		else
		{
			SelectItem(hClickedItem);
			SetItemState(hClickedItem, TVIS_SELECTED, TVIS_SELECTED);
		}
		// If the previously selected item was selected, re-select it
		if ( bIsSelectedItemSelected && hSelectedItem != hClickedItem )
			SetItemState( hSelectedItem, TVIS_SELECTED, TVIS_SELECTED );
		// Store as first selected item (if not already stored)
		if ( m_hFirstSelectedItem==NULL )
			m_hFirstSelectedItem = hClickedItem;
	}
	else
	{
		// Clear selection of all "multiple selected" items first
		if(hOldItem==hClickedItem)
			m_hFirstSelectedItem=hClickedItem;
		else
		{
			ClearSelection();
			// Then select the clicked item
			SelectItem( hClickedItem );
			SetItemState( hClickedItem, TVIS_SELECTED, TVIS_SELECTED );
			// Store as first selected item
			m_hFirstSelectedItem = hClickedItem;
		}
	}
	// Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED 
	// notification provided by Windows' treectrl, we must now produce one ourselves,
	// so that our parent gets to know about the change of selection.
	m_bSelectionComplete = TRUE;
	if ( pWnd )
	{
		tv.hdr.hwndFrom = GetSafeHwnd();
		tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );
		tv.hdr.code = TVN_SELCHANGED;
		tv.itemNew.hItem = m_hClickedItem;
		tv.itemNew.state = GetItemState( m_hClickedItem, 0xffffffff );
		tv.itemNew.lParam = GetItemData( m_hClickedItem );
		tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;
		tv.action = TVC_UNKNOWN;
		pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv );
	}
}

void CXhTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd* pWnd = GetParent();
	if (nChar==VK_NEXT || nChar==VK_PRIOR)
	{
		if (!(GetKeyState(VK_SHIFT)&0x8000))
		{
			// User pressed Pg key without holding 'Shift':
			// Clear multiple selection (if multiple) and let base class do 
			// normal selection work!
			if ( GetSelectedCount()>1 )
				ClearSelection( TRUE );
			CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
			m_hFirstSelectedItem = GetSelectedItem();
			return;
		}
		// Flag signaling that selection process is NOT complete.
		// (Will prohibit TVN_SELCHANGED from being sent to parent)
		m_bSelectionComplete = FALSE;
		// Let base class select the item
		CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
		HTREEITEM hSelectedItem = GetSelectedItem();
		// Then select items in between
		SelectItems( m_hFirstSelectedItem, hSelectedItem );
		
		// Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED 
		// notification provided by Windows' treectrl, we must now produce one ourselves,
		// so that our parent gets to know about the change of selection.
		m_bSelectionComplete = TRUE;
		if (pWnd)
		{
			NM_TREEVIEW tv;
			memset(&tv.itemOld, 0, sizeof(tv.itemOld));
			tv.hdr.hwndFrom = GetSafeHwnd();
			tv.hdr.idFrom = GetWindowLong(GetSafeHwnd(), GWL_ID);
			tv.hdr.code = TVN_SELCHANGED;
			tv.itemNew.hItem = hSelectedItem;
			tv.itemNew.state = GetItemState(hSelectedItem, 0xffffffff);
			tv.itemNew.lParam = GetItemData(hSelectedItem);
			tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;
			tv.action = TVC_UNKNOWN;
			pWnd->SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
		}
	}
	else if (nChar==VK_UP||nChar==VK_DOWN)
	{
		// Find which item is currently selected
		HTREEITEM hSelectedItem = GetSelectedItem();
		HTREEITEM hNextItem;
		if (nChar==VK_UP)
			hNextItem = GetPrevVisibleItem(hSelectedItem);
		else
			hNextItem = GetNextVisibleItem(hSelectedItem);
		
		if (!(GetKeyState(VK_SHIFT)&0x8000))
		{
			// User pressed arrow key without holding 'Shift':
			// Clear multiple selection (if multiple) and let base class do 
			// normal selection work!
			if (GetSelectedCount()>1)
				ClearSelection(TRUE);
			
			if (hNextItem)
				CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
			m_hFirstSelectedItem = GetSelectedItem();
			return;
		}
		
		if (hNextItem)
		{
			// Flag signaling that selection process is NOT complete.
			// (Will prohibit TVN_SELCHANGED from being sent to parent)
			m_bSelectionComplete = FALSE;
			// If the next item is already selected, we assume user is
			// "moving back" in the selection, and thus we should clear 
			// selection on the previous one
			BOOL bSelect = !( GetItemState( hNextItem, TVIS_SELECTED ) & TVIS_SELECTED );
			// Select the next item (this will also deselect the previous one!)
			SelectItem( hNextItem );
			// Now, re-select the previously selected item
			if ( bSelect || ( !( GetItemState( hSelectedItem, TVIS_SELECTED ) & TVIS_SELECTED ) ) )
				SelectItems( m_hFirstSelectedItem, hNextItem );
			// Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED 
			// notification provided by Windows' treectrl, we must now produce one ourselves,
			// so that our parent gets to know about the change of selection.
			m_bSelectionComplete = TRUE;
			if (pWnd)
			{
				NM_TREEVIEW tv;
				memset(&tv.itemOld, 0, sizeof(tv.itemOld));
				tv.hdr.hwndFrom = GetSafeHwnd();
				tv.hdr.idFrom = GetWindowLong(GetSafeHwnd(), GWL_ID);
				tv.hdr.code = TVN_SELCHANGED;
				tv.itemNew.hItem = hNextItem;
				tv.itemNew.state = GetItemState(hNextItem, 0xffffffff);
				tv.itemNew.lParam = GetItemData(hNextItem);
				tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;
				tv.action = TVC_UNKNOWN;
				pWnd->SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
			}
		}
		
		// Since the base class' OnKeyDown() isn't called in this case,
		// we must provide our own TVN_KEYDOWN notification to the parent
		CWnd* pWnd = GetParent();
		if (pWnd)
		{
			NMTVKEYDOWN tvk;
			tvk.hdr.hwndFrom = GetSafeHwnd();
			tvk.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );
			tvk.hdr.code = TVN_KEYDOWN;
			tvk.wVKey = nChar;
			tvk.flags = 0;
			pWnd->SendMessage( WM_NOTIFY, tvk.hdr.idFrom, (LPARAM)&tvk );
		}
	}
	else // Behave normally
		CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
}

///////////////////////////////////////////////////////////////////////////////
// Get number of selected items
UINT CXhTreeCtrl::GetSelectedCount() const
{
	// Only visible items should be selected!
	UINT uCount=0;
	for ( HTREEITEM hItem = GetRootItem(); hItem!=NULL; hItem = GetNextVisibleItem( hItem ) )
	{
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			uCount++;
	}	
	return uCount;
}

///////////////////////////////////////////////////////////////////////////////
// Overloaded to catch our own special code
HTREEITEM CXhTreeCtrl::GetNextItem(HTREEITEM hItem, UINT nCode)
{
	if (nCode==TVGN_EX_ALL)
	{
		// This special code lets us iterate through ALL tree items regardless 
		// of their parent/child relationship (very handy)
		HTREEITEM hNextItem;
		// If it has a child node, this will be the next item
		hNextItem = GetChildItem( hItem );
		if (hNextItem)
			return hNextItem;
		// Otherwise, see if it has a next sibling item
		hNextItem = GetNextSiblingItem(hItem);
		if (hNextItem)
			return hNextItem;
		// Finally, look for next sibling to the parent item
		HTREEITEM hParentItem=hItem;
		while (!hNextItem && hParentItem)
		{
			// No more children: Get next sibling to parent
			hParentItem = GetParentItem(hParentItem);
			hNextItem = GetNextSiblingItem(hParentItem);
		}
		return hNextItem; // will return NULL if no more parents
	}
	else
		return CTreeCtrl::GetNextItem(hItem, nCode); // standard processing
}

///////////////////////////////////////////////////////////////////////////////
// Helpers to list out selected items. (Use similar to GetFirstVisibleItem(), 
// GetNextVisibleItem() and GetPrevVisibleItem()!)
HTREEITEM CXhTreeCtrl::GetFirstSelectedItem()
{
	for(HTREEITEM hItem = GetRootItem(); hItem!=NULL; hItem = GetNextVisibleItem(hItem))
	{
		if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
			return hItem;
	}
	return NULL;
}

HTREEITEM CXhTreeCtrl::GetNextSelectedItem(HTREEITEM hItem)
{
	for (hItem = GetNextVisibleItem(hItem); hItem!=NULL; hItem = GetNextVisibleItem(hItem))
	{
		if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
			return hItem;
	}
	return NULL;
}

HTREEITEM CXhTreeCtrl::GetPrevSelectedItem(HTREEITEM hItem)
{
	for(hItem = GetPrevVisibleItem(hItem); hItem!=NULL; hItem = GetPrevVisibleItem(hItem))
	{
		if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
			return hItem;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Select/unselect item without unselecting other items
BOOL CXhTreeCtrl::SelectItemEx(HTREEITEM hItem, BOOL bSelect/*=TRUE*/)
{
	HTREEITEM hSelItem = GetSelectedItem();
	if (hItem==hSelItem)
	{
		if(!bSelect)
		{
			SelectItem(NULL);
			return TRUE;
		}
		return FALSE;
	}
	SelectItem(hItem);
	m_hFirstSelectedItem=hItem;
	// Reselect previous "real" selected item which was unselected byt SelectItem()
	if (hSelItem)
		SetItemState(hSelItem, TVIS_SELECTED, TVIS_SELECTED);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Select visible items between specified 'from' and 'to' item (including these!)
// If the 'to' item is above the 'from' item, it traverses the tree in reverse 
// direction. Selection on other items is cleared!
BOOL CXhTreeCtrl::SelectItems(HTREEITEM hFromItem, HTREEITEM hToItem)
{
	// Determine direction of selection 
	// (see what item comes first in the tree)
	HTREEITEM hItem = GetRootItem();
	while(hItem && hItem!=hFromItem && hItem!=hToItem)
		hItem = GetNextVisibleItem( hItem );
	if (!hItem)
		return FALSE; // Items not visible in tree
	BOOL bReverse = (hItem==hToItem);
	// "Really" select the 'to' item (which will deselect 
	// the previously selected item)
	SelectItem( hToItem );
	// Go through all visible items again and select/unselect
	hItem = GetRootItem();
	BOOL bSelect = FALSE;
	while(hItem)
	{
		if (hItem == (bReverse ? hToItem : hFromItem))
			bSelect = TRUE;
		if (bSelect)
		{
			if (!(GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED))
				SetItemState( hItem, TVIS_SELECTED, TVIS_SELECTED );
		}
		else
		{
			if (GetItemState( hItem, TVIS_SELECTED) & TVIS_SELECTED)
				SetItemState(hItem, 0, TVIS_SELECTED);
		}
		if (hItem == (bReverse ? hFromItem : hToItem))
			bSelect = FALSE;
		hItem = GetNextVisibleItem(hItem);
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Clear selected state on all visible items
void CXhTreeCtrl::ClearSelection(BOOL bMultiOnly/*=FALSE*/)
{
	for (HTREEITEM hItem=GetRootItem(); hItem!=NULL; hItem=GetNextVisibleItem(hItem))
	{
		if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
			SetItemState(hItem, 0, TVIS_SELECTED);
	}
}

///////////////////////////////////////////////////////////////////////////////
// If a node is collapsed, we should clear selections of its child items 
BOOL CXhTreeCtrl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if(pNMTreeView->action == TVE_COLLAPSE)
	{
		HTREEITEM hItem = GetChildItem(pNMTreeView->itemNew.hItem);
		while(hItem)
		{
			if(GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
				SetItemState(hItem, 0, TVIS_SELECTED);
			// Get the next node: First see if current node has a child
			HTREEITEM hNextItem = GetChildItem(hItem);
			if (!hNextItem)
			{
				// No child: Get next sibling item
				if(!(hNextItem = GetNextSiblingItem(hItem)))
				{
					HTREEITEM hParentItem = hItem;
					while(!hNextItem)
					{
						// No more children: Get parent
						if (!(hParentItem = GetParentItem(hParentItem)))
							break;
						// Quit when parent is the collapsed node
						// (Don't do anything to siblings of this)
						if (hParentItem == pNMTreeView->itemNew.hItem)
							break;
						// Get next sibling to parent
						hNextItem = GetNextSiblingItem(hParentItem);
					}
					// Quit when parent is the collapsed node
					if(hParentItem == pNMTreeView->itemNew.hItem)
						break;
				}
			}
			hItem = hNextItem;
		}
	}
	*pResult = 0;
	return FALSE; // Allow parent to handle this notification as well
}

///////////////////////////////////////////////////////////////////////////////
// Intercept TVN_SELCHANGED and pass it only to the parent window of the
// selection process is finished
BOOL CXhTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Return TRUE if selection is not complete. This will prevent the 
	// notification from being sent to parent.
	return !m_bSelectionComplete; 
}


///////////////////////////////////////////////////////////////////////////////
// Ensure the multiple selected items are drawn correctly when loosing/getting
// the focus
BOOL CXhTreeCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Invalidate();
	*pResult = 0;
	return FALSE;
}

BOOL CXhTreeCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Invalidate();
	*pResult = 0;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Retreives a tree ctrl item given the item's data
HTREEITEM CXhTreeCtrl::ItemFromData(DWORD dwData, HTREEITEM hStartAtItem/*=NULL*/) const
{
	// Traverse all items in tree control
	HTREEITEM hItem;
	if(hStartAtItem)
		hItem = hStartAtItem;
	else
		hItem = GetRootItem();
	while(hItem)
	{
		if(dwData==(DWORD)GetItemData(hItem))
			return hItem;
		// Get first child node
		HTREEITEM hNextItem = GetChildItem(hItem);
		if(!hNextItem)
		{
			// Get next sibling child
			hNextItem = GetNextSiblingItem(hItem);
			if (!hNextItem)
			{
				HTREEITEM hParentItem=hItem;
				while(!hNextItem && hParentItem)
				{
					// No more children: Get next sibling to parent
					hParentItem = GetParentItem( hParentItem );
					hNextItem = GetNextSiblingItem( hParentItem );
				}
			}
		}
		hItem = hNextItem;
	}
	return NULL;
}

BOOL CXhTreeCtrl::DeleteAllSonItems(HTREEITEM hParentItem)
{
	if(hParentItem==NULL)
		return FALSE;
	m_bSelectionComplete=FALSE;	//��ΪFALSEɾ����ʱ������TVN_SELCHANGED��Ϣ�������� wht 15-08-20
	HTREEITEM hChildItem=GetChildItem(hParentItem);
	while(hChildItem)
	{
		DeleteItem(hChildItem);
		hChildItem=GetNextItem(hParentItem,TVGN_CHILD);
	}
	m_bSelectionComplete=TRUE;
	return TRUE;
}

BOOL CXhTreeCtrl::DeleteItem(HTREEITEM hItem)
{
	if(m_hClickedItem==hItem)
		m_hClickedItem=NULL;
	return CTreeCtrl::DeleteItem(hItem);
}

BOOL CXhTreeCtrl::DeleteAllItems()
{
	m_hClickedItem=NULL;
	return CTreeCtrl::DeleteAllItems();
}

void CXhTreeCtrl::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)  
{  
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);  
	NMCUSTOMDRAW nmCustomDraw = pNMCD->nmcd;  
	switch(nmCustomDraw.dwDrawStage)  
	{  
	case CDDS_ITEMPREPAINT:  
		{  
			if(nmCustomDraw.uItemState & CDIS_SELECTED)  
			{
				pNMCD->clrTextBk = RGB(51,153,255);  
				pNMCD->clrText = RGB(255,255,255);
			}
		}  
	default:  
		break;  
	}  
	*pResult = 0;  
	*pResult |= CDRF_NOTIFYPOSTPAINT;  
	*pResult |= CDRF_NOTIFYITEMDRAW;  
}