// tcpip_mgr_common.cpp : Defines the entry point for the DLL application.
//


#include "tcpip_mgr_common_init.h"

#include "cpf/path_tools.h"
#include "cpf/strtools.h"
#include "AppSubtypeReader.h"


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

//////////////////////////////////////////////////////////////////////////

ACE_Memmap_Log_Msg *					g_pACE_Memmap_Log_Msg = NULL;
unsigned int							g_live_log_level = -1;

bool									g_bDisnableAllInterrupt = false;	//禁用所有的阻断
int										g_nAuditSavelevel = 2;	//保存审计的级别
int										g_nHttpAuditType = 0x01;
int										g_save_html_only_have_title = 1;
bool									g_bDisnableAllWarninglevel = false;	//禁用所有的告警级别

bool									g_bDisnableActionStat = false;
bool									g_bAuditWebLogon = false;			//是否审计web登陆


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL CreateMemmapLogMsg(int live_log_size,unsigned int live_log_level);


int g_init_nums = 0;

void tcpip_mgr_common_init(int live_log_size,unsigned int live_log_level)
{
	++g_init_nums;

	if( g_init_nums != 1 )
		return;

	CPF::init();

	CreateMemmapLogMsg(0,0);

	CAppSubtypeReader::ReadAppTypeTree();

	return;
}

void tcpip_mgr_common_fini()
{
	--g_init_nums;

	if( g_init_nums > 0 )
		return;

	//CSubtypeInterConvert::Destroy();

	if( g_pACE_Memmap_Log_Msg )
	{
		delete g_pACE_Memmap_Log_Msg;
		g_pACE_Memmap_Log_Msg = NULL;
	}

	CAppSubtypeReader::CloseAppTypeTree();

	CPF::fini();

	return;
}

//单位为字节，0表示不启动该功能能
BOOL CreateMemmapLogMsg(int live_log_size,unsigned int live_log_level)
{
	if( live_log_size > 0 )
	{
		g_live_log_level = live_log_level;

		g_pACE_Memmap_Log_Msg = new ACE_Memmap_Log_Msg;

		if( !g_pACE_Memmap_Log_Msg )
		{
			return false;
		}

		ACE_Time_Value curtime = ACE_OS::gettimeofday();
		char nbuffer[255];
		ACE_OS::sprintf(nbuffer, "log/probelog_map_%s.txt", CPF::FormatTime(5, curtime.sec()));
		const char * plogname = CPF::GetModuleFullFileName( nbuffer );

		if( !g_pACE_Memmap_Log_Msg->init(plogname,live_log_size) )
		{
			delete g_pACE_Memmap_Log_Msg;
			g_pACE_Memmap_Log_Msg = NULL;
			return false;
		}
	}

	return true;
}
