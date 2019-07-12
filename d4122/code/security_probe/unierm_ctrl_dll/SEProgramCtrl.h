//////////////////////////////////////////////////////////////////////////
//SEProgramCtrl.h

#pragma once

#include "MonitorManagerConfig.h"
#include "ProbeDoCmd.h"

class CProbeSysInfo;
class MonitorSvc;

class CSEProgramCtrl
{
public:
	CSEProgramCtrl(void);
	~CSEProgramCtrl(void);

public:
	void init(ACE_Log_Msg* pLogInstance,
		CMonitorManager * pMonitorMgr,
		MonitorSvc * pMonitorSvc);

public:
	CProbeDoCmd		m_probe_cmd;

public:
	void RefreshSysInfo();

public:
	PROBESERVICE_CONFIG_PARAM		m_config_param;

public:
	// CProbeSysInfo *		m_pProbeSysInfo;

private:
	CMonitorManager	*	m_pMonitorMgr;
	MonitorSvc *		m_pMonitorSvc;

	ACE_Log_Msg*		m_pLogInstance;

private:
	BOOL ReadProbeServiceConfigParam();

};

