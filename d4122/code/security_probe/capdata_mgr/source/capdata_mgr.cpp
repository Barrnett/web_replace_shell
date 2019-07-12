// capdata_mgr.cpp : Defines the entry point for the DLL application.
//


#include "capdata_mgr.h"
#include "CapdataObserver.h"

#ifdef OS_WINDOWS

#ifdef _MANAGED
#pragma managed(push, off)
#endif

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

#ifdef _MANAGED
#pragma managed(pop)
#endif

#endif

CCapdataObserver *			g_pCapDataObserver = NULL;

CAPDATA_MGR_API bool CAPDATA_MGR_Init(const LPACECTSTR& param )
{
	CPF::init(0);

	ACE_ASSERT(!g_pCapDataObserver);

	if( !g_pCapDataObserver )
	{
		g_pCapDataObserver = new CCapdataObserver;

		if( !g_pCapDataObserver )
			return false;

		if( !g_pCapDataObserver->init() )
		{
			delete g_pCapDataObserver;
			g_pCapDataObserver = NULL;
			return false;
		}

	}

	return TRUE;
}

CAPDATA_MGR_API  void CAPDATA_MGR_Close()
{
	if( g_pCapDataObserver )
	{
		g_pCapDataObserver->fini();
		delete g_pCapDataObserver;
		g_pCapDataObserver = NULL;
	}

	CPF::fini();
}

CAPDATA_MGR_API  CMyBaseObserver * CAPDATA_MGR_GetWorkInstance()
{
	ACE_ASSERT(g_pCapDataObserver);
	return g_pCapDataObserver;

}