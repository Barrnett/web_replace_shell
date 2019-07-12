#include <assert.h>

#include "ace/OS_NS_time.h"
#include "cpf/ComImp.h"
#include "cpf/memmgr.h"


MSG_BLOCK * CMsgBlockManager::Malloc(int nBufferSize){
	if(nBufferSize<0) return NULL;
	
	//MSG_BLOCK * pMsgBlock=(MSG_BLOCK *)malloc(sizeof(MSG_BLOCK));
	MSG_BLOCK * pMsgBlock=(MSG_BLOCK *)CPF::MyMemAlloc(sizeof(MSG_BLOCK));

	if(pMsgBlock==NULL) return NULL;

	//char * pBuffer=(char *)malloc(nBufferSize+sizeof(PDU_HDR));
	char * pBuffer=(char *)CPF::MyMemAlloc(nBufferSize+sizeof(PDU_HDR));
	if(pBuffer==NULL) {
		//free(pMsgBlock);
		CPF::MyMemFree(pMsgBlock);
		return NULL;
	}

	pMsgBlock->pPduHdr=(PDU_HDR *)pBuffer;
	pMsgBlock->pBuffer=pBuffer+sizeof(PDU_HDR);
	pMsgBlock->nBufferSize=nBufferSize;
	pMsgBlock->nDataLen=0;
	return pMsgBlock;
}

//根据请求的块，来生成应当块
MSG_BLOCK * CMsgBlockManager::Malloc_Rsp(MSG_BLOCK * pReqMsgBlock,int nBufferSize,ACE_UINT16 nResponseCode)
{
	MSG_BLOCK * pMsgBlock = CMsgBlockManager::Malloc(nBufferSize);

	if( pMsgBlock )
	{
		pMsgBlock->pPduHdr->nResponseCode = nResponseCode;

		pMsgBlock->pPduHdr->nOperation = pReqMsgBlock->pPduHdr->nOperation;

		pMsgBlock->pPduHdr->nDestId = pReqMsgBlock->pPduHdr->nSrcId;
		pMsgBlock->pPduHdr->nSrcId = pReqMsgBlock->pPduHdr->nDestId;

		pMsgBlock->pPduHdr->nRequestSeq = pReqMsgBlock->pPduHdr->nRequestSeq;

		pMsgBlock->pPduHdr->nPduType = PDU_TYPE_RESPONSE;
	}

	return pMsgBlock;
}


int CMsgBlockManager::ReSize(MSG_BLOCK * pMsgBlock,int nBufferSize){
	if(nBufferSize<0) return COM_MEM;

	char * pBuffer=(char *)CPF::MyMemReAlloc(pMsgBlock->pPduHdr,nBufferSize+sizeof(PDU_HDR));
	//char * pBuffer=(char *)realloc(pMsgBlock->pPduHdr,nBufferSize+sizeof(PDU_HDR));
	
	if(pBuffer==NULL) return COM_MEM;

	pMsgBlock->pPduHdr=(PDU_HDR *)pBuffer;
	pMsgBlock->pBuffer=pBuffer+sizeof(PDU_HDR);
	pMsgBlock->nBufferSize=nBufferSize;

	return COM_SUCCESS;
}

int CMsgBlockManager::AddSize(MSG_BLOCK * pMsgBlock,int nAddBufferSize)
{
	return ReSize(pMsgBlock,pMsgBlock->nBufferSize + nAddBufferSize);
}

int CMsgBlockManager::Free(MSG_BLOCK * pMsgBlock)
{
	if( pMsgBlock )
	{
		//free(pMsgBlock->pPduHdr);
		CPF::MyMemFree(pMsgBlock->pPduHdr);
		//free(pMsgBlock);
		CPF::MyMemFree(pMsgBlock);
	}


	return COM_SUCCESS;
}


const char * CMsgBlockManager::ReadPtr(MSG_BLOCK& msgBlock,int nOffset,int nSize)
{
	if( nOffset < 0 || nSize < 0  )
	{
		assert(false);
		return NULL;
	}

	if(nOffset+nSize > msgBlock.nDataLen)
	{
		return NULL;
	}

	return msgBlock.pBuffer+nOffset;
}

char * CMsgBlockManager::WritePtr(MSG_BLOCK& msgBlock,int nOffset,int nSize)
{
	if( nOffset < 0 || nSize < 0  )
	{
		assert(false);
		return NULL;
	}

	if(nOffset+nSize > msgBlock.nBufferSize)
	{
		int nRet=CMsgBlockManager::ReSize(&msgBlock,nOffset+nSize +CMsgBlockManager::DEFAULT_BUFFER_SIZE);

		if(nRet!=COM_SUCCESS) return NULL;
	}

	msgBlock.nDataLen=mymax(msgBlock.nDataLen,(int)(nOffset+nSize));

	return msgBlock.pBuffer+nOffset;
}


//////////////////////////////////////////////////////////////////////////
//CServantImp
//////////////////////////////////////////////////////////////////////////

int CServantImp::SetServantId(int nServantId) {
	assert(nServantId>=0);
	if(m_nServantId==-1){
		m_nServantId=nServantId;
		return COM_SUCCESS;
	}
	return COM_SERVANT_SETID_TWICE;
}

int CServantImp::Open(void)
{
	if( m_bmultithread )
	{
		return activate();
	}

	return 0;
}

int CServantImp::Close(void){

	if( m_bmultithread )
	{
		MSG_DATA msgData;
		msgData.nMsgType=MSG_DATA::MSG_TYPE_STOP;
		msgData.pConnection=NULL;
		msgData.pMsgBlock=NULL;

		put_msg_data(msgData);

		return wait();
	}
	else
	{
		return 0;
	}
}

int CServantImp::svc(void)
{
	while(1)
	{
		ACE_Message_Block * mb =NULL;

		int nRet=getq(mb);

		if(nRet==-1) 
			break;

		if( -1 == handle_msg_block(mb) )
			break;
	}

	//get all message
	while(1)
	{
		ACE_Time_Value timeOut;

		ACE_Message_Block * mb =NULL;

		int nRet=getq(mb,&timeOut);
		if(nRet==-1) 
			break;
	
		MSG_DATA * pMsgData=(MSG_DATA *)mb->rd_ptr();
		if(pMsgData->pMsgBlock!=NULL)
			CMsgBlockManager::Free(pMsgData->pMsgBlock);
		if(pMsgData->pConnection!=NULL)
			pMsgData->pConnection->remove_reference();
		mb->release();
	}
	return 0;
}

int	CServantImp::put_msg_data(MSG_DATA& MsgData, ACE_Time_Value *timeout)
{
	if( m_bmultithread )
	{
		ACE_Message_Block * mb=new ACE_Message_Block(sizeof(MSG_DATA));

		if( !mb )
		{
			return -1;
		}
		
		memcpy(mb->wr_ptr(),&MsgData,sizeof(MSG_DATA));

		return putq(mb,timeout);
	}
	else
	{
		handle_msg_data(MsgData);
	}

	return 0;
}

int	CServantImp::handle_msg_data(MSG_DATA& MsgData)
{
	switch(MsgData.nMsgType)
	{
	case MSG_DATA::MSG_TYPE_STOP:
		return -1;
		break;

	case MSG_DATA::MSG_TYPE_CLOSE:
		OnClose(MsgData.pConnection->GetRemoteAddr());
		MsgData.pConnection->remove_reference();
		return 0;
		break;
	
	case MSG_DATA::MSG_TYPE_CONNECTED_SUCCESS:
		on_connect_success(MsgData.pConnection->GetRemoteAddr());
		MsgData.pConnection->remove_reference();
		return 0;
		break;

	case MSG_DATA::MSG_TYPE_CONNECTED_FAILED:
		{
			const char * pdata = CMsgBlockManager::ReadPtr(*MsgData.pMsgBlock,0,sizeof(int));
			int reason = (int)CData_Stream_LE::GetUint32(pdata);

			on_connect_failed(reason,MsgData.pConnection->GetRemoteAddr());
		}
		break;

	case MSG_DATA::MSG_TYPE_DATA:
		{
			switch(MsgData.pMsgBlock->pPduHdr->nPduType)
			{
			case PDU_TYPE_NOTIFY:
				{
					OnNotify(MsgData.pConnection->GetRemoteAddr(),MsgData.pMsgBlock);
				}
				break;
			case PDU_TYPE_REQUEST:
				{
					ACE_UINT32 begin_tick = GetTickCount();

					MY_ACE_DEBUG(m_pInnerLogInstance,
						(LM_INFO,
						ACE_TEXT("[Info][%D]before OnRequest:%d\n"),
						MsgData.pMsgBlock->pPduHdr->nOperation
						));

					MSG_BLOCK * pRetMsgBlock = NULL;

					int	nResponseCode=OnRequest(MsgData.pConnection->GetRemoteAddr(),MsgData.pMsgBlock,pRetMsgBlock);

					ACE_UINT32 end_tick = GetTickCount();

					if( end_tick - begin_tick > 5*1000 )
					{
						MY_ACE_DEBUG(m_pInnerLogInstance,
							(LM_INFO,
							ACE_TEXT("[Info][%D]after OnRequest:%d,spend_time=%d(ms)\n"),
							MsgData.pMsgBlock->pPduHdr->nOperation,end_tick-begin_tick
							));
					}
					else
					{
						MY_ACE_DEBUG(m_pInnerLogInstance,
							(LM_INFO,
							ACE_TEXT("[Info][%D]after OnRequest:%d,spend_time=%d(ms)\n"),
							MsgData.pMsgBlock->pPduHdr->nOperation,end_tick-begin_tick
							));
					}
						

					if(nResponseCode!=RESPONSE_CODE_SUCCESS)
					{
						if( !pRetMsgBlock )
						{
							pRetMsgBlock=CMsgBlockManager::Malloc(0);
						}
					}
					else
					{
						assert(pRetMsgBlock);
					}

					if( pRetMsgBlock )
					{
						PDU_HDR * pPduHdr=pRetMsgBlock->pPduHdr;
						PDU_HDR * pRetPduHdr=MsgData.pMsgBlock->pPduHdr;

						pPduHdr->nDestId=pRetPduHdr->nSrcId;
						pPduHdr->nSrcId=pRetPduHdr->nDestId;
						pPduHdr->nPduType=PDU_TYPE_RESPONSE;
						pPduHdr->nRequestSeq=pRetPduHdr->nRequestSeq;
						pPduHdr->nOperation=pRetPduHdr->nOperation;
						pPduHdr->nResponseCode=nResponseCode;

						MsgData.pConnection->SendPdu(pRetMsgBlock,MsgData.pConnection->get_send_timeout());

						CMsgBlockManager::Free(pRetMsgBlock);
					}
				}
				break;//end case MSG_DATA::PDU_TYPE_REQUEST:

			default:
				assert(0);
				break;
			}//end switch(MsgData.pMsgBlock->pPduHdr->nPduType)
		}
		break;//end case MSG_DATA::MSG_TYPE_DATA:

	default:
		assert(0);
		break;

	}//end switch(MsgData.nMsgType)

	MsgData.pConnection->remove_reference();
	CMsgBlockManager::Free(MsgData.pMsgBlock);

	return 0;
}


//返回-1表示要退出处理
int	CServantImp::handle_msg_block(ACE_Message_Block *mb)
{
	MSG_DATA * pMsgData=(MSG_DATA *)mb->rd_ptr();
	
	int nrtn = handle_msg_data(*pMsgData);

	mb->release();

	return nrtn;
}

//////////////////////////////////////////////////////////////////////////
//CProxyImp
//////////////////////////////////////////////////////////////////////////

CProxyImp::CProxyImp()
:m_condition(m_requestMutex)
{
	m_pMsgBlock = NULL;
	m_nProxyId=-1;
	m_pConnectionImp=NULL;
	m_nRequestSeq=0;
	m_bRequest=false;
}
CProxyImp::~CProxyImp()
{ 
	m_requestMutex.acquire();

	if(m_pConnectionImp!=NULL) 
	{
		m_pConnectionImp->DetachProxy(this);
	}

	if(m_bRequest)
	{
		m_condition.signal();
	}

	m_requestMutex.release();

	if( m_pMsgBlock )
	{//在超时的时候可能导致内存泄露，因此在最后要释放内存．

		CMsgBlockManager::Free(m_pMsgBlock);
		m_pMsgBlock = NULL;
	}

	//ACE_OS::sleep(10);
}

int CProxyImp::Open(void)
{
	return 0;
}

int CProxyImp::Close(void)
{
	if( m_pMsgBlock )
	{//在超时的时候可能导致内存泄露，因此在最后要释放内存．

		CMsgBlockManager::Free(m_pMsgBlock);
		m_pMsgBlock = NULL;
	}

	return 0;
}

bool CProxyImp::IsServerConnected() const
{
	if( !m_pConnectionImp )
		return false;

	return (m_pConnectionImp->get_handle() != ACE_INVALID_HANDLE);
}

void CProxyImp::SetConnectionImp(CConnection_ForClient_Imp * pConnectionImp) { 
	ACE_Guard<ACE_Thread_Mutex> guard(m_requestMutex);

	m_pConnectionImp=pConnectionImp;
}

int CProxyImp::SendRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout)
{
	MSG_BLOCK * pMsgBlock_Request = CMsgBlockManager::Malloc(0);

	pMsgBlock_Request->pBuffer = (char *)pdata;
	pMsgBlock_Request->nDataLen = (int)ndatalen;
	pMsgBlock_Request->pPduHdr->nOperation = operation;

	int nrtn = SendRequest(servantid,pMsgBlock_Request,pResponseMsgBlock,nTimeout);


	CMsgBlockManager::Free(pMsgBlock_Request);

	return nrtn;
}

int CProxyImp::SendNotify(int servantid,short operation,const void * pdata,size_t ndatalen)
{
	MSG_BLOCK * pMsgBlock_Request = CMsgBlockManager::Malloc(0);

	pMsgBlock_Request->pBuffer = (char *)pdata;
	pMsgBlock_Request->nDataLen = (int)ndatalen;
	pMsgBlock_Request->pPduHdr->nOperation = operation;

	int nrtn = SendNotify(servantid,pMsgBlock_Request);

	CMsgBlockManager::Free(pMsgBlock_Request);

	return nrtn;
}


int CProxyImp::SendRequest(int servantid,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pResponseMsgBlock, int nTimeout){
	assert(pMsgBlock);

	ACE_Guard<ACE_Thread_Mutex> guard(m_requestMutex);

	if(m_bRequest) 
	{
		ACE_ASSERT(FALSE);
		return COM_MAX_REQUEST;
	}

	int nRet=COM_NOT_ATTACH;

	if(m_pConnectionImp){
		PDU_HDR * pPduHdr=pMsgBlock->pPduHdr;
		pPduHdr->nPduType=PDU_TYPE_REQUEST;
		pPduHdr->nDestId=servantid;
		pPduHdr->nSrcId=m_nProxyId;
		pPduHdr->nRequestSeq=++m_nRequestSeq;

		if( m_pMsgBlock )
		{//在超时之后可能导致内存泄露
			CMsgBlockManager::Free(m_pMsgBlock);
			m_pMsgBlock = NULL;
		}

		ACE_UINT32 begin_tick = GetTickCount();

		MY_ACE_DEBUG(m_pConnectionImp->GetLogInstance(),
			(LM_INFO,
			ACE_TEXT("[Info][%D]before send_Request:%d\n"),
			pMsgBlock->pPduHdr->nOperation
			));

		{
			ACE_Time_Value send_TimeOut(nTimeout);

			nRet= m_pConnectionImp->SendPdu(pMsgBlock,&send_TimeOut);
		}

		if(nRet==COM_SUCCESS) 
		{
			m_bRequest=true;

			ACE_Time_Value recv_TimeOut=ACE_OS::gettimeofday();

			recv_TimeOut += (time_t)nTimeout;

			int nWaitRet=m_condition.wait(&recv_TimeOut);

			if(nWaitRet==-1 || m_pMsgBlock==NULL)
			{
				nRet= COM_TIMEOUT;
			}
			else
			{
				pResponseMsgBlock=m_pMsgBlock;
				m_pMsgBlock = NULL;
				nRet=COM_SUCCESS;
			}

			m_bRequest=false;

			ACE_UINT32 end_tick = GetTickCount();

			if( nRet== COM_TIMEOUT || end_tick - begin_tick > 5 * 1000 )
			{
				MY_ACE_DEBUG(m_pConnectionImp->GetLogInstance(),
					(LM_INFO,
					ACE_TEXT("[Info][%D]after recv rsp on request:%d,spend_time=%d(ms)\n"),
					pMsgBlock->pPduHdr->nOperation,end_tick - begin_tick
					));
			}
			else
			{
				MY_ACE_DEBUG(m_pConnectionImp->GetLogInstance(),
					(LM_INFO,
					ACE_TEXT("[Info][%D]after recv rsp on request:%d,spend_time=%d(ms)\n"),
					pMsgBlock->pPduHdr->nOperation,end_tick - begin_tick
					));
			}

		}
	}
	return nRet;
}

int CProxyImp::SendNotify(int servantid,MSG_BLOCK * pMsgBlock){
	assert(pMsgBlock);

	int nRet=COM_NOT_ATTACH;
	ACE_Guard<ACE_Thread_Mutex> guard(m_requestMutex);
	if(m_pConnectionImp){
		PDU_HDR * pPduHdr=pMsgBlock->pPduHdr;
		pPduHdr->nPduType=PDU_TYPE_NOTIFY;
		pPduHdr->nDestId=servantid;
		pPduHdr->nSrcId=m_nProxyId;

		nRet=m_pConnectionImp->SendPdu(pMsgBlock,NULL);
	}
	return nRet;
}

int CProxyImp::OnResponse(MSG_BLOCK * pMsgBlock){
	assert(pMsgBlock);

	ACE_Guard<ACE_Thread_Mutex> guard(m_requestMutex);

	if( m_bRequest!=true || pMsgBlock->pPduHdr->nRequestSeq != m_nRequestSeq) 
	{
		return COM_INVALID_RESPONSE;
	}

	m_pMsgBlock=pMsgBlock;
	m_condition.signal();

	return COM_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//CConnectionImp
//////////////////////////////////////////////////////////////////////////

CConnectionImp::CConnectionImp()
{
	zero_recv_msgblock();
}

CConnectionImp::~CConnectionImp()
{
}

int CConnectionImp::SendPdu(MSG_BLOCK * pMsgBlock,const ACE_Time_Value *timeout)
{
	assert(pMsgBlock);

	if( get_handle() == ACE_INVALID_HANDLE )
	{
		return COM_OTHER;
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);
	pMsgBlock->pPduHdr->nPduLen=pMsgBlock->nDataLen+sizeof(PDU_HDR);

	//should convert pduhdr to network order
	 
	if( (char *)pMsgBlock->pPduHdr + sizeof(PDU_HDR) == pMsgBlock->pBuffer )
	{//如果数据头和数据是在一起的，则同时发送
		size_t nRet=m_stream.send_n(pMsgBlock->pPduHdr,pMsgBlock->pPduHdr->nPduLen,timeout);

		if(nRet==0) return  COM_EOF;
		if(nRet==-1) return COM_SEND_ERR;
	}
	else
	{
		//如果数据头和数据是不在一起的，则分开发送
		ssize_t nRet=m_stream.send_n(pMsgBlock->pPduHdr,sizeof(PDU_HDR),timeout);

		if(nRet==0) return  COM_EOF;
		if(nRet==-1) return COM_SEND_ERR;

		if( pMsgBlock->nDataLen > 0 )
		{
			nRet = m_stream.send_n(pMsgBlock->pBuffer,pMsgBlock->nDataLen,timeout);

			if(nRet==0) return  COM_EOF;
			if(nRet==-1) return COM_SEND_ERR;
		}
	}

	return COM_SUCCESS;
}

//read_nums==0，表示是收到消息后，调用该函数
int CConnectionImp::RecvData(int read_nums)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);

	if(m_nRead<4)
	{

		int val = 0;
		ACE::record_and_set_non_blocking_mode (m_stream.get_handle(), val);

		int nRet=m_stream.recv(m_len_buf+m_nRead,(size_t)(4-m_nRead),0);

		ACE::restore_non_blocking_mode (m_stream.get_handle(), val);

		
		if(nRet==0) 
		{
			return  COM_EOF;
		}

		if(nRet==-1) 
		{
			return ((EWOULDBLOCK == errno)?COM_SUCCESS:COM_RECV_ERR);
		}

		m_nRead += nRet;

		if( m_nRead == 4 )
		{
			int nMsgLen=LBUF_TO_UINT(m_len_buf);

			m_pMsgBlock=CMsgBlockManager::Malloc(nMsgLen);
			if(m_pMsgBlock==NULL)
			{
				m_nRead = 0;
				return COM_RECV_ERR;//no memory
			}

			m_pMsgBlock->pPduHdr->nPduLen=nMsgLen;
			m_pMsgBlock->nDataLen=nMsgLen-sizeof(PDU_HDR);
		}
		else
		{
			return 0;
		}
	}


	int val = 0;
	ACE::record_and_set_non_blocking_mode (m_stream.get_handle(), val);

	int nRet=m_stream.recv((char *)(m_pMsgBlock->pPduHdr)+m_nRead,(size_t)(m_pMsgBlock->pPduHdr->nPduLen-m_nRead));

	ACE::restore_non_blocking_mode (m_stream.get_handle(), val);


	if(nRet==0) 
	{
		return  COM_EOF;
	}

	if(nRet==-1) 
	{
		return ((EWOULDBLOCK == errno)?COM_SUCCESS:COM_RECV_ERR);
	}

	m_nRead+=nRet;

	return 0;
}

/// Return the local endpoint address in the referenced <ACE_Addr>.
/// Returns 0 if successful, else -1.
int CConnectionImp::get_local_addr (ACE_INET_Addr &addr) const
{
	return m_stream.get_local_addr(addr);
}

/**
* Return the address of the remotely connected peer (if there is
* one), in the referenced <ACE_Addr>. Returns 0 if successful, else
* -1.
*/
int CConnectionImp::get_remote_addr (ACE_INET_Addr &addr) const
{
	return m_stream.get_remote_addr(addr);

}

//////////////////////////////////////////////////////////////////////////
//CConnection_ForServer_Imp
//////////////////////////////////////////////////////////////////////////

CConnection_ForServer_Imp::CConnection_ForServer_Imp(CServerImp * pServerImp,
													 const ACE_Time_Value * recv_timeout,
													 const ACE_Time_Value * send_timeout)
:m_pServerImp(pServerImp)
{
	reference_counting_policy().value(ACE_Event_Handler::Reference_Counting_Policy::ENABLED);
	
	for(int nLoop=0;nLoop<MAX_SERVANT_NUM;nLoop++)
	{
		m_pServantImp[nLoop]=NULL;
	}

	if( recv_timeout )
	{
		m_recv_timeout = new ACE_Time_Value;

		*m_recv_timeout = *recv_timeout;
	}
	else
	{
		m_recv_timeout = NULL;
	}

	if( send_timeout )
	{
		m_send_timeout = new ACE_Time_Value;

		*m_send_timeout = *send_timeout;
	}
	else
	{
		m_send_timeout = NULL;
	}

}


CConnection_ForServer_Imp::~CConnection_ForServer_Imp()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);

	m_stream.close();

	for(int nLoop=0;nLoop<MAX_SERVANT_NUM;nLoop++)
	{
		if(m_pServantImp[nLoop]==NULL) continue;
		DetachServant(m_pServantImp[nLoop]);
	}

	if(m_pMsgBlock)
	{
		CMsgBlockManager::Free(m_pMsgBlock);
		m_nRead = 0;
		m_pMsgBlock = NULL;
	}

	if( m_send_timeout )
	{
		delete m_send_timeout;
		m_send_timeout = NULL;
	}

	if( m_recv_timeout )
	{
		delete m_recv_timeout;
		m_recv_timeout = NULL;
	}
}

int CConnection_ForServer_Imp::CloseByServer()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);

	if(reactor()!=NULL)
	{
		//		reactor()->remove_handler(this,ACE_Event_Handler::READ_MASK|ACE_Event_Handler::DONT_CALL);
		reactor()->remove_handler(this,ACE_Event_Handler::ALL_EVENTS_MASK);
	}

	m_pServerImp = NULL;

	remove_reference();

	return 0;	
	
}

int CConnection_ForServer_Imp::AttachServant(CServantImp * pServantImp){
	assert(pServantImp);
	int nServantId=pServantImp->GetServantId();

	if(nServantId<0 ||nServantId>=MAX_SERVANT_NUM) 
	{
		ACE_ASSERT(FALSE);
		return COM_MAX_SERVANT;
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);

	if(m_pServantImp[nServantId]!=NULL) 
	{
		ACE_ASSERT(FALSE);
		return COM_SERVANT_ATTACH_TWICE;
	}

	m_pServantImp[nServantId]=pServantImp;

	return COM_SUCCESS;
}
int CConnection_ForServer_Imp::DetachServant(CServantImp * pServantImp){
	assert(pServantImp);
	int nServantId=pServantImp->GetServantId();
	if(nServantId<0 ||nServantId>=MAX_SERVANT_NUM) return COM_NOT_ATTACH;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);

	if(m_pServantImp[nServantId]!=pServantImp) 
	{
		ACE_ASSERT(FALSE);
		return COM_NOT_ATTACH;
	}

	m_pServantImp[nServantId]=NULL;

	return COM_SUCCESS;
}

int CConnection_ForServer_Imp::handle_input(ACE_HANDLE)
{
	int nRet = 0;

	int read_nums = 0;

	while ( 1 )
	{
		nRet = RecvData(read_nums);

		++read_nums;

		if( nRet != 0 )
			return -1;

		if(m_pMsgBlock && m_nRead==m_pMsgBlock->pPduHdr->nPduLen)
		{
			PDU_HDR * pPduHdr=m_pMsgBlock->pPduHdr;
			//should convert pduhdr to host order

			switch (pPduHdr->nPduType) 
			{
			case PDU_TYPE_REQUEST:
			case PDU_TYPE_NOTIFY:
				if(pPduHdr->nDestId>=0 && pPduHdr->nDestId<MAX_SERVANT_NUM && m_pServantImp[pPduHdr->nDestId]!=NULL){
					MSG_DATA MsgData;
					MsgData.nMsgType=MSG_DATA::MSG_TYPE_DATA;
					MsgData.pConnection=this;
					MsgData.pMsgBlock=m_pMsgBlock;

					add_reference();
					m_pServantImp[pPduHdr->nDestId]->put_msg_data(MsgData);
				}
				else if(pPduHdr->nPduType==PDU_TYPE_REQUEST){
					CMsgBlockHelper_Stream::Clear(*m_pMsgBlock);
					int nTempId=pPduHdr->nDestId;
					pPduHdr->nDestId=pPduHdr->nSrcId;
					pPduHdr->nSrcId=nTempId;
					pPduHdr->nPduType=PDU_TYPE_RESPONSE;
					pPduHdr->nResponseCode=RESPONSE_CODE_NO_DEST;

					nRet = SendPdu(m_pMsgBlock,m_send_timeout);
					CMsgBlockManager::Free(m_pMsgBlock);
				}
				else {
					CMsgBlockManager::Free(m_pMsgBlock);
				}

				break;
			case PDU_TYPE_RESPONSE:
			default:
				ACE_ASSERT(FALSE);			
				CMsgBlockManager::Free(m_pMsgBlock);
				zero_recv_msgblock();
				return -1;
			}

			zero_recv_msgblock();
		}
		else
		{
			break;
		}
	}

	return nRet;
}

int CConnection_ForServer_Imp::handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask)
{
	for(int nLoop=0;nLoop<MAX_SERVANT_NUM;nLoop++)
	{
		if(m_pServantImp[nLoop]==NULL) continue;
		MSG_DATA MsgData;
		MsgData.nMsgType=MSG_DATA::MSG_TYPE_CLOSE;
		MsgData.pConnection=this;
		MsgData.pMsgBlock=NULL;
		add_reference();
		m_pServantImp[nLoop]->put_msg_data(MsgData);
	}

	if( reactor() )
	{
		reactor()->remove_handler(this,ACE_Event_Handler::ALL_EVENTS_MASK|ACE_Event_Handler::DONT_CALL);
	}

	if(m_pServerImp)
	{
		m_pServerImp->ConnectionClosed(this);
		remove_reference();
	}

     m_stream.close();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//CConnection_ForClient_Imp
//////////////////////////////////////////////////////////////////////////

CConnection_ForClient_Imp::CConnection_ForClient_Imp()
{
	for(int nLoop=0;nLoop<MAX_PROXY_NUM;nLoop++)
	{
		m_pProxyImp[nLoop]=NULL;
	}

	m_pInnerLogInstance = NULL;
}

CConnection_ForClient_Imp::~CConnection_ForClient_Imp()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);

	m_stream.close();

	for(int nLoop=0;nLoop<MAX_PROXY_NUM;nLoop++)
	{
		if(m_pProxyImp[nLoop]==NULL) continue;
		DetachProxy(m_pProxyImp[nLoop]);
	}

	if(m_pMsgBlock)
	{
		CMsgBlockManager::Free(m_pMsgBlock);
		m_nRead = 0;
		m_pMsgBlock = NULL;
	}
}

int CConnection_ForClient_Imp::CloseByUser(void)
{
	if( reactor() )
	{
		reactor()->remove_handler(this,ACE_Event_Handler::READ_MASK|ACE_Event_Handler::DONT_CALL); 
	}
	reactor(NULL);

	m_stream.close();

	return 0;
}

int CConnection_ForClient_Imp::ConnectServer(const char * pszServerName,int hostorder_port,ACE_Reactor * pReactor,const ACE_Time_Value *timeout)
{
	if( !pReactor )
	{
		reactor(ACE_Reactor::instance());
	}
	else
	{
		reactor(pReactor);
	}

	ACE_INET_Addr * pRemoteAddr=GetRemoteAddr();
	pRemoteAddr->set(hostorder_port,pszServerName);

	ACE_SOCK_Connector aceConnector;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);

	int ret=aceConnector.connect(m_stream,*pRemoteAddr,timeout);
	if(ret==-1) return COM_CONNECT_ERR;

	return reactor()->register_handler(this,ACE_Event_Handler::READ_MASK); 
}

int CConnection_ForClient_Imp::ConnectServer(ACE_UINT32 netorder_ip,int hostorder_port,ACE_Reactor * pReactor,const ACE_Time_Value *timeout)
{
	return ConnectServer(
		ACE_OS::inet_ntoa(*(in_addr *)&netorder_ip),
		hostorder_port,pReactor,timeout );
}

int CConnection_ForClient_Imp::AttachProxy(CProxyImp * pProxyImp){
	assert(pProxyImp);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);


	for(int nLoop=0;nLoop<MAX_PROXY_NUM;nLoop++){
		if(m_pProxyImp[nLoop]==NULL) {
			m_pProxyImp[nLoop]=pProxyImp;
			pProxyImp->SetProxyId(nLoop);
			pProxyImp->SetConnectionImp(this);
			return COM_SUCCESS;
		}
	}

	return COM_MAX_PROXY;
}

int CConnection_ForClient_Imp::DetachProxy(CProxyImp * pProxyImp){
	assert(pProxyImp);
	int nProxyId=pProxyImp->GetProxyId();
	if(nProxyId<0 || nProxyId>=MAX_PROXY_NUM) return COM_NOT_ATTACH;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_client);

	if(m_pProxyImp[nProxyId]!=pProxyImp) return COM_NOT_ATTACH;

	m_pProxyImp[nProxyId]=NULL;
	pProxyImp->SetConnectionImp(NULL);

	return COM_SUCCESS;
}

int CConnection_ForClient_Imp::handle_input(ACE_HANDLE)
{
	BOOL bGetCurData = FALSE;

	int read_nums = 0;

	while( !bGetCurData )
	{
		int nRet = RecvData(read_nums);

		read_nums++;

		if( nRet != 0 )
			return -1;

		if(m_pMsgBlock  && m_nRead==m_pMsgBlock->pPduHdr->nPduLen)
		{
			PDU_HDR * pPduHdr=m_pMsgBlock->pPduHdr;
			//should convert pduhdr to host order

			switch (pPduHdr->nPduType) 
			{			
			case PDU_TYPE_RESPONSE:
				if(pPduHdr->nDestId>=0 && pPduHdr->nDestId<MAX_PROXY_NUM && m_pProxyImp[pPduHdr->nDestId]!=NULL)
				{
					if(m_pProxyImp[pPduHdr->nDestId]->OnResponse(m_pMsgBlock)!=COM_SUCCESS)
					{
						CMsgBlockManager::Free(m_pMsgBlock);
					}
					else
					{
						bGetCurData = TRUE;
					}
				}
				else
				{ 
					CMsgBlockManager::Free(m_pMsgBlock);
				}
				break;
			case PDU_TYPE_REQUEST:
			case PDU_TYPE_NOTIFY:
			default:
				ACE_ASSERT(false);
				CMsgBlockManager::Free(m_pMsgBlock);
				zero_recv_msgblock();
				return -1;
			}
			zero_recv_msgblock();
		}
		else
		{
			break;
		}
	}

	return 0;
}

int CConnection_ForClient_Imp::handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask)
{
	for(int nLoop=0;nLoop<MAX_PROXY_NUM;nLoop++)
	{
		if(m_pProxyImp[nLoop]!=NULL)
		{
			m_pProxyImp[nLoop]->OnServerClose();
		}
	}

	if( reactor() )
	{
		reactor()->remove_handler(this,ACE_Event_Handler::READ_MASK|ACE_Event_Handler::DONT_CALL); 
	}

	m_stream.close();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//CServerImp
//////////////////////////////////////////////////////////////////////////

CServerImp::CServerImp(int nMaxClientNum)
:m_nMaxClientNums(nMaxClientNum)
{
	m_pConnectionImp = new LPConnection_ForServer_Imp[m_nMaxClientNums];

	int nLoop;
	for(nLoop=0;nLoop<m_nMaxClientNums;nLoop++){
		m_pConnectionImp[nLoop]=NULL;
	}

	for(nLoop=0;nLoop<MAX_SERVANT_NUM;nLoop++)
		m_pServantImp[nLoop]=NULL;

	m_send_timeout = NULL;
	m_recv_timeout = NULL;
}

CServerImp::~CServerImp()
{
	if( m_pConnectionImp )
	{
		delete []m_pConnectionImp;
		m_pConnectionImp = NULL;
	}

	if( m_send_timeout )
	{
		delete m_send_timeout;
		m_send_timeout = NULL;
	}

	if( m_recv_timeout )
	{
		delete m_recv_timeout;
		m_recv_timeout = NULL;
	}

}

int CServerImp::BuildServer(int port,ACE_Reactor * pReactor,
							const ACE_Time_Value * recv_timeout,
							const ACE_Time_Value * send_timeout)
{
	if( !pReactor )
	{
		reactor(ACE_Reactor::instance()); 
	}
	else
	{
		reactor(pReactor);
	}

	if( send_timeout )
	{
		m_send_timeout = new ACE_Time_Value;
		*m_send_timeout = *send_timeout;
	}

	if( recv_timeout )
	{
		m_recv_timeout = new ACE_Time_Value;
		*m_recv_timeout = *recv_timeout;
	}

	ACE_INET_Addr addr(port);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_server);

	int nRet=m_acceptor.open(addr, 1);//SO_REUSEADDR

	if(nRet!=0) return COM_BUILD_SERVER;

	return reactor()->register_handler(this,ACE_Event_Handler::ACCEPT_MASK); 
}

int CServerImp::Close(void)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_server);

	for(int nLoop=0;nLoop<m_nMaxClientNums;nLoop++)
	{
		if(m_pConnectionImp[nLoop]!=NULL)
		{
			m_pConnectionImp[nLoop]->CloseByServer();
			m_pConnectionImp[nLoop] = NULL;
		}
	}

	if(reactor())
	{
		reactor()->remove_handler(this,ACE_Event_Handler::ACCEPT_MASK|ACE_Event_Handler::DONT_CALL);
	}

	int nRet=m_acceptor.close();
	if(nRet!=0) return COM_CLOSE_HANDLE;
	return COM_SUCCESS;
}

int CServerImp::ConnectionClosed(CConnection_ForServer_Imp * pConnectionImp){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_server);

	for(int nLoop=0;nLoop<m_nMaxClientNums;nLoop++){
		if(m_pConnectionImp[nLoop]==pConnectionImp){
			m_pConnectionImp[nLoop]=NULL;
			return COM_SUCCESS;
		}
	}
	
//	assert(0);
	return COM_SUCCESS;
}

int CServerImp::AttachServant(CServantImp * pServantImp){
	assert(pServantImp);
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_server);

	int nServantId=pServantImp->GetServantId();
	if(nServantId<0 || nServantId>=MAX_SERVANT_NUM)
		return COM_MAX_SERVANT;
	if(m_pServantImp[nServantId]!=NULL) return COM_SERVANT_ATTACH_TWICE;

	m_pServantImp[nServantId]=pServantImp;
	for(int nLoop=0;nLoop<m_nMaxClientNums;nLoop++){
		if(m_pConnectionImp[nLoop]!=NULL){
			m_pConnectionImp[nLoop]->AttachServant(pServantImp);
		}
	}
	
	return COM_SUCCESS;
}

int CServerImp::DetachServant(CServantImp * pServantImp){
	assert(pServantImp);
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_server);

	int nServantId=pServantImp->GetServantId();
	if(nServantId<0 || nServantId>=MAX_SERVANT_NUM || m_pServantImp[nServantId]!=pServantImp)
		return COM_NOT_ATTACH;

	for(int nLoop=0;nLoop<m_nMaxClientNums;nLoop++){
		if(m_pConnectionImp[nLoop]!=NULL){
			m_pConnectionImp[nLoop]->DetachServant(pServantImp);
		}
	}

	m_pServantImp[nServantId] = NULL;

	return COM_SUCCESS;
}

int CServerImp::DetachAllServant()
{
	for(int nServantId = 0; nServantId < MAX_SERVANT_NUM; ++nServantId )
	{
		if( m_pServantImp[nServantId] )
		{
			DetachServant(m_pServantImp[nServantId]);
			m_pServantImp[nServantId] = NULL;
		}
	}

	return COM_SUCCESS;
}

int CServerImp::handle_input(ACE_HANDLE){
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex_server);

	CConnection_ForServer_Imp * pConnectionImp=new CConnection_ForServer_Imp(this,
		m_recv_timeout,m_send_timeout);

	if(pConnectionImp==NULL) return 0;

	int nRet=m_acceptor.accept(pConnectionImp->GetStream(),pConnectionImp->GetRemoteAddr());
	if(nRet==-1)
	{
		on_connect_failed(1,pConnectionImp);
		pConnectionImp->remove_reference();
		return 0;
	}
	int nIndex=-1,nLoop;
	for(nLoop=0;nLoop<m_nMaxClientNums;nLoop++){
		if(m_pConnectionImp[nLoop]==NULL){
			nIndex=nLoop;
			break;
		}
	}

	if(nIndex==-1)
	{
		on_connect_failed(2,pConnectionImp);

		pConnectionImp->remove_reference();

		return 0;
	}	

	m_pConnectionImp[nIndex]=pConnectionImp;

	for(nLoop=0;nLoop<MAX_SERVANT_NUM;nLoop++){
		if(m_pServantImp[nLoop]!=NULL){
			pConnectionImp->AttachServant(m_pServantImp[nLoop]);
		}
	}

	on_connect_success(pConnectionImp);

	reactor()->register_handler(pConnectionImp,ACE_Event_Handler::READ_MASK);
	
	return 0;
}

int CServerImp::handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask)
{
	return 0;
}

void CServerImp::on_connect_failed(int reason,CConnection_ForServer_Imp * pConnectionImp)
{
	for(size_t nLoop=0;nLoop<MAX_SERVANT_NUM;nLoop++)
	{
		if(m_pServantImp[nLoop]!=NULL)
		{
			MSG_DATA MsgData;
			MsgData.nMsgType=MSG_DATA::MSG_TYPE_CONNECTED_FAILED;
			MsgData.pConnection=pConnectionImp;

			MsgData.pMsgBlock=CMsgBlockManager::Malloc(sizeof(int));
			char * pdata = CMsgBlockManager::WritePtr(*MsgData.pMsgBlock);
			CData_Stream_LE::PutUint32(pdata,reason);

			pConnectionImp->add_reference();
			m_pServantImp[nLoop]->put_msg_data(MsgData);
		}
	}

	return;
}


void CServerImp::on_connect_success(CConnection_ForServer_Imp * pConnectionImp)
{
	for(size_t nLoop=0;nLoop<MAX_SERVANT_NUM;nLoop++)
	{
		if(m_pServantImp[nLoop]!=NULL)
		{
			MSG_DATA MsgData;
			MsgData.nMsgType=MSG_DATA::MSG_TYPE_CONNECTED_SUCCESS;
			MsgData.pConnection=pConnectionImp;
			MsgData.pMsgBlock=NULL;
			pConnectionImp->add_reference();
			m_pServantImp[nLoop]->put_msg_data(MsgData);
		}
	}

	return;
}


//////////////////////////////////////////////////////////////////////////
//CBaseDoCmdTool
//////////////////////////////////////////////////////////////////////////

#include "cpf/Data_Stream.h"
#include "ace/Log_Msg.h"

void CBaseDoCmdTool::log_request(
		ACE_Log_Msg * pLogInstance,
		ACE_INET_Addr * pRemoteAddr,
		const char * msg_info)
{
	char buf[64] = {0};

	pRemoteAddr->addr_to_string(buf,sizeof(buf));

	MY_ACE_DEBUG(pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]recv %s cmd from '%s'\n"),msg_info,buf));

	return;
}

MSG_BLOCK * CBaseDoCmdTool::MakeUint16ErrorMsgBlock(
		MSG_BLOCK * pMsgBlock,
		ACE_UINT16 nerror,
		ACE_UINT16 nResponseCode)
{
	MSG_BLOCK * pResponMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,sizeof(ACE_UINT16),nResponseCode);

	if( pResponMsgBlock )
	{
		char * pdata = CMsgBlockHelper_Stream::WritePtr(*pResponMsgBlock,0,sizeof(ACE_UINT16));

		CData_Stream_LE::PutUint16(pdata,nerror);
	}

	return pResponMsgBlock;
}

MSG_BLOCK * CBaseDoCmdTool::MakeUint32ErrorMsgBlock(
		MSG_BLOCK * pMsgBlock,
		ACE_UINT32 nerror,
		ACE_UINT16 nResponseCode)
{
	MSG_BLOCK * pResponMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,sizeof(ACE_UINT32),nResponseCode);

	if( pResponMsgBlock )
	{
		char * pdata = CMsgBlockHelper_Stream::WritePtr(*pResponMsgBlock,0,sizeof(ACE_UINT32));

		CData_Stream_LE::PutUint32(pdata,nerror);
	}

	return pResponMsgBlock;
}

