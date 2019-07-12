//////////////////////////////////////////////////////////////////////////
//BaseDoCmd.cpp

#include "StdAfx.h"
#include "BaseDoCmd.h"
#include "cpf/Data_Stream.h"
#include "ProbeServant.h"
#include "cpf/TinyXmlEx.h"

CBaseDoCmd::CBaseDoCmd(void)
{
	m_pLogInstance = NULL;
}

CBaseDoCmd::~CBaseDoCmd(void)
{
}


void CBaseDoCmd::init(ACE_Log_File_Msg* pLogInstance)
{
	m_pLogInstance = pLogInstance;

	return;
}

//将消息转发给服务器。
int CBaseDoCmd::TransRequestToServer(ACE_UINT32 server,ACE_UINT16 port,int servantid,
										ACE_UINT16 operation,MSG_BLOCK * pMsgBlock,
										MSG_BLOCK * & pRetMsgBlock,int ntimeout)
{
	pRetMsgBlock = NULL;

	//连接服务器
	CNetClientWithOneProxy<CProxyImp>	client;

	if( !client.init(server,port) )
	{
		return -1;
	}

	client.start();

	int nrtn = 0;

	//向服务器发出请求
	MSG_BLOCK * pMsgBlock_Response_From_Server = NULL;

	do{

		if( 0 != client.GetProxy()->SendRequest(servantid,operation,
			pMsgBlock->pBuffer,pMsgBlock->nDataLen,
			pMsgBlock_Response_From_Server,ntimeout) )
		{
			nrtn = -2;
			break;
		}

		//直接将服务器返回的数据返回给界面
		MSG_BLOCK * pMsgBlock_Response_To_Client = CMsgBlockManager::Malloc_Rsp(pMsgBlock,
			pMsgBlock_Response_From_Server->nDataLen,
			pMsgBlock_Response_From_Server->pPduHdr->nResponseCode);

		if( !pMsgBlock_Response_To_Client )
		{
			nrtn = -3;
			break;
		}

		char * pdata_response_to_client = CMsgBlockHelper_Stream::WritePtr(
			*pMsgBlock_Response_To_Client,0,
			pMsgBlock_Response_From_Server->nDataLen);

		CData_Stream_LE::PutFixString(pdata_response_to_client,
			pMsgBlock_Response_From_Server->nDataLen,
			pMsgBlock_Response_From_Server->pBuffer);

		pRetMsgBlock = pMsgBlock_Response_To_Client;

	}while(0);

	if( pMsgBlock_Response_From_Server )
	{
		CMsgBlockManager::Free(pMsgBlock_Response_From_Server);
	}

	client.stop();
	client.close();

	return nrtn;
}

int CBaseDoCmd::TransRequestToServer(CConnectToServerMgr& sock_mgr,int servantid,
									ACE_UINT16 operation,MSG_BLOCK * pMsgBlock,
									MSG_BLOCK * & pRetMsgBlock,int ntimeout)
{
	pRetMsgBlock = NULL;
	
	int nrtn = 0;

	//向服务器发出请求
	MSG_BLOCK * pMsgBlock_Response_From_Server = NULL;

	do{

		if( 0 != sock_mgr.SendRequest(servantid,operation,
			pMsgBlock->pBuffer,pMsgBlock->nDataLen,
			pMsgBlock_Response_From_Server,ntimeout) )
		{
			nrtn = -2;
			break;
		}

		//直接将服务器返回的数据返回给界面
		MSG_BLOCK * pMsgBlock_Response_To_Client = CMsgBlockManager::Malloc_Rsp(pMsgBlock,
			pMsgBlock_Response_From_Server->nDataLen,
			pMsgBlock_Response_From_Server->pPduHdr->nResponseCode);

		if( !pMsgBlock_Response_To_Client )
		{
			nrtn = -3;
			break;
		}

		char * pdata_response_to_client = CMsgBlockHelper_Stream::WritePtr(
			*pMsgBlock_Response_To_Client,0,
			pMsgBlock_Response_From_Server->nDataLen);

		CData_Stream_LE::PutFixString(pdata_response_to_client,
			pMsgBlock_Response_From_Server->nDataLen,
			pMsgBlock_Response_From_Server->pBuffer);

		pRetMsgBlock = pMsgBlock_Response_To_Client;

	}while(0);

	if( pMsgBlock_Response_From_Server )
	{
		CMsgBlockManager::Free(pMsgBlock_Response_From_Server);
	}

	return nrtn;
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
		const char * pdata_rsp = CMsgBlockHelper_Stream::ReadPtr(*pResponseMsgBlock,0,pResponseMsgBlock->nDataLen);
	}

	CMsgBlockManager::Free(pRequstMsgBlock);
	CMsgBlockManager::Free(pResponseMsgBlock);

	return 0;
}
