//////////////////////////////////////////////////////////////////////////
//MyPlatRunCtrl.cpp

#include "PacketIO/MyPlatRunCtrl.h"
#include <ace/OS_NS_unistd.h>
#include "ace/OS_NS_Thread.h"
#include "cpf/os_syscall.h"

#define TEST_DEBUG

CMyPlatRunCtrl::CMyPlatRunCtrl()
:m_empty_sleep_time(0,5*1000)
{
	m_pDataSource = NULL;
	m_enState = CMyPlatRunCtrl::STATE_UNINIT;
	m_enCmd = CMD_NONE;

	m_pLogInstance = NULL;

	m_pMonitorEvent = NULL;

	m_monitor_event_interval = 0;

	m_copy_drv_data = 0;

	m_stoptime = ACE_Time_Value::zero;
	m_starttime = ACE_Time_Value::zero;

	m_lastPktLenType = PACKET_EMPTY;
	
	m_runinfo.reset();

	m_pCopyDataBuffer = NULL;
	m_nCopyDataBufferLen = 0;

	memset(m_arr_observers,0x00,sizeof(m_arr_observers));
	m_cur_observer_nums = 0;

}

CMyPlatRunCtrl::~CMyPlatRunCtrl()
{
	close();
}


#ifdef OS_LINUX
//// linux下的_mm_pause不是真正的pause// 自己实现一个
__inline static void __attribute__((__gnu_inline__, __always_inline__, __artificial__))_mm_pause (void)
{	__asm__ __volatile__ ("pause" : : :"memory");}
#endif 

BOOL CMyPlatRunCtrl::init(ACE_Time_Value empty_sleep_time,
						  ACE_Log_Msg * pLogInstance,
						  const char * monitor_event_name,
						  int monitor_event_interval,
						  int copy_drv_data)
{
	m_empty_sleep_time = empty_sleep_time;

	m_pLogInstance = pLogInstance;

	m_monitor_event_interval = monitor_event_interval/3;

	m_copy_drv_data = copy_drv_data;

	if( m_copy_drv_data )
	{
		m_nCopyDataBufferLen = 2048;
		m_pCopyDataBuffer = new BYTE[m_nCopyDataBufferLen+2];
	}
	else
	{
		m_pCopyDataBuffer = NULL;
		m_nCopyDataBufferLen = 0;
	}

	if( m_monitor_event_interval <= 0 )
	{
		m_monitor_event_interval = 3;
	}

	if( monitor_event_name && strlen(monitor_event_name) > 0 )
	{
		m_pMonitorEvent = new my_event_t;
		if (NULL == m_pMonitorEvent)
		{
			MY_ACE_DEBUG(m_pLogInstance, (LM_ERROR,ACE_TEXT("[Err][%D]m_pMonitorEvent create failed.n")));
			return false;
		}

		if( -1 == MY_EVENT::event_init_for_server(m_pMonitorEvent,0,1,0,monitor_event_name) )
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_ERROR,ACE_TEXT("[Err][%D]create event for monitor_event failed.\n")));
			delete m_pMonitorEvent;
			m_pMonitorEvent = NULL;
		}
		else
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_ERROR,ACE_TEXT("[Info][%D]create event for monitor_event:%s success.\n"),
				monitor_event_name));
		}
	}
	else
	{
		MY_ACE_DEBUG(m_pLogInstance,
			(LM_ERROR,ACE_TEXT("[Info][%D]monitor_event_name is empty,can not create monitor event!!\n")));
	}

	m_enState = CMyPlatRunCtrl::STATE_IDLE;
	m_enCmd = CMD_NONE;

	m_starttime = ACE_Time_Value::zero;
	m_stoptime = ACE_Time_Value::zero;

	m_cur_observer_nums = 0;

	m_runinfo.reset();

	return true;
}

void CMyPlatRunCtrl::close()
{
	if( CMyPlatRunCtrl::STATE_IDLE != m_enState 
		&& CMyPlatRunCtrl::STATE_UNINIT != m_enState )
	{//如果还没有停止，则先停止
		StopTest();
	}

	if( m_pMonitorEvent )
	{
		MY_EVENT::event_destroy(m_pMonitorEvent);
		delete m_pMonitorEvent;
		m_pMonitorEvent = NULL;
	}

	if( m_pCopyDataBuffer )
	{
		delete []m_pCopyDataBuffer;
		m_pCopyDataBuffer = NULL;
	}

	m_nCopyDataBufferLen = 0;

	m_enState = CMyPlatRunCtrl::STATE_UNINIT;

	m_cur_observer_nums = 0;

	return;

}

void CMyPlatRunCtrl::GetRunInfo(CMyPlatRunCtrl::PlatRun_Info& runinfo) const
{
	this->GetRunInfo();

	memcpy(&runinfo,&m_runinfo,sizeof(m_runinfo));

}

const CMyPlatRunCtrl::PlatRun_Info& CMyPlatRunCtrl::GetRunInfo() const
{
	if( m_pDataSource )
	{
		m_pDataSource->GetDroppedPkts(
			const_cast<CMyPlatRunCtrl::PlatRun_Info&>(m_runinfo).ullDroppedPkts,
			const_cast<CMyPlatRunCtrl::PlatRun_Info&>(m_runinfo).ullDroppedBytes);
	}

	return m_runinfo;
}


bool CMyPlatRunCtrl::StartTest(IRecvDataSource* pdataSource)
{
	if( m_enState != CMyPlatRunCtrl::STATE_IDLE )
	{
		ACE_ASSERT(FALSE);
		MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Err][%D]please call CMyPlatRunCtrl::init() before StartTest()\n")));
		return false;
	}

	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CMyPlatRunCtrl::StartTest\n")));

	m_pDataSource = pdataSource;

	if( !m_pDataSource )
	{
		MY_ACE_DEBUG(m_pLogInstance,(LM_ERROR,ACE_TEXT("[Err][%D]m_pDataSource is empty\n")));
		//ACE_ASSERT(false);
		return false;
	}
	
	m_runinfo.reset();

	m_starttime = ACE_Time_Value::zero;

	m_lastPktLenType = PACKET_EMPTY;
	m_lastPktStamp = CTimeStamp32::zero;

	activate();

	if (CMyPlatRunCtrl::STATE_IDLE == m_enState)
	{
		m_enCmd = CMD_START;
		WaitDone();
		return true;
	}
	else
	{
		ACE_ASSERT(false);
		return false;
	}
}

/*add by zhongjh*/
bool CMyPlatRunCtrl::StartTest(IRecvDataSource* pdataSource, CSerialDataRecv* pSerialRecv)
{
	m_pSerialDataRecv = pSerialRecv;

	return StartTest(pdataSource);
}

bool CMyPlatRunCtrl::StopTest()
{
	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CMyPlatRunCtrl::StopTest\n")));

	if (CMyPlatRunCtrl::STATE_IDLE == m_enState 
		|| CMyPlatRunCtrl::STATE_UNINIT == m_enState )
	{		
		m_pDataSource = NULL;
		return true;
	}

	m_enCmd = CMD_STOP;

	WaitDone();

	//等待线程结束
	while( CMyPlatRunCtrl::STATE_IDLE != m_enState )
	{
		ACE_OS::sleep(1);
	}

	m_pDataSource = NULL;

	return true;

}

bool CMyPlatRunCtrl::PauseTest()
{
	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CMyPlatRunCtrl::PauseTest\n")));

	if (CMyPlatRunCtrl::STATE_RUN == m_enState)
	{
		m_enCmd = CMD_PAUSE;
		WaitDone();
		return true;
	}
	else
	{
		ACE_ASSERT(false);
		return false;
	}
}

bool CMyPlatRunCtrl::ContinueTest()
{
	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CMyPlatRunCtrl::ContinueTest\n")));

	if (CMyPlatRunCtrl::STATE_PAUSE == m_enState)
	{
		m_enCmd = CMD_CONTINUE;
		WaitDone();
		return true;
	}
	else
	{
		ACE_ASSERT(false);
		return false;
	}
}

void CMyPlatRunCtrl::Reset()
{
	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CMyPlatRunCtrl::Reset\n")));

	for(int i = 0; i < m_cur_observer_nums; ++i)
	{
		m_arr_observers[i]->Reset();
	}
}

void CMyPlatRunCtrl::OnStart()
{
	for(int i = 0; i < m_cur_observer_nums; ++i)
	{
		m_arr_observers[i]->OnStart();
	}
}

void CMyPlatRunCtrl::OnStop(int type)
{
	for(int i = 0; i < m_cur_observer_nums; ++i)
	{
		m_arr_observers[i]->OnStop(type);
	}
}

void CMyPlatRunCtrl::OnPause()
{
	for(int i = 0; i < m_cur_observer_nums; ++i)
	{
		m_arr_observers[i]->OnPause();
	}
}

void CMyPlatRunCtrl::OnContinue()
{
	for(int i = 0; i < m_cur_observer_nums; ++i)
	{
		m_arr_observers[i]->OnContinue();
	}
}

int CMyPlatRunCtrl::svc(void)
{
	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CMyPlatRunCtrl::svc\n")));

    while (CMD_START != m_enCmd)
    {
        // 等待开始命令
        ACE_ASSERT(CMyPlatRunCtrl::STATE_IDLE == m_enState);
        ACE_ASSERT(CMD_NONE == m_enCmd);
        ACE_OS::sleep(1);
    }

	CPF::BindThreadCPU(1<<1);

    m_enCmd = CMD_NONE;
    m_enState = CMyPlatRunCtrl::STATE_RUN;

	m_stoptime = ACE_Time_Value::zero;
	m_starttime = ACE_OS::gettimeofday();

	//type=0表示用户停止，type=1表示数据源出错，type=2表示数据回放结束
	int stoptype = 0;

    OnStart();

    BOOL bRet = m_pDataSource->StartToRecv();
	if (FALSE == bRet)
	{
		MY_ACE_DEBUG(m_pLogInstance, (LM_ERROR, ACE_TEXT("[Err][%D]m_pDataSource->StartToRecv failed, maybe Driver not install.'\n\n")));
	}

	/*add by zhongjh*/
	if (NULL == m_pSerialDataRecv)
	{
		m_pSerialDataRecv = new CSerialDataRecv;
		if (NULL == m_pSerialDataRecv)
		{
			MY_ACE_DEBUG(m_pLogInstance,(LM_ERROR,ACE_TEXT("[Err][%D]m_pSerialDataRecv create failed.\n")));
		}
	}
	else
	{
		m_pSerialDataRecv->f_StartRecv();
	}


	int nEvent_Sec_Nums = 0;//多少秒要将监视的事件激活

	unsigned int max_empty_nums = m_monitor_event_interval*1000/2;

	if( m_empty_sleep_time.get_msec() != 0 )
	{
		max_empty_nums = (m_monitor_event_interval*1000/2)/m_empty_sleep_time.get_msec();
	}
	
	unsigned int nEvent_empyt_nums = 0;//连续多少个PACKET_EMPTY

	RECV_PACKET		packet;

    while(CMyPlatRunCtrl::STATE_STOPPING != m_enState)
    {
		// 处理命令
		if (CMD_NONE != m_enCmd)
		{
			DoCommand();
		}

		++m_runinfo.ullReadLoops;

		if( CMyPlatRunCtrl::STATE_RUN == m_enState )
		{
			#ifdef _DEBUG
				PACKET_LEN_TYPE prevPktLenType = m_lastPktLenType;
			#endif

			// RUN状态
			m_lastPktLenType = m_pDataSource->GetPacket(packet);

			switch(m_lastPktLenType)
			{
			case PACKET_OK:
				{
					////ACE_ASSERT(m_lastPktStamp<=CTimeStamp32(packet.pHeaderinfo->stamp));
					////m_lastPktStamp = CTimeStamp32(packet.pHeaderinfo->stamp);

					////++m_runinfo.ullTotalPackets;
					////m_runinfo.ullTotalBytes += packet.nPktlen;

					nEvent_empyt_nums = 0;

					/*m_copy_drv_data is always 0, delete by zhongjh
					if( m_copy_drv_data && packet.nPktlen <= 10*1024 )
					{
						if( packet.nPktlen > m_nCopyDataBufferLen  )
						{
							delete []m_pCopyDataBuffer;

							m_nCopyDataBufferLen = packet.nPktlen;

							m_pCopyDataBuffer = new BYTE[m_nCopyDataBufferLen+2];
						}

						memcpy(m_pCopyDataBuffer,packet.pPacket,packet.nPktlen);

						m_pCopyDataBuffer[packet.nPktlen] = 0;
						m_pCopyDataBuffer[packet.nPktlen+1] = 0;

						packet.pPacket = m_pCopyDataBuffer;
					}*/

					PushPacketToObservers(m_lastPktLenType, packet);
				}
				break;

			case PACKET_CALL_NEXT:
				{
					//do nothing;
				}
				break;


			case PACKET_TIMER:
				{
					////ACE_ASSERT(m_lastPktStamp<=CTimeStamp32(packet.pHeaderinfo->stamp));
					////m_lastPktStamp = CTimeStamp32(packet.pHeaderinfo->stamp);

					////++m_runinfo.ullTimerPackets;
					++nEvent_Sec_Nums;

					nEvent_empyt_nums = 0;

					if( m_monitor_event_interval <= nEvent_Sec_Nums )
					{
						////if( m_pMonitorEvent )
						{
							MY_EVENT::event_signal(m_pMonitorEvent);
							nEvent_Sec_Nums = 0;
						}
					}

					PushPacketToObservers(m_lastPktLenType,packet);

					/*add by zhongjh*/
					do
					{
						m_lastPktLenType = m_pSerialDataRecv->f_GetPacket(packet);
						switch (m_lastPktLenType)
						{
							case PACKET_SERIAL_OK:
								PushPacketToObservers(m_lastPktLenType, packet);
								break;

							default:
								break;
						}
						
					}while(PACKET_SERIAL_EMPTY != m_lastPktLenType);
				}
				break;

			case PACKET_NO_MORE:
				m_enState = CMyPlatRunCtrl::STATE_STOPPING;//需要退出读数据循环
				stoptype = 2;
				break;

			case PACKET_DRV_ERROR:
				m_enState = CMyPlatRunCtrl::STATE_STOPPING;//需要退出读数据循环
				stoptype = 1;
				break;

			case PACKET_EMPTY:
				{
					++nEvent_empyt_nums;

					if( nEvent_empyt_nums >= max_empty_nums )
					{
						if( m_pMonitorEvent )
						{
							MY_EVENT::event_signal(m_pMonitorEvent);
							nEvent_Sec_Nums = 0;
						}

						nEvent_empyt_nums = 0;
					}

					if( m_empty_sleep_time.usec() == 0 && m_empty_sleep_time.sec() == 0 )
					{
						_mm_pause();
					}
					else
					{
						m_pDataSource->WaitForPacket(m_empty_sleep_time);
					}
				}
				break;

			default:
				//ACE_ASSERT(FALSE);
			break;
			}//end switch(m_lastPktLenType)

		}//end if( CMyPlatRunCtrl::STATE_RUN == m_enState )		
		else if (CMyPlatRunCtrl::STATE_PAUSE == m_enState)
		{// 处理暂停状态
			ACE_OS::sleep(1);
			continue;
		}
		else if (CMyPlatRunCtrl::STATE_STOPPING == m_enState)
		{
			stoptype = 0;
			break;
		}
	}//end while(CMyPlatRunCtrl::STATE_IDLE != m_enState)

	m_stoptime = ACE_OS::gettimeofday();

    m_pDataSource->StopRecving();

    OnStop(stoptype);

	m_enState = CMyPlatRunCtrl::STATE_IDLE;

    return 0;
};

void CMyPlatRunCtrl::DoCommand()
{	
	ACE_ASSERT(CMD_NONE != m_enCmd);

	// 处理命令
	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]CMyPlatRunCtrl::DoCommand in packet_thread\n")));

	switch (m_enState)
	{
	case CMyPlatRunCtrl::STATE_RUN:
		{
			switch (m_enCmd)
			{
			case CMD_STOP:
				{
					m_enCmd = CMD_NONE;
					m_enState = CMyPlatRunCtrl::STATE_STOPPING;
				}
				break;
			case CMD_PAUSE:
				{
					m_enCmd = CMD_NONE;
					m_enState = CMyPlatRunCtrl::STATE_PAUSE;
				}
				break;
			default:
				{
					ACE_ASSERT(false);
				}
			}
		}
		break;
	case CMyPlatRunCtrl::STATE_PAUSE:
		{
			switch (m_enCmd)
			{
			case CMD_STOP:
				{
					m_enCmd = CMD_NONE;
					m_enState = CMyPlatRunCtrl::STATE_STOPPING;
				}
				break;
			case CMD_CONTINUE:
				{
					m_enCmd = CMD_NONE;
					m_enState = CMyPlatRunCtrl::STATE_RUN;
					OnContinue();
				}
				break;
			default:
				{
					ACE_ASSERT(false);
				}
			}
		}
		break;
	default:
		{
			ACE_ASSERT(false);
		}
	}
	
	m_enCmd = CMD_NONE;
	return;
}

bool CMyPlatRunCtrl::AttachObserver(CMyBaseObserver* pObserver)
{
	if( m_cur_observer_nums >= MAX_OBSERVER_NUMS )
	{
		return false;
	}

	for(int i = 0; i < m_cur_observer_nums; ++i)
	{
		if( m_arr_observers[i] == pObserver )
		{
			return false;
		}	
	}
 
	pObserver->m_pPlatRunCtrl = this;

	m_arr_observers[m_cur_observer_nums] = pObserver;
	++m_cur_observer_nums;

	//CFluxControlFrameObserver::OnAttachOKToPlatCtrl
	if (pObserver->OnAttachOKToPlatCtrl() == false) { return(false); }

    return true;
}

bool CMyPlatRunCtrl::DetachObserver(CMyBaseObserver* pObserver)
{
	int index;

	for(index = 0; index < m_cur_observer_nums; ++index)
    {
		if( m_arr_observers[index] == pObserver )
		{
			pObserver->OnDetachFromPlatCtrl();

			break;
		}	
    }

	if( index < m_cur_observer_nums )
	{
		for(int j = index+1; j < m_cur_observer_nums; ++j)
		{
			m_arr_observers[j-1] = m_arr_observers[j];
		}

		--m_cur_observer_nums;

		return true;
	}

    return false;
}

void CMyPlatRunCtrl::DetachAllObserver()
{
    m_cur_observer_nums = 0;
}

void CMyPlatRunCtrl::WaitDone()
{
	while( CMD_NONE != m_enCmd )
	{
		ACE_OS::sleep(ACE_Time_Value(1));//5ms
	}

	return;
}

void CMyPlatRunCtrl::SignalMonitorEvent()
{
	if (m_pMonitorEvent)
	{
		MY_EVENT::event_signal(m_pMonitorEvent);
	}

	return;
}

