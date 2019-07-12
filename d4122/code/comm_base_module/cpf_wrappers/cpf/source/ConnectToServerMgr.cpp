//////////////////////////////////////////////////////////////////////////
//CConnectToServerMgr.cpp

#include "cpf/ConnectToServerMgr.h"
#include "cpf/addr_tools.h"
#include "ace/High_Res_Timer.h"

CConnectToServerMgr::CConnectToServerMgr(void)
{
	m_hServerPort = 0;

	m_re_connect_to_server_interval = 0;

	m_bStop = true;

	m_pInnerLogInstance = NULL;
}

CConnectToServerMgr::~CConnectToServerMgr(void)
{
}


BOOL CConnectToServerMgr::StartReConnectMonitor()
{
	m_bStop = false;

	schedule_timer();
	
	m_ActiveTimer.timer_queue()->gettimeofday(ACE_High_Res_Timer::gettimeofday_hr);

	m_ActiveTimer.activate();

	return true;
}

void CConnectToServerMgr::StopReConnectMonitor()
{
	m_bStop = true;

	m_ActiveTimer.stop_and_remove_all_timer();

	return;
}


void CConnectToServerMgr::schedule_timer()
{
	m_ActiveTimer.schedule(this,
		(const void *)TIMER_ReConnectToServer,
		ACE_High_Res_Timer::gettimeofday_hr()+ACE_Time_Value(m_re_connect_to_server_interval),
		ACE_Time_Value(m_re_connect_to_server_interval,0) );

	return;

}

int CConnectToServerMgr::handle_timeout(const ACE_Time_Value& tv,const void *arg)
{
	if( m_bStop )
	{
		return 0;
	}

	int int_param = (int)arg;

	switch(int_param)
	{
	case TIMER_ReConnectToServer:
		{
			CConnectToServerMgr_Lock locker(*this);

			if( !IsConnectedToServer() )
			{
				if( OnBeforeReConnect() )
				{
					BOOL bOK = DoConnectToServer();

					OnAfterReconnect(bOK);
				}				
			}			
		}
		break;

	default:
		break;
	}

	return 0;

}

BOOL CConnectToServerMgr::IsConnectedToServer() const
{
	if( m_net_client.GetProxy()
		&& m_net_client.GetProxy()->IsServerConnected() )
	{
		return true;
	}

	return false;
}


BOOL CConnectToServerMgr::ConnectToServer()
{
	CConnectToServerMgr_Lock locker(*this);

	if( IsConnectedToServer() )
	{
		return true;
	}

	return DoConnectToServer();
}

void CConnectToServerMgr::DisConnectToServer()
{
	CConnectToServerMgr_Lock locker(*this);

	m_net_client.stop();
	m_net_client.close();

	return;
}


BOOL CConnectToServerMgr::DoConnectToServer()
{
	m_net_client.stop();
	m_net_client.close();

	m_net_client.SetLogInstance(m_pInnerLogInstance);

	if( !m_net_client.init(m_host_name.c_str(),m_hServerPort) )
	{
		return false;
	}

	m_net_client.start();

	return true;
}

void CConnectToServerMgr::init(ACE_UINT32 hIP,ACE_UINT16 hport,
							   int re_connect_to_server_interval)
{
	m_host_name = CPF::hip_to_string(hIP);

	m_hServerPort = hport;

	m_re_connect_to_server_interval = re_connect_to_server_interval;
}

void CConnectToServerMgr::init(const char * host_name,ACE_UINT16 hport,int re_connect_to_server_interval)
{
	m_host_name = host_name;

	m_hServerPort = hport;

	m_re_connect_to_server_interval = re_connect_to_server_interval;

}


void CConnectToServerMgr::close()
{
	CConnectToServerMgr_Lock locker(*this);

	StopReConnectMonitor();

	DisConnectToServer();

	return;
}

int CConnectToServerMgr::SendRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout)
{
	CConnectToServerMgr_Lock locker(*this);

	if( !IsConnectedToServer() )
		return -1;

	return m_net_client.GetProxy()->SendRequest(servantid,operation,pdata,ndatalen,pResponseMsgBlock,nTimeout);
}

int CConnectToServerMgr::SendNotify(int servantid,short operation,const void * pdata,size_t ndatalen)
{
	CConnectToServerMgr_Lock locker(*this);

	if( !IsConnectedToServer() )
		return -1;

	return m_net_client.GetProxy()->SendNotify(servantid,operation,pdata,ndatalen);
}

//user interface
int CConnectToServerMgr::SendRequest(int servantid,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pResponseMsgBlock, int nTimeout)
{
	CConnectToServerMgr_Lock locker(*this);

	if( !IsConnectedToServer() )
		return -1;

	return m_net_client.GetProxy()->SendRequest(servantid,pMsgBlock,pResponseMsgBlock,nTimeout);
}

int CConnectToServerMgr::SendNotify(int servantid,MSG_BLOCK * pMsgBlock)
{
	CConnectToServerMgr_Lock locker(*this);

	if( !IsConnectedToServer() )
		return -1;

	return m_net_client.GetProxy()->SendNotify(servantid,pMsgBlock);
}

