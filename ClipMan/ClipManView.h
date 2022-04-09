
// ClipManView.h : interface of the CClipManView class
//

#if _MSC_VER > 1000
#pragma once
#include "ClipManDoc.h"
#endif // _MSC_VER > 1000

class CClipManView : public CEditView
{
protected: // create from serialization only
	CClipManView();
	DECLARE_DYNCREATE(CClipManView)

// Attributes
public:
	CClipManDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CClipManView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ClipManView.cpp
inline CClipManDoc* CClipManView::GetDocument() const
   { return reinterpret_cast<CClipManDoc*>(m_pDocument); }
#endif

