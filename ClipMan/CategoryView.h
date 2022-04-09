#pragma once


// CCategoryView view

class CCategoryView : public CTreeView
{

	DECLARE_DYNCREATE(CCategoryView)

protected:
	CCategoryView();           // protected constructor used by dynamic creation
	virtual ~CCategoryView();

public:
	virtual void OnInitialUpdate();
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	HTREEITEM m_hActiveItem;
	HTREEITEM getTreeItemByData(HTREEITEM parent, CString data);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint ptMousePos);
	afx_msg void OnAddfolder();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


