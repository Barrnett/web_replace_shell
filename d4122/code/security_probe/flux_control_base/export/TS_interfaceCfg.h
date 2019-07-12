
#pragma once

#include "flux_control_base_export.h"
#include "InterfaceCfg.h"
#include "PacketIO/EtherDataSourceBase.h"
#include "PacketIO/EtherDataStreamBase.h"
#include "PacketIO/DataSourceMgr.h"
#include "PacketIO/BufferPktThreadDataSource.h"
#include "cpf/TinyXmlEx.h"
#include "tcpip_mgr_common_init.h"
#include "cpf/Common_Func_Macor.h"


#define ERRNO_READ_CFG_FAILED			-100

class FLUX_CONTROL_BASE_CLASS CTS_InterfaceCfg : public CInterfaceCfg
{
public:
	CTS_InterfaceCfg(SIGNAL_MONITOR_EVENT pSignal_event_func, void * signal_param);
	~CTS_InterfaceCfg();

public:
	BOOL init(int buffer_pkt_thread, int& error_code, std::string& card_info, const char * drv_name);
	virtual void fin();

public:
	
	virtual ACE_UINT32 GetSendBufferUsed(OUT ACE_UINT32* pTotalLen)
	{
		if (m_pEtherioWriteDataStream)
		{
			return m_pEtherioWriteDataStream->GetSendBufferUsed(pTotalLen);
		}
		else
		{
			if (pTotalLen) { *pTotalLen = 0; }

			return 0;
		}

	}

	virtual ACE_UINT32 GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen)
	{
		if (m_pBufferPktThreadDataSource)
		{
			return m_pBufferPktThreadDataSource->GetRecvBufferUsed(pTotalLen);
		}

		if (m_pEtherioDataSource)
		{
			return m_pEtherioDataSource->GetRecvBufferUsed(pTotalLen);
		}

		if (pTotalLen)
		{
			*pTotalLen = 0;
		}

		return 0;
	}

	virtual std::string	GetCardName(int pos, int card_index) const;

	virtual BOOL FindCard(const char * card_name) const;

	virtual BOOL GetProbeAllRunCardInfo(std::vector<ACE_UINT64>& vt_card_info);

public:
	virtual IRecvDataSource * GetRecvDataSource()
	{
		if (m_pBufferPktThreadDataSource)
		{
			return m_pBufferPktThreadDataSource;
		}

		return m_pEtherioDataSource;
	}

	virtual IWriteDataStream * GetWriteDataStream()
	{
		return m_pEtherioWriteDataStream;
	}

private:
	INT_PTR							m_buffer_pkt_thread;
	CBufferPktThreadDataSource *	m_pBufferPktThreadDataSource;

private:
	CEtherioDataSource *		m_pEtherioDataSource;
	CEtherioWriteDataStream *	m_pEtherioWriteDataStream;
};
