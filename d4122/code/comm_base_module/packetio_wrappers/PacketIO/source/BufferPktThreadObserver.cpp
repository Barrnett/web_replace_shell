//////////////////////////////////////////////////////////////////////////
//BufferPktThreadObserver.cpp

#include "PacketIO/BufferPktThreadObserver.h"
#include "cpf/PacketQueManager.h"

ACE_THR_FUNC_RETURN CBufferPktThreadObserver::MyProcThread (void * param)
{
	CBufferPktThreadObserver * pObserver = 
		(CBufferPktThreadObserver *)param;

	pObserver->MyThreadWork();

	return NULL;
}

CBufferPktThreadObserver::CBufferPktThreadObserver(void)
{
	m_pPQM  = NULL;
	m_pReader = NULL;
	m_pWriter = NULL;

	m_hThread = 0;

	m_pRealObserver = NULL;

	m_bdelete = false;
	m_block = true;

	m_bQuit = FALSE;
}

CBufferPktThreadObserver::~CBufferPktThreadObserver(void)
{
	fini();	
}


//需要缓存的大小，如果buffersize==0，表示不需要使用缓存和使用多线程
BOOL CBufferPktThreadObserver::init(unsigned int buffersize,
									CMyBaseObserver * pRealObserver,
									bool bdelete,bool block)
{
	m_pRealObserver = pRealObserver;

	m_bdelete = bdelete;
	m_block = block;

	if( buffersize > 0 )
	{
		m_pPQM = new CPacketQueManager;

		if( m_pPQM )
		{
			void * pbuffer = m_pPQM->CreatePQM(buffersize);

			if( pbuffer )
			{
				m_pReader = m_pPQM->CreateReader();
				m_pWriter = m_pPQM->CreateWritter();
			}
			else
			{
				delete m_pPQM;
				m_pPQM = NULL;
				return false;
			}
		}
	}

	return TRUE;
}

void CBufferPktThreadObserver::fini()
{
	if( m_pPQM )
	{
		m_pPQM->DestroyPQM();
		delete m_pPQM;
		m_pPQM = NULL;

		m_pReader = NULL;
		m_pWriter = NULL;
	}

	if( m_bdelete )
	{
		delete m_pRealObserver;
		m_pRealObserver = NULL;
	}
}

BOOL CBufferPktThreadObserver::Reset()
{
	m_pRealObserver->Reset();

	if( m_pPQM )
	{
		m_pPQM->ClearQueue();
	}

	return true;
}

BOOL CBufferPktThreadObserver::OnStart()
{
	m_pRealObserver->OnStart();

	if( m_pPQM )
	{
		m_bQuit = FALSE;

		m_pPQM->ClearQueue();

		ACE_Thread::spawn(MyProcThread,this,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			NULL,&m_hThread);
	}
	
	return true;
}

BOOL CBufferPktThreadObserver::OnStop(int type)
{
	if( m_pPQM )
	{
		m_bQuit = TRUE;

		ACE_THR_FUNC_RETURN status;

		ACE_Thread::join(m_hThread,&status);
	}

	m_pRealObserver->OnStop(type);

	return true;
}

BOOL CBufferPktThreadObserver::OnPause()
{
	m_pRealObserver->OnPause();

	return true;
}

BOOL CBufferPktThreadObserver::OnContinue()
{
	m_pRealObserver->OnContinue();

	return true;
}

BOOL CBufferPktThreadObserver::Push(PACKET_LEN_TYPE type,RECV_PACKET& packet)
{
	if( !m_pWriter )
	{
		return m_pRealObserver->Push(type,packet);
	}

	if( PACKET_OK == type )
	{//缓存数据

	}
	else if( PACKET_TIMER == type )
	{//缓存定时数据

		packet.nCaplen = 0;
	}
	else
	{
		ACE_ASSERT(FALSE);
	}

	unsigned int sizebuffer = Package_OnePacket(NULL,type,packet);

	BYTE * pdata = NULL;

	if( m_block )
	{
		pdata = m_pWriter->ReserveBufferWritePacket(sizebuffer,&m_bQuit);
	}
	else
	{
		pdata = m_pWriter->ReserveBufferWritePacket(sizebuffer);
	}

	if( m_bQuit )
	{
		return true;
	}
	else if( pdata )
	{
		Package_OnePacket(pdata,type,packet);

		m_pWriter->WriteReservedBuffer();
	}
	
	return TRUE;

}

void CBufferPktThreadObserver::MyThreadWork()
{
	UINT_PTR packetlen = 0;
	void * pdata = NULL;

	int nEmptyNums = 0;

	while( 1 )
	{
		packetlen = m_pReader->ReadNextPacket(pdata);

		if( packetlen == 0 )
		{//如果没有数据

			if( m_bQuit )//如果要求循环退出
			{
				if( ++nEmptyNums > 2 )
					break;
				else
					ACE_OS::sleep(ACE_Time_Value(0,5000));
			}
			else
			{
				ACE_OS::sleep(ACE_Time_Value(0,5000));
			}
		}
		else
		{
			PACKET_LEN_TYPE lentype;

			RECV_PACKET * pPacket = NULL;

			UnPackage_OnePacket(pdata,lentype,pPacket);

			switch(lentype)
			{
			case PACKET_OK:
			case PACKET_TIMER:
				{
					m_pRealObserver->Push(lentype,*pPacket);
				}
				break;

			default:
				ACE_ASSERT(false);
				break;
			}		
		}
	}

	return;
}

