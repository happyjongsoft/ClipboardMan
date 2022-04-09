#pragma once
#include "afxwin.h"


// CInputBox dialog

class CInputBox : public CDialog
{
	DECLARE_DYNAMIC(CInputBox)

public:
	CInputBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInputBox();

	CString getInputValue();
// Dialog Data
	enum { IDD = IDD_INPUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_string;
	afx_msg void OnPaint();
};
