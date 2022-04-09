#pragma once
#include "ClipObject.h"
class CClipTextObject :
	public CClipObject
{
public:
	DECLARE_SERIAL(CClipTextObject)

	CClipTextObject();
	~CClipTextObject();

	bool setData(HGLOBAL hglb);
	CSize drawObject(CDC* pDC, CRect pos, BOOL strech);
	CString getContents();
	int	getIconIndex(){ return 1; };
	bool copyToClipboard();
	CString getText();

	void		Serialize(CArchive& archive);
private:
	CString		m_text;
};

