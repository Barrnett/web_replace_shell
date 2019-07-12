// TestControl.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TestControl.h"
#include "TestControlDlg.h"
#include "cpf/cpf.h"
#include "cpf/other_tools.h"
#include "TabWndEx.h"
#include "cpf/CrushDumpFun.h"
#include "cpf/path_tools.h"
#include "TestControlParam.h"
#include "config_center.h"
#include "cpf/os_syscall.h"
#include "UniERMOEMInfo.h"
#include "cpf/TinyXmlEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestControlApp

BEGIN_MESSAGE_MAP(CTestControlApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestControlApp construction

CTestControlApp::CTestControlApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTestControlApp object

CTestControlApp theApp;


// CTestControlApp initialization


#ifdef OS_WINDOWS

#define DEF_DUMP_CALLSTACK_TXT	0

BOOL EnableDebugPriv();
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

#endif


BOOL CTestControlApp::InitInstance()
{
	EnableDebugPriv();

	//某段程序需要阻止操作系统进入挂起时，可以使用下面代码来完成。
	::SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED | ES_CONTINUOUS);

	CPF::init();

	ConfigCenter_Initialize();

	SelectCPU();

	CCTestControlParam_Singleton::instance()->Read(
		ConfigCenter_Obj()->GetProbeFullConfigName("test_control_param.xml").c_str());

	if (CCTestControlParam_Singleton::instance()->m_bCreateDump )
	{
		SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
	}	

	if( !CPF::TestProcMutex_For_Server("Test_Control.exe") )
	{
		AfxMessageBox("程序已经运行!",MB_OK|MB_ICONWARNING);
		CPF::fini();

		return false;
	}

	CTabWndEx::init_clolr();
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CTestControlDlg dlg(m_lpCmdLine);
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	ConfigCenter_UnInitialize();

	CPF::fini();

	//当处理完成后，还要恢复操作系统的电源管理功能，使用下面代码完成。

	::SetThreadExecutionState(ES_CONTINUOUS);

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


void CTestControlApp::SelectCPU()
{
}

#ifdef OS_WINDOWS

BOOL EnableDebugPriv() 
{ 
	HANDLE hToken;
	LUID sedebugnameValue; 
	TOKEN_PRIVILEGES tkp; 

	if ( ! OpenProcessToken( GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) ) 
	{
		return FALSE; 
	}

	if ( ! LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) ) 
	{ 
		CloseHandle( hToken ); 
		return FALSE;
	} 

	tkp.PrivilegeCount = 1; 
	tkp.Privileges[0].Luid = sedebugnameValue; 
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	if ( ! AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) ) 
	{
		CloseHandle( hToken );
		return FALSE;
	}

	return true;
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	char str_path[MAX_PATH];
	char str_full_name[MAX_PATH];

	CPF::GetModuleFileName(str_path, sizeof(str_path));

	ACE_TCHAR* pExeName = CPF::GetLastFileNameSeperator(str_path);
	if( !pExeName )
	{
		ACE_ASSERT(FALSE);
		exit(-1);
	}

	*(pExeName) = 0;
	pExeName ++;

	ACE_OS::sprintf(str_full_name, "%s\\dump\\", str_path);

	CPF::CreateFullDirecory(str_full_name);
	
	ACE_OS::sprintf(str_full_name, "%s\\dump\\%s(%d)_%s", str_path, pExeName,sizeof(void *)*8,CPF::get_curtime_string(5));

	CreateDumpFile(str_full_name, pException);


#if DEF_DUMP_CALLSTACK_TXT
	//CreateCallStackFile(str_full_name, pException);
#endif

	exit(-1);


	return EXCEPTION_EXECUTE_HANDLER;  
}
#endif