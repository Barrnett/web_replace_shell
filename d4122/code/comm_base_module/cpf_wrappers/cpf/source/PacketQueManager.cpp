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
{//������������������Ѿ���������pqm�Ķ��У�ֱ�ӹ���ʹ��

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
		//ʣ��ռ�
		UINT_PTR left_space = m_pShareBufferHeader->dwReadPos - m_pShareBufferHeader->dwWritePos;

		return m_pShareBufferHeader->dwSizeBuffer - left_space;
	}

}

//����û��Ѿ������˻�������ͨ��pBuffer�����������pBuffer��0�����ڲ�������ɾ��
//������ɹ�������NULL.����ɹ������ػ������ĵ�ַ
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

	//����ǿ����ڴ���,������ڴ��С������100.
	if( dwSizeBuffer < (100+uDataHeadersize) )
		return NULL;

	//�û����û�д�������ָ�룬���ڲ�����
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

	//+sizeof(PACKET_HEADER)��Ŀ����Ϊ�˵�ǰ�ռ��β�ڴ治������һ������ʱ��
	//�����ڴ��㹻����һ��PACKET_HEADER�ṹ��������Ϊ��Ч��
	//+uDataHeadersize��Ϊ�˴���û�������ͷ
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

	//��Ҫ����m_pShareBufferHeader->dwReadPos,
	//��Ϊ�ڲ�ͬ��ʱ�����m_pShareBufferHeader->dwReadPos,m_pShareBufferHeader->dwReadPos���ܷ����仯

	UINT_PTR dwTempReadPos  = m_pqm->m_pShareBufferHeader->dwReadPos;

	if (m_pqm->m_pShareBufferHeader->dwWritePos < dwTempReadPos)
	{
		//ע��:������dwTempReadPos - m_pShareBufferHeader->dwWritePos>=dwRequiredMinFreeLen
		//ͷ��ָ����β��ָ��֮��Ŀ��л��������Է���
		if (dwTempReadPos - m_pqm->m_pShareBufferHeader->dwWritePos > dwRequiredMinFreeLen)
		{
			m_dwNowWritePos = m_pqm->m_pShareBufferHeader->dwWritePos;
		}
	}
	else
	{	
		//ע��:��(����)��m_dwEndPos - m_pShareBufferHeader->dwWritePos>=dwRequiredMinFreeLen
		//β���Ŀ��л���������ֱ�ӷ���
		if (m_pqm->m_dwEndPos - m_pqm->m_pShareBufferHeader->dwWritePos >= dwRequiredMinFreeLen)
		{
			m_dwNowWritePos = m_pqm->m_pShareBufferHeader->dwWritePos;
		}
		else
		{			
			//ע��:������dwTempReadPos>=dwRequiredMinFreeLen
			if (dwTempReadPos > dwRequiredMinFreeLen)//ǰ��Ŀ��л��������Է���
			{
				//��β�����еĻ���������Ϊһ����Ч֡
				//ʵ�ʳ���Ϊ0����ʾһ����յ���Ч֡
				((CPacketQueManager::PACKET_HEADER *)(m_pqm->m_pInfoBuffer + m_pqm->m_pShareBufferHeader->dwWritePos))->dwOrgSize = 0;

				//������Ч֡
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

	//������С
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
//�ӻ�������ȡ�ߵ�ǰͷ��ָʾ�İ�����Ϣ�����������ð�.
//
//����ֵ��0--û��ȡ��������Ϣ
//        ���ݰ�����--ȡ����ǰ����Ϣ�����lenΪ0���ð�����Ч��
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
	{	//��һ����д�հ׻������Ŀհ���Ӧ������ȥ,������
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
















