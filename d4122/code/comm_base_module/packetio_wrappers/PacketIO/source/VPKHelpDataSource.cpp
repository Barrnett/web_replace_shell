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
	{//如果以前没有备份，则需要读新的数据

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
		//如果以前没有备份，才需要备份
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
		{//如果以前有备份，则返回以前备份的数据

			type = m_bkType;
			packet = m_bkRecvPacket;
		}

		m_bkType = PACKET_EMPTY;
	}

	return type;
}



//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
BOOL CVPKHelpDataSource::StartToRecv()
{
	m_bkType = PACKET_EMPTY;

	m_vpkGenrator.reset();

	return IExtDataSource::StartToRecv();
}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CVPKHelpDataSource::StopRecving()
{
	IExtDataSource::StopRecving();
}

//将读数据位置指向开始的地方。
BOOL CVPKHelpDataSource::SeekToBegin()
{
	m_vpkGenrator.reset();

	return IExtDataSource::SeekToBegin();
}

