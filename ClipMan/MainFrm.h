
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ClipboardView.h"

#define WM_CLIPITEM_CHANGED (WM_APP + 1)
#define WM_CATEGORY_CHANGED (WM_APP + 2)
#define WM_REFRESH_CLIP_LIST (WM_APP + 3)

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CSplitterWnd m_wholeSplitter;
	CSplitterWnd m_topSplitter;
	BOOL m_bInitSplitter;

// Operations
public:
	void GetAllClipboardFormats();
	void SetCbNotAvailable();
	BOOL IsClipboardDataAvailable();
	UINT GetClipboardFormat(BOOL calcul = FALSE);
	LPCSTR GetClipboardFormatNames(UINT format = 0);
	HGLOBAL GetClipboardData();
	void CBLink();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	
	// clipboard formats
#define NBPRIORITY   8                 // nbr format codes priority
	UINT        m_uFormat[NBPRIORITY];  // priority format table
	UINT        m_cbFormats[32];        // available formats
	int         m_cbFormatsCnt;         // available formats count
	DWORD       m_cbDataAvailable;      // available data formats flags
	UINT        m_oldFormat;            // last format request

	HWND        m_hWndChain;            // next clipboard Viewer

	LPCTSTR     m_sound;                // sound resource id

	CString		m_selfClipObjectFormat;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnDestroy();
	afx_msg void OnDrawClipboard();
protected:
	afx_msg LRESULT OnClipitemChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCategoryChanged(WPARAM wParam, LPARAM lParam);
};


