//////////////////////////////////////////////////////////////////////////
//SockManager.h

#pragma once

#include "BaseDoCmd.h"
#include "MyConnectToServerMgr.h"

class CFCServerFacade;

class CFCSockManager
{
public:
	CFCSockManager();
	~CFCSockManager();

public:
	BOOL init(CFCServerFacade * pFCServerFacade);

	void fini();

private:
	void Inner_init(ACE_UINT32 fc_probe_ip,ACE_UINT16 fc_probe_port,
		int fc_probe_cmd_servantid,int fc_probe_live_servantid,
		ACE_UINT32 fc_monitor_ip,ACE_UINT16 fc_monitor_port,
		int fc_monitor_cmd_servantid,int fc_monitor_live_servantid);

public:
	int TransRequestToFCProbe_Cmd(ACE_UINT16 operation,MSG_BLOCK * pMsgBlock,
		MSG_BLOCK * & pRetMsgBlock,int ntimeout)
	{
		return CBaseDoCmd::TransRequestToServer(m_fc_probe_sock,
			m_fc_probe_cmd_servantid,
			operation,pMsgBlock,pRetMsgBlock,ntimeout);

	}

	int TransRequestToFCProbe_Live(ACE_UINT16 operation,MSG_BLOCK * pMsgBlock,
		MSG_BLOCK * & pRetMsgBlock,int ntimeout)
	{
		return CBaseDoCmd::TransRequestToServer(m_fc_probe_sock,
			m_fc_probe_live_servantid,
			operation,pMsgBlock,pRetMsgBlock,ntimeout);

	}

	int TransRequestToFCMonitor(ACE_UINT16 operation,MSG_BLOCK * pMsgBlock,
		MSG_BLOCK * & pRetMsgBlock,int ntimeout)
	{
		return CBaseDoCmd::TransRequestToServer(m_fc_monitor_sock,
			m_fc_monitor_servantid,
			operation,pMsgBlock,pRetMsgBlock,ntimeout);
	}

	int TransRequestToFCProbe_Cmd(
		MSG_BLOCK * pMsgBlock,
		MSG_BLOCK * & pRetMsgBlock,int ntimeout)
	{
		return TransRequestToFCProbe_Cmd(pMsgBlock->pPduHdr->nOperation,pMsgBlock,pRetMsgBlock,ntimeout);
	}

	int TransRequestToFCProbe_Live(
		MSG_BLOCK * pMsgBlock,
		MSG_BLOCK * & pRetMsgBlock,int ntimeout)
	{
		return TransRequestToFCProbe_Live(pMsgBlock->pPduHdr->nOperation,pMsgBlock,pRetMsgBlock,ntimeout);
	}

	int TransRequestToFCMonitor(
		MSG_BLOCK * pMsgBlock,
		MSG_BLOCK * & pRetMsgBlock,int ntimeout)
	{
		return TransRequestToFCMonitor(pMsgBlock->pPduHdr->nOperation,pMsgBlock,pRetMsgBlock,ntimeout);
	}

public:
	int SendNewVersionCfg_ToFCProbe(const char * config_file_name);

public:
	ACE_UINT32			m_fc_probe_ip;
	ACE_UINT16			m_fc_probe_port;
	int					m_fc_probe_cmd_servantid;
	int					m_fc_probe_live_servantid;

	ACE_UINT32			m_fc_monitor_ip;
	ACE_UINT16			m_fc_monitor_port;
	int					m_fc_monitor_servantid;

public:
	CFCConnectToServerMgr	m_fc_probe_sock;
	CConnectToServerMgr		m_fc_monitor_sock;

public:
	ACE_UINT32				m_manager_ip;

private:
	CFCServerFacade *	m_pFCServerFacade;

};


