//////////////////////////////////////////////////////////////////////////
//ProbeServant.h

#pragma once

#include "cpf/ComImp.h"
#include <string>
#include "MonitorTask.h"
#include "cpf/NetcomBaseServant.h"
#include "MonitorManagerConfig.h"

class CProbeDoCmd;

class CProbeServant : public CServantImp
{
public:
	CProbeServant();
	virtual ~CProbeServant(void);

	void init(ACE_Log_Msg* pLogInstance,
		CProbeDoCmd * pProbeDoCmd,
		PROBESERVICE_CONFIG_PARAM * pConfigInfo);

	int test_request();

public:
	virtual void OnNotify(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock);
	virtual int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:

	int				m_connect_nums;
	int				m_disconnect_nums;

private:
	CProbeDoCmd	*		m_probe_cmd;

	ACE_Log_Msg*					m_pLogInstance;
	PROBESERVICE_CONFIG_PARAM *		m_pConfigInfo;


};


class CProbeNetCom : public CNetServerWithOneServant<CProbeServant, CMyServerLogImp>
{
public:
	bool init(
		int port,int servantid,bool multithread,int nMaxClientNum,
		ACE_Time_Value * recv_timeout,
		ACE_Time_Value * send_timeout)
	{
		if( CNetServerWithOneServant<CProbeServant,CMyServerLogImp>::init(port,servantid,
			multithread,nMaxClientNum,
			recv_timeout,send_timeout) == false )
		{
			return false;
		}

		return true;
	}

	void set_param(ACE_Log_Msg* pLogInstance,
		CProbeDoCmd * pProbeDoCmd,
		PROBESERVICE_CONFIG_PARAM * pConfigInfo)
	{
		GetNetServer()->GetServer()->SetLogInstance(pLogInstance);

		this->GetServant()->init(pLogInstance,
			pProbeDoCmd,
			pConfigInfo);
	}


};

