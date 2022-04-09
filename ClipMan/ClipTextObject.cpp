#include "stdafx.h"
#include "ClipTextObject.h"

IMPLEMENT_SERIAL(CClipTextObject, CClipObject, 1)

CClipTextObject::CClipTextObject()
{
	m_format = CLIP_TYPE_TEXT;
	m_title = m_format + "-" + m_time;
	m_text = "";
}

CClipTextObject::~CClipTextObject()
{
}

bool CClipTextObject::setData(HGLOBAL hglb)
{
	long len = long(::GlobalSize(hglb));

	LPVOID data = new LPVOID[len];
	memcpy(data, GlobalLock(hglb), len);

	GlobalUnlock(hglb);

	m_text = CString((LPTSTR)(data));
	delete data;
	return true;
}

CSize CClipTextObject::drawObject(CDC* pDC, CRect pos, BOOL strech)
{
	pos.DeflateRect(5, 5);

	LPTSTR pBuf = (LPTSTR)(m_text.GetBuffer());
	
	int len = strlen(pBuf);
	PutBigText(pDC, pBuf, getLength(pBuf, len), &pos, 0x810/*CF_TEXT*/);

	return CSize(pos.right + 5, pos.bottom + 5);
}

void CClipTextObject::Serialize(CArchive& archive)								// Function for Save and Open
{
	CObject::Serialize(archive);

	if (archive.IsStoring())
	{
		// Put static data
		archive << m_format << m_time << m_title << m_text;		

		// Put array data
		int cnt = m_customIds.GetCount();
		archive << cnt;

		POSITION pos;
		for (pos = m_customIds.GetHeadPosition(); pos != NULL;)
		{
			CString s = m_customIds.GetNext(pos);
			archive << s;
		}
	}
	else
	{
		// Get static data
		archive >> m_format >> m_time >> m_title >> m_text;	

		// Get array data
		int cnt;
		CString s;

		archive >> cnt;
		for (int i = 0; i < cnt; i++)
		{
			archive >> s;
			m_customIds.AddTail(s);
		}
	}
	
}

CString CClipTextObject::getText()
{
	CString s = m_text;
	s.Replace("\n", "\r\n");
	return s;
}

bool CClipTextObject::copyToClipboard()
{
	const char* output = (LPTSTR)(m_text.GetBuffer());
	const size_t len = strlen(output) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), output, len);
	GlobalUnlock(hMem);

	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();

	return true;
}

CString CClipTextObject::getContents()
{
	return m_text;
}