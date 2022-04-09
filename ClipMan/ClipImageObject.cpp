#include "stdafx.h"
#include "ClipImageObject.h"

IMPLEMENT_SERIAL(CClipImageObject, CClipObject, 1)

CClipImageObject::CClipImageObject()
{
	m_format = CLIP_TYPE_IMAGE;
	m_title = m_format + "-" + m_time;
	m_width = m_height = m_planes = m_bitsPixel = 0;	
}

CClipImageObject::~CClipImageObject()
{
}

bool CClipImageObject::setData(HGLOBAL hglb)
{
	HBITMAP hBmp = (HBITMAP)hglb;
	BITMAP bmp;
	
	::GetObject(hBmp, sizeof(bmp), &bmp);

	m_width = bmp.bmWidth;
	m_height = bmp.bmHeight;
	m_planes = bmp.bmPlanes;
	m_bitsPixel = bmp.bmBitsPixel;

	int size = m_width * m_height * m_planes * m_bitsPixel;
	LPVOID data = new LPVOID[size];
	::GetBitmapBits((HBITMAP)hBmp, size, data);

	m_path = generateRandomPath(64);
	writeData("data\\" + m_path , data);

	delete data;

	return true;
}

void CClipImageObject::removeTempData()
{
	DeleteFileA("data\\" + m_path);
}

CSize CClipImageObject::drawObject(CDC* pDC, CRect pos, BOOL strech)
{
	pos.DeflateRect(5, 5);

	LPVOID data = readData("data\\" + m_path);
	HBITMAP hBmp = CreateBitmap(m_width, m_height, m_planes, m_bitsPixel, data);
	DrawBmp(pDC, hBmp, &pos, TRUE);

	DeleteObject(hBmp);
	free(data);

	return CSize(pos.right + 5, pos.bottom + 5);
}

// Function for Save and Open
void CClipImageObject::Serialize(CArchive& archive)								
{
	CObject::Serialize(archive);

	if (archive.IsStoring())
	{
		// Put static data
		archive << m_format << m_time << m_title << m_path << m_width << m_height << m_planes << m_bitsPixel;

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
		archive >> m_format >> m_time >> m_title >> m_path >> m_width >> m_height >> m_planes >> m_bitsPixel;

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

bool CClipImageObject::copyToClipboard()
{
	LPVOID data = readData("data\\" + m_path);
	HBITMAP hBmp = CreateBitmap(m_width, m_height, m_planes, m_bitsPixel, data);

	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBmp);
	CloseClipboard();

	DeleteObject(hBmp);
	free(data);

	return true;
}

CString CClipImageObject::getContents()
{
	CString contents;
	contents.Format("Image Width:%d , Height:%d", m_width, m_height);
	return contents;
}

BOOL CClipImageObject::writeData(CString fn, LPVOID data)
{
	try{
		int size = m_width * m_height * m_planes * m_bitsPixel;

		CFile file;
		file.Open(fn, CFile::modeWrite | CFile::modeCreate);
		file.Write(data, size);
		file.Close();

	}
	catch (CException* e)
	{

	}
	return TRUE;
}

LPVOID CClipImageObject::readData(CString fn)
{
	int size = m_width * m_height * m_planes * m_bitsPixel;
	LPVOID buffer = new LPVOID[size];
	try{
		CFile file;
		file.Open(fn, CFile::modeRead);
		file.Read(buffer, size);
		file.Close();

	}
	catch (CException* e)
	{

	}
	return buffer;
}

CString CClipImageObject::generateRandomPath(int len)
{
	// Set seed based on time
	srand(time(NULL));

	char* s = new char[len + 1];
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum)-1)];
	}

	s[len] = 0;
	
	CString sRet = CString(s);
	delete s;
	return sRet;
}