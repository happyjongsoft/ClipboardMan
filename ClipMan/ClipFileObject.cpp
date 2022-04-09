#include "stdafx.h"
#include "ClipFileObject.h"

IMPLEMENT_SERIAL(CClipFileObject, CClipObject, 1)

CClipFileObject::CClipFileObject()
{
	m_format = CLIP_TYPE_FILE;
	m_title = m_format + "-" + m_time;
	m_fileCount = 0;
	m_filePath = "";
}


CClipFileObject::~CClipFileObject()
{
}

bool CClipFileObject::setData(HGLOBAL hglb)
{
	DROPFILES* pDf = (DROPFILES*)GlobalLock(hglb);
	TCHAR c;
	
	// The file contains Unicode characters
	PWORD pW = PWORD(PBYTE(pDf) + pDf->pFiles);

	m_fileCount = 0;
	m_filePath = "";

	// Extract a file names
	while (*pW) {
		while (c = (TCHAR)*pW++) m_filePath += c;
		m_filePath += "\r\n";
		m_fileCount++;
	}

	::GlobalUnlock(hglb);
	return true;
}

CSize CClipFileObject::drawObject(CDC* pDC, CRect pos, BOOL strech)
{
	pos.DeflateRect(5, 5);

	CString s;
	s.Format("%d Files : \n", m_fileCount);

	PutText(pDC, s + m_filePath, &pos);

	return CSize(pos.right + 5, pos.bottom + 5);
}

void CClipFileObject::Serialize(CArchive& archive)								// Function for Save and Open
{
	CObject::Serialize(archive);

	if (archive.IsStoring())
		archive << m_format << m_time << m_title << m_fileCount << m_filePath;			// Save Clips
	else
		archive >> m_format >> m_time >> m_title >> m_fileCount >> m_filePath;			// Open Clips

	if (archive.IsStoring())
	{
		// Put static data
		archive << m_format << m_time << m_title << m_fileCount << m_filePath;

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
		archive >> m_format >> m_time >> m_title >> m_fileCount >> m_filePath;

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

CString CClipFileObject::getText()
{
	CString s;
	s.Format("%d Files : \r\n", m_fileCount);
	return s + m_filePath;
}

bool CClipFileObject::copyToClipboard()
{
	
	int i = 0;
	CStringArray saItems;
	int size = 0;
	for (CString sItem = m_filePath.Tokenize("\r\n", i); i >= 0; sItem = m_filePath.Tokenize("\r\n", i))
	{
		saItems.Add(sItem);
		size += sItem.GetLength() + 1;
	}
	size++;

	size_t ssize = (size_t)(sizeof(DROPFILES)+size * sizeof(WORD));

	HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, ssize);

	PWORD pData = new WORD[size];
	int index = 0;
	for (i = 0; i < saItems.GetCount(); i++)
	{
		CString sItem = saItems.GetAt(i);
		for (int j = 0; j < sItem.GetLength(); j++)
		{
			*(pData+index) = (WORD)sItem.GetAt(j);
			index++;
		}

		*(pData+index) = (WORD)'\0';
		index++;
	}
	*(pData+index) = (WORD)'\0';

	DROPFILES* pDropFiles = (DROPFILES*) ::GlobalLock(hMem);
	pDropFiles->pFiles = sizeof(DROPFILES);
	pDropFiles->fWide = true;

	PWORD pW = PWORD(PBYTE(pDropFiles) + pDropFiles->pFiles);

	CopyMemory(pW, pData, size * sizeof(WORD));
	GlobalUnlock(hMem);
	
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_HDROP, hMem);
	CloseClipboard();
	

	return true;
}

CString CClipFileObject::getContents()
{
	CString s;
	s.Format("%d Files", m_fileCount);

	return s;
}