// VPKHelpDataSource.cpp: implementation of the CVPKHelpDataSource class.
//
//////////////////////////////////////////////////////////////////////

#include "PacketIO/VPKHelpDataSource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVPKHelpDataSource::CVPKHelpDataSource()
:m_vpkGenrator(1000),m_bEnableVirtualPacket(TRUE)
{
	memset(&m_virtualHeaderInfo,0x00,sizeof(m_virtualHeaderInfo));

	m_virtualHeaderInfo.btHeaderLength = sizeof(m_virtualHeaderInfo);
	m_virtualHeaderInfo.btVersion = 1;

	m_bkType = PACKET_EMPTY;
	m_bkRecvPacket.pHeaderinfo = NULL;

}

CVPKHelpDataSource::~CVPKHelpDataSource()
{
	Close();
}


PACKET_LEN_TYPE CVPKHelpDataSource::GetPacket(RECV_PACKET& packet)
{
	if( m_bEnableVirtualPacket && !m_pRealDataSource->IsEnableTimerPacket() )
	{
		return GetPacketWithVirtualPacket(packet);
	}
	else
	{
		return m_pRealDataSource->GetPacket(packet);
	}

}

PACKET_LEN_TYPE	CVPKHelpDataSource::GetPacketWithVirtualPacket(RECV_PACKET& packet)
{
	PACKET_LEN_TYPE type = PACKET_EMPTY;

	if( !IsBackupLastPkt() )
	{//�����ǰû�б��ݣ�����Ҫ���µ�����

		type = m_pRealDataSource->GetPacket(packet);

		ACE_ASSERT(type != PACKET_TIMER );

		if( type != PACKET_OK )
		{
			return type;			
		}
		
		m_LastEffectivePacketStamp = packet.pHeaderinfo->stamp;
	}
	
	const CTimeStamp32 * pVirtulaStamp = 
		m_vpkGenrator.GenratorVirtualPkt(m_LastEffectivePacketStamp);

	if( pVirtulaStamp )
	{
		//�����ǰû�б��ݣ�����Ҫ����
		if( !IsBackupLastPkt() )
		{
			m_bkType = type;
			m_bkRecvPacket = packet;
		}

		m_virtualHeaderInfo.stamp = pVirtulaStamp->m_ts;
		packet.pHeaderinfo = &m_virtualHeaderInfo;
		packet.pPacket = NULL;
		packet.nCaplen = 0;
		packet.nPktlen = 0;

		return PACKET_TIMER;
	}
	else
	{
		if( IsBackupLastPkt() )
		{//�����ǰ�б��ݣ��򷵻���ǰ���ݵ�����

			type = m_bkType;
			packet = m_bkRecvPacket;
		}

		m_bkType = PACKET_EMPTY;
	}

	return type;
}



//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
BOOL CVPKHelpDataSource::StartToRecv()
{
	m_bkType = PACKET_EMPTY;

	m_vpkGenrator.reset();

	return IExtDataSource::StartToRecv();
}

//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
void CVPKHelpDataSource::StopRecving()
{
	IExtDataSource::StopRecving();
}

//��������λ��ָ��ʼ�ĵط���
BOOL CVPKHelpDataSource::SeekToBegin()
{
	m_vpkGenrator.reset();

	return IExtDataSource::SeekToBegin();
}

