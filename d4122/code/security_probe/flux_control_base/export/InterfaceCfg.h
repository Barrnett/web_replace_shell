//////////////////////////////////////////////////////////////////////////
//InterfaceCfg.h

#pragma once

#include "flux_control_base_export.h"
#include "PacketIO/EtherioDataSource.h"
#include "PacketIO/EtherioSendPacket.h"
#include "PacketIO/DataSourceMgr.h"

typedef int (*SIGNAL_MONITOR_EVENT)(void * param);

class FLUX_CONTROL_BASE_CLASS CInterfaceCfg
{
public:
	CInterfaceCfg(SIGNAL_MONITOR_EVENT pSignalEventFunc,void * param)
	{
		m_pSignalEventFunc = pSignalEventFunc;
		m_pSignalParam = param;
	}

	virtual ~CInterfaceCfg(void)
	{
	}

public:
	virtual void fin() = 0;

public:
	virtual IRecvDataSource * GetRecvDataSource() = 0;
	virtual IWriteDataStream * GetWriteDataStream() = 0;

	virtual ACE_UINT32 GetSendBufferUsed(OUT ACE_UINT32* pTotalLen) = 0;
	virtual ACE_UINT32 GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen) = 0;

	//pos表示链路号，card_index=1表示LAN,card_index=0表示WAN
	virtual std::string	GetCardName(int pos,int card_index) const = 0;

	virtual BOOL FindCard(const char * card_name) const = 0;

	virtual BOOL GetProbeAllRunCardInfo(std::vector<ACE_UINT64>& vt_card_info) = 0;

public:
	SIGNAL_MONITOR_EVENT	m_pSignalEventFunc;
	void *					m_pSignalParam;

public:
	typedef struct tagIP_CARD_INFO
	{
		tagIP_CARD_INFO()
		{
			memset(this,0x00,sizeof(*this));
		}

		ACE_UINT32	host_ip;
		ACE_UINT32  host_mask;
		ACE_UINT32  host_gateway;

		ACE_UINT32	dns1;
		ACE_UINT32	dns2;

		BYTE		mac_addr[6];
		char		friend_name[64];

	}IP_CARD_INFO;

public:
	int SetDevManagerIP();

public:
	static BOOL JudegSameIP(const std::vector<CInterfaceCfg::IP_CARD_INFO>&	vt_ip_card_info,
		const char * card_name,ACE_UINT32 ipaddr,ACE_UINT32 ipmask,ACE_UINT32 ipgw);

	static BOOL JudegSameDNS(const std::vector<CInterfaceCfg::IP_CARD_INFO>&	vt_ip_card_info,
		const char * card_name,ACE_UINT32 dns1,ACE_UINT32 dns2);

public:
	static BOOL GetAllIPCardInfo(std::vector<CInterfaceCfg::IP_CARD_INFO>& vt_ip_cardinfo);

	static BOOL GetCardDNS(int adapter_index,ACE_UINT32& dns1,ACE_UINT32& dns2);

public:


};
