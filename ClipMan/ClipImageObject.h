#pragma once
#include "ClipObject.h"
class CClipImageObject :
	public CClipObject
{
public:
	DECLARE_SERIAL(CClipImageObject)

	CClipImageObject();
	~CClipImageObject();

	bool setData(HGLOBAL hglb);
	CSize drawObject(CDC* pDC, CRect pos, BOOL strech);
	CString getContents();
	int	getIconIndex(){ return 2; };
	bool copyToClipboard();
	void removeTempData();

	void		Serialize(CArchive& archive);
private:
	CString m_path;
	int m_width;
	int m_height;
	int m_planes;
	int m_bitsPixel;

	BOOL writeData(CString fn, LPVOID data);
	LPVOID readData(CString fn);
	CString generateRandomPath(int len);
};

