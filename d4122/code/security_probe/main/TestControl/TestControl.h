// TestControl.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

class CVersionCompInfo;

// CTestControlApp:
// See TestControl.cpp for the implementation of this class
//
//#include "TestThread.h"

class CTestControlApp : public CWinApp
{
public:
	CTestControlApp();

// Overrides
	public:
	virtual BOOL InitInstance();

	//CTestThread m_thread;

private:
	void SelectCPU();


// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTestControlApp theApp;