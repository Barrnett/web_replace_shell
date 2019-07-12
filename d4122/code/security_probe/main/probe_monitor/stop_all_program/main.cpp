// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cpf/other_tools.h"
#include "ace/OS_main.h"
#include "stop_all_program.h"
#include "ace/Init_ACE.h"  // xumx add 2016-6-22

#ifndef _DEBUG
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#endif

int DoStop();

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

#endif

int
ACE_TMAIN (int argc, ACE_TCHAR* argv[])
{
	ACE::init();

#ifdef OS_WINDOWS
	EnableDebugPriv();
#endif

	DoStop();

	ACE_OS::sleep(3);

	ACE::fini();


	return 0;
}

int DoStop()
{
	CStop_all_program stop_tool;

	if( !stop_tool.init() )
		return -1;

	//先关闭以前的程序
	printf("\n开始关闭程序....\n");

	stop_tool.StopOldProgram();

	printf("\n程序已经关闭....\n");

	stop_tool.close();

	return 0;
}

