// flux_control.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "flux_control.h"
#include "FluxControlObserver.h"
#include "FluxControlFrameObserver.h"
#include "config_center.h"
#include "cpf/ACE_Log_File_Msg.h"

#ifdef OS_WINDOWS

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#endif


CFluxControlFrameObserver *		g_pFluxControlFrameObserver = NULL;

CFluxControlObserver *			g_pFluxControlObserver = NULL;

FLUX_CONTROL_API bool FLUX_CONTROL_Init(const LPACECTSTR& param )
{
	CPF::init(0);

	if( !g_pFluxControlFrameObserver )
	{
		g_pFluxControlFrameObserver = new CFluxControlFrameObserver;

		if( !g_pFluxControlFrameObserver )
			return false;

		std::string str_driver_name = "etherio.sys";

		if( !g_pFluxControlFrameObserver->init(str_driver_name.c_str()) )
		{
			delete g_pFluxControlFrameObserver;
			g_pFluxControlFrameObserver = NULL;
			return false;
		}
	}

	return TRUE;
}

FLUX_CONTROL_API  void FLUX_CONTROL_Close()
{
	if( g_pFluxControlFrameObserver )
	{
		g_pFluxControlFrameObserver->fin();
		delete g_pFluxControlFrameObserver;
		g_pFluxControlFrameObserver = NULL;
	}

	CPF::fini();
}

FLUX_CONTROL_API  CMyBaseObserver * FLUX_CONTROL_GetWorkInstance()
{
	ACE_ASSERT(g_pFluxControlFrameObserver);
	return g_pFluxControlFrameObserver;

}

FLUX_CONTROL_API void FLUX_CONTROL_SetRebootHandler(IFCRebootHandler * pFCRebootHandler)
{
	if( g_pFluxControlObserver )
		g_pFluxControlObserver->SetRebootHandler(pFCRebootHandler);
	else	
		ACE_ASSERT(FALSE);
}

FLUX_CONTROL_API IRecvDataSource * FLUX_CONTROL_GetRecvDataSource()
{
	if( g_pFluxControlObserver && g_pFluxControlObserver->GetInterfaceCfg() )
		return g_pFluxControlObserver->GetInterfaceCfg()->GetRecvDataSource();
	else
		return NULL;
}

FLUX_CONTROL_API IWriteDataStream * FLUX_CONTROL_GetWriteDataStream()
{
	if( g_pFluxControlObserver && g_pFluxControlObserver->GetInterfaceCfg() )
		return g_pFluxControlObserver->GetInterfaceCfg()->GetWriteDataStream();
	else
		return NULL;
}

FLUX_CONTROL_API int FLUX_CONTROL_GetCardNums()
{
	return 0;
}


FLUX_CONTROL_API int FLUX_CONTROL_GetCardStat(int pos,STAT_BI_ITEM& item,std::string* p_str_card_name)
{
	return false;
}

FLUX_CONTROL_API void FLUX_CONTROL_GetLinkNums(int line_type,int trans_type,UINT& total,UINT& cur)
{
	total = cur = 0;
}


FLUX_CONTROL_API void FLUX_CONTROL_GetNatLinkNums(int link_type,UINT& total,UINT& cur)
{
	total = cur = 0;
}


FLUX_CONTROL_API ACE_UINT32 FLUX_CONTROL_GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen)
{
	if( g_pFluxControlObserver && g_pFluxControlObserver->GetInterfaceCfg() )
	{
		return g_pFluxControlObserver->GetInterfaceCfg()->GetRecvBufferUsed(pTotalLen);
	}

	return 0;
}


FLUX_CONTROL_API ACE_UINT32 FLUX_CONTROL_GetSendBufferUsed(OUT ACE_UINT32* pTotalLen)
{
	if( g_pFluxControlObserver && g_pFluxControlObserver->GetInterfaceCfg() )
	{
		return g_pFluxControlObserver->GetInterfaceCfg()->GetSendBufferUsed(pTotalLen);
	}

	return 0;
}


FLUX_CONTROL_API BOOL FLUX_CONTROL_GetAllNetCardInfo(BOOL& bDrvOK,std::vector<std::string>& vt_cardinfo)
{
	if( g_pFluxControlObserver && g_pFluxControlObserver->m_pInterfaceCfgFactory )
	{
		g_pFluxControlObserver->m_pInterfaceCfgFactory->GetAllNetCardInfo(bDrvOK,vt_cardinfo);

		return true;
	}

	return false;
}

FLUX_CONTROL_API BOOL FLUX_CONTROL_StartCapCptData(BOOL bUseFlt,
												const char * flt_filename,
												std::vector< std::string > vt_from_card_name,
												const Cpt_Write_Param& cpt_param)
{
	if( g_pFluxControlObserver )
	{
		return g_pFluxControlObserver->StartCapDataForTest(
			DRIVER_TYPE_FILE_ZCCPT,
			bUseFlt,flt_filename,
			vt_from_card_name,
			cpt_param.one_filesize_mb,cpt_param.num_files,
			cpt_param.captime,
			cpt_param.strPath.c_str(),
			cpt_param.strFileName.c_str());
	}

	return false;
}


FLUX_CONTROL_API BOOL FLUX_CONTROL_StartCapEthrealData(BOOL bUseFlt,
												const char * flt_filename,
												std::vector< std::string > vt_from_card_name,
												const Ethereal_Write_Param& ethreal_param)
{
	if( g_pFluxControlObserver )
	{
		return g_pFluxControlObserver->StartCapDataForTest(
			DRIVER_TYPE_FILE_ETHEREAL,
			bUseFlt,flt_filename,
			vt_from_card_name,
			ethreal_param.one_filesize_mb,ethreal_param.num_files,
			ethreal_param.captime,
			ethreal_param.strPath.c_str(),
			ethreal_param.strFileName.c_str());
	}

	return false;
}

FLUX_CONTROL_API void FLUX_CONTROL_StopCapData()
{
	if( g_pFluxControlObserver )
	{
		g_pFluxControlObserver->StopCapData(false);
	}

	return;
}


FLUX_CONTROL_API void FLUX_CONTROL_Do_OutPut()
{
	if( g_pFluxControlObserver )
	{
		g_pFluxControlObserver->DoOutPut();
	}

	return;
}

FLUX_CONTROL_API const std::string& FLUX_CONTROL_GetCurCodeStep()
{
	static std::string str_empty;

	return str_empty;
	
}

FLUX_CONTROL_API void FLUX_CONTROL_EanbelTraceCode(BOOL bEnable)
{
	
}

FLUX_CONTROL_API void FLUX_CONTROL_SetStopEventInfo(const char * stop_event_name,int stop_event_interval)
{
	if( g_pFluxControlFrameObserver )
	{
		g_pFluxControlFrameObserver->SetStopEventInfo(stop_event_name,stop_event_interval);
	}

	return;
}

FLUX_CONTROL_API BOOL FLUX_CONTROL_IsGetStopEvent()
{
	if( g_pFluxControlFrameObserver )
	{
		return g_pFluxControlFrameObserver->IsGetStopEvent();
	}

	return false;

}

////
FLUX_CONTROL_API CSerialDataRecv * FLUX_CONTROL_GetSerialDataRecv()
{
	if (g_pFluxControlObserver && g_pFluxControlObserver->m_pSerialInterface)
		return g_pFluxControlObserver->m_pSerialInterface->m_piSerialDataRecv;
	else
		return NULL;
}

FLUX_CONTROL_API CSerialDataSend * FLUX_CONTROL_GetSerialDataSend()
{
	if (g_pFluxControlObserver && g_pFluxControlObserver->m_pSerialInterface)
		return g_pFluxControlObserver->m_pSerialInterface->m_piSerialDataSend;
	else
		return NULL;
}

