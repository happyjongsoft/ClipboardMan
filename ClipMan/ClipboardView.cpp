// ClipboardView.cpp : implementation file
//

#include "stdafx.h"
#include "ClipMan.h"
#include "ClipboardView.h"
#include "MainFrm.h"
#include "DrawWrapper.h"
#include "ClipManDoc.h"

// CClipboardView

IMPLEMENT_DYNCREATE(CClipboardView, CEditView)

CClipboardView::CClipboardView()
{

}

CClipboardView::~CClipboardView()
{
}


BEGIN_MESSAGE_MAP(CClipboardView, CEditView)

	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CClipboardView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
	DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
	UINT nID, CCreateContext* pContext)
{
	// standard Window creation
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle,
		rect, pParentWnd, nID, pContext))
		return FALSE;
		
	return TRUE;
}

// CClipboardView diagnostics

#ifdef _DEBUG
void CClipboardView::AssertValid() const
{
	CEditView::AssertValid();
}

#ifndef _WIN32_WCE
void CClipboardView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif
#endif //_DEBUG


///////////////////////////////////////////////////////////////////////////////
// Display a clipboard buffer

CSize CClipboardView::DrawClipboard(CDC* pDC, CRect pos, BOOL strech)
{
	// Get selected object
	CClipObject* clipObject = ((CClipManDoc*)GetDocument())->getCurrentClipObject();
	if (clipObject == NULL)
		return CSize(pos.right, pos.bottom);
		
	return clipObject->drawObject(pDC, pos, strech);
}

void CClipboardView::OnPaint()
{
	CClipObject* clipObject = ((CClipManDoc*)GetDocument())->getCurrentClipObject();
	if (clipObject == NULL)
	{
		CEditView::OnPaint();
		return;
	}
	
	if (clipObject->getFormat() == "Image")
	{
		CPaintDC dc(this);
		CRect pos;
		GetClientRect(pos);
		CSize sz = DrawClipboard(&dc, pos, FALSE);
	}
	else
	{
		CEditView::OnPaint();
	}
}

void CClipboardView::updatePreview()
{
	// Set as READONLY
	GetEditCtrl().SetReadOnly(true);

	// Draw clipobject
	CClipObject* clipObject = ((CClipManDoc*)GetDocument())->getCurrentClipObject();
	if (clipObject == NULL)
		SetWindowText("");		
	else
		SetWindowText(clipObject->getText());
	// Will be called OnPaint()
}
