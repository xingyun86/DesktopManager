
// DesktopManager.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDesktopManagerApp:
// See DesktopManager.cpp for the implementation of this class
//

class CDesktopManagerApp : public CWinApp
{
public:
	CDesktopManagerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDesktopManagerApp theApp;
