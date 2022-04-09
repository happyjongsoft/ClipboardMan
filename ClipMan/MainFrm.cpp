
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ClipMan.h"
#include "MainFrm.h"
#include "ClipManView.h"
#include "CategoryView.h"
#include "ClipListView.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CHANGECBCHAIN()
	ON_WM_DRAWCLIPBOARD()
	ON_MESSAGE(WM_CLIPITEM_CHANGED, &CMainFrame::OnClipitemChanged)
	ON_MESSAGE(WM_CATEGORY_CHANGED, &CMainFrame::OnCategoryChanged)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bInitSplitter = false;

	const static UINT auPriorityList[NBPRIORITY] = { 0,// reserved for the last used
		CF_OWNERDISPLAY, CF_TEXT, CF_UNICODETEXT,
		CF_BITMAP, CF_DIB, CF_ENHMETAFILE, CF_HDROP };

	for (int i = 0; i < NBPRIORITY; i++) 
		m_uFormat[i] = auPriorityList[i];

	m_hWndChain = NULL;
	m_sound = MAKEINTRESOURCE(IDR_NEWMSG);
	m_cbDataAvailable = 0;
	m_cbFormatsCnt = 0;
	m_oldFormat = UINT_MAX;

	m_selfClipObjectFormat = "";
	// Create a directory to store temporary data
	if (GetFileAttributes("data") == INVALID_FILE_ATTRIBUTES) {
		CreateDirectory("data", NULL);
	}
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// Split main frame as 3 parts
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)/* ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME)*/)
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	
	GetAllClipboardFormats();
	m_hWndChain = SetClipboardViewer();
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// Clear the style FWS_ADDTOTITLE
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
	// Remove menu
	cs.hMenu = NULL; 
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

// CMainFrame message handlers

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	//Calculate client size
	CRect cr;
	GetWindowRect(&cr);
	
	// Create the main splitter with 2 row and 1 columns
	if (!m_wholeSplitter.CreateStatic(this, 2, 1))
		return FALSE;

	if (!m_topSplitter.CreateStatic(&m_wholeSplitter, 1, 2, WS_CHILD | WS_VISIBLE,
		m_wholeSplitter.IdFromRowCol(0, 0)))
		return FALSE;

	if (!m_topSplitter.CreateView(0, 0, RUNTIME_CLASS(CCategoryView),
		CSize(cr.Width() * 0.3, cr.Height() / 2), pContext))
		return FALSE;

	if (!m_topSplitter.CreateView(0, 1, RUNTIME_CLASS(CClipListView),
		CSize(cr.Width() * 0.7, cr.Height() / 2), pContext))
		return FALSE;
		
	if (!m_wholeSplitter.CreateView(1, 0, RUNTIME_CLASS(CClipboardView),
		CSize(cr.Width(), 0), pContext))
		return FALSE;

	// Initialize Preview
	((CClipboardView*)(m_wholeSplitter.GetPane(1, 0)))->updatePreview();
	
	m_bInitSplitter = true;

	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect cr;
	GetWindowRect(&cr);

	if (m_bInitSplitter && nType != SIZE_MINIMIZED)
	{
		m_wholeSplitter.SetRowInfo(0, cr.Height() / 2, 50);
		m_wholeSplitter.SetColumnInfo(0, cx, 0);
		
		m_topSplitter.SetRowInfo(0, cr.Height() / 2, 50);
		m_topSplitter.SetColumnInfo(0, cr.Width() * 0.3, 50);
		m_wholeSplitter.RecalcLayout();
	}
}

LPCSTR CMainFrame::GetClipboardFormatNames(UINT format)
{
	static TCHAR szFormatName[80];

	if (format == 0) format = m_uFormat[0];

	switch (format) {
	case 0:                return "empty";
	case CF_TEXT:          return "CF_TEXT";
	case CF_BITMAP:        return "CF_BITMAP";
	case CF_METAFILEPICT:  return "CF_METAFILEPICT";
	case CF_SYLK:          return "CF_SYLK";
	case CF_DIF:           return "CF_DIF";
	case CF_TIFF:          return "CF_TIFF";
	case CF_OEMTEXT:       return "CF_OEMTEXT";
	case CF_DIB:           return "CF_DIB";
	case CF_PALETTE:       return "CF_PALETTE";
	case CF_PENDATA:       return "CF_PENDATA";
	case CF_RIFF:          return "CF_RIFF";
	case CF_WAVE:          return "CF_WAVE";
	case CF_UNICODETEXT:   return "CF_UNICODETEXT";
	case CF_ENHMETAFILE:   return "CF_ENHMETAFILE";
	case CF_HDROP:         return "CF_HDROP";
	case CF_LOCALE:        return "CF_LOCALE";
	case CF_MAX:           return "CF_MAX";

	case CF_OWNERDISPLAY:  return "CF_OWNERDISPLAY";
	case CF_DSPTEXT:       return "CF_DSPTEXT";
	case CF_DSPBITMAP:     return "CF_DSPBITMAP";
	case CF_DSPMETAFILEPICT:return "CF_DSPMETAFILEPICT";
	case CF_DSPENHMETAFILE:return "CF_DSPENHMETAFILE";
		
	default:
		if (CF_PRIVATEFIRST == (format & 0xFFFFFF00)) {
			wsprintf(szFormatName, _T("CF_PRIVATE_%02X"), format);
			return (LPCSTR)szFormatName;
		}
		else if (CF_GDIOBJFIRST == (format & 0xFFFFFF00)) {
			wsprintf(szFormatName, _T("CF_GDIOBJ_%02X"), format);
			return (LPCSTR)szFormatName;
		}
		else if (::GetClipboardFormatName(format, szFormatName,
			sizeof(szFormatName))) {
			return (LPCSTR)szFormatName;
		}
	}
	wsprintf(szFormatName, _T("unknown_%08x"), format);
	return (LPCSTR)szFormatName;
}

UINT CMainFrame::GetClipboardFormat(BOOL calcul)
{
	if (calcul)
		m_uFormat[0] = ::GetPriorityClipboardFormat(m_uFormat, NBPRIORITY);
	return m_uFormat[0];
}

void CMainFrame::SetCbNotAvailable()
{
	m_oldFormat = UINT_MAX;
	// Invalid a current format entry
	for (int i = 0; i < m_cbFormatsCnt; i++) {
		if (m_cbFormats[i] == m_uFormat[0]) {
			m_cbDataAvailable |= (1 << i);
			break;
		}
	}
}

BOOL CMainFrame::IsClipboardDataAvailable()
{
	for (int i = 0; i < m_cbFormatsCnt; i++) {
		if (m_cbFormats[i] == m_uFormat[0])
			return ((m_cbDataAvailable & (1 << i)) == 0);
	}
	return FALSE;
}

HGLOBAL CMainFrame::GetClipboardData()
{
	HGLOBAL hGlobal = NULL;

	if (m_oldFormat != m_uFormat[0]) {
		BeginWaitCursor();
		SetMessageText("Read clipboard data...");
		if (IsClipboardDataAvailable()) {
			// Get a clipboard global data ptr
			if (!(hGlobal = ::GetClipboardData(m_uFormat[0]))) 
				SetCbNotAvailable();
			else 
				m_oldFormat = m_uFormat[0]; // to optimize a same request
		}
		SetMessageText(AFX_IDS_IDLEMESSAGE);
		EndWaitCursor();
	}
	else {
		// this isn't the first request of this format
		if (!(hGlobal = ::GetClipboardData(m_uFormat[0]))) SetCbNotAvailable();
	}

	return hGlobal;
}

void CMainFrame::GetAllClipboardFormats()
{
	UINT i = 0, cnt = ::CountClipboardFormats();

	if (cnt > 32) cnt = 32;    // only 32 entries max
	m_cbDataAvailable = 0;     // all available entries
	m_oldFormat = UINT_MAX;    // force to check

	// Reads available clipboard formats and fills m_cbFormats table
	if (OpenClipboard()) {
		UINT uFormat = 0;

		while ((uFormat = ::EnumClipboardFormats(uFormat)) && (i < cnt))
			m_cbFormats[i++] = uFormat;

		CloseClipboard();
	}
	m_cbFormatsCnt = i;

	// Computs a new format to display
	GetClipboardFormat(TRUE);
}

void CMainFrame::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter)
{
	CFrameWnd::OnChangeCbChain(hWndRemove, hWndAfter);

	// TODO: Add your message handler code here
	TRACE("CMainFrame::OnChangeCbChain %08x %08x\n", hWndRemove, hWndAfter);
	if (m_hWndChain) ::SendMessage(m_hWndChain, WM_CHANGECBCHAIN,
		(WPARAM)hWndRemove, (LPARAM)hWndAfter);
}

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// TODO: Add your message handler code here

	ChangeClipboardChain(m_hWndChain);
	m_hWndChain = NULL;
}

void CMainFrame::OnDrawClipboard()
{
	CFrameWnd::OnDrawClipboard();

	// TODO: Add your message handler code here

	// Send this draw message to the next Viewer
	if (m_hWndChain)
		::SendMessage(m_hWndChain, WM_DRAWCLIPBOARD, 0, 0);

	// Update available clipboard formats
	GetAllClipboardFormats();

	UINT format = GetClipboardFormat();
	CString fName(GetClipboardFormatNames());
	HGLOBAL hglb = NULL;

	if (OpenClipboard()) {
		// Read a clipboard data and display it
		if ((hglb = GetClipboardData())) {
			CClipObject* clipObject;
			if (format == CF_TEXT)
			{
				clipObject = new CClipTextObject();
			}
			else if (format == CF_BITMAP)
			{
				clipObject = new CClipImageObject();
			}
			else if (format == CF_HDROP)
			{
				clipObject = new CClipFileObject();
			}
			else
			{
				CloseClipboard();
				return;
			}
						
			// Put clip object to clip-list
			if (clipObject->getFormat() != m_selfClipObjectFormat)
			{
				if (m_sound) ::PlaySound(m_sound, AfxGetResourceHandle(), SND_RESOURCE);
				clipObject->setData(hglb);
				if (((CClipManDoc*)GetActiveDocument())->putClipObject(clipObject))
					::SendMessage(m_topSplitter.GetPane(0, 1)->m_hWnd, WM_DRAWCLIPBOARD, 0, 0);
				else
				{
					clipObject->removeTempData();
					delete clipObject;
				}
			}
			else
				delete clipObject;

			m_selfClipObjectFormat = "";
		}

		CloseClipboard();
	}
}

afx_msg LRESULT CMainFrame::OnClipitemChanged(WPARAM wParam, LPARAM lParam)
{
	((CClipboardView*)(m_wholeSplitter.GetPane(1, 0)))->updatePreview();
	
	// Copy data to clipboard
	CClipObject* obj = ((CClipManDoc*)GetActiveDocument())->getCurrentClipObject();
	if (obj != NULL)
	{
		m_selfClipObjectFormat = obj->getFormat();
		obj->copyToClipboard();
	}
	
	return 0;
}

afx_msg LRESULT CMainFrame::OnCategoryChanged(WPARAM wParam, LPARAM lParam)
{
	// Send message to listview
	::SendMessage(m_topSplitter.GetPane(0, 1)->m_hWnd, WM_REFRESH_CLIP_LIST, 0, 0);

	return 0;
}
