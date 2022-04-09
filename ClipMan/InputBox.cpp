// InputBox.cpp : implementation file
//

#include "stdafx.h"
#include "ClipMan.h"
#include "InputBox.h"
#include "afxdialogex.h"


// CInputBox dialog

IMPLEMENT_DYNAMIC(CInputBox, CDialog)

CInputBox::CInputBox(CWnd* pParent /*=NULL*/)
	: CDialog(CInputBox::IDD, pParent)
	, m_string(_T(""))
{

}

CInputBox::~CInputBox()
{
}

void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_VALUE, m_string);
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CInputBox message handlers
CString CInputBox::getInputValue()
{
	return m_string;
}

void CInputBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	GetDlgItem(IDC_VALUE)->SetFocus();
	
	// Do not call CDialog::OnPaint() for painting messages
}

