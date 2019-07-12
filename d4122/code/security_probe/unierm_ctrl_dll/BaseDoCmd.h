//////////////////////////////////////////////////////////////////////////
//BaseDoCmd.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/ComImp.h"
#include "MonitorManagerConfig.h"

class CProbeServant;

class CBaseDoCmd
{
public:
	CBaseDoCmd(void);
	virtual ~CBaseDoCmd(void);

public:
	void init(ACE_Log_Msg* pLogInstance,
		PROBESERVICE_CONFIG_PARAM * pConfigInfo);

public:
	int test_simple_request(int operation,CProbeServant * pProbeServant);

protected:
	ACE_Log_Msg*					m_pLogInstance;
	PROBESERVICE_CONFIG_PARAM *		m_pConfigInfo;

};
