//////////////////////////////////////////////////////////////////////////
//MyConnectToServerMgr.cpp

#include "StdAfx.h"
#include "MyConnectToServerMgr.h"
#include "FCServerFacade.h"

CMyConnectToServerMgr::CMyConnectToServerMgr(void)
{
	m_pFCServerFacade = NULL;
}

CMyConnectToServerMgr::~CMyConnectToServerMgr(void)
{
}

void CMyConnectToServerMgr::SetFCServerFacade(CFCServerFacade * pFCServerFacade)
{
	m_pFCServerFacade = pFCServerFacade;

	return;
}

int CMyConnectToServerMgr::SendRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout)
{
	int nrtn = CConnectToServerMgr::SendRequest(servantid,operation,pdata,ndatalen,pResponseMsgBlock,nTimeout);

	/*2017-3-10 deleted by zhongjh. nResponseCode非零的报文直接返回给client
	if( nrtn == 0 )
	{
		if( pResponseMsgBlock->pPduHdr->nResponseCode != RESPONSE_CODE_SUCCESS )
		{
			CMsgBlockHelper_Stream::Free(pResponseMsgBlock);
			pResponseMsgBlock = NULL;

			nrtn = -1;
		}
	}*/

	return nrtn;
}

//user interface
int CMyConnectToServerMgr::SendRequest(int servantid,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pResponseMsgBlock, int nTimeout)
{
	int nrtn = CConnectToServerMgr::SendRequest(servantid,pMsgBlock,pResponseMsgBlock,nTimeout);

	/*2017-3-10 deleted by zhongjh. nResponseCode非零的报文直接返回给client
	if( nrtn == 0 )
	{
		if( pResponseMsgBlock->pPduHdr->nResponseCode != RESPONSE_CODE_SUCCESS )
		{
			CMsgBlockHelper_Stream::Free(pResponseMsgBlock);
			pResponseMsgBlock = NULL;

			nrtn = -1;
		}
	}*/

	return nrtn;
}



//////////////////////////////////////////////////////////////////////////
//CFCConnectToServerMgr
//////////////////////////////////////////////////////////////////////////



CFCConnectToServerMgr::CFCConnectToServerMgr()
{
}

CFCConnectToServerMgr::~CFCConnectToServerMgr()
{

}

BOOL CFCConnectToServerMgr::IsSockReady()
{
	if( !IsConnectedToServer() )
		return false;

	return true;
}

BOOL CFCConnectToServerMgr::OnBeforeReConnect()
{
	return true;
}

void CFCConnectToServerMgr::OnAfterReconnect(BOOL bReconnectOK)
{
	if( bReconnectOK )
	{
		if( m_pFCServerFacade )
		{
			m_pFCServerFacade->OnFCReconnectedToServer(bReconnectOK,this);
		}
	}


	return;
}

int CFCConnectToServerMgr::SendRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout)
{
	if( !IsSockReady() )
		return -1;

	return CMyConnectToServerMgr::SendRequest(servantid,operation,pdata,ndatalen,pResponseMsgBlock,nTimeout);

}

//user interface
int CFCConnectToServerMgr::SendRequest(int servantid,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pResponseMsgBlock, int nTimeout)
{
	if( !IsSockReady() )
		return -1;

	return CMyConnectToServerMgr::SendRequest(servantid,pMsgBlock,pResponseMsgBlock,nTimeout);

}


int CFCConnectToServerMgr::SendSyncRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout)
{
	return CMyConnectToServerMgr::SendRequest(servantid,operation,pdata,ndatalen,pResponseMsgBlock,nTimeout);

}




