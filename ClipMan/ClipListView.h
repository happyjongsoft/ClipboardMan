#pragma once

typedef enum {
	SORT_None = 0,
	SORT_AZ = 1,
	SORT_ZA = -1,
} SortOrder;

// CClipListView view

class CClipListView : public CListView
{
	DECLARE_DYNCREATE(CClipListView)

protected:
	CClipListView();           // protected constructor used by dynamic creation
	virtual ~CClipListView();

public:
	virtual void OnInitialUpdate();

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

	bool		m_isInitialized;
	CImageList	m_imageList;


	CImageList* m_pDragImage;
	BOOL		m_bDragging;
	int			m_nDragIndex, m_nDropIndex;
	CWnd*		m_pDropWnd;
	CPoint		m_ptDropPoint;

public:
	afx_msg void OnDrawClipboard();
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	afx_msg LRESULT OnRefreshClipList(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
};


