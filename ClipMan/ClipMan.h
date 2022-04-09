
// ClipMan.h : main header file for the ClipMan application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CClipManApp:
// See ClipMan.cpp for the implementation of this class
//

class CClipManApp : public CWinApp
{
public:
	CClipManApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CClipManApp theApp;
