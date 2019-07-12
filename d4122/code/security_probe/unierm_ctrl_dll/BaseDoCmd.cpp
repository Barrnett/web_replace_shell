//////////////////////////////////////////////////////////////////////////
//BaseDoCmd.cpp

#include "stdafx.h"

#include "BaseDoCmd.h"
//#include "ProbeSysInfo.h"
#include "cpf/Data_Stream.h"
#include "ProbeServant.h"

CBaseDoCmd::CBaseDoCmd(void)
{
	m_pLogInstance = NULL;

	m_pConfigInfo = NULL;
}

CBaseDoCmd::~CBaseDoCmd(void)
{
}


void CBaseDoCmd::init(ACE_Log_Msg* pLogInstance,
					  PROBESERVICE_CONFIG_PARAM * pConfigInfo)
{
	m_pLogInstance = pLogInstance;

	m_pConfigInfo = pConfigInfo;

	return;
}


int CBaseDoCmd::test_simple_request(int operation,CProbeServant * pProbeServant)
{
	if( !pProbeServant )
		return -1;

	MSG_BLOCK * pRequstMsgBlock = CMsgBlockManager::Malloc(4);

	char * pdata = CMsgBlockManager::WritePtr(*pRequstMsgBlock);

	CData_Stream_LE::PutUint32(pdata,0);

	pRequstMsgBlock->pPduHdr->nOperation = operation;

	MSG_BLOCK * pResponseMsgBlock = NULL;

	ACE_INET_Addr addr(8001,"127.0.0.1");

	pProbeServant->OnRequest(&addr,pRequstMsgBlock,pResponseMsgBlock);

	if( pResponseMsgBlock )
	{
		/*const char * pdata_rsp = */CMsgBlockHelper_Stream::ReadPtr(*pResponseMsgBlock,0,pResponseMsgBlock->nDataLen);
	}

	CMsgBlockManager::Free(pRequstMsgBlock);
	CMsgBlockManager::Free(pResponseMsgBlock);

	return 0;
}
