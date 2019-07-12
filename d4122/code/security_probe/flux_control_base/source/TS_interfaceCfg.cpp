#include "stdafx.h"
#include "TS_interfaceCfg.h"
#include "TestControlParam.h"
#include "config_center.h"
//////////////////////

#ifdef OS_WINDOWS

#include "Iphlpapi.h"
#include "../../../comm_base_module/packetio_wrappers/PacketIO/source/etherio/EtherioDllExport.h"

#pragma comment(lib,"Iphlpapi.lib")
#else
#include "PacketIO/DataSourceHelper.h"
#include "cpf/adapter_info_query.h"
#include "../../../comm_base_module/packetio_wrappers/PacketIO/source/etherio/EtherioDllExport.h"
#endif

CTS_InterfaceCfg::CTS_InterfaceCfg(SIGNAL_MONITOR_EVENT pSignal_event_func,void * signal_param)
:CInterfaceCfg(pSignal_event_func, signal_param)
{
	m_pEtherioDataSource = NULL;
	m_pEtherioWriteDataStream = NULL;

	m_pBufferPktThreadDataSource = NULL;

	m_buffer_pkt_thread = false;

}
CTS_InterfaceCfg::~CTS_InterfaceCfg()
{
	fin();

	return;
}
/////////////

void CTS_InterfaceCfg::fin()
{
	if (m_pBufferPktThreadDataSource)
	{
		m_pBufferPktThreadDataSource->CloseDataSource();
		delete m_pBufferPktThreadDataSource;
		m_pBufferPktThreadDataSource = NULL;
	}

	if (m_pEtherioDataSource)
	{
		m_pEtherioDataSource->Close();
		delete m_pEtherioDataSource;
		m_pEtherioDataSource = NULL;
	}

	if (m_pEtherioWriteDataStream)
	{
		m_pEtherioWriteDataStream->Close();
		delete m_pEtherioWriteDataStream;
		m_pEtherioWriteDataStream = NULL;
	}

	return;
}

/////////////////

std::string	CTS_InterfaceCfg::GetCardName(int pos, int card_index) const 
{ 
	std::string st(""); return(st); 
}

BOOL CTS_InterfaceCfg::FindCard(const char * card_name) const 
{ 
#if 0 // xumx delete 2016-6-28
	for (int i = 0; i < sizeof(m_vt_card) / sizeof(m_vt_card[0]); ++i)
	{
		if (m_vt_card[i].card_type != CARD_TYPE_NULL 
			&& stricmp(m_vt_card[i].card_name.c_str(), card_name) == 0)
		{
			return true;
		}
	}
#endif
	return false;  
}

BOOL CTS_InterfaceCfg::GetProbeAllRunCardInfo(std::vector<ACE_UINT64>& vt_card_info) 
{ 
#if 0 // xumx delete 2016-6-28
	for (int i = 0; i < sizeof(m_vt_card) / sizeof(m_vt_card[0]); ++i)
	{
		if (m_vt_card[i].card_type != CARD_TYPE_NULL)
		{
			vt_card_info.push_back(m_vt_card[i].mac_addr);
		}
	}
#endif
	return true;
}
/////////////////

BOOL CTS_InterfaceCfg::init(int buffer_pkt_thread, int& error_code, std::string& card_info, const char * drv_name)
{	
	do
	{
		m_buffer_pkt_thread = buffer_pkt_thread;

		//	EN_TIME_STAMP_MODE_RDTSC_NO=6

		m_pEtherioDataSource = new CEtherioDataSource;

		if (!m_pEtherioDataSource){ break; }

		if 
		(
			!m_pEtherioDataSource->Open
			(
				1,
				drv_name,
				NULL,
				card_info.c_str(),
				5,
				true,
				6,
				true,
				&error_code
			)
		)
		{
			if (error_code == 0)
			{
				error_code = 10;//表示没有发现任何网卡;
			}
			else if (error_code == -10) { return(false); }

			break;
		}

		m_pEtherioWriteDataStream = new CEtherioWriteDataStream;

		if (!m_pEtherioWriteDataStream){ break; }

		if 
		(
			!m_pEtherioWriteDataStream->Open
			(
				1,
				0,
				drv_name,
				NULL,
				card_info.c_str(),
				1,
				5
			)
		)
		{
			break;
		}

		if (m_buffer_pkt_thread)
		{
			m_pBufferPktThreadDataSource = new CBufferPktThreadDataSource;

			m_pBufferPktThreadDataSource->init
			(
				m_pEtherioDataSource,
				false,
				5 * 1024 * 1024,
				ACE_Time_Value(0, CCTestControlParam_Singleton::instance()->m_empty_sleep_time * 1000)
			);
		}

		return true;

	} while (0);

	fin();

	return false;
}
//////////////////
