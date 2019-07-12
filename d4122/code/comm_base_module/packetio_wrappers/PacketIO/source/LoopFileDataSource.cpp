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


//��ʼ�������ݣ�ע�ⲻ�Ǵ��豸��������Щ����û�о���ʵ�֡�
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

//ֹͣ�������ݣ�ע�ⲻ�ǹر��豸��������Щ����û�о���ʵ�֡�
void CLoopFileDataSource::StopRecving()
{
	return IExtDataSource::StopRecving();

}

//��������
PACKET_LEN_TYPE CLoopFileDataSource::GetPacket(RECV_PACKET& packet)
{
	PACKET_LEN_TYPE lentype = m_pRealDataSource->GetPacket(packet);

	if( PACKET_TIMER == lentype || PACKET_OK == lentype )
	{//���������Ч�ģ������¼���ʱ��

		if( m_re_calculate_time )
		{
			//��¼���һ������ʵ��ʱ��
			m_lastPktTime.m_ts = packet.pHeaderinfo->stamp;

			if( 0 == m_lastBeginTime.m_ts.sec )
			{//����ǵ�һ����
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
		{//���Ҫ��ѭ��������ѭ�������Ѿ�����
			return PACKET_NO_MORE;
		}
		
		if( m_re_calculate_time )
		{
			//���û�а���,m_deltaҪ����
			m_delta += m_lastPktTime - m_lastBeginTime;

			m_lastBeginTime.m_ts.sec = 0;
			m_lastBeginTime.m_ts.ns = 0;
		}

		if( !m_pRealDataSource->SeekToBegin() )
		{
			return PACKET_DRV_ERROR;
		}

		//���ظð���Ч��Ҫ���ϲ����¶�ȡ
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

//��������λ��ָ��ʼ�ĵط���
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

