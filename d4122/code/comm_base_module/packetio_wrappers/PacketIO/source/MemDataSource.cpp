//////////////////////////////////////////////////////////////////////////
//MemDataSource.cpp

#include "PacketIO/MemDataSource.h"

CMemDataSource::CMemDataSource(void)
:m_nMaxLen(0),m_nMinLen(0),m_pModelData(NULL)
{
	memset(&m_packetheader,0x00,sizeof(m_packetheader));
	m_packetheader.btHeaderLength = sizeof(m_packetheader);
}

CMemDataSource::~CMemDataSource(void)
{
	Close();
}

bool CMemDataSource::GetDataSourceName(std::string& name)
{
	name = "memory_random_data";

	return true;		
}

void CMemDataSource::init(unsigned int minlen,unsigned int maxlen,WORD wdProto,
		  const unsigned char * pModelData,unsigned int modeldata_len)
{
	m_nMaxLen = maxlen;
	m_nMinLen = minlen;

	if( pModelData && modeldata_len != 0 )
	{
		m_pModelData = new unsigned char[m_nMaxLen];

		memcpy(m_pModelData,pModelData,mymin(modeldata_len,m_nMaxLen));

		if( modeldata_len < m_nMaxLen )
		{
			memset(m_pModelData+modeldata_len,0x00,m_nMaxLen-modeldata_len);
		}
	}
	else
	{
		m_pModelData = new unsigned char[maxlen];

		for( unsigned int i = 0; i < maxlen; ++i)
		{
			m_pModelData[i] = (i&0xff);
		}
	}

	m_packetheader.wdProto = wdProto;

	return;
}

void CMemDataSource::fini()
{
	if( m_pModelData )
	{
		delete []m_pModelData;
		m_pModelData = NULL;
	}

	return;
}

//�൱��init+OpenDataSource
BOOL CMemDataSource::Open(unsigned int minlen,unsigned int maxlen,WORD wdProto,
						  const unsigned char * pModelData,unsigned int modeldata_len)
{
	init(minlen,maxlen,wdProto,pModelData,modeldata_len);

	OpenDataSource();

	return true;
}

//�൱��CloseDataSource()+fini
void CMemDataSource::Close()
{
	CloseDataSource();

	fini();
}

//��������
PACKET_LEN_TYPE CMemDataSource::GetPacket(RECV_PACKET& packet)
{
	if( m_nMaxLen != m_nMinLen )
	{
		++m_nCurLen;

		if( m_nCurLen >= m_nMaxLen+1)
			m_nCurLen = m_nMinLen;
	}
	else
	{
		m_nCurLen = m_nMinLen;
	}

	m_packetheader.stamp.from_ace_timevalue(ACE_OS::gettimeofday());

	packet.pHeaderinfo = &m_packetheader;
	packet.pPacket = m_pModelData;
	packet.nCaplen = packet.nPktlen = (int)m_nCurLen;
	packet.nOffsetBit = 0;
	packet.nCapAddiBitLen = 0;

	return PACKET_OK;
}

	
//���豸���������Ӧ���ڲ������ú�֮�����
//�ɹ�����0,���򷵻�-1
int CMemDataSource::OpenDataSource()
{
	if( m_nMaxLen != m_nMinLen )
	{
		ACE_OS::srand((unsigned int)ACE_OS::gettimeofday().sec());
	}

	m_nCurLen = m_nMinLen-1;

	return 0;
}

//�ر��豸
void CMemDataSource::CloseDataSource()
{
	return;
}

//�õ���Ϊ�ϲ㴦�����������ײ㶪������Ŀ,-1��ʾ��Ч
void CMemDataSource::GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
{
	pkts = bytes = (ACE_UINT64)0;

}

//��ȡ�豸��������Ϣ��1��ʾ���ӣ�0��ʾû�����ӣ�-1��ʾδ֪
int CMemDataSource::GetConnectState()
{
	return 1;
}


