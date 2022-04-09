// ClipListView.cpp : implementation file
//

#include "stdafx.h"
#include "ClipObject.h"
#include "ClipMan.h"
#include "ClipListView.h"
#include "MainFrm.h"
#include "ClipManDoc.h"
#include "CategoryView.h"

#include <ctime>

SortOrder geOrder = SORT_None;
int       gnSortClm = -1;
int CALLBACK MyCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

// CClipListView

IMPLEMENT_DYNCREATE(CClipListView, CListView)

CClipListView::CClipListView()
{
	// Initialize member variables
	m_isInitialized = FALSE;
	m_bDragging = FALSE;
}

CClipListView::~CClipListView()
{
	for (int i = 0; i < m_imageList.GetImageCount(); i++)
	{
		m_imageList.Remove(i);
	}
	m_imageList.DeleteImageList();
}

void CClipListView::OnInitialUpdate()
{
	// Get controller of view
	CListCtrl &listCtrl = GetListCtrl();

	// Enable Header-Draggable
	// listCtrl.SetExtendedStyle(listCtrl.GetExtendedStyle() | LVS_EX_HEADERDRAGDROP);

	// Create image list to show icons
	m_imageList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 4, 1);

	CBitmap bm_clipboard, bm_image, bm_text, bm_file;
	bm_clipboard.LoadBitmap(IDB_CLIPBOARD);
	bm_text.LoadBitmap(IDB_TEXT); 
	bm_image.LoadBitmap(IDB_IMAGE);
	bm_file.LoadBitmap(IDB_FILE);

	m_imageList.Add(&bm_clipboard, RGB(0, 0, 0));
	m_imageList.Add(&bm_text, RGB(0, 0, 0));
	m_imageList.Add(&bm_image, RGB(0, 0, 0));
	m_imageList.Add(&bm_file, RGB(0, 0, 0));

	bm_clipboard.DeleteObject();
	bm_text.DeleteObject();
	bm_image.DeleteObject();
	bm_file.DeleteObject();

	// Setup icons to list controller
	listCtrl.SetImageList(&m_imageList, LVSIL_SMALL);

	// Create header
	listCtrl.SetExtendedStyle(listCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	DWORD dwStyle = ::GetWindowLong(listCtrl.GetSafeHwnd(), GWL_STYLE);

	if ((dwStyle & LVS_TYPEMASK) != LVS_REPORT)
	{
		::SetWindowLong(listCtrl.GetSafeHwnd(), GWL_STYLE, (dwStyle /*& ~LVS_TYPEMASK*/) | LVS_REPORT);
	}

		// Create Header
	int nColumn = 0;
	listCtrl.InsertColumn(nColumn++, "Title", LVCFMT_LEFT, 200);
	listCtrl.InsertColumn(nColumn++, "Content", LVCFMT_LEFT, 350);
	listCtrl.InsertColumn(nColumn++, "Date/Time", LVCFMT_LEFT, 200);

	// Marked as INITIALIZED
	m_isInitialized = TRUE;
}

BEGIN_MESSAGE_MAP(CClipListView, CListView)
	ON_WM_DRAWCLIPBOARD()
	ON_NOTIFY_REFLECT(NM_CLICK, &CClipListView::OnNMClick)
	ON_MESSAGE(WM_REFRESH_CLIP_LIST, &CClipListView::OnRefreshClipList)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &CClipListView::OnLvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CClipListView::OnLvnColumnclick)
END_MESSAGE_MAP()


// CClipListView diagnostics

#ifdef _DEBUG
void CClipListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CClipListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG

// Message handler of DRAWCLIPBOARD
void CClipListView::OnDrawClipboard()
{
	CListView::OnDrawClipboard();
	
	// Check if initialized this view
	if (!m_isInitialized)
		return;
	
	// Get controller of view
	CListCtrl &listCtrl = GetListCtrl();

	int index;
	CClipObject* clipObject = ((CClipManDoc*)GetDocument())->getCurrentClipObject();
	
	if (clipObject == NULL)
		return;

	CString format = clipObject->getFormat();

	// Create row
	if (format == ((CClipManDoc*)GetDocument())->getCategory() || ((CClipManDoc*)GetDocument())->getCategory() == CLIP_TYPE_ALL)
	{
		index = listCtrl.InsertItem(LVIF_TEXT | LVIF_IMAGE, 0, clipObject->getTitle(), 0, 0, clipObject->getIconIndex(), NULL);
		listCtrl.SetItem(index, 1, LVIF_TEXT, clipObject->getContents(), 0, 0, 0, NULL);
		listCtrl.SetItem(index, 2, LVIF_TEXT, clipObject->getTime(), 0, 0, 0, NULL);
		
		for (int i = 0; i < listCtrl.GetItemCount(); i++)
		{
			listCtrl.SetItemState(i, ~LVIS_SELECTED, LVIS_SELECTED);
		}

		listCtrl.SetItemState(index, LVIS_FOCUSED, LVIS_FOCUSED);
		listCtrl.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);

		listCtrl.SetItemData(index, (LPARAM)(clipObject));

		((CClipManDoc*)GetDocument())->setCurrentClipObject(clipObject);

		GetListCtrl().SortItems(MyCompareFunc, 0);

		// Send message to MainFrame
		::SendMessage(((CMainFrame*)AfxGetMainWnd())->m_hWnd, WM_CLIPITEM_CHANGED, 0, 0);
	}	
}

void CClipListView::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	LPNMITEMACTIVATE pitem = (LPNMITEMACTIVATE)pNMHDR;

	// Get selected row-index of listview
	int rowIndex = pitem->iItem;	
	if (rowIndex == -1)
		return;
	
	CClipObject* obj = (CClipObject*)(GetListCtrl().GetItemData(rowIndex));

	((CClipManDoc*)GetDocument())->setCurrentClipObject(obj);
	// Send message to MainFrame
	::SendMessage(((CMainFrame*)AfxGetMainWnd())->m_hWnd, WM_CLIPITEM_CHANGED, 0, 0);
	*pResult = 0;
}

afx_msg LRESULT CClipListView::OnRefreshClipList(WPARAM wParam, LPARAM lParam)
{
	// Get list controller
	CListCtrl &listCtrl = GetListCtrl();

	// Delete all items
	listCtrl.DeleteAllItems();
	
	if (((CClipManDoc*)GetDocument())->getClipFilteredList().GetCount() == 0)
	{
		((CClipManDoc*)GetDocument())->setCurrentClipObject(NULL);
		// Send message to MainFrame
		::SendMessage(((CMainFrame*)AfxGetMainWnd())->m_hWnd, WM_CLIPITEM_CHANGED, 0, 0);
		return 0;
	}

	int index = -1;

	for (int i = ((CClipManDoc*)GetDocument())->getClipFilteredList().GetCount() -1 ; i >= 0; i--)
	{
		CClipObject* obj = ((CClipManDoc*)GetDocument())->getClipFilteredList().GetAt(i);
		index = listCtrl.InsertItem(LVIF_TEXT | LVIF_IMAGE, 0, obj->getTitle(), 0, 0, obj->getIconIndex(), NULL);

		// Select first item
		listCtrl.SetItem(index, 1, LVIF_TEXT, obj->getContents(), 0, 0, 0, NULL);
		listCtrl.SetItem(index, 2, LVIF_TEXT, obj->getTime(), 0, 0, 0, NULL);
		
		listCtrl.SetItemData(index, (LPARAM)(obj));
	}

	listCtrl.SetItemState(index, LVIS_FOCUSED, LVIS_FOCUSED);
	listCtrl.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
	// Send message to MainFrame
	::SendMessage(((CMainFrame*)AfxGetMainWnd())->m_hWnd, WM_CLIPITEM_CHANGED, 0, 0);

	return 0;
}

void CClipListView::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	// Start of dragging
	// Get list controller
	CListCtrl &listCtrl = GetListCtrl();

	//RECORD THE INDEX OF THE ITEM BEIGN DRAGGED AS m_nDragIndex
	m_nDragIndex = ((NM_LISTVIEW *)pNMHDR)->iItem;

	//CREATE A DRAG IMAGE FROM THE CENTER POINT OF THE ITEM IMAGE
	POINT pt;
	pt.x = 8;
	pt.y = 8;
	m_pDragImage = listCtrl.CreateDragImage(m_nDragIndex, &pt);
	m_pDragImage->BeginDrag(0, CPoint(8, 8));
	m_pDragImage->DragEnter(
		GetDesktopWindow(), ((NM_LISTVIEW *)pNMHDR)->ptAction);
	

	//SET THE FLAGS INDICATING A DRAG IN PROGRESS
	m_bDragging = TRUE;
	m_nDropIndex = -1;
	m_pDropWnd = &listCtrl;

	//CAPTURE ALL MOUSE MESSAGES IN CASE THE USER DRAGS OUTSIDE OF THE VIEW
	SetCapture();
}

void CClipListView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// show image While dragging
	if (m_bDragging)
	{
		m_ptDropPoint = point;
		ClientToScreen(&m_ptDropPoint);

		//MOVE THE DRAG IMAGE
		m_pDragImage->DragMove(m_ptDropPoint);

		//TEMPORARILY UNLOCK WINDOW UPDATES
		m_pDragImage->DragShowNolock(FALSE);

		//CONVERT THE DROP POINT TO CLIENT CO-ORDIANTES
		m_pDropWnd = WindowFromPoint(m_ptDropPoint);
		m_pDropWnd->ScreenToClient(&m_ptDropPoint);

		//LOCK WINDOW UPDATES
		m_pDragImage->DragShowNolock(TRUE);
	}
	CListView::OnMouseMove(nFlags, point);
}

void CClipListView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// End of drop
	if (m_bDragging) 
	{
		//RELEASE THE MOUSE CAPTURE AND END THE DRAGGING
		::ReleaseCapture();
		m_bDragging = FALSE;
		m_pDragImage->DragLeave(GetDesktopWindow());
		m_pDragImage->EndDrag();

		//GET THE WINDOW UNDER THE DROP POINT
		CPoint pt(point);
		ClientToScreen(&pt);
		m_pDropWnd = WindowFromPoint(pt);

		//DROP THE ITEM ON THE LIST
		if (m_pDropWnd->IsKindOf(RUNTIME_CLASS(CCategoryView)))
		{
			CClipObject* obj = (CClipObject*)(GetListCtrl().GetItemData(m_nDragIndex));
			((CClipManDoc*)GetDocument())->setDragClipObject(obj);
		}

		for (int i = 0; i < m_pDragImage->GetImageCount(); i++)
		{
			m_pDragImage->Remove(i);
			
		}
		m_pDragImage->DeleteImageList();
		delete m_pDragImage;
		m_pDragImage = NULL;

	}
	CListView::OnLButtonUp(nFlags, point);
}

// Sort list items
void CClipListView::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{	
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	if (geOrder == SORT_None) 
		geOrder = SORT_AZ;
	else
		geOrder = (SortOrder)-geOrder;

	gnSortClm = pNMLV->iSubItem; 

	GetListCtrl().SortItems(MyCompareFunc, 0);
	

	
	HDITEM Item;
	Item.mask = HDI_TEXT;

	Item.pszText = "Title";
	GetListCtrl().GetHeaderCtrl()->SetItem(0, &Item);
	Item.pszText = "Content";
	GetListCtrl().GetHeaderCtrl()->SetItem(1, &Item);
	Item.pszText = "Date/Time";
	GetListCtrl().GetHeaderCtrl()->SetItem(2, &Item);

	if (geOrder == SORT_ZA) {

		switch (gnSortClm) {
		default:
			Item.pszText = "Title     \\/";
			break;
		case  1:
			Item.pszText = "Content     \\/";
			break;
		case  2:
			Item.pszText = "Date/Time     \\/";
			break;
		}
	}
	else{
		switch (gnSortClm) {
		default:
			Item.pszText = "Title     /\\";
			break;
		case  1:
			Item.pszText = "Content     /\\";
			break;
		case  2:
			Item.pszText = "Date/Time     /\\";
			break;
		}
	}

	GetListCtrl().GetHeaderCtrl()->SetItem(gnSortClm, &Item);
	/*
	if (GetListCtrl().GetHeaderCtrl()->GetItem(gnSortClm, &headerInfo))
	{
		headerInfo.mask = HDI_FORMAT;

		if (geOrder)
		{
			//headerInfo.fmt |= HDF_SORTUP;
			//headerInfo.fmt &= ~HDF_SORTDOWN;
			headerInfo.fmt = (headerInfo.fmt & ~HDF_SORTDOWN) | HDF_SORTUP;
		}
		else
		{
			//headerInfo.fmt |= HDF_SORTDOWN;
			//headerInfo.fmt &= ~HDF_SORTUP;
			headerInfo.fmt = (headerInfo.fmt & ~HDF_SORTUP) | HDF_SORTDOWN;
		}

		//bool a;
		//a = Header_SetItem(ListView_GetHeader(m_hWnd), gnSortClm, &headerInfo);
		//GetListCtrl().GetHeaderCtrl()->SetItem(gnSortClm, &headerInfo);
		GetListCtrl().GetHeaderCtrl()->Invalidate();
	}
	GetListCtrl().Invalidate();
	*/
	
	*pResult = 0;
}

// Callback for sort list items
int CALLBACK MyCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int nRet = 0;

	CClipObject* pr1 = (CClipObject*)lParam1;
	CClipObject* pr2 = (CClipObject*)lParam2;
	if (geOrder == SORT_ZA) {
		pr2 = (CClipObject*)lParam1;
		pr1 = (CClipObject*)lParam2;
	}

	if (pr1 == NULL || pr2 == NULL)
		return nRet;

	CString s1, s2;
	switch (gnSortClm) {
	default:
		s1 = pr1->getTitle();
		s2 = pr2->getTitle();
		break;
	case  1:
		s1 = pr1->getContents();
		s2 = pr2->getContents();
		break;
	case  2:
		s1 = pr1->getTime();
		s2 = pr2->getTime();
		break;
	}

	if (s1 > s2) nRet = 1;
	if (s1 < s2) nRet = -1;
	return(nRet);
}

