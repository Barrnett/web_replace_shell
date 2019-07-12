//////////////////////////////////////////////////////////////////////////
//HostIPMacFinder.cpp

#include "stdafx.h"
#include "HostIPMacFinder.h"
#include "cpf/addr_tools.h"
#include "cpf/Data_Stream.h"
#include "tcpip_mgr_common_init.h"

CHostIPMacFinder::CHostIPMacFinder(void)
{
}

CHostIPMacFinder::~CHostIPMacFinder(void)
{
	m_ip_mac_finder.Destroy();
}

BOOL CHostIPMacFinder::Create(int max_nums)
{
	return m_ip_mac_finder.Create(max_nums);

}

void CHostIPMacFinder::ComeAPacket(const BYTE * macaddr,ACE_UINT32 hostorder_ip,int cur_time)
{
	InnerValue * pValue = m_ip_mac_finder.FindOnly(hostorder_ip);

	if( pValue )
	{
		pValue->last_active_time = cur_time;

		if( CPF::IsMacAddrEqual(&pValue->macaddr,macaddr) )
		{
			return;
		}

		pValue->macaddr = *(ACE_UINT64 *)macaddr;

		pValue->update_time = cur_time;
	}
	else
	{
		pValue = m_ip_mac_finder.AllocBlockByKey(hostorder_ip);

		if( pValue )
		{
			pValue->hostorder_ip = hostorder_ip;

			pValue->macaddr = *(ACE_UINT64 *)macaddr;

			pValue->last_active_time = cur_time;

			pValue->update_time = cur_time;

			pValue->last_save_time = 0;
		}
	}

	return;

}


void CHostIPMacFinder::HandleFirstPacket(const char * pTempPath,const char * pPath,
										 int interval,const CTimeStamp32& stamp)
{
	m_StaticOutputMgr.Create(pTempPath,pPath,
		"txt",
		1,
		interval,
		stamp,
		S_SaveStatic,
		this );

	return;

}

void CHostIPMacFinder::S_SaveStatic(const CTimeStamp32& curTs, void * param)
{
	CHostIPMacFinder * pHostIPMacFinder = (CHostIPMacFinder *)param;

	pHostIPMacFinder->SaveStatic(curTs);

	return;
	
}

void CHostIPMacFinder::OnPacketTimer(const CTimeStamp32& stamp)
{
	m_StaticOutputMgr.OnTimer(stamp);
}

void CHostIPMacFinder::SaveStatic(const CTimeStamp32& curTs)
{
	const int DATA_VERSION = 2;

	int index = m_ip_mac_finder.GetHeadBlock();

	while(index != -1 )
	{
		InnerValue * pValue = m_ip_mac_finder.GetNextBlockFromHead(index);

		if( pValue->last_active_time > pValue->last_save_time )
		{
			pValue->last_save_time = curTs.GetSEC();

			char output_buf[128];

			char * pcur = output_buf;

			CData_Stream_LE::PutUint64(pcur,DEF_STAT_TXT_FLAG_BEGIN);
			CData_Stream_LE::PutUint32(pcur,DATA_VERSION);
			CData_Stream_LE::PutUint32(pcur,pValue->hostorder_ip);
			CData_Stream_LE::PutUint64(pcur,pValue->macaddr);
			CData_Stream_LE::PutUint32(pcur,pValue->update_time);
			CData_Stream_LE::PutUint32(pcur,pValue->last_active_time);
		
			CData_Stream_LE::PutUint64(pcur,DEF_STAT_TXT_FLAG_END);
			m_StaticOutputMgr.WriteDataToFile(0,output_buf,pcur-output_buf);
		}

	}

	return;
}
