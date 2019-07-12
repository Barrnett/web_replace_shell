//////////////////////////////////////////////////////////////////////////
//CConfigAndCtrlDoCmd.h

#pragma once

#include "cpf/ACE_Log_File_Msg.h"
#include "cpf/ComImp.h"

class CFluxControlObserver;

class CCConfigAndCtrlDoCmd
{
public:
	CCConfigAndCtrlDoCmd(void);
	~CCConfigAndCtrlDoCmd(void);

public:
	void init(ACE_Log_File_Msg * pLogInstance,
		CFluxControlObserver *	pFluxControlObserver);
	void close();

public:
	int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	BOOL OnCmd_SetCfgFile(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_GetCfgFile(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_clear_service_statistic(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_EnableMgmtTunnel(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_DisableMgmtTunnel(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_ApplyUpdateCfg(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_CheckUpdateCfg(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);

private:
	ACE_Log_File_Msg *			m_pLogInstance;

};
