//////////////////////////////////////////////////////////////////////////
//EmptyDataSource.cpp

#include "PacketIO/EmptyDataSource.h"

CEmptyDataSource::CEmptyDataSource(void)
{
	m_drv_type = DRIVER_TYPE_EMPTY;
}

CEmptyDataSource::~CEmptyDataSource(void)
{
}

//��������Դ�ĵ�����
bool CEmptyDataSource::GetDataSourceName(std::string& name)
{
	name = "EMPTY_DATA_SOURCE";

	return true;
}


//��������
PACKET_LEN_TYPE CEmptyDataSource::GetPacket(RECV_PACKET& packet)
{
	ACE_Time_Value now_time = ACE_OS::gettimeofday();

	memset(&packet,0x00,sizeof(PACKET_HEADER));

	packet.pHeaderinfo = &m_packet_header;

	if( now_time.sec() - m_start_time.sec() > 1 )
	{
		m_packet_header.stamp.sec = (ACE_INT32)now_time.sec();

		m_start_time = now_time;

		return PACKET_TIMER;
	}
	else
	{
		return PACKET_EMPTY;
	}

}

