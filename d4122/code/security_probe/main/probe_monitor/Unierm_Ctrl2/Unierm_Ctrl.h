
// Unierm_Ctrl.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUnierm_CtrlApp:
// �йش����ʵ�֣������ Unierm_Ctrl.cpp
//

class CUnierm_CtrlApp : public CWinApp
{
public:
	CUnierm_CtrlApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUnierm_CtrlApp theApp;