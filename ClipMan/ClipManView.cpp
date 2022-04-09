
// ClipManView.cpp : implementation of the CClipManView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ClipMan.h"
#endif

#include "ClipManDoc.h"
#include "ClipManView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClipManView

IMPLEMENT_DYNCREATE(CClipManView, CEditView)

BEGIN_MESSAGE_MAP(CClipManView, CEditView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CClipManView construction/destruction

CClipManView::CClipManView()
{
	// TODO: add construction code here

}

CClipManView::~CClipManView()
{
}

BOOL CClipManView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CClipManView drawing

void CClipManView::OnDraw(CDC* /*pDC*/)
{
	CClipManDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CClipManView printing

BOOL CClipManView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CClipManView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CClipManView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing



}


// CClipManView diagnostics

#ifdef _DEBUG
void CClipManView::AssertValid() const
{
	CView::AssertValid();
}

void CClipManView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CClipManDoc* CClipManView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClipManDoc)));
	return (CClipManDoc*)m_pDocument;
}
#endif //_DEBUG


// CClipManView message handlers
