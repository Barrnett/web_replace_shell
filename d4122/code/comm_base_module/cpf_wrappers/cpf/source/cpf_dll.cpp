// cpf_dll.cpp : Defines the entry point for the DLL application.
//

#include "cpf/cpf.h"
#include "cpf/memmgr.h"
#include "ace/ACE.h"
#include "ace/Init_ACE.h"

extern bool init_memmgr(ULONG heap_size);
extern void close_memmgr(int log_leak);

#ifdef _WINDOWS_

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ULONG g_uInitNums = 0;

void CPF::init(UINT heap_size)
{
	if( g_uInitNums == 0 )
	{
		ACE::init();

		ACE_OS::socket_init (ACE_WSOCK_VERSION);

		init_memmgr(heap_size);
	}

	++g_uInitNums;

	return;
}

void CPF::fini(int log_leak)
{
	--g_uInitNums;

	if( g_uInitNums == 0 )
	{
		close_memmgr(log_leak);

		ACE_OS::socket_fini();

		ACE::fini();
	}

	return;	
}

std::string g_HOME_CHAR_SET("GBK");

void CPF::set_home_charset(const char * homecharset)
{
	g_HOME_CHAR_SET = homecharset;
}

const char * CPF::get_home_charset()
{
	return g_HOME_CHAR_SET.c_str();
}

//void CPF::dump_memory(const char * lpsMtFileName, const char * lpsNoMtFileName)
//{
//	//if( g_pmemMgr_mtsafe && lpsMtFileName )
//	//	g_pmemMgr_mtsafe->Dump(lpsMtFileName);
//
//	//if( g_pmemMgr_no_mtsafe && lpsNoMtFileName )
//	//	g_pmemMgr_no_mtsafe->Dump(lpsNoMtFileName);
//
//	return;
//}
//
//void CPF::reclaim_memory(int type)
//{
//	/*if( (type & 0x01) && g_pmemMgr_mtsafe )
//	{
//		g_pmemMgr_mtsafe->ReclaimBuffer();
//	}
//
//	if( (type & 0x02) && g_pmemMgr_no_mtsafe )
//	{
//		g_pmemMgr_no_mtsafe->ReclaimBuffer();
//	}*/
//
//	return;
//}


BOOL  CPF::GetOSVerIs64Bit()
{
#ifdef OS_WINDOWS
	BOOL bRet=FALSE;

	SYSTEM_INFO si;
	memset(&si,0x00,sizeof(si));

	typedef VOID(__stdcall*GETNATIVESYSTEMINFO)(LPSYSTEM_INFO lpSystemInfo);

	GETNATIVESYSTEMINFO fnGetNativeSystemInfo;

	fnGetNativeSystemInfo=(GETNATIVESYSTEMINFO)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),"GetNativeSystemInfo");

	if (fnGetNativeSystemInfo!=NULL)
	{
		fnGetNativeSystemInfo(&si);
	}
	else
	{
		GetSystemInfo(&si);
	}

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||   
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )

	{ 
		bRet=TRUE;
	}

	return bRet;

#else
	return (sizeof(void *)==8);

#endif

}