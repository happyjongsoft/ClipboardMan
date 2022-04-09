
// ClipManDoc.cpp : implementation of the CClipManDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ClipMan.h"
#endif

#include "ClipManDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CClipManDoc

IMPLEMENT_DYNCREATE(CClipManDoc, CDocument)

BEGIN_MESSAGE_MAP(CClipManDoc, CDocument)
END_MESSAGE_MAP()


// CClipManDoc construction/destruction

CClipManDoc::CClipManDoc()
{
	// TODO: add one-time construction code here
	m_type = CLIP_TYPE_ALL;
	m_curClipObject = NULL;
	m_DragClipObject = NULL;

	try{
		CFile fClip("data\\ClipHistory", CFile::modeRead);			// Open Clip Objects
		CArchive arClip(&fClip, CArchive::load);
		m_lstClipObject.Serialize(arClip);
		arClip.Close();
		fClip.Close();

		CFile fType("data\\ClipCategory", CFile::modeRead);			// Open Clip Type Objects
		CArchive arType(&fType, CArchive::load);
		m_lstTypeObject.Serialize(arType);
		arType.Close();
		fType.Close();

		setCategory(m_type);
	}
	catch (CException* e)
	{

	}
}

CClipManDoc::~CClipManDoc()
{
}

BOOL CClipManDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


// CClipManDoc serialization

void CClipManDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CClipManDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CClipManDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CClipManDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CClipManDoc diagnostics

#ifdef _DEBUG
void CClipManDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CClipManDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CClipManDoc commands
bool CClipManDoc::putClipObject(CClipObject* obj)
{
	if (this == NULL)
		return false;

	m_lstClipObject.InsertAt(0, obj);
	if (m_type == "All" || obj->getFormat() == m_type)
	{
		m_lstClipFilteredObject.InsertAt(0, obj);
		m_curClipObject = obj;
	}else
		m_curClipObject = NULL;

	return true;
}

CTypedPtrArray<CObArray, CClipObject*>& CClipManDoc::getClipList()
{
	return m_lstClipObject;
}

bool CClipManDoc::putTypeObject(CClipTypeObject* obj)
{
	if (this == NULL)
		return false;

	m_lstTypeObject.Add(obj);

	return true;
}

CTypedPtrArray<CObArray, CClipTypeObject*>& CClipManDoc::getTypeList()
{
	return m_lstTypeObject;
}


CTypedPtrArray<CObArray, CClipObject*>& CClipManDoc::getClipFilteredList()
{
	return m_lstClipFilteredObject;
}

CClipObject* CClipManDoc::getCurrentClipObject()
{
	return m_curClipObject;
}

void CClipManDoc::setCurrentClipObject(CClipObject* obj)
{
	m_curClipObject = obj;
}

void CClipManDoc::setCategory(CString type)
{
	m_type = type;

	m_lstClipFilteredObject.RemoveAll();

	for (int i = m_lstClipObject.GetCount() - 1; i >= 0; i--)
	{
		CClipObject* obj = m_lstClipObject.GetAt(i);

		if (type.GetLength() < 20)
		{
			// Check default clip type(Text|Image|File)
			if (obj->getFormat() == m_type || m_type == CLIP_TYPE_ALL){
				m_lstClipFilteredObject.InsertAt(0, obj);
				m_curClipObject = obj;
			}
		}
		else
		{
			// Check custom parent Ids(pid's length is 20)
			if (obj->isChildOfCustomCategory(type)){
				m_lstClipFilteredObject.InsertAt(0, obj);
				m_curClipObject = obj;
			}
		}
	}
}

void CClipManDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	try{
		CFile fClip("data\\ClipHistory", CFile::modeWrite | CFile::modeCreate);
		CArchive arClip(&fClip, CArchive::store);
		m_lstClipObject.Serialize(arClip);
		arClip.Close();
		fClip.Close();

		CFile fType("data\\ClipCategory", CFile::modeWrite | CFile::modeCreate);
		CArchive arType(&fType, CArchive::store);
		m_lstTypeObject.Serialize(arType);
		arType.Close();
		fType.Close();

		int i = 0;
		while (i < m_lstClipObject.GetSize())
		{
			delete m_lstClipObject.GetAt(i++);
		}
		m_lstClipObject.RemoveAll();
		m_lstClipFilteredObject.RemoveAll();

		i = 0;
		while (i < m_lstTypeObject.GetSize())
		{
			delete m_lstTypeObject.GetAt(i++);
		}
		m_lstTypeObject.RemoveAll();

		CDocument::OnCloseDocument();

	}
	catch (CException* e)
	{

	}
}
