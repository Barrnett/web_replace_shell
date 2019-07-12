//////////////////////////////////////////////////////////////////////////
//LoopFileDataSource.cpp

#include "PacketIO/LoopFileDataSource.h"

CLoopFileDataSource::CLoopFileDataSource(void)
{
	m_delta.m_ts.sec = 0;
	m_delta.m_ts.ns = 0;

	m_lastBeginTime.m_ts.sec = 0;
	m_lastBeginTime.m_ts.ns = 0;

	m_lastPktTime.m_ts.sec = 0;
	m_lastPktTime.m_ts.ns = 0;

	m_nDefLoops = 0;
	m_nCurLoops = 0;
	m_re_calculate_time = true;
}

CLoopFileDataSource::~CLoopFileDataSource(void)
{
	Close();
}


//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
BOOL CLoopFileDataSource::StartToRecv()
{
	m_delta.m_ts.sec = 0;
	m_delta.m_ts.ns = 0;

	m_lastBeginTime.m_ts.sec = 0;
	m_lastBeginTime.m_ts.ns = 0;

	m_lastPktTime.m_ts.sec = 0;
	m_lastPktTime.m_ts.ns = 0;

	m_nCurLoops = 0;

	return IExtDataSource::StartToRecv();
}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void CLoopFileDataSource::StopRecving()
{
	return IExtDataSource::StopRecving();

}

//接收数据
PACKET_LEN_TYPE CLoopFileDataSource::GetPacket(RECV_PACKET& packet)
{
	PACKET_LEN_TYPE lentype = m_pRealDataSource->GetPacket(packet);

	if( PACKET_TIMER == lentype || PACKET_OK == lentype )
	{//如果包是有效的，则重新计算时间

		if( m_re_calculate_time )
		{
			//记录最近一个包的实际时间
			m_lastPktTime.m_ts = packet.pHeaderinfo->stamp;

			if( 0 == m_lastBeginTime.m_ts.sec )
			{//如果是第一个包
				m_lastBeginTime.m_ts = packet.pHeaderinfo->stamp;
			}

			CTimeStamp32 curTime = packet.pHeaderinfo->stamp;

			curTime += m_delta;

			packet.pHeaderinfo->stamp = curTime.m_ts;
		}

		return lentype;

	}
	else if( PACKET_NO_MORE == lentype )
	{
		++m_nCurLoops;

		if( m_nDefLoops != 0 && m_nCurLoops >= m_nDefLoops )
		{//如果要求循环，并且循环次数已经够了
			return PACKET_NO_MORE;
		}
		
		if( m_re_calculate_time )
		{
			//如果没有包了,m_delta要增加
			m_delta += m_lastPktTime - m_lastBeginTime;

			m_lastBeginTime.m_ts.sec = 0;
			m_lastBeginTime.m_ts.ns = 0;
		}

		if( !m_pRealDataSource->SeekToBegin() )
		{
			return PACKET_DRV_ERROR;
		}

		//返回该包无效，要求上层重新读取
		return PACKET_CALL_NEXT;
	}
	else
	{
		return lentype;
	}
}

void CLoopFileDataSource::GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops)
{	
	if( !m_pRealDataSource )
	{
		nloops = (unsigned int)m_nDefLoops;

		pkts = bytes = -1;
		return;
	}

	m_pRealDataSource->GetTotalPkts(pkts,bytes,nloops);

	nloops *= (unsigned int)m_nDefLoops;

	return;

}

//将读数据位置指向开始的地方。
BOOL CLoopFileDataSource::SeekToBegin()
{
	m_nCurLoops = 0;

	if( m_re_calculate_time )
	{
		m_delta.m_ts.sec = 0;
		m_delta.m_ts.ns = 0;

		m_lastBeginTime.m_ts.sec = 0;
		m_lastBeginTime.m_ts.ns = 0;

		m_lastPktTime.m_ts.sec = 0;
		m_lastPktTime.m_ts.ns = 0;
	}

	return IExtDataSource::SeekToBegin();
}

