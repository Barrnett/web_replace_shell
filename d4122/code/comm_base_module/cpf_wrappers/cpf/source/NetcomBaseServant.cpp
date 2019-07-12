//////////////////////////////////////////////////////////////////////////
//NetcomBaseServant.cpp

#include "cpf/NetcomBaseServant.h"

//////////////////////////////////////////////////////////////////////////
//CMyServerLogImp
//////////////////////////////////////////////////////////////////////////
CMyServerLogImp::CMyServerLogImp(int nMaxClientNum)
:CServerImp(nMaxClientNum)
{
	m_pLogInstance = NULL;

	m_connect_nums = 0;
	m_disconnect_nums = 0;
}

CMyServerLogImp::~CMyServerLogImp()
{

}
void CMyServerLogImp::on_connect_failed(int reason,CConnection_ForServer_Imp * pConnectionImp)
{
	char buf[200] = {0};

	pConnectionImp->GetRemoteAddr()->addr_to_string(buf,sizeof(buf));

	static int nums = 0;

	if( 2 == reason )
	{
		MY_ACE_DEBUG(m_pLogInstance,
			(LM_INFO,ACE_TEXT("[Err][%D]client '%s' connectd to me failed.too many clients.err_code=%d,error_nums=%d!\n"),
			buf,reason,++nums));
	}
	else if( 1 == reason )
	{
		MY_ACE_DEBUG(m_pLogInstance,
			(LM_INFO,ACE_TEXT("[Err][%D]client '%s' connectd to me failed.error on accept.err_code=%d,error_nums=%d!\n"),
			buf,reason,++nums));
	}
	else
	{
		MY_ACE_DEBUG(m_pLogInstance,
			(LM_INFO,ACE_TEXT("[Err][%D]client '%s' connectd to me failed.other error.err_code=%d,error_nums=%d!\n"),
			buf,reason,++nums));
	}

	CServerImp::on_connect_failed(reason,pConnectionImp);

}

void CMyServerLogImp::on_connect_success(CConnection_ForServer_Imp * pConnectionImp)
{
	char buf[100] = {0};

	pConnectionImp->GetRemoteAddr()->addr_to_string(buf,sizeof(buf));

	++m_connect_nums;

	MY_ACE_DEBUG(m_pLogInstance,
		(LM_INFO,ACE_TEXT("[Info][%D]client '%s' connectd to me success!total connect=%d,cur_active=%d\n"),
		buf,m_connect_nums,m_connect_nums-m_disconnect_nums));

	CServerImp::on_connect_success(pConnectionImp);

}

int CMyServerLogImp::ConnectionClosed(CConnection_ForServer_Imp * pConnectionImp)
{
	char buf[100] = {0};

	pConnectionImp->GetRemoteAddr()->addr_to_string(buf,sizeof(buf));

	++m_disconnect_nums;

	MY_ACE_DEBUG(m_pLogInstance,
		(LM_INFO,ACE_TEXT("[Info][%D]client '%s' disconnectd from me!total close=%d,cur_active=%d\n"),
		buf,m_disconnect_nums,m_connect_nums-m_disconnect_nums));

	return CServerImp::ConnectionClosed(pConnectionImp);
}


//////////////////////////////////////////////////////////////////////////
//CNetClientBaseServant
//////////////////////////////////////////////////////////////////////////

class CMyConnection_ForClient_Imp_Ex : public CConnection_ForClient_Imp
{
public:
	CMyConnection_ForClient_Imp_Ex(CNetClientBaseServant * pBaseServant)
	{
		m_pBaseServant = pBaseServant;
	}

	virtual ~CMyConnection_ForClient_Imp_Ex()
	{

	}
	virtual int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask)
	{
		m_pBaseServant->OnServerClosed();

		return CConnection_ForClient_Imp::handle_close(handle,close_mask);

	}

private:
	CNetClientBaseServant * m_pBaseServant;

};

CNetClientBaseServant::CNetClientBaseServant()
{
	m_pClient = NULL;
	m_pReactor = NULL;

	m_pInnerLogInstance = NULL;
}

CNetClientBaseServant::~CNetClientBaseServant()
{
	close();
}

bool CNetClientBaseServant::init(const char * pszServerName,int hostorder_port,const ACE_Time_Value *timeout)
{
	close();

	m_pClient=new CMyConnection_ForClient_Imp_Ex(this);
	if( !m_pClient )
		return false;

	m_pReactor=new ACE_Reactor;
	if( !m_pReactor )
	{
		close();
		return false;
	}

	m_pClient->SetLogInstance(m_pInnerLogInstance);

	if( 0 != m_pClient->ConnectServer(pszServerName,hostorder_port,m_pReactor,timeout) )
	{
		close();
		return false;
	}

	return true;
}

bool CNetClientBaseServant::init(ACE_UINT32 netorder_ip,int hostorder_port,const ACE_Time_Value *timeout)
{
	return this->init(
		ACE_OS::inet_ntoa(*(in_addr *)&netorder_ip),
		hostorder_port,timeout );
}

void CNetClientBaseServant::OnServerClosed()
{
	return;
}

void CNetClientBaseServant::close()
{
	// 这里的调用会导致svc异常
	stop();

	if( m_pClient )
	{
		m_pClient->CloseByUser();

		delete m_pClient;
		m_pClient = NULL;
	}

	if( m_pReactor )
	{
		m_pReactor->close();
		delete m_pReactor;
		m_pReactor = NULL;
	}

	return;
}

/// Return the local endpoint address in the referenced <ACE_Addr>.
/// Returns 0 if successful, else -1.
int CNetClientBaseServant::get_local_addr (ACE_INET_Addr &addr) const
{
	if( !m_pClient )
		return -1;

	return m_pClient->get_local_addr(addr);
}

/**
* Return the address of the remotely connected peer (if there is
* one), in the referenced <ACE_Addr>. Returns 0 if successful, else
* -1.
*/
int CNetClientBaseServant::get_remote_addr (ACE_INET_Addr &addr) const
{
	if( !m_pClient )
		return -1;

	return m_pClient->get_remote_addr(addr);
}

BOOL CNetClientBaseServant::IsConnectedToServer() const
{
	return (m_pClient && m_pClient->get_handle() != ACE_INVALID_HANDLE);
}

int CNetClientBaseServant::AttachProxy(CProxyImp * pProxyImp)
{
	return m_pClient->AttachProxy(pProxyImp);
}

int CNetClientBaseServant::DetachProxy(CProxyImp * pProxyImp)
{
	return m_pClient->DetachProxy(pProxyImp);
}

int CNetClientBaseServant::svc()
{	
	m_pReactor->owner(ACE_Thread::self());

	return m_pReactor->run_reactor_event_loop();
}

void CNetClientBaseServant::start()
{
	this->activate();
}

void CNetClientBaseServant::stop()
{
	if( m_pReactor )
	{
		m_pReactor->end_reactor_event_loop();

		wait();
	}
}

