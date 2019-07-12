//////////////////////////////////////////////////////////////////////////
//InterfaceFactory.cpp

#include "stdafx.h"
#include "InterfaceCfg.h"
#include "InterfaceFactory.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/path_tools.h"
#include "cpf/addr_tools.h"
#include "cpf/strtools.h"
#include "TestControlParam.h"
#include "cpf/HostEtherCardMgr.h"

#include "TS_interfaceCfg.h"  // xumx add 2016-6-23

#ifdef OS_WINDOWS

#include "Iphlpapi.h"
#pragma comment(lib,"Iphlpapi.lib")
#include "../../../comm_base_module/packetio_wrappers/PacketIO/source/etherio/EtherioDllExport.h"
#else
#include "PacketIO/DataSourceHelper.h"
#include "cpf/adapter_info_query.h"
#include "../../../comm_base_module/packetio_wrappers/PacketIO/source/etherio/EtherioDllExport.h"
#endif


//////////////////////////////////////////////////////////////////////////
//	

CInterfaceFactory::CInterfaceFactory()
{
	m_pInterfaceCfg = NULL;
}

CInterfaceFactory::~CInterfaceFactory()
{
	

}

BOOL CInterfaceFactory::init
(
	const char  *config_path,
	int buffer_pkt_thread,
	int& error_code,
	std::string& card_info,
	const char * drv_name,
	SIGNAL_MONITOR_EVENT pSignal_event_func,
	void * signal_param,
	IBeforeSendPacketHandle * pBeforeSendPacketHandle
)
{
	// xumx add 2016-6-23
	CTS_InterfaceCfg *pInterfaceCfg = new CTS_InterfaceCfg(pSignal_event_func, signal_param);
	if (NULL == pInterfaceCfg)
	{
		return FALSE;
	}

	if(pInterfaceCfg->init(buffer_pkt_thread, error_code, card_info, drv_name) == FALSE)
	{
		delete pInterfaceCfg;
		m_pInterfaceCfg = NULL;
		return(FALSE);
	}

	m_pInterfaceCfg = pInterfaceCfg;

	return(TRUE);
}

void CInterfaceFactory::fin()
{
	if( m_pInterfaceCfg )
	{
		m_pInterfaceCfg->fin();
		delete m_pInterfaceCfg;
		m_pInterfaceCfg = NULL;
	}
}

void CInterfaceFactory::GetAllNetCardInfo(BOOL& bDrvOK,std::vector<std::string>& vt_cardinfo)
{
	vt_cardinfo.clear();

	CHostEtherCardMgr host_ether_mgr;
	
	int count = host_ether_mgr.Init();

	for(int i = 0; i < count; ++i)
	{
		vt_cardinfo.push_back(host_ether_mgr.m_array_card_info[i]._szShowName);
	}

	bDrvOK = IfDrvLoadOK();

	return;
}

BOOL CInterfaceFactory::IfDrvLoadOK()
{
	int bStaticLoad = 1;

	std::string str_driver_file = "etherio.sys";
	std::string str_driver_name = "etherio";

	int count = EtherIO_InitializeEx(str_driver_file.c_str(), str_driver_name.c_str(), bStaticLoad);

	EtherIO_UnInitialize();	

	return (count>0);
}

void CInterfaceFactory::GetProbeAllRunCardInfo(std::vector<ACE_UINT64>& vt_cardinfo)
{
	if( m_pInterfaceCfg )
	{
		m_pInterfaceCfg->GetProbeAllRunCardInfo(vt_cardinfo);
	}
}

BOOL CInterfaceFactory::FindCard(const char * card_name) const
{
	if( !m_pInterfaceCfg )
		return false;

	return m_pInterfaceCfg->FindCard(card_name);
}

int CInterfaceFactory::GetCardIndexByShowName(const char * card_name) const
{
	return EtherIO_GetOneIndexByShowName(card_name);
}

