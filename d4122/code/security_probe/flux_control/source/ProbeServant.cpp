//////////////////////////////////////////////////////////////////////////
//ProbeServant.cpp

#include "stdafx.h"
#include "ProbeServant.h"
#include "cpf/path_tools.h"
#include "FluxControlObserver.h"
#include "SockOperationId.h"
#include "config_center.h"


//////////////////////////////////////////////////////////////////////////
//CCmdServant
//////////////////////////////////////////////////////////////////////////

CCmdServant::CCmdServant()
{
	m_pLogInstance = NULL;
}

CCmdServant::~CCmdServant(void)
{
	m_ConfigAndCtrlDoCmd.close();
	return;
}

void CCmdServant::init(ACE_Log_File_Msg* pLogInstance,
					   CFluxControlObserver * pFluxControlObserver)
{

	m_pLogInstance = pLogInstance;

	m_ConfigAndCtrlDoCmd.init(pLogInstance,pFluxControlObserver);

	return;
}


int CCmdServant::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	if( (pMsgBlock->pPduHdr->nOperation > 1000) && (pMsgBlock->pPduHdr->nOperation < 2000) )
	{
		return m_ConfigAndCtrlDoCmd.OnRequest(pRemoteAddr,pMsgBlock,pRetMsgBlock);
	}

	{
		char buf[64] = {0};

		pRemoteAddr->addr_to_string(buf,sizeof(buf));

		MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Err][%D]recv %u cmd from '%s' is not defined!!!\r\n"),pMsgBlock->pPduHdr->nOperation,buf));

		//ACE_ASSERT(FALSE);

		return -1;
	}

}

//////////////////////////////////////////////////////////////////////////
//CLiveStatServant
//////////////////////////////////////////////////////////////////////////
CLiveStatServant::CLiveStatServant()
{
	m_pLogInstance = NULL;
}

CLiveStatServant::~CLiveStatServant(void)
{
	m_FCLiveDoCmd.close();

	return;
}

void CLiveStatServant::init(ACE_Log_File_Msg* pLogInstance,
					   CFluxControlObserver * pFluxControlObserver)
{
	m_pLogInstance = pLogInstance;

	m_FCLiveDoCmd.init(pLogInstance);

	return;
}


int CLiveStatServant::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	if( (pMsgBlock->pPduHdr->nOperation >= 2000) && (pMsgBlock->pPduHdr->nOperation < 3000) )
	{

		//在这个里面会读取app_tree的信息，另外一个线程（命令）会修改app_tree
		int nrtn = m_FCLiveDoCmd.OnRequest(pRemoteAddr,pMsgBlock,pRetMsgBlock);


		return nrtn;
	}

	{
		char buf[64] = {0};

		pRemoteAddr->addr_to_string(buf,sizeof(buf));

		MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Err][%D]recv %u cmd from '%s' is not defined!!!\r\n"),pMsgBlock->pPduHdr->nOperation,buf));

		//ACE_ASSERT(FALSE);

		return -1;
	}

}

//////////////////////////////////////////////////////////////////////////
//CProbeNetCom
//////////////////////////////////////////////////////////////////////////

CProbeNetCom::CProbeNetCom()
{
	m_pCmdServant = NULL;
	m_pLiveStatServant = NULL;
}

CProbeNetCom::~CProbeNetCom()
{
	close();
}


bool CProbeNetCom::init(
		  int port,int cmd_servantid,int live_servantid,bool multithread,int nMaxClientNum,
		  ACE_Time_Value * recv_timeout,
		  ACE_Time_Value * send_timeout,
		  CFluxControlObserver * pFluxControlObserver,
		  ACE_Log_File_Msg * pLogMsg)
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
		m_pCmdServant->init(pLogMsg,pFluxControlObserver);

		m_pCmdServant->SetServantId(cmd_servantid);
		m_pCmdServant->SetServantMultithread(multithread);

		AttachServant(m_pCmdServant);

		m_pCmdServant->Open();
	}


	{
		m_pLiveStatServant = new CLiveStatServant;
		m_pLiveStatServant->init(pLogMsg,pFluxControlObserver);

		m_pLiveStatServant->SetServantId(live_servantid);
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

	if( m_pLiveStatServant )
	{
		m_pLiveStatServant->Close();
		DetachServant(m_pLiveStatServant);
		delete m_pLiveStatServant;
		m_pLiveStatServant = NULL;
	}

	return;
}
