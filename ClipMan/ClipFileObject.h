#pragma once
#include "ClipObject.h"

class CClipFileObject :
	public CClipObject
{
public:
	DECLARE_SERIAL(CClipFileObject)

	CClipFileObject();
	~CClipFileObject();

	bool setData(HGLOBAL hglb);
	CSize drawObject(CDC* pDC, CRect pos, BOOL strech);
	CString getContents();
	int	getIconIndex(){ return 3; };
	bool copyToClipboard();
	CString getText();

	void		Serialize(CArchive& archive);
private:
	CString	m_filePath;
	int		m_fileCount;
};

