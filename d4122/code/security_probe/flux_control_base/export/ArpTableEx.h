//////////////////////////////////////////////////////////////////////////
//ArpTableEx.h

#pragma once

#include "flux_control_base_export.h"

#include "cpf/CPF_ArpTable.h"
#include "cpf/TimeStamp32.h"
#include "cpf/ComImp.h"

class CTS_InterfaceCfg;

class FLUX_CONTROL_BASE_CLASS CArpTableEx : public CPF_ArpTable
{
public:
	CArpTableEx(CTS_InterfaceCfg * pTS_InterfaceCfg);
	~CArpTableEx(void);

public:
	int OnPacketTimer(const CTimeStamp32& cur_ts);

public:
	void RefreshArp(const CTimeStamp32& cur_ts);

public:
	BOOL GetArpListInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	CTimeStamp32 m_last_check_ts;

	CTS_InterfaceCfg * m_pTS_InterfaceCfg;

};

