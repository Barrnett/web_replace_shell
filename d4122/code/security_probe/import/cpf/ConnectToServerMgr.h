//////////////////////////////////////////////////////////////////////////
//CConnectToServerMgr.h

#pragma once

//管理和服务之间的连接,出现连接断后会自动重新连接

#include "cpf/cpf.h"
#include "ace/Event_Handler.h"
#include "cpf/MyACETimer.h"
#include "cpf/NetcomBaseServant.h"

class CPF_CLASS CConnectToServerMgr : public ACE_Event_Handler
{
private:
	enum{TIMER_ReConnectToServer = 0};

public:
	CConnectToServerMgr(void);
	virtual ~CConnectToServerMgr(void);

	void init(ACE_UINT32 hIP,ACE_UINT16 hport,
		int re_connect_to_server_interval);

	void init(const char * host_name,ACE_UINT16 hport,
		int re_connect_to_server_interval);

	void close();

public:
	BOOL ConnectToServer();
	void DisConnectToServer();

public:
	BOOL IsConnectedToServer() const;

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
	virtual int SendRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);
	virtual int SendNotify(int servantid,short operation,const void * pdata,size_t ndatalen);

	//user interface
	virtual int SendRequest(int servantid,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);
	virtual int SendNotify(int servantid,MSG_BLOCK * pMsgBlock);

public:
	//返回主机字节序IP地址
	std::string	GetServerHost() const
	{	return m_host_name; }

	ACE_UINT16 GetServerPort() const
	{	return m_hServerPort;	}

	/// Return the local endpoint address in the referenced <ACE_Addr>.
	/// Returns 0 if successful, else -1.
	int get_local_addr (ACE_INET_Addr &addr) const
	{
		return m_net_client.get_local_addr(addr);
	}

	/**
	* Return the address of the remotely connected peer (if there is
	* one), in the referenced <ACE_Addr>. Returns 0 if successful, else
	* -1.
	*/
	int get_remote_addr (ACE_INET_Addr &addr) const
	{
		return m_net_client.get_remote_addr(addr);
	}

public:
	BOOL StartReConnectMonitor();
	void StopReConnectMonitor();

public:
	//已经出现连接关闭了，准备重新连接
	//返回true,表示要进行重新连接，
	//返回false,表示这次不要重新连接，
	//注意函数在这个模块的定时器线程中调用
	virtual BOOL OnBeforeReConnect()
	{
		return true;
	}

	//重新连接之后的回调。注意函数在这个模块的定时器线程中调用
	virtual void OnAfterReconnect(BOOL bReconnectOK)
	{
	}

private:
	BOOL DoConnectToServer();

private:
	void schedule_timer();

	virtual int handle_timeout(const ACE_Time_Value& tv,const void *arg);


private:
	CMyACETimer 				m_ActiveTimer;

	BOOL						m_bStop;

	ACE_Log_Msg *				m_pInnerLogInstance;

private:
	CNetClientWithOneProxy<CProxyImp>	m_net_client;

	std::string							m_host_name;
	ACE_UINT16							m_hServerPort;
	int									m_re_connect_to_server_interval;

public:
	void Lock()
	{
		m_tm.acquire();
	}

	void UnLock()
	{
		m_tm.release();
	}

private:
	ACE_Recursive_Thread_Mutex	m_tm;

};


class CPF_CLASS CConnectToServerMgr_Lock
{
public:
	CConnectToServerMgr_Lock(CConnectToServerMgr& mgr)
		:m_mgr(mgr)
	{
		m_mgr.Lock();
	}

	~CConnectToServerMgr_Lock()
	{
		m_mgr.UnLock();
	}

private:
	CConnectToServerMgr&	m_mgr;
};