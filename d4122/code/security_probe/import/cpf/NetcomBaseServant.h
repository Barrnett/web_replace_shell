//////////////////////////////////////////////////////////////////////////
//NetcomBaseServant.h

#pragma once

#include "cpf/ComImp.h"

class CPF_CLASS CMyServerLogImp : public CServerImp
{
public:
	CMyServerLogImp(int nMaxClientNum = 10);
	virtual ~CMyServerLogImp();

	void SetLogInstance(ACE_Log_Msg * pLogMsg)
	{
		m_pLogInstance = pLogMsg;
	}

public:
	//连接失败,失败原因reason=1表示没有建立连接(accept失败)，2表示客户端过多，其他没有定义
	//缺省实现是通知所有servant某个人连接失败
	virtual void on_connect_failed(int reason,CConnection_ForServer_Imp * pConnectionImp);
	
	//连接成功，缺省实现是通知所有servant某个人连接成功
	virtual void on_connect_success(CConnection_ForServer_Imp * pConnectionImp);

	virtual int ConnectionClosed(CConnection_ForServer_Imp * pConnectionImp);

protected:
	int					m_disconnect_nums;
	int					m_connect_nums;
	ACE_Log_Msg *		m_pLogInstance;

};


template<class SERVER_IMP_TYPE=CServerImp>
class CNetServerBaseServant: public ACE_Task<ACE_MT_SYNCH>
{
public:
	CNetServerBaseServant()
	{
		m_pServer = NULL;
		m_pReactor = NULL;

	}

	~CNetServerBaseServant()
	{
		close();
	}


	bool init(int port,int nMaxClientNum,
		const ACE_Time_Value * recv_timeout,
		const ACE_Time_Value * send_timeout)
	{
		m_pServer=new SERVER_IMP_TYPE(nMaxClientNum);
		if( !m_pServer )
			return false;

		m_pReactor=new ACE_Reactor;
		if( !m_pReactor )
			return false;

		if( 0 != m_pServer->BuildServer(port,m_pReactor,
			recv_timeout,send_timeout) )
		{
			return false;
		}

		return true;
	}


	int AttachServant(CServantImp * pServantImp)
	{
		return m_pServer->AttachServant(pServantImp);
	}

	int DetachServant(CServantImp * pServantImp)
	{
		return m_pServer->DetachServant(pServantImp);
	}

	void close()
	{
		stop();

		if( m_pServer )
		{
			m_pServer->DetachAllServant();

			m_pServer->Close();
			delete m_pServer;
			m_pServer = NULL;
		}

		if( m_pReactor )
		{
			m_pReactor->close();
			delete m_pReactor;
			m_pReactor = NULL;
		}

		return;
	}

	virtual int svc()
	{
		m_pReactor->owner(ACE_Thread::self());

		return m_pReactor->run_reactor_event_loop();
	}

	void start()
	{
		this->activate();
	}

	void stop()
	{
		if( m_pReactor )
		{
			m_pReactor->end_reactor_event_loop();

			wait();
		}
	}

	SERVER_IMP_TYPE * GetServer()
	{
		return m_pServer;
	}

private:
	SERVER_IMP_TYPE *	m_pServer;
	ACE_Reactor *				m_pReactor;

	int							m_bstop;

};

template<class SERVANT_IMP_TYPE,class SERVER_IMP_TYPE=CServerImp>
class CNetServerWithOneServant
{
public:
	CNetServerWithOneServant()
	{
		m_pServant = NULL;

	}
	~CNetServerWithOneServant()
	{

	}

public:
	bool init(int port,int servant_id,
		bool servant_multithread,
		int nMaxClientNum,
		const ACE_Time_Value * recv_timeout,
		const ACE_Time_Value * send_timeout);

	void close();

	void start()
	{	m_netserver_base_servant.start();	}

	void stop()
	{	m_netserver_base_servant.stop();	}

	SERVANT_IMP_TYPE *GetServant()
	{
		return m_pServant;
	}

	CNetServerBaseServant<SERVER_IMP_TYPE> * GetNetServer()
	{
		return &m_netserver_base_servant;
	}

private:
	SERVANT_IMP_TYPE * m_pServant;

private:
	CNetServerBaseServant<SERVER_IMP_TYPE>	m_netserver_base_servant;

};


template<class SERVANT_IMP_TYPE,class SERVER_IMP_TYPE>
bool CNetServerWithOneServant<SERVANT_IMP_TYPE,SERVER_IMP_TYPE>::init(int port,int servant_id,
													  bool servant_multithread,
													  int nMaxClientNum,
													  const ACE_Time_Value * recv_timeout,
													  const ACE_Time_Value * send_timeout)
{
	if( m_netserver_base_servant.init(port,nMaxClientNum,
		recv_timeout,send_timeout) == false )
	{
		return false;
	}

	m_pServant = new SERVANT_IMP_TYPE;
	if( !m_pServant )
		return false;

	m_pServant->SetServantId(servant_id);
	m_pServant->SetServantMultithread(servant_multithread);

	m_netserver_base_servant.AttachServant(m_pServant);

	m_pServant->Open();

	return true;
}

template<class SERVANT_IMP_TYPE,class SERVER_IMP_TYPE>
void CNetServerWithOneServant<SERVANT_IMP_TYPE,SERVER_IMP_TYPE>::close()
{
	if( m_pServant )
	{
		m_netserver_base_servant.DetachServant(m_pServant);

		m_pServant->Close();

		delete m_pServant;
		m_pServant = NULL;
	}

	m_netserver_base_servant.close();

	return;

}


//////////////////////////////////////////////////////////////////////////
//CNetClientBaseServant
//////////////////////////////////////////////////////////////////////////

class CPF_CLASS CNetClientBaseServant: public ACE_Task<ACE_MT_SYNCH>
{
public:
	CNetClientBaseServant(void);
	~CNetClientBaseServant(void);

public:
	bool init(const char * pszServerName,int port,const ACE_Time_Value *timeout=NULL);
	bool init(ACE_UINT32 netorder_ip,int hostorder_port,const ACE_Time_Value *timeout=NULL);

	void close();

	int AttachProxy(CProxyImp * pProxyImp);
	int DetachProxy(CProxyImp * pProxyImp);

	void start();
	void stop();

	BOOL IsConnectedToServer() const;

	virtual int svc();
	virtual void OnServerClosed();

public:
	inline ACE_Log_Msg * GetLogInstance()
	{
		return m_pInnerLogInstance;
	}

	inline void SetLogInstance(ACE_Log_Msg * pLogInstance)
	{
		m_pInnerLogInstance = pLogInstance;
	}


public:
	/// Return the local endpoint address in the referenced <ACE_Addr>.
	/// Returns 0 if successful, else -1.
	int get_local_addr (ACE_INET_Addr &) const;

	/**
	* Return the address of the remotely connected peer (if there is
	* one), in the referenced <ACE_Addr>. Returns 0 if successful, else
	* -1.
	*/
	int get_remote_addr (ACE_INET_Addr &) const;

private:
	CConnection_ForClient_Imp * m_pClient;
	ACE_Reactor *				m_pReactor;

	int							m_bstop;

	ACE_Log_Msg *				m_pInnerLogInstance;
};


template<class PROXY_IMP_TYPE>
class CNetClientWithOneProxy
{
public:
	CNetClientWithOneProxy()
	{
		m_pProxy = NULL;
		m_pInnerLogInstance = NULL;

	}
	~CNetClientWithOneProxy()
	{
		close();
	}

public:
	inline ACE_Log_Msg * GetLogInstance()
	{
		return m_pInnerLogInstance;
	}

	inline void SetLogInstance(ACE_Log_Msg * pLogInstance)
	{
		m_pInnerLogInstance = pLogInstance;
	}
public:
	bool init(const char * szServerName,int port,const ACE_Time_Value *timeout=NULL);

	bool init(ACE_UINT32 netorder_ip,int hostorder_port,const ACE_Time_Value *timeout=NULL)
	{
		return this->init(
			ACE_OS::inet_ntoa(*(in_addr *)&netorder_ip),
			hostorder_port,timeout );
	}

	void close();

	BOOL IsConnectedToServer() const
	{
		return m_netclient_base_servant.IsConnectedToServer();
	}

	void start()
	{	m_netclient_base_servant.start();	}

	void stop()
	{	m_netclient_base_servant.stop();	}

	PROXY_IMP_TYPE *GetProxy() const
	{
		return m_pProxy;
	}

public:
	/// Return the local endpoint address in the referenced <ACE_Addr>.
	/// Returns 0 if successful, else -1.
	int get_local_addr (ACE_INET_Addr &addr) const
	{
		return m_netclient_base_servant.get_local_addr(addr);
	}

	/**
	* Return the address of the remotely connected peer (if there is
	* one), in the referenced <ACE_Addr>. Returns 0 if successful, else
	* -1.
	*/
	int get_remote_addr (ACE_INET_Addr &addr) const
	{
		return m_netclient_base_servant.get_remote_addr(addr);
	}

private:
	PROXY_IMP_TYPE * m_pProxy;

	ACE_Log_Msg *	m_pInnerLogInstance;

private:
	CNetClientBaseServant	m_netclient_base_servant;
};

template<class PROXY_IMP_TYPE>
bool CNetClientWithOneProxy<PROXY_IMP_TYPE>::init(const char * szServerName,int port,const ACE_Time_Value *timeout)
{
	m_netclient_base_servant.SetLogInstance(m_pInnerLogInstance);

	if( m_netclient_base_servant.init(szServerName,port,timeout) == false )
		return false;

	m_pProxy = new PROXY_IMP_TYPE;
	if( !m_pProxy )
		return false;

	m_pProxy->Open();

	m_netclient_base_servant.AttachProxy(m_pProxy);

	return true;
}

template<class PROXY_IMP_TYPE>
void CNetClientWithOneProxy<PROXY_IMP_TYPE>::close()
{
	if( m_pProxy )
	{
		m_netclient_base_servant.DetachProxy(m_pProxy);

		m_pProxy->Close();

		delete m_pProxy;
		m_pProxy = NULL;
	}

	m_netclient_base_servant.close();

	return;

}

