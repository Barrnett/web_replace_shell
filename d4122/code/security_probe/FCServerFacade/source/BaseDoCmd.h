//////////////////////////////////////////////////////////////////////////
//BaseDoCmd.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/ComImp.h"
#include "cpf/ConnectToServerMgr.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "sstream"

class CProbeServant;

class CBaseDoCmd
{
public:
	CBaseDoCmd(void);
	virtual ~CBaseDoCmd(void);

public:
	void init(ACE_Log_File_Msg* pLogInstance);

	static int TransRequestToServer(ACE_UINT32 server,ACE_UINT16 port,int servantid,
		ACE_UINT16 operation,MSG_BLOCK * pMsgBlock,
		MSG_BLOCK * & pRetMsgBlock,int ntimeout);

	static int TransRequestToServer(CConnectToServerMgr& sock_mgr,int servantid,
		ACE_UINT16 operation,MSG_BLOCK * pMsgBlock,
		MSG_BLOCK * & pRetMsgBlock,int ntimeout);

public:
	int test_simple_request(int operation,CProbeServant * pProbeServant);

protected:
	ACE_Log_File_Msg*					m_pLogInstance;

};
