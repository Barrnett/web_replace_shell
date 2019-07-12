//////////////////////////////////////////////////////////////////////////
//PktSpeedCtrl.cpp

#include "cpf/PktSpeedCtrl.h"
#include "ace/OS_NS_sys_time.h"

enum{
	USS_PER_SECOND= 1000000//一秒钟有多少个us
};

C_lrt_PktSpeedCtrl::C_lrt_PktSpeedCtrl(void)
{
	m_nBytesConfig = 0;
	m_nBytesSended = 0;

	m_bFull = FALSE;
}

C_lrt_PktSpeedCtrl::~C_lrt_PktSpeedCtrl(void)
{
}

BOOL C_lrt_PktSpeedCtrl::init(ACE_UINT64 speed,unsigned int uint_nums)
{
	CPktSpeedCtrl::init(speed,uint_nums);

	if( speed == 0 )
	{
		m_nBytesConfig = 0;
	}
	else if( speed == (ACE_UINT64)-1 )
	{
		m_nBytesConfig = ((ACE_UINT64)-1);
	}
	else
	{
		if( uint_nums == 0 || uint_nums > 1000*1000 )
		{
			uint_nums = 1;
		}

		//计算每个时间段应该发送多少个字节
		m_nBytesConfig = (speed/8)/(ACE_UINT64)uint_nums;

		m_BlockTimeUs = USS_PER_SECOND/uint_nums;
			
	}
	
	Reset();

	 return true;
}

//老算法可能
/*

BOOL C_lrt_PktSpeedCtrl::SubClass_IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime)
{	
	if( m_nBytesConfig == 0 )//不能发送数据
		return false;

	if( m_nBytesConfig == (unsigned int)-1 )//不进行控制
		return true;

	if( m_bFull )
	{
		ACE_UINT64 cur_time = GetTickCount_us();
		
		ACE_UINT64 usec_diff = cur_time - m_FirstTime;

		if( usec_diff <= m_BlockTimeUs )
		{
			if( sleeptime )
			{
				sleeptime->sec((m_BlockTimeUs - usec_diff)/USS_PER_SECOND);
				sleeptime->usec((m_BlockTimeUs - usec_diff)%USS_PER_SECOND);
			}

			return false;
		}
		else
		{
			m_FirstTime += m_BlockTimeUs;
			m_bFull = FALSE;
		}
	}

	if( m_nBytesSended >= m_nBytesConfig )
	{//如果已经发出的数据大于需要发送的数据

	
		ACE_UINT64 cur_time = GetTickCount_us();

		ACE_UINT64 usec_diff = cur_time - m_FirstTime;

		ACE_UINT64 next_time = ((ACE_UINT64)m_BlockTimeUs)*m_nBytesSended/m_nBytesConfig;

		if( usec_diff <= next_time )
		{
			m_bFull = TRUE;

			if( sleeptime )
			{
				sleeptime->sec((next_time - usec_diff)/USS_PER_SECOND);
				sleeptime->usec((next_time - usec_diff)%USS_PER_SECOND);
			}

			m_nBytesSended -= m_nBytesConfig;

			return false;
		}
		else
		{
			m_FirstTime += m_BlockTimeUs;

			m_nBytesSended -= m_nBytesConfig;

			m_nBytesSended += (unsigned int)pktlen;

			return true;
		}
	}
	else
	{
		m_nBytesSended += (unsigned int)pktlen;

		return true;
	}
}
*/

BOOL C_lrt_PktSpeedCtrl::SubClass_IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime)
{	
	if( m_nBytesConfig == 0 )//不能发送数据
		return false;

	if( m_nBytesConfig == (ACE_UINT64)-1 )//不进行控制
		return true;

	if( m_bFull )
	{
		ACE_UINT64 cur_time = GetTickCount_us();

		ACE_UINT64 next_time = ((ACE_UINT64)m_BlockTimeUs)*m_nBytesSended/m_nBytesConfig;

		ACE_UINT64 usec_diff = cur_time - m_FirstTime;

		if( usec_diff <= next_time )
		{
			if( sleeptime )
			{
				sleeptime->sec((next_time - usec_diff)/USS_PER_SECOND);
				sleeptime->usec((next_time - usec_diff)%USS_PER_SECOND);
			}

			return false;
		}
		else
		{
			ACE_UINT64 nums = next_time/(ACE_UINT64)m_BlockTimeUs;

			m_FirstTime += nums*m_BlockTimeUs;

			m_nBytesSended -= nums*m_nBytesConfig;

			m_bFull = FALSE;
		}
	}

	{
		m_nBytesSended += (ACE_UINT64)pktlen;

		if( m_nBytesSended >= m_nBytesConfig )
		{
			m_bFull = TRUE;
		}

		return true;
	}

}


void C_lrt_PktSpeedCtrl::SubClass_Reset()
{
	m_FirstTime = GetTickCount_us();

	m_nBytesSended = 0;

	m_bFull = false;

	return;
}


//////////////////////////////////////////////////////////////////////////
//C_winhrt_PktSpeedCtrl
//////////////////////////////////////////////////////////////////////////
#ifdef OS_WINDOWS

C_winhrt_PktSpeedCtrl::C_winhrt_PktSpeedCtrl()
{
	m_Frequency_Per_us = 0;
	m_Frequency.QuadPart = 0;
	m_FirstCounter.QuadPart = 0;
	m_BlockCounter.QuadPart = 0;

	m_bFull = FALSE;
	m_nBytesConfig = 0;
	m_nBytesSended = 0;
}

C_winhrt_PktSpeedCtrl::~C_winhrt_PktSpeedCtrl()
{

}

BOOL C_winhrt_PktSpeedCtrl::init(ACE_UINT64 speed,unsigned int uint_nums)
{
	CPktSpeedCtrl::init(speed,uint_nums);

	if( speed == 0 )
	{
		m_nBytesConfig = 0;
	}
	else if( speed == (ACE_UINT64)-1 )
	{
		m_nBytesConfig = ((ACE_UINT64)-1);
	}
	else
	{
		if( !QueryPerformanceFrequency(&m_Frequency) )
		{
			return false;
		}

		m_Frequency_Per_us = ((double)m_Frequency.QuadPart)/(1000*1000);

		if( uint_nums == 0 || uint_nums > 1000*1000 )
		{
			uint_nums = 1;
		}
		//计算每个时间段应该发送多少个字节
		m_nBytesConfig = (speed/8)/uint_nums;

		m_FirstCounter.QuadPart = 0;

		//每个发送单元应该有多少个counter
		m_BlockCounter.QuadPart = m_Frequency.QuadPart/uint_nums;

	}

	Reset();

	return true;
}

//老的算法可能有
/*
BOOL C_winhrt_PktSpeedCtrl::SubClass_IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime)
{	
	if( m_nBytesConfig == 0 )//不能发送数据
		return false;

	if( m_nBytesConfig == (unsigned int)-1 )//不进行控制
		return true;

	if( m_FirstCounter.QuadPart == 0 )
	{
		QueryPerformanceCounter(&m_FirstCounter);
	}

	if( m_bFull )
	{
		LARGE_INTEGER cur_counter;

		QueryPerformanceCounter(&cur_counter);

		LARGE_INTEGER diff_counter;
		diff_counter.QuadPart = cur_counter.QuadPart - m_FirstCounter.QuadPart;

		if( diff_counter.QuadPart <= m_BlockCounter.QuadPart )
		{
			if( sleeptime )
			{
				ACE_UINT64 temp_sleep = ACE_UINT64((m_BlockCounter.QuadPart - diff_counter.QuadPart)/m_Frequency_Per_us);

				sleeptime->sec(temp_sleep/USS_PER_SECOND);
				sleeptime->usec(temp_sleep%USS_PER_SECOND);
			}

			return false;
		}
		else
		{
			m_FirstCounter.QuadPart += m_BlockCounter.QuadPart;
			m_bFull = FALSE;
		}
	}

	if( m_nBytesSended >= m_nBytesConfig )
	{//如果已经发出的数据大于需要发送的数据


		LARGE_INTEGER cur_counter;

		QueryPerformanceCounter(&cur_counter);

		LARGE_INTEGER diff_counter;
		diff_counter.QuadPart = cur_counter.QuadPart - m_FirstCounter.QuadPart;

		LONGLONG next_time = m_nBytesSended*m_BlockCounter.QuadPart/m_nBytesConfig;

		if( diff_counter.QuadPart <= next_time )
		{
			m_bFull = TRUE;

			if( sleeptime )
			{
				ACE_UINT64 temp_sleep = ACE_UINT64((next_time - diff_counter.QuadPart)/m_Frequency_Per_us);

				sleeptime->sec(temp_sleep/USS_PER_SECOND);
				sleeptime->usec(temp_sleep%USS_PER_SECOND);
			}

			m_nBytesSended -= m_nBytesConfig;

			return false;
		}
		else
		{
			m_FirstCounter.QuadPart += m_BlockCounter.QuadPart;

			m_nBytesSended -= m_nBytesConfig;
			
			m_nBytesSended += (unsigned int)pktlen;

			return true;
		}
	}
	else
	{
		m_nBytesSended += (unsigned int)pktlen;

		return true;
	}

	return false;
}
*/

BOOL C_winhrt_PktSpeedCtrl::SubClass_IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime)
{	
	if( m_nBytesConfig == 0 )//不能发送数据
		return false;

	if( m_nBytesConfig == (ACE_UINT64)-1 )//不进行控制
		return true;

	if( m_FirstCounter.QuadPart == 0 )
	{
		QueryPerformanceCounter(&m_FirstCounter);
	}

	if( m_bFull )
	{
		LARGE_INTEGER cur_counter;

		QueryPerformanceCounter(&cur_counter);

		if( cur_counter.QuadPart < m_FirstCounter.QuadPart )
		{//时间出现了反向，可能用户手动修改过时间

			Reset();
			return false;
		}

		LARGE_INTEGER diff_counter;
		diff_counter.QuadPart = cur_counter.QuadPart - m_FirstCounter.QuadPart;

		LONGLONG next_time = m_nBytesSended*m_BlockCounter.QuadPart/m_nBytesConfig;

		if( diff_counter.QuadPart <= next_time )
		{
			if( sleeptime )
			{
				ACE_UINT64 temp_sleep = ACE_UINT64((next_time - diff_counter.QuadPart)/m_Frequency_Per_us);

				sleeptime->sec(temp_sleep/USS_PER_SECOND);
				sleeptime->usec(temp_sleep%USS_PER_SECOND);
			}

			return false;
		}
		else
		{
			ACE_UINT64 nums = next_time/m_BlockCounter.QuadPart;

			m_FirstCounter.QuadPart += nums*m_BlockCounter.QuadPart;

			m_nBytesSended -= nums * m_nBytesConfig;

			m_bFull = FALSE;
		}
	}

	{
		m_nBytesSended += (ACE_UINT64)pktlen;

		if( m_nBytesSended >= m_nBytesConfig )
		{
			m_bFull = TRUE;
		}

		return true;
	}

	return false;
}

void C_winhrt_PktSpeedCtrl::SubClass_Reset()
{
	QueryPerformanceCounter(&m_FirstCounter);

	m_nBytesSended = 0;

	m_bFull = false;

	return;
}


#endif//OS_WINDOWS

CPF_CLASS CPktSpeedCtrl * CreatePktSpeedCtrl(int type,ACE_UINT64 speed,unsigned unit_nums)
{
	CPktSpeedCtrl * pPktSpeedCtrl = NULL;

	if( type == 0 )
	{
#ifdef OS_WINDOWS
		pPktSpeedCtrl = new C_winhrt_PktSpeedCtrl;

		if( !pPktSpeedCtrl->init(speed,unit_nums) )
		{
			delete pPktSpeedCtrl;
			pPktSpeedCtrl = NULL;
		}
#else
		pPktSpeedCtrl = NULL;
#endif
	}

	if( !pPktSpeedCtrl && type != 2 )
	{
		pPktSpeedCtrl = new C_lrt_PktSpeedCtrl;

		if( !pPktSpeedCtrl->init(speed,unit_nums) )
		{
			delete pPktSpeedCtrl;
			pPktSpeedCtrl = NULL;
		}
	}

	return pPktSpeedCtrl;
}


