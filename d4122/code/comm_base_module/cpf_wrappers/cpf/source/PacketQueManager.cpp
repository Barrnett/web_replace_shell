// PacketQueManager.cpp: implementation of the CPacketQueManager class.
//
//////////////////////////////////////////////////////////////////////

#include "cpf/PacketQueManager.h"
#include "cpf/Common_Func_Macor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CPacketQueManager::CPacketQueManager()
:m_pShareBufferHeader(NULL),m_pqmReader(NULL),m_pqmWriter(NULL)
{

	m_pInfoBuffer = NULL;
	m_dwEndPos = 0;

	m_bCreateBuffer = false;
}

CPacketQueManager::~CPacketQueManager()
{
	DestroyPQM();
}

CPacketQueManager::CPacketQueManager(const CPacketQueManager& pqm)
{
	ACE_UNUSED_ARG(pqm);
	ACE_ASSERT(FALSE);
}
const CPacketQueManager& CPacketQueManager::operator = (const CPacketQueManager& pqm)
{
	ACE_UNUSED_ARG(pqm);

	ACE_ASSERT(FALSE);

	return *this;
}

void * CPacketQueManager::CreatePQM(UINT_PTR dwSizeBuffer, UINT_PTR uDataHeadersize,void * pBuffer)
{
	return FormatShareBuffer(dwSizeBuffer,uDataHeadersize,pBuffer);
}

void * CPacketQueManager::CreatePQM(void * pBuffer)
{//这个函数用于其他人已经创建好了pqm的队列，直接过来使用

	m_bCreateBuffer = false;

	m_pShareBufferHeader = (CPacketQueManager::SHARE_BUFFER_HEADER *)pBuffer;
	m_pInfoBuffer		 = (BYTE *)pBuffer + sizeof(CPacketQueManager::SHARE_BUFFER_HEADER);

	m_dwEndPos = m_pShareBufferHeader->dwSizeBuffer;

	return pBuffer;
}

void CPacketQueManager::DestroyPQM()
{
	if( m_pqmReader )
	{
		m_pqmReader->Destroy();
		delete m_pqmReader;
		m_pqmReader = NULL;
	}

	if( m_pqmWriter )
	{
		m_pqmWriter->Destroy();
		delete m_pqmWriter;
		m_pqmWriter = NULL;
	}

	if( m_bCreateBuffer )
	{
		delete []((BYTE *)m_pShareBufferHeader);
		m_bCreateBuffer = FALSE;
		m_pShareBufferHeader = NULL;
	}

	return;
}

ACE_UINT32 CPacketQueManager::GetBufferUsed(OUT ACE_UINT32* pTotalLen) const
{
	if( pTotalLen )
	{
		*pTotalLen = m_pShareBufferHeader->dwSizeBuffer;
	}

	if( m_pShareBufferHeader->dwWritePos >= m_pShareBufferHeader->dwReadPos )
	{
		return m_pShareBufferHeader->dwWritePos - m_pShareBufferHeader->dwReadPos;
	}
	else
	{
		//剩余空间
		UINT_PTR left_space = m_pShareBufferHeader->dwReadPos - m_pShareBufferHeader->dwWritePos;

		return m_pShareBufferHeader->dwSizeBuffer - left_space;
	}

}

//如果用户已经开辟了缓冲区，通过pBuffer传进来，如果pBuffer＝0，由内部创建和删除
//如果不成功，返回NULL.如果成功，返回缓冲区的地址
CPQMWriter * CPacketQueManager::CreateWritter()
{
	m_pqmWriter = new CPQMWriter;

	if( m_pqmWriter )
	{
		m_pqmWriter->Create(this);
	}	

	return m_pqmWriter;
}


CPQMReader * CPacketQueManager::CreateReader()
{
	m_pqmReader = new CPQMReader;

	if( m_pqmReader )
	{
		m_pqmReader->Create(this);
	}	

	return m_pqmReader;
}


void CPacketQueManager::ClearQueue()
{
	m_pqmReader->ClearQueue();
	m_pqmWriter->ClearQueue();

	if( m_pShareBufferHeader )
	{
		m_pShareBufferHeader->dwReadPos = 0;
		m_pShareBufferHeader->dwWritePos = 0;			
	}

	return;
}

void * CPacketQueManager::FormatShareBuffer(UINT_PTR dwSizeBuffer, UINT_PTR uDataHeadersize,void * pBuffer)
{
	ACE_ASSERT( dwSizeBuffer >= (100+uDataHeadersize) );

	//如果是开辟内存者,传入的内存大小至少是100.
	if( dwSizeBuffer < (100+uDataHeadersize) )
		return NULL;

	//用户如果没有传入数据指针，则内部创建
	if( !pBuffer )
	{
		pBuffer = new BYTE[dwSizeBuffer];

		if( !pBuffer )
			return NULL;

		m_bCreateBuffer = TRUE;
	}

	m_pShareBufferHeader = (CPacketQueManager::SHARE_BUFFER_HEADER *)pBuffer;
	m_pInfoBuffer		 = (BYTE *)pBuffer + sizeof(CPacketQueManager::SHARE_BUFFER_HEADER);

	m_pShareBufferHeader->dwReadPos = 0;
	m_pShareBufferHeader->dwWritePos = 0;

	m_pShareBufferHeader->u64ReadNums = 0;
	m_pShareBufferHeader->u64WriteNums = 0;

	//+sizeof(PACKET_HEADER)的目的是为了当前空间结尾内存不够放下一个包的时候，
	//后面内存足够放下一个PACKET_HEADER结构的数据置为无效。
	//+uDataHeadersize是为了存放用户的数据头
	m_dwEndPos = dwSizeBuffer - 
		(sizeof(CPacketQueManager::SHARE_BUFFER_HEADER)+sizeof(CPacketQueManager::PACKET_HEADER)+uDataHeadersize);

	m_pShareBufferHeader->dwSizeBuffer = m_dwEndPos;

	m_pShareBufferHeader->dwDataHeaderSize = uDataHeadersize;

	return m_pShareBufferHeader;
}

void * CPacketQueManager::GetHeaderData(UINT_PTR* uHeaderLen) const
{
	ACE_ASSERT(m_pShareBufferHeader);
	if( m_pShareBufferHeader == NULL )
		return NULL;

	if( uHeaderLen )
		*uHeaderLen = m_pShareBufferHeader->dwDataHeaderSize;

	if( m_pShareBufferHeader->dwDataHeaderSize == 0 )
		return NULL;

	return (void *)((BYTE *)m_pShareBufferHeader + m_pShareBufferHeader->dwSizeBuffer + sizeof(SHARE_BUFFER_HEADER));
}

////////////////////////////////////////////////////////////////////////////
//CPQMWriter
//////////////////////////////////////////////////////////////////////////

CPQMWriter::CPQMWriter()
:m_dwLastReservePackLen(0),m_dwNowWritePos(0)
{
	m_pqm = NULL;
}

CPQMWriter::~CPQMWriter()
{

}

void CPQMWriter::Create(CPacketQueManager * pqm)
{
	m_dwLastReservePackLen = 0;

	m_pqm = pqm;
}

void CPQMWriter::Destroy()
{
	
}


BOOL CPQMWriter::WritePacket(const void * packet,UINT_PTR dwPacketSize)
{
	BYTE * pBuffer = ReserveBufferWritePacket(dwPacketSize);

	if( pBuffer != NULL )
	{
		if( (int)dwPacketSize > 0 )
			memcpy( pBuffer, packet, dwPacketSize );

		WriteReservedBufferUseRealSize(dwPacketSize);

		return TRUE;		
	}

	return FALSE;
}


BYTE *  CPQMWriter::ReserveBufferWritePacket(UINT_PTR dwPacketSize)
{
	ACE_ASSERT( m_pqm->m_pShareBufferHeader && dwPacketSize > 0 );

	if( dwPacketSize == 0 )
		return NULL;

	m_dwNowWritePos=(UINT_PTR)-1;	

	UINT_PTR dwRequiredMinFreeLen = GET_BYTE_ALIGN(dwPacketSize + sizeof(CPacketQueManager::PACKET_HEADER),sizeof(PVOID));

	//需要拷贝m_pShareBufferHeader->dwReadPos,
	//因为在不同的时候访问m_pShareBufferHeader->dwReadPos,m_pShareBufferHeader->dwReadPos可能发生变化

	UINT_PTR dwTempReadPos  = m_pqm->m_pShareBufferHeader->dwReadPos;

	if (m_pqm->m_pShareBufferHeader->dwWritePos < dwTempReadPos)
	{
		//注意:不能用dwTempReadPos - m_pShareBufferHeader->dwWritePos>=dwRequiredMinFreeLen
		//头部指针与尾部指针之间的空闲缓冲区可以分配
		if (dwTempReadPos - m_pqm->m_pShareBufferHeader->dwWritePos > dwRequiredMinFreeLen)
		{
			m_dwNowWritePos = m_pqm->m_pShareBufferHeader->dwWritePos;
		}
	}
	else
	{	
		//注意:能(可以)用m_dwEndPos - m_pShareBufferHeader->dwWritePos>=dwRequiredMinFreeLen
		//尾部的空闲缓冲区可以直接分配
		if (m_pqm->m_dwEndPos - m_pqm->m_pShareBufferHeader->dwWritePos >= dwRequiredMinFreeLen)
		{
			m_dwNowWritePos = m_pqm->m_pShareBufferHeader->dwWritePos;
		}
		else
		{			
			//注意:不能用dwTempReadPos>=dwRequiredMinFreeLen
			if (dwTempReadPos > dwRequiredMinFreeLen)//前面的空闲缓冲区可以分配
			{
				//将尾部空闲的缓冲区填充成为一个无效帧
				//实际长度为0，表示一个填空的无效帧
				((CPacketQueManager::PACKET_HEADER *)(m_pqm->m_pInfoBuffer + m_pqm->m_pShareBufferHeader->dwWritePos))->dwOrgSize = 0;

				//跳过无效帧
				m_pqm->m_pShareBufferHeader->dwWritePos = 0;
				m_dwNowWritePos = 0;

				return ReserveBufferWritePacket(dwPacketSize);
			}
		}
	}

	if (m_dwNowWritePos != (UINT_PTR)-1)
	{
		m_dwLastReservePackLen = dwPacketSize;

		return (m_pqm->m_pInfoBuffer+m_dwNowWritePos+sizeof(CPacketQueManager::PACKET_HEADER));
	}

	return NULL;

}

BOOL CPQMWriter::WriteReservedBufferUseRealSize(UINT_PTR dwRealPacketSize)
{
	ACE_ASSERT(m_dwNowWritePos!=(UINT_PTR)-1);
	ACE_ASSERT( dwRealPacketSize <= m_dwLastReservePackLen && dwRealPacketSize > 0 );

	if( dwRealPacketSize == 0 
		|| dwRealPacketSize > m_dwLastReservePackLen )
	{
		return FALSE;
	}

	((CPacketQueManager::PACKET_HEADER *)(m_pqm->m_pInfoBuffer + m_dwNowWritePos))->dwOrgSize = dwRealPacketSize;

	//调整大小
	m_dwNowWritePos += GET_BYTE_ALIGN(dwRealPacketSize + sizeof(CPacketQueManager::PACKET_HEADER),sizeof(PVOID));	

	m_pqm->m_pShareBufferHeader->dwWritePos = m_dwNowWritePos;	

	++m_pqm->m_pShareBufferHeader->u64WriteNums;

	return TRUE;
}





/////////////////////////////////////////////////////////////////////////////////////
//CPQMReader
/////////////////////////////////////////////////////////////////////////////////////

CPQMReader::CPQMReader()
:m_dwLastReadedPacketLen(0)
{
	m_pqm = NULL;
}

CPQMReader::~CPQMReader()
{

}

void CPQMReader::Create(CPacketQueManager * pqm)
{
	m_pqm = pqm;

	m_dwLastReadedPacketLen = 0;
}

void CPQMReader::Destroy()
{
	m_dwLastReadedPacketLen = 0;
}


//
//从缓冲区中取走当前头部指示的包的信息，但不拷贝该包.
//
//返回值：0--没有取到数据信息
//        数据包长度--取到当前包信息，如果len为0，该包是无效的
//
//
UINT_PTR CPQMReader::ReadNextPacket(void*& pPakcet)
{
	ACE_ASSERT( m_pqm->m_pShareBufferHeader );	

	if( m_dwLastReadedPacketLen != 0 )
	{
		SkipToNextPacket();
		m_dwLastReadedPacketLen = 0;
	}

	ACE_ASSERT(m_dwLastReadedPacketLen==0);

	if(m_pqm->m_pShareBufferHeader->dwWritePos == m_pqm->m_pShareBufferHeader->dwReadPos )
	{
		return 0;
	}

	m_dwLastReadedPacketLen = ((CPacketQueManager::PACKET_HEADER *)(m_pqm->m_pInfoBuffer + m_pqm->m_pShareBufferHeader->dwReadPos))->dwOrgSize;

	if (m_dwLastReadedPacketLen == 0)
	{	//是一个添写空白缓冲区的空包，应该跳过去,继续读
		m_pqm->m_pShareBufferHeader->dwReadPos = 0;

		m_dwLastReadedPacketLen = ReadNextPacket(pPakcet);

		return m_dwLastReadedPacketLen;
	}
	else
	{
		pPakcet = m_pqm->m_pInfoBuffer + m_pqm->m_pShareBufferHeader->dwReadPos + sizeof(CPacketQueManager::PACKET_HEADER);			
	}		

	return m_dwLastReadedPacketLen;	
}

void CPQMReader::SkipToNextPacket()
{
	ACE_ASSERT( m_pqm->m_pShareBufferHeader 
		&& m_pqm->m_pShareBufferHeader->dwReadPos != m_pqm->m_pShareBufferHeader->dwWritePos );

	UINT_PTR dwPacketLen = ((CPacketQueManager::PACKET_HEADER *)(m_pqm->m_pInfoBuffer + m_pqm->m_pShareBufferHeader->dwReadPos))->dwOrgSize;

	ACE_ASSERT(dwPacketLen!=0);

	UINT_PTR dwNextReadPos = m_pqm->m_pShareBufferHeader->dwReadPos 
		+ GET_BYTE_ALIGN(dwPacketLen,sizeof(PVOID)) 
		+ sizeof(CPacketQueManager::PACKET_HEADER);

	ACE_ASSERT(dwNextReadPos<=m_pqm->m_dwEndPos);

	++m_pqm->m_pShareBufferHeader->u64ReadNums;

	m_pqm->m_pShareBufferHeader->dwReadPos = dwNextReadPos;

	return;
}
















