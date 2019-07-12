//////////////////////////////////////////////////////////////////////////
//InterfaceFactory.h

#pragma once

#include "flux_control_base_export.h"
#include "PacketIO/EtherioDataSource.h"
#include "PacketIO/EtherioSendPacket.h"
#include "PacketIO/DataSourceMgr.h"
#include "InterfaceCfg.h"

class FLUX_CONTROL_BASE_CLASS CInterfaceFactory
{
public:
	CInterfaceFactory();
	~CInterfaceFactory();

public:
	BOOL init(const char  *config_path,int buffer_pkt_thread,int& error_code,std::string& card_info,const char * drv_name,
		SIGNAL_MONITOR_EVENT pSignal_event_func,void * signal_param,IBeforeSendPacketHandle * pBeforeSendPacketHandle);

	void fin();
	

	std::string	GetCardName(int pos,int card_index) const;

	//得到所有实际网卡的信息
	void GetAllNetCardInfo(BOOL& bDrvOK,std::vector<std::string>& vt_cardinfo);
	void GetProbeAllRunCardInfo(std::vector<ACE_UINT64>& vt_cardinfo);

	BOOL IfDrvLoadOK();


public:
	BOOL FindCard(const char * card_name) const;

public:
	inline CInterfaceCfg * GetInterfaceCfg()
	{
		return m_pInterfaceCfg;
	}

	inline const CInterfaceCfg * GetInterfaceCfg() const
	{
		return m_pInterfaceCfg;
	}

private:
	CInterfaceCfg * m_pInterfaceCfg;

public:
	//如果没有则返回-1
	int GetCardIndexByShowName(const char * card_name) const;

};

