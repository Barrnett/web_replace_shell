//////////////////////////////////////////////////////////////////////////
//ProbeServant .cpp

#include "StdAfx.h"
#include "ProbeServant.h"
#include "cpf/path_tools.h"
#include "FCLiveDoCmd.h"
#include "CConfigAndCtrlDoCmd.h"
#include "SysMaintenanceDoCmd.h"
#include "SockOperationId.h"
#include "config_center.h"

CProbeServant::CProbeServant()
{
	m_pLogInstance = NULL;

	m_log_put_msg = FALSE;

}

CProbeServant::~CProbeServant(void)
{

	return;
}

void CProbeServant::init(ACE_Log_File_Msg* pLogInstance,BOOL log_put_msg)
{
	m_pLogInstance = pLogInstance;
	m_log_put_msg = log_put_msg;

	return;
}


int CProbeServant::test_request()
{
#ifdef _DEBUG
	//test_simple_request(1,this);

	//test_simple_request(2,this);

	//test_simple_request(11,this);
	//test_simple_request(12,this);
#endif//_DEBUG

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//CCmdServant
//////////////////////////////////////////////////////////////////////////

CCmdServant::CCmdServant()
{
	m_pConfigAndCtrlDoCmd = NULL;
}

CCmdServant::~CCmdServant()
{
	if( m_pConfigAndCtrlDoCmd )
	{
		delete m_pConfigAndCtrlDoCmd;
		m_pConfigAndCtrlDoCmd = NULL;
	}

	return;
}

void CCmdServant::init(ACE_Log_File_Msg* pLogInstance,
					   CFCServerFacade * pFCServerFacade,
					   CFCSockManager* pFCSockManager)
{
	CProbeServant::init(pLogInstance,true);

	m_pConfigAndCtrlDoCmd = new CCConfigAndCtrlDoCmd;

	m_pConfigAndCtrlDoCmd->init(pLogInstance,pFCSockManager,pFCServerFacade);

	return;
}


int CCmdServant::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	if( (pMsgBlock->pPduHdr->nOperation > 1000) && (pMsgBlock->pPduHdr->nOperation < 2000) )
	{
		if( m_pConfigAndCtrlDoCmd )
		{
			return m_pConfigAndCtrlDoCmd->OnRequest(pRemoteAddr,pMsgBlock,pRetMsgBlock);
		}
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////
//CSysMaintenanceServant
//////////////////////////////////////////////////////////////////////////

CSysMaintenanceServant::CSysMaintenanceServant()
{
	m_pSysMaintenanceDoCmd = NULL;
}

CSysMaintenanceServant::~CSysMaintenanceServant()
{
	if( m_pSysMaintenanceDoCmd )
	{
		delete m_pSysMaintenanceDoCmd;
		m_pSysMaintenanceDoCmd = NULL;
	}

	return;
}

void CSysMaintenanceServant::init(ACE_Log_File_Msg* pLogInstance,
								  CFCSockManager* pFCSockManager,
								  CFCServerFacade * pFCServerFacade)
{
	CProbeServant::init(pLogInstance);

	m_pSysMaintenanceDoCmd = new CSysMaintenanceDoCmd;

	m_pSysMaintenanceDoCmd->init(pLogInstance,pFCSockManager,pFCServerFacade);

	return;
}


int CSysMaintenanceServant::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	if( (pMsgBlock->pPduHdr->nOperation >= 3000) && (pMsgBlock->pPduHdr->nOperation < 4000) )
	{
		if( m_pSysMaintenanceDoCmd )
		{
			return m_pSysMaintenanceDoCmd->OnRequest(pRemoteAddr,pMsgBlock,pRetMsgBlock);
		}
	}

	return -1;
}


//////////////////////////////////////////////////////////////////////////
//CLiveStatServant
//////////////////////////////////////////////////////////////////////////

CLiveStatServant::CLiveStatServant()
{
	m_pFCLiveDoCmd = NULL;
}

CLiveStatServant::~CLiveStatServant()
{
	if( m_pFCLiveDoCmd )
	{
		delete m_pFCLiveDoCmd;
		m_pFCLiveDoCmd = NULL;
	}

}

void CLiveStatServant::init(ACE_Log_File_Msg* pLogInstance,
							CFCSockManager* pFCSockManager,
							CFCServerFacade * pFCServerFacade)
{
	CProbeServant::init(pLogInstance);

	m_pFCLiveDoCmd = new CFCLiveDoCmd;

	m_pFCLiveDoCmd->init(pLogInstance,pFCSockManager,pFCServerFacade);
}


int CLiveStatServant::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	//业务实时
	if(pMsgBlock->pPduHdr->nDestId != LIVE_SERVANT_ID){ return(-1); }

	if( (pMsgBlock->pPduHdr->nOperation >= 2000) && (pMsgBlock->pPduHdr->nOperation < 3000) )
	{
		if( m_pFCLiveDoCmd )
		{
			return m_pFCLiveDoCmd->OnRequest(pRemoteAddr,pMsgBlock,pRetMsgBlock);
		}
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CCConfigAndCtrlDoCmd * CProbeNetCom::GetConfigAndCtrlDoCmd()
{
	if( m_pCmdServant )
		return m_pCmdServant->GetConfigAndCtrlDoCmd();
	else
		return NULL;
}

bool CProbeNetCom::init(
	int port,
	int cmd_servant_id,
	int live_stat_servant_id,		  
	int sys_maintance_servand_id,
	bool multithread,int nMaxClientNum,
	ACE_Time_Value * recv_timeout,
	ACE_Time_Value * send_timeout,
	ACE_Log_File_Msg * pLogMsg,
	CFCServerFacade * pFCServerFacade,
	CFCSockManager* pFCSockManager)
{
	if( CNetServerBaseServant<CMyServerLogImp>::init(port,			
		nMaxClientNum,
		recv_timeout,
		send_timeout) == false )
	{
		return false;
	}

	((CMyServerLogImp *)GetServer())->SetLogInstance( pLogMsg );

	{
		m_pCmdServant = new CCmdServant;
		m_pCmdServant->init(pLogMsg,pFCServerFacade,pFCSockManager);

		m_pCmdServant->SetServantId(cmd_servant_id);
		m_pCmdServant->SetServantMultithread(multithread);

		AttachServant(m_pCmdServant);//CNetServerBaseServant<CMyServerLogImp>::AttachServant

		m_pCmdServant->Open();
	}

	{
		m_pSysMaintenanceServant = new CSysMaintenanceServant;
		m_pSysMaintenanceServant->init(pLogMsg,pFCSockManager,pFCServerFacade);

		m_pSysMaintenanceServant->SetServantId(sys_maintance_servand_id);
		m_pSysMaintenanceServant->SetServantMultithread(multithread);

		AttachServant(m_pSysMaintenanceServant);

		m_pSysMaintenanceServant->Open();
	}

	{
		m_pLiveStatServant = new CLiveStatServant;
		m_pLiveStatServant->init(pLogMsg,pFCSockManager,pFCServerFacade);

		m_pLiveStatServant->SetServantId(live_stat_servant_id);
		m_pLiveStatServant->SetServantMultithread(multithread);

		AttachServant(m_pLiveStatServant);

		m_pLiveStatServant->Open();
	}

	return true;
}

void CProbeNetCom::close()
{
	if( m_pCmdServant )
	{
		m_pCmdServant->Close();
		DetachServant(m_pCmdServant);
		delete m_pCmdServant;
		m_pCmdServant = NULL;
	}

	if( m_pSysMaintenanceServant )
	{
		m_pSysMaintenanceServant->Close();
		DetachServant(m_pSysMaintenanceServant);
		delete m_pSysMaintenanceServant;
		m_pSysMaintenanceServant = NULL;
	}

	if( m_pLiveStatServant )
	{
		m_pLiveStatServant->Close();
		DetachServant(m_pLiveStatServant);
		delete m_pLiveStatServant;
		m_pLiveStatServant = NULL;
	}

	return;
}
