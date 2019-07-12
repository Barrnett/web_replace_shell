//////////////////////////////////////////////////////////////////////////
//FCLiveCmd.h

#pragma once

#include "cpf/ComImp.h"
#include "cpf/ACE_Log_File_Msg.h"

class CFCLiveDoCmd
{
public:
	CFCLiveDoCmd(void);
	~CFCLiveDoCmd(void);

	void init(ACE_Log_File_Msg* pLogInstance);
	void close();

public:
	int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	ACE_Log_File_Msg* m_pLogInstance;

private:
	int f_OnCmd_GetE1Stat(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	int f_OnCmd_GetTunnelStatus(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock); // add by xumx, 2016-10-30
	int f_OnCmd_GetFlowStatistic(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);  // add by xumx, 2016-10-28
};
