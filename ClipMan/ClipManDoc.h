
// ClipManDoc.h : interface of the CClipManDoc class
//

#pragma once
#include "ClipObject.h"
#include "ClipTextObject.h"
#include "ClipImageObject.h"
#include "ClipFileObject.h"
#include "ClipTypeObject.h"

#include <afxtempl.h>

class CClipManDoc : public CDocument
{
protected: // create from serialization only
	CClipManDoc();
	DECLARE_DYNCREATE(CClipManDoc)

// Attributes
public:
	CTypedPtrArray<CObArray, CClipTypeObject*>					m_lstTypeObject;

	CTypedPtrArray<CObArray, CClipObject*>						m_lstClipObject;
	CTypedPtrArray<CObArray, CClipObject*>						m_lstClipFilteredObject;
	CClipObject*												m_curClipObject;
	CString														m_type;
	CClipObject*												m_DragClipObject;

// Operations
public:
	bool											putTypeObject(CClipTypeObject* obj);
	CTypedPtrArray<CObArray, CClipTypeObject*>&		getTypeList();

	bool											putClipObject(CClipObject* obj);
	CTypedPtrArray<CObArray, CClipObject*>&			getClipList();
	CTypedPtrArray<CObArray, CClipObject*>&			getClipFilteredList();

	CClipObject*									getCurrentClipObject();
	void											setCurrentClipObject(CClipObject* obj);

	void											setCategory(CString type);
	CString											getCategory(){ return m_type; };

	void											setDragClipObject(CClipObject* obj){ m_DragClipObject = obj; };
	CClipObject*									getDragClipObject(){ return m_DragClipObject; };
	
// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CClipManDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:


// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual void OnCloseDocument();
};
