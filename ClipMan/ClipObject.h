#pragma once
#include "DrawWrapper.h"
// CClipObject command target
#define CLIP_TYPE_ALL		"All"
#define CLIP_TYPE_TEXT		"Text"
#define CLIP_TYPE_IMAGE		"Image"
#define CLIP_TYPE_FILE		"File"

class CClipObject : public CObject
{

public:
	DECLARE_SERIAL(CClipObject)

	CClipObject(); 
	virtual ~CClipObject();

	CString getTitle();	
	CString getFormat();
	CString getTime();	
	void	putCustomId(CString pid);
	bool	isChildOfCustomCategory(CString pid);

public:
	virtual 	bool setData(HGLOBAL data){ return true; };
	virtual 	CString getContents(){ return ""; };
	virtual		CSize drawObject(CDC* pDC, CRect pos, BOOL strech){ return CSize(pos.right, pos.bottom); }
	virtual		int getIconIndex(){ return 0; };
	virtual		bool copyToClipboard(){ return false; };
	virtual		CString getText(){ return ""; };
	virtual		void Serialize(CArchive& archive);
	virtual		void removeTempData(){};

protected:
	CStringList	m_customIds;
	CString		m_format;
	CString		m_title;
	CString		m_time;

	CString getCurrentTime();
};
