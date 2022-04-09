#pragma once
// CClipboardView view

class CClipboardView : public CEditView
{
	DECLARE_DYNCREATE(CClipboardView)

protected:
	CClipboardView();           // protected constructor used by dynamic creation
	virtual ~CClipboardView();

	CSize DrawClipboard(CDC *pDC, CRect pos, BOOL strech);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

protected:
	DECLARE_MESSAGE_MAP()
public:
	void updatePreview();
	afx_msg void OnPaint();
};


