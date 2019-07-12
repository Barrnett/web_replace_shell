//////////////////////////////////////////////////////////////////////////
//TimeAdjustTool.cpp

#include "TimeAdjustTool.h"


//////////////////////////////////////////////////////////////////////////
//CTimeAdjustTool
//////////////////////////////////////////////////////////////////////////

CTimeAdjustTool::CTimeAdjustTool(void)
{
	m_nAdjustType = 0;
}

CTimeAdjustTool::~CTimeAdjustTool(void)
{
}


BOOL CTimeAdjustTool::init(int nAdjustType,const CTimeStamp32 * pAdjustTimeStamp)
{
	m_nAdjustType = nAdjustType;

	if( m_nAdjustType == 2 )
	{
		if( !pAdjustTimeStamp ||
			(pAdjustTimeStamp->GetSEC() == 0 && pAdjustTimeStamp->GetNS() == 0)
			)
		{
			m_orgin_adjust_time = CTimeStamp32::zero;
		}
		else 
		{
			m_orgin_adjust_time = *pAdjustTimeStamp;
		}	
	}

	return true;
}

void CTimeAdjustTool::fini()
{
	return;
}

void CTimeAdjustTool::start_to_work()
{
	m_firstPacketTime = CTimeStamp32::zero;

	if( m_nAdjustType == 2 )
	{
		if( m_orgin_adjust_time == CTimeStamp32::zero )
		{
			m_adjustTime.m_ts.from_ace_timevalue(ACE_OS::gettimeofday());
		}
		else 
		{
			m_adjustTime = m_orgin_adjust_time;
		}	
	}

	return;
}

void CTimeAdjustTool::stop_working()
{
	return;
}

void CTimeAdjustTool::do_work(PACKET_LEN_TYPE& lentype,RECV_PACKET& packet)
{
	if( 2 == m_nAdjustType )
	{
		if( m_firstPacketTime.GetSEC() == 0 )
		{
			if( PACKET_TIMER == lentype || PACKET_OK == lentype )
			{
				m_firstPacketTime = packet.pHeaderinfo->stamp;
			}
		}

		if( PACKET_TIMER == lentype || PACKET_OK == lentype )
		{
			CTimeStamp32 curtime = m_adjustTime + 
				CTimeStamp32(packet.pHeaderinfo->stamp) - m_firstPacketTime;

			packet.pHeaderinfo->stamp.sec = curtime.GetSEC();
			packet.pHeaderinfo->stamp.ns = curtime.GetNS();
		}
	}
	else if( 1 == m_nAdjustType )
	{
		if( PACKET_TIMER == lentype || PACKET_OK == lentype )
		{
			packet.pHeaderinfo->stamp.from_ace_timevalue(ACE_OS::gettimeofday());
		}	
	}

	return;
}


//////////////////////////////////////////////////////////////////////////
//CCopyAndDiscardTool
//////////////////////////////////////////////////////////////////////////
CCopyAndDiscardTool::CCopyAndDiscardTool()
{
	m_bCopyPkt = false;
	m_discard_rate = 0;
}

CCopyAndDiscardTool::~CCopyAndDiscardTool()
{

}

BOOL CCopyAndDiscardTool::init(BOOL bCopyPkt,unsigned int discard_rate)
{
	m_bCopyPkt = bCopyPkt;

	if( m_bCopyPkt )
	{
		m_CopyData.SetBufferSize(65536);
	}

	m_discard_rate = discard_rate;

	return true;
}

void CCopyAndDiscardTool::fini()
{
	return;
}

void CCopyAndDiscardTool::start_to_work()
{
	if( m_discard_rate )
	{
		ACE_OS::srand((unsigned int)(ACE_OS::gettimeofday().sec()));
	}
}

void CCopyAndDiscardTool::stop_working()
{
	return;
}

void CCopyAndDiscardTool::do_work(PACKET_LEN_TYPE& lentype,RECV_PACKET& packet)
{
	if( PACKET_OK == lentype || PACKET_TIMER == lentype )
	{
		if( m_discard_rate != 0 
			&& ((unsigned int)ACE_OS::rand() % m_discard_rate == 0) )
		{
			lentype = PACKET_CALL_NEXT;

			return;
		}
	}

	if( PACKET_OK  == lentype )
	{
		if( m_bCopyPkt )
		{
			m_CopyData.SetData((BYTE *)packet.pPacket,packet.nCaplen);

			packet.pPacket = m_CopyData.GetData();
		}

		return;
	}
	else
	{//do nothing

	}

	return;

}


//////////////////////////////////////////////////////////////////////////
//CLoopTool
//////////////////////////////////////////////////////////////////////////

CLoopTool::CLoopTool()
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

CLoopTool::~CLoopTool()
{

}


void CLoopTool::start_to_work()
{
	m_delta.m_ts.sec = 0;
	m_delta.m_ts.ns = 0;

	m_lastBeginTime.m_ts.sec = 0;
	m_lastBeginTime.m_ts.ns = 0;

	m_lastPktTime.m_ts.sec = 0;
	m_lastPktTime.m_ts.ns = 0;

	m_nCurLoops = 0;
}

void CLoopTool::stop_working()
{
	return;

}

void CLoopTool::do_work(PACKET_LEN_TYPE& lentype,RECV_PACKET& packet)
{
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

		return;

	}
	else if( PACKET_NO_MORE == lentype )
	{
		++m_nCurLoops;

		if( m_nDefLoops != 0 && m_nCurLoops >= m_nDefLoops )
		{//如果要求循环，并且循环次数已经够了
			lentype = PACKET_NO_MORE;

			return;
		}

		if( m_re_calculate_time )
		{
			//如果没有包了,m_delta要增加
			m_delta += m_lastPktTime - m_lastBeginTime;

			m_lastBeginTime.m_ts.sec = 0;
			m_lastBeginTime.m_ts.ns = 0;
		}

		if( !m_pRecvDataSource->SeekToBegin() )
		{
			lentype = PACKET_DRV_ERROR;

			return;
		}

		//返回该包无效，要求上层重新读取
		lentype = PACKET_CALL_NEXT;

		return;
	}
	else
	{
		return;
	}
}


