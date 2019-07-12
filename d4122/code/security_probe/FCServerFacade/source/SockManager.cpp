//////////////////////////////////////////////////////////////////////////
//SockManager.cpp

#include "StdAfx.h"
#include "SockManager.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/path_tools.h"

#include "config_fc_server_xml.h"
#include "FCServerFacade.h"
#include "SockOperationId.h"

//////////////////////////////////////////////////////////////////////////
//CFCSockManager
//////////////////////////////////////////////////////////////////////////

CFCSockManager::CFCSockManager()
{
	m_pFCServerFacade = NULL;

	m_manager_ip = 0;
}

CFCSockManager::~CFCSockManager()
{
}

BOOL CFCSockManager::init(CFCServerFacade * pFCServerFacade)
{
	m_pFCServerFacade = pFCServerFacade;

	Inner_init(FCServerConfig_Obj()->fc_probe.ip,
				FCServerConfig_Obj()->fc_probe.port,
				FCServerConfig_Obj()->fc_probe.cmd_servant_id,
				FCServerConfig_Obj()->fc_probe.live_servant_id,
				FCServerConfig_Obj()->fc_monitor.ip,
				FCServerConfig_Obj()->fc_monitor.port,
				FCServerConfig_Obj()->fc_monitor.cmd_servant_id,
				FCServerConfig_Obj()->fc_monitor.live_servant_id);

	return true;
}

void CFCSockManager::Inner_init(ACE_UINT32 fc_probe_ip,ACE_UINT16 fc_probe_port,
								int fc_probe_cmd_servantid,int fc_probe_live_servantid,
								ACE_UINT32 fc_monitor_ip,ACE_UINT16 fc_monitor_port,
								int fc_monitor_cmd_servantid,int fc_monitor_live_servantid)
{
	m_fc_probe_ip = fc_probe_ip;
	m_fc_probe_port = fc_probe_port;
	m_fc_probe_cmd_servantid = fc_probe_cmd_servantid;
	m_fc_probe_live_servantid = fc_probe_live_servantid;

	m_fc_monitor_ip = fc_monitor_ip;
	m_fc_monitor_port = fc_monitor_port;
	m_fc_monitor_servantid = fc_monitor_cmd_servantid;

	m_fc_probe_sock.init(fc_probe_ip,fc_probe_port,5);
	m_fc_probe_sock.SetFCServerFacade(m_pFCServerFacade);
	m_fc_probe_sock.ConnectToServer();
	m_fc_probe_sock.StartReConnectMonitor();

	m_fc_monitor_sock.init(fc_monitor_ip,fc_monitor_port,5);
	m_fc_monitor_sock.ConnectToServer();
	m_fc_monitor_sock.StartReConnectMonitor();

	return;
}


void CFCSockManager::fini()
{
	m_fc_probe_sock.close();
	m_fc_monitor_sock.close();

	return;
}

BOOL CFCSockManager::SendNewVersionCfg_ToFCProbe(const char * config_file_name)
{
	char buf_reqeust[1024];

	char * pdata = buf_reqeust;

	CData_Stream_LE::PutString(pdata, config_file_name, 4);

	MSG_BLOCK * pRspBlockMsg = NULL;

	int nrtn = m_fc_probe_sock.SendRequest(m_fc_probe_cmd_servantid,
		OP_ID_SET_CONFIG_FILE,
		buf_reqeust, (int)(pdata - buf_reqeust), 
		pRspBlockMsg,10);

	if (pRspBlockMsg)
	{
		CMsgBlockManager::Free(pRspBlockMsg);
	}

	return (nrtn == 0);

}


