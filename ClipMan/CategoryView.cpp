// CategoryView.cpp : implementation file
//

#include "stdafx.h"
#include "ClipMan.h"
#include "CategoryView.h"
#include "ClipManDoc.h"
#include "MainFrm.h"
#include "InputBox.h"

// CCategoryView

IMPLEMENT_DYNCREATE(CCategoryView, CTreeView)

CCategoryView::CCategoryView()
{

}

CCategoryView::~CCategoryView()
{
}

void CCategoryView::OnInitialUpdate()
{
	m_hActiveItem = NULL;

	// Get controller of CTreeView
	CTreeCtrl &treeCtrl = this->GetTreeCtrl();

	// Setup image list for setting icons
	CImageList imageList;
	imageList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 5, 1);

	CBitmap bm_clipboard, bm_image, bm_text, bm_file, bm_new;	
	bm_clipboard.LoadBitmap(IDB_CLIPBOARD);
	bm_text.LoadBitmap(IDB_TEXT);
	bm_image.LoadBitmap(IDB_IMAGE);
	bm_file.LoadBitmap(IDB_FILE);
	bm_new.LoadBitmap(IDB_NEW);
	
	imageList.Add(&bm_clipboard, RGB(0, 0, 0));
	imageList.Add(&bm_text, RGB(0, 0, 0));	
	imageList.Add(&bm_image, RGB(0, 0, 0));
	imageList.Add(&bm_file, RGB(0, 0, 0));
	imageList.Add(&bm_new, RGB(0, 0, 0));

	bm_clipboard.DeleteObject();
	bm_text.DeleteObject();
	bm_image.DeleteObject();
	bm_file.DeleteObject();
	bm_new.DeleteObject();

	// Set up Image list
	treeCtrl.SetImageList(&imageList, TVSIL_NORMAL);

	// Initialize all category items
	HTREEITEM hClipRoot, hClipText, hClipImage, hClipFile, hClipCustom;
	hClipRoot = treeCtrl.InsertItem("All Clips", 0, 0, TVI_ROOT);
	hClipText = treeCtrl.InsertItem("Text", 1, 1, hClipRoot);
	hClipImage = treeCtrl.InsertItem("Image", 2, 2, hClipRoot);
	hClipFile = treeCtrl.InsertItem("File", 3, 3, hClipRoot);

	hClipCustom = treeCtrl.InsertItem("Custom", 0, 0, TVI_ROOT);
	
	treeCtrl.SetItemData(hClipRoot, (DWORD)"All");
	treeCtrl.SetItemData(hClipText, (DWORD)"Text");
	treeCtrl.SetItemData(hClipImage, (DWORD)"Image");
	treeCtrl.SetItemData(hClipFile, (DWORD)"File");

	treeCtrl.SetItemData(hClipCustom, (DWORD)"%Custom%%Custom%%Custom%");

	// Expand root-item of treeview
	treeCtrl.Expand(hClipRoot, TVGN_NEXTVISIBLE);

	// Create custom categories.
	CTypedPtrArray<CObArray, CClipTypeObject*> &typeLst = ((CClipManDoc*)GetDocument())->getTypeList();
	for (int i = 0; i < typeLst.GetCount(); i++)
	{
		CString id = typeLst.GetAt(i)->getId();
		CString pid = typeLst.GetAt(i)->getParentId();
		CString name = typeLst.GetAt(i)->getName();

		HTREEITEM hti = getTreeItemByData(treeCtrl.GetRootItem(), pid);
		if (hti != NULL)
		{
			HTREEITEM newItem = treeCtrl.InsertItem(name, 0, 0, hti);

			CStringA strAId(id);
			treeCtrl.SetItemData(newItem, (DWORD)((const char*)strAId));

			treeCtrl.Expand(hti, TVGN_NEXTVISIBLE);
		}
	}
}

BEGIN_MESSAGE_MAP(CCategoryView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CCategoryView::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CCategoryView::OnNMRClick)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ADDFOLDER, &CCategoryView::OnAddfolder)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CCategoryView diagnostics

#ifdef _DEBUG
void CCategoryView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CCategoryView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCategoryView message handlers

HTREEITEM CCategoryView::getTreeItemByData(HTREEITEM parent, CString data)
{
	CTreeCtrl &treeCtrl = this->GetTreeCtrl();
	HTREEITEM hti;

	hti = parent;
	{
		
		do{
			CString pid = CString((char*)(treeCtrl.GetItemData(hti)));
			if (pid == data)
				return hti;
			
			if (treeCtrl.ItemHasChildren(hti))
			{
				HTREEITEM t_hti = getTreeItemByData(treeCtrl.GetChildItem(hti), data);
				if (t_hti != NULL)
					return t_hti;

			}
		} while (hti = treeCtrl.GetNextItem(hti, TVGN_NEXT));
	}
	return NULL;
}

void CCategoryView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	CTreeCtrl &treeCtrl = this->GetTreeCtrl();
	HTREEITEM item  = treeCtrl.GetSelectedItem();

	CString selData = CString((char*)(treeCtrl.GetItemData(item)));
	
	((CClipManDoc*)GetDocument())->setCategory(selData);
	::SendMessage(((CMainFrame*)AfxGetMainWnd())->m_hWnd, WM_CATEGORY_CHANGED, 0, 0);

	*pResult = 0;
}


void CCategoryView::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	// Send WM_CONTEXTMENU to self

	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());

	*pResult = 0;
}


void CCategoryView::OnContextMenu(CWnd* pWnd, CPoint ptMousePos)
{
	// TODO: Add your message handler code here
	// if Shift-F10
	if (ptMousePos.x == -1 && ptMousePos.y == -1)
		ptMousePos = (CPoint)GetMessagePos();

	ScreenToClient(&ptMousePos);

	UINT uFlags;
	HTREEITEM htItem;

	CTreeCtrl &treeCtrl = this->GetTreeCtrl();
	htItem = treeCtrl.HitTest(ptMousePos, &uFlags);
	
	if (htItem == NULL)
		return;

	m_hActiveItem = htItem;

	CMenu menu;
	CMenu* pPopup;

	// the font popup is stored in a resource
	menu.LoadMenu(IDR_TREE_CONTEXT_MENU);
	pPopup = menu.GetSubMenu(0);
	ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, this);

}


void CCategoryView::OnAddfolder()
{
	if (m_hActiveItem == NULL)
		return;
		
	// TODO: Add your command handler code here
	CInputBox dialog;
	
	if (dialog.DoModal() == IDOK) {
		// Do something
		CString folderName = dialog.getInputValue();
		if (folderName.IsEmpty())
			return;

		// Get controller of CTreeView
		CTreeCtrl &treeCtrl = this->GetTreeCtrl();

		CString pid = CString((char*)(treeCtrl.GetItemData(m_hActiveItem)));
		CClipTypeObject *typeObj = new CClipTypeObject();
		typeObj->setName(folderName);
		typeObj->setParentId(pid);

		if (((CClipManDoc*)GetDocument())->putTypeObject(typeObj))
		{
			HTREEITEM newItem;
			newItem = treeCtrl.InsertItem(folderName, 0, 0, m_hActiveItem);

			CStringA strAId(typeObj->getId());

			treeCtrl.SetItemData(newItem, (DWORD)((const char*)strAId));
			treeCtrl.Expand(m_hActiveItem, TVGN_NEXTVISIBLE);
		}
		else
				delete typeObj;

		m_hActiveItem = NULL;
	}

}


void CCategoryView::OnMouseMove(UINT nFlags, CPoint ptMousePos)
{
	// TODO: Add your message handler code here and/or call default

	CClipObject* draggingObj = ((CClipManDoc*)GetDocument())->getDragClipObject();
	if (draggingObj != NULL)
	{
		if (ptMousePos.x == -1 && ptMousePos.y == -1)
			ptMousePos = (CPoint)GetMessagePos();

		UINT uFlags;
		HTREEITEM htItem;

		CTreeCtrl &treeCtrl = this->GetTreeCtrl();
		htItem = treeCtrl.HitTest(ptMousePos, &uFlags);

		if (htItem != NULL)
		{
			CString pid = CString((char*)(treeCtrl.GetItemData(htItem)));
			CString name = treeCtrl.GetItemText(htItem);

			draggingObj->putCustomId(pid);
		}
		((CClipManDoc*)GetDocument())->setDragClipObject(NULL);
	}

	CTreeView::OnMouseMove(nFlags, ptMousePos);
}
