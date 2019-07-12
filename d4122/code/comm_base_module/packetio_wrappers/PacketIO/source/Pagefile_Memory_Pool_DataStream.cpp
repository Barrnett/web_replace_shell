//////////////////////////////////////////////////////////////////////////
//Pagefile_Memory_Pool_DataStream.h


#include "PacketIO/Pagefile_Memory_Pool_DataStream.h"

CPagefile_Memory_Pool_DataStream::CPagefile_Memory_Pool_DataStream(void)
{
	m_memsize = 0;

	ZeroInitQueue(&m_mem_que);
}

CPagefile_Memory_Pool_DataStream::~CPagefile_Memory_Pool_DataStream(void)
{
	Close();

	return;
}

//�൱��init+OpenDataStream
BOOL CPagefile_Memory_Pool_DataStream::Open(LPCSTR sharename,DWORD memsize)
{
	if( !init(sharename,memsize) )
		return false;

	if( 0 != OpenDataStream() )
	{
		return false;
	}

	return true;
}

//�൱��CloseDataStream+fini
void CPagefile_Memory_Pool_DataStream::Close()
{
	CloseDataStream();

	fini();

	return;
}


BOOL CPagefile_Memory_Pool_DataStream::init(LPCSTR share_mem_name, DWORD memsize)
{
	m_strShareName = share_mem_name;
	m_memsize = memsize;

	return true;

}

void CPagefile_Memory_Pool_DataStream::fini()
{
	return;
}

//���豸���������Ӧ���ڲ������ú�֮�����
//�ɹ�����0,���򷵻�-1
int CPagefile_Memory_Pool_DataStream::OpenDataStream()
{
	if( m_strShareName.empty() )
		return -1;

	int first_time = 0;

	void * mem_buffer = m_cpf_share_mem.init(m_strShareName.c_str(),m_memsize,first_time);

	if( !mem_buffer )
		return -1;

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

//�ر��豸
void CPagefile_Memory_Pool_DataStream::CloseDataStream()
{
	QueueUnInitialize(&m_mem_que);

	m_cpf_share_mem.fin();

	return;
}

//�������ݣ��ɹ�����0,ʧ�ܷ���-1
int CPagefile_Memory_Pool_DataStream::WritePacketToDevWithPacket(PACKET_LEN_TYPE type,
																 const RECV_PACKET& packet,
																 int index_array,
																 int try_nums)
{
	unsigned int sizebuffer = Package_OnePacket(NULL,type,packet);

	for(unsigned int i = 0; i < (unsigned int)try_nums; ++i)
	{
		BYTE * pdata = (BYTE *)QueueWriteAllocateBuffer(&m_mem_que,sizebuffer);

		if( pdata )
		{
			Package_OnePacket(pdata,type,packet);

			QueueWriteDataFinished(&m_mem_que,sizebuffer);

			return 0;
		}
		else
		{
			if( i != try_nums -1 )
			{
				ACE_OS::sleep(ACE_Time_Value(0,5000));
			}
		}
	}

	return -1;
}

int CPagefile_Memory_Pool_DataStream::WritePacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,
									   IN const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
									   int attach_len,const void * attach_data,
									   int index_array,int try_nums)
{
	unsigned int sizebuffer = Package_OnePacket(NULL,type,packet,context,attach_len,attach_data);

	for(unsigned int i = 0; i < (unsigned int)try_nums; ++i)
	{
		BYTE * pdata = (BYTE *)QueueWriteAllocateBuffer(&m_mem_que,sizebuffer);

		if( pdata )
		{
			Package_OnePacket(pdata,type,packet,context,attach_len,attach_data);

			QueueWriteDataFinished(&m_mem_que,sizebuffer);

			return 0;
		}
		else
		{
			if( i != try_nums -1 )
			{
				ACE_OS::sleep(ACE_Time_Value(0,5000));
			}
		}
	}

	return -1;
}


//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
int CPagefile_Memory_Pool_DataStream::GetConnectState()
{
	return m_cpf_share_mem.GetMemBuffer()? 1 : 0;
}

IFileWriteDataStream * CPagefile_Memory_Pool_DataStream::CloneAfterIni()
{
	CPagefile_Memory_Pool_DataStream * pPagefile_Memory_Pool_DataStream = new CPagefile_Memory_Pool_DataStream;

	if( pPagefile_Memory_Pool_DataStream )
	{
		if( !pPagefile_Memory_Pool_DataStream->init(m_strShareName.c_str(),(DWORD)m_memsize) )
		{
			delete pPagefile_Memory_Pool_DataStream;
			return NULL;
		}

		return pPagefile_Memory_Pool_DataStream;

	}

	return NULL;
}

ACE_UINT32 CPagefile_Memory_Pool_DataStream::GetBufferUsed(OUT ACE_UINT32 * total) const
{
	if( m_mem_que.m_pQueueBaseAddress )
	{
		return QueueQueryUsedBytes(&m_mem_que,(DWORD *)total);
	}	
	else
	{
		if( total )
		{
			*total = 0;
		}

		return 0;
	}
}

bool CPagefile_Memory_Pool_DataStream::GetDataStreamName(std::string& name)
{
	char buf[256] = {0};

	name = m_strShareName;
	name += "(share_memory)";

	return true;
}