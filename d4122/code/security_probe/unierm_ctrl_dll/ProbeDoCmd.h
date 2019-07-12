//////////////////////////////////////////////////////////////////////////
//ProbeDoCmd.h

#pragma once


#include <string>
#include "cpf/NetcomBaseServant.h"
#include "ace/INET_Addr.h"
#include "BaseDoCmd.h"
#include "cpf/ostypedef.h"

class CMonitorManager;
class CProbeSysInfo;
class MonitorSvc;

class CProbeDoCmd : public CBaseDoCmd
{
public:
	CProbeDoCmd(void);
	~CProbeDoCmd(void);

	void init_ex(ACE_Log_Msg* pLogInstance,
		 CMonitorManager* pMonitorMgr,
		 MonitorSvc * pMonitorSvc,
		 PROBESERVICE_CONFIG_PARAM * pConfigInfo,
		 CProbeSysInfo *	pProbeSysInfo);

public:
	void OnCmd_StartProgram(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	void OnCmd_StopProgram(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	void OnCmd_RestartProgram(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock);

	void OnCmd_GetSysState(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	void OnCmd_GetSysTime(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	void OnCmd_SetSysTime(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	
	void OnCmd_GetProgramRunState(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);	

	void OnCmd_SetRebootTimeInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	void OnCmd_GetRebootTimeInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);	

public:
	int test_request(CProbeServant * pProbeServant);

private:
	//bTempState是不是临时的状态
	//如果是，则状态不会写入配置文件。下次监控程序启动的时候，还按照以前的配置启动
	//如果不是，则状态会写入配置文件。下次监控程序启动的时候，会按照现在的配置启动
	int ChangeConfigState(const char * prg_name,int state,BOOL bTempState);

	int start_probe(const char * prg_name,BOOL bTempState);
	int stop_probe(const char * prg_name,BOOL bTempState);

	size_t GetProbeSysInfo(MSG_BLOCK * pResponMsgBlock);
	size_t GetProbeSysTime(MSG_BLOCK * pResponMsgBlock);
//	int SetProbeSysTime(MSG_BLOCK * pReqMsgBlock);

	int ExtractSystemTime(MSG_BLOCK * pReqMsgBlock,SYSTEMTIME& sys_time);


private:
	int test_request_config_renew(CProbeServant * pProbeServant);

private:
	CMonitorManager	*	m_pMonitorMgr;
	//CProbeSysInfo *		m_pProbeSysInfo;
	MonitorSvc *		m_pMonitorSvc;

	ACE_UINT16			m_probe_server_port;
	int					m_probe_servant_id;

};
