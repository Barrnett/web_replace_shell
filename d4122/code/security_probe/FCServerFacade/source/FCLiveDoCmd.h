//////////////////////////////////////////////////////////////////////////
//FCLiveDoCmd.h

#pragma once

#include "BaseDoCmd.h"
#include "cpf/CommonQueueManager.h"
#include "LMB_serial_class.h"

class CFCSockManager;
class CFCServerFacade;

class CFCLiveDoCmd : public CBaseDoCmd
{
public:
	CFCLiveDoCmd(void);
	~CFCLiveDoCmd(void);

public:
	BOOL init(ACE_Log_File_Msg* pLogInstance,
		CFCSockManager* pFCSockManager,
		CFCServerFacade *pFCServerFacade);

	void fini();

public:
	int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	//BOOL OnCmd_TransToFCProbe_Live(MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	void f_get_LAN_card_statuse(UINT32 lan_count, UINT8 * lan_statuse);
	int f_OnCmd_GetLanStatus(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	int f_OnCmd_GetE1Status(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock); // add by xumx, 2016-10-27
	int f_OnCmd_GetTunnelStatus(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock); // add by xumx, 2016-10-27
	int f_OnCmd_GetFlowStatistic(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);  // add by xumx, 2016-10-27
	int f_OnCmd_GetSysInfo(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);  // add by xumx, 2016-10-30
	int f_OnCmd_GetSoftVersion(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);  // add by xumx, 2016-12-8

private:
	ACE_Log_File_Msg*		m_pLogInstance;

public:
	CFCSockManager *	m_pFCSockManager;
	CFCServerFacade *	m_pFCServerFacade;

};

