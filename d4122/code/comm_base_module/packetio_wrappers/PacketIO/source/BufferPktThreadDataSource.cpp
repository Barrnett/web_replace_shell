////////////////////////////////////////////////////////////////////////////
//BufferPktThreadDataSource.cpp

#include "PacketIO/BufferPktThreadDataSource.h"
#include "ace/Thread.h"
#include "cpf/os_syscall.h"

ACE_THR_FUNC_RETURN CBufferPktThreadDataSource::MyProcThread (void * param)
{
	CBufferPktThreadDataSource * pDataSource = 
		(CBufferPktThreadDataSource *)param;

	pDataSource->MyThreadWork();

	return NULL;
}

CBufferPktThreadDataSource::CBufferPktThreadDataSource(void)
{
	m_share_buffer_size = 0;

	m_hThread = 0;

	ZeroInitQueue(&m_share_queue);

	m_alloc_buffer = NULL;
	m_share_buffer_size = 0;

	m_bQuit = TRUE;

	return;
}

CBufferPktThreadDataSource::~CBufferPktThreadDataSource(void)
{
	fini();
}

//pRealDatasource，实际的数据源。这个模块从这个数据源读取数据，并且缓存到内存中
//然后提供接口让应用程序来访问缓存的数据
//bufsize:缓存的大小.如果为0,表示不使用多线程.所有的访问直接转到pRealDatasource
BOOL CBufferPktThreadDataSource::init(IRecvDataSource * pRealDatasource,bool bdelete,unsigned int bufsize,const ACE_Time_Value& empty_sleep_time)
{
	m_empty_sleep_time = empty_sleep_time;

	IExtDataSource::init(pRealDatasource,bdelete);

	m_share_buffer_size = bufsize;

	if( m_share_buffer_size )
	{
		m_alloc_buffer = new BYTE[m_share_buffer_size];

		QueueInitialize_new(&m_share_queue,m_alloc_buffer,m_share_buffer_size,0);
	}

	return TRUE;
}

void CBufferPktThreadDataSource::fini()
{
	IExtDataSource::fini();

	if( m_share_buffer_size )
	{
		QueueUnInitialize(&m_share_queue);

		if( m_alloc_buffer )
		{
			delete m_alloc_buffer;
			m_alloc_buffer = NULL;
		}
	}

	return;
}


//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
BOOL CBufferPktThreadDataSource::StartToRecv()
{
	if( IExtDataSource::StartToRecv() == FALSE )
		return false;

	if( m_share_buffer_size )
	{
		m_bQuit = FALSE;

		QueueClearQueue(&m_share_queue);

		ACE_Thread::spawn(MyProcThread,this,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			NULL,&m_hThread);

	}

	return true;
}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CBufferPktThreadDataSource::StopRecving()
{
	if( m_pRealDataSource )
	{
		if( m_share_buffer_size )
		{
			m_bQuit = TRUE;

			ACE_THR_FUNC_RETURN status;

			ACE_Thread::join(m_hThread,&status);

			m_hThread = 0;

			QueueClearQueue(&m_share_queue);
		}

		IExtDataSource::StopRecving();
	}
	
	return;
}

//接收数据
PACKET_LEN_TYPE CBufferPktThreadDataSource::GetPacket(RECV_PACKET& packet)
{
	if( !m_share_buffer_size )
	{
		return m_pRealDataSource->GetPacket(packet);
	}

	BYTE * pdata = NULL;

	UINT_PTR packetlen = (UINT_PTR)QueueReadNextPacket(&m_share_queue,&pdata);

	if( packetlen == 0 )
	{//如果没有数据

		return PACKET_EMPTY;
	}

	PACKET_LEN_TYPE lentype;

	RECV_PACKET * pPacket = NULL;

	UnPackage_OnePacket(pdata,lentype,pPacket);

	switch(lentype)
	{
	case PACKET_OK:
	case PACKET_TIMER:
		{
			memcpy(&packet,pPacket,sizeof(packet));

	/*		JUST FOR TEST
	for(int i = 0; i < packet.nCaplen; ++i)
			{
				ACE_ASSERT(((BYTE *)packet.pPacket)[i] == (i&0xff));
			}*/
		}
		break;

	case PACKET_NO_MORE:
	case PACKET_DRV_ERROR:
		{
			memcpy(&packet,pPacket,sizeof(packet));
		}
		break;

	default:
		ACE_ASSERT(FALSE);
		break;
	}

	return lentype;
	
}

#ifdef OS_LINUX
//// linux下的_mm_pause不是真正的pause// 自己实现一个
__inline static void __attribute__((__gnu_inline__, __always_inline__, __artificial__))_mm_pause (void)
{	__asm__ __volatile__ ("pause" : : :"memory");}
#endif 


void CBufferPktThreadDataSource::MyThreadWork()
{
	RECV_PACKET		packet;
	PACKET_LEN_TYPE lastPktLenType;
	
	CPF::BindThreadCPU(1<<0);

	while( !m_bQuit )
	{
		lastPktLenType = m_pRealDataSource->GetPacket(packet);

		if( PACKET_OK == lastPktLenType )
		{//缓存数据

			unsigned int sizebuffer = Package_OnePacket(NULL,lastPktLenType,packet);

			BYTE * pdata = (BYTE *)QueueWriteAllocateBuffer(&m_share_queue,sizebuffer);

			if( pdata )
			{
				Package_OnePacket(pdata,lastPktLenType,packet);

				QueueWriteDataFinished(&m_share_queue,sizebuffer);
			}
		}
		else if( PACKET_TIMER == lastPktLenType )
		{//缓存定时数据
			packet.nCaplen = 0;

			unsigned int sizebuffer = Package_OnePacket(NULL,lastPktLenType,packet);

			BYTE * pdata = (BYTE *)QueueWriteAllocateBuffer(&m_share_queue,sizebuffer);

			if( pdata )
			{
				Package_OnePacket(pdata,lastPktLenType,packet);

				QueueWriteDataFinished(&m_share_queue,sizebuffer);
			}

		}
		else if( PACKET_NO_MORE == lastPktLenType || PACKET_DRV_ERROR == lastPktLenType )
		{
			packet.nCaplen = 0;

			unsigned int sizebuffer = Package_OnePacket(NULL,lastPktLenType,packet);

			BYTE * pdata = (BYTE *)QueueWriteAllocateBuffer(&m_share_queue,sizebuffer);

			if( pdata )
			{
				Package_OnePacket(pdata,lastPktLenType,packet);

				QueueWriteDataFinished(&m_share_queue,sizebuffer);
			}

			break;
		}
		else if( PACKET_EMPTY == lastPktLenType )
		{
			if( m_empty_sleep_time.sec() == 0  && m_empty_sleep_time.usec() == 0 )
			{
				_mm_pause();
			}
			else
			{
				m_pRealDataSource->WaitForPacket(ACE_Time_Value(0,1000));
			}

			continue;	
		}
		else if( PACKET_CALL_NEXT )
		{
			//do nothing
			continue;
		}
		else
		{
			ACE_ASSERT(FALSE);

			//do nothing
			continue;
		}
	}

	return;
}

ACE_UINT32 CBufferPktThreadDataSource::GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen)
{
	if( m_share_buffer_size )
	{
		return (ACE_UINT32)QueueQueryUsedBytes(&m_share_queue,(DWORD *)pTotalLen);
	}
	else
	{
		if( pTotalLen )
			*pTotalLen = 0;

		return 0;
	}
}

int CBufferPktThreadDataSource::WaitForPacket(const ACE_Time_Value& sleep_time)
{
	ACE_OS::sleep(ACE_Time_Value(0,1000));

	//ACE_OS::sleep(sleep_time);

	return 0;
}