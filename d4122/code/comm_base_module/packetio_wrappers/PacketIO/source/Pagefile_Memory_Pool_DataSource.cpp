//////////////////////////////////////////////////////////////////////////
//Pagefile_Memory_Pool_DataSource.cpp

#include "PacketIO/Pagefile_Memory_Pool_DataSource.h"

CPagefile_Memory_Pool_DataSource::CPagefile_Memory_Pool_DataSource(void)
:m_memsize(0)
{
	m_drv_type = DRIVER_TYPE_Pagefile_Memory_Pool;

	ZeroInitQueue(&m_mem_que);
}

CPagefile_Memory_Pool_DataSource::~CPagefile_Memory_Pool_DataSource(void)
{
	Close();
}


//相当于init+OpenDataSource
BOOL CPagefile_Memory_Pool_DataSource::Open(const char * share_name,size_t memsize)
{
	if( !this->init(share_name,memsize) )
	{
		return false;
	}

	return (this->OpenDataSource() == 0);
}

//相当于CloseDataSource()+fini
void CPagefile_Memory_Pool_DataSource::Close()
{
	this->CloseDataSource();

	this->fini();
}

BOOL CPagefile_Memory_Pool_DataSource::init(const char * share_name,size_t memsize)
{
	m_strShareName = share_name;
	m_memsize = memsize;

	return true;
}

void CPagefile_Memory_Pool_DataSource::fini()
{
	return;
}


bool CPagefile_Memory_Pool_DataSource::GetDataSourceName(std::string& name)
{
	char buf[256] = {0};

	name = m_strShareName;
	name += "(share_memory)";

	return true;		
}


int CPagefile_Memory_Pool_DataSource::OpenDataSource()
{
	if( m_strShareName.empty() )
		return -1;

	int first_time = 0;

	void * mem_buffer = m_cpf_share_mem.init(m_strShareName.c_str(),m_memsize,first_time);

	if( !mem_buffer )
	{
		return -1;
	}

	if( first_time )
	{
		QueueInitialize_new(&m_mem_que,(UCHAR *)mem_buffer,m_cpf_share_mem.GetMemSize(),0);
	}
	else
	{
		QueueInitialize_flow(&m_mem_que,(UCHAR *)mem_buffer,0);
	}

	return 0;
}


void CPagefile_Memory_Pool_DataSource::CloseDataSource()
{
	QueueUnInitialize(&m_mem_que);

	m_cpf_share_mem.fin();

	return;
}

PACKET_LEN_TYPE CPagefile_Memory_Pool_DataSource::GetPacket(RECV_PACKET& packet)
{
	void * pdata = NULL;

	int packetlen = QueueReadNextPacket(&m_mem_que,(PUCHAR *)&pdata);

	if( packetlen <= 0 )
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

ACE_UINT32 CPagefile_Memory_Pool_DataSource::GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen)
{
	if( m_mem_que.m_pQueueBaseAddress )
	{
		return QueueQueryUsedBytes(&m_mem_que,(DWORD *)pTotalLen);
	}	
	else
	{
		if( pTotalLen )
		{
			*pTotalLen = 0;
		}
		
		return 0;
	}
}

void CPagefile_Memory_Pool_DataSource::GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
{
	pkts = 0;
	bytes = 0;

	return;
}


int CPagefile_Memory_Pool_DataSource::GetConnectState()
{
	return m_cpf_share_mem.GetMemBuffer()? 1 : 0;
}



//询问这个设备是否可以产生定时包。
BOOL CPagefile_Memory_Pool_DataSource::IsEnableTimerPacket() const
{
	return true;
}

