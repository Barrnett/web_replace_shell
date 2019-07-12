//////////////////////////////////////////////////////////////////////////
//DoublePktSpeedCtrl.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/PktSpeedCtrl.h"

template<typename PktSpeedCtrl_TYPE>
class CDoublePktSpeedCtrl
{
public:
	CDoublePktSpeedCtrl(void)
	{
	}

	virtual ~CDoublePktSpeedCtrl(void)
	{
	}

	CDoublePktSpeedCtrl& operator=(const CDoublePktSpeedCtrl& other)
	{
		if( this != &other )
		{
			this->m_UpPktSpeedCtrl.init(other.m_UpPktSpeedCtrl.GetSpeed(),
				other.m_UpPktSpeedCtrl.GetUnitNums());

			this->m_DownPktSpeedCtrl.init(other.m_DownPktSpeedCtrl.GetSpeed(),
				other.m_DownPktSpeedCtrl.GetUnitNums());
		}

		return *this;
	}

	int operator == (const CDoublePktSpeedCtrl& other) const
	{
		if( m_UpPktSpeedCtrl.GetSpeed() != other.m_UpPktSpeedCtrl.GetSpeed() )
		{
			return 0;
		}

		if( m_DownPktSpeedCtrl.GetSpeed() != other.m_DownPktSpeedCtrl.GetSpeed() )
		{
			return 0;
		}

		if( m_UpPktSpeedCtrl.GetUnitNums() !=  other.m_UpPktSpeedCtrl.GetUnitNums() )
		{
			return 0;
		}

		return 1;
	}
	
	int operator != (const CDoublePktSpeedCtrl& other) const
	{
		return !(*this == other);
	}

public:
	BOOL init(ACE_UINT64 up_speed,
		ACE_UINT64 down_speed,
		unsigned int uint_nums=50)
	{
		m_UpPktSpeedCtrl.init(up_speed,uint_nums);
		m_DownPktSpeedCtrl.init(down_speed,uint_nums);

		return true;
	}

	BOOL IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime,DIR_IO dir)
	{
		if( dir == DIR_I2O )
			return (int)m_UpPktSpeedCtrl.IfCanSendPkt(pktlen,sleeptime);
		else if( dir == DIR_O2I )
			return (int)m_DownPktSpeedCtrl.IfCanSendPkt(pktlen,sleeptime);

		ACE_ASSERT(FALSE);

		return 1;
	}

	//重新开始计算流量，一般每秒钟调用一次本函数
	unsigned int Reset()
	{
		m_UpPktSpeedCtrl.Reset();
		m_DownPktSpeedCtrl.Reset();

		return 0;
	}

	void WaitTimeToNextPkt(size_t pktlen,DIR_IO dir)
	{
		ACE_Time_Value waittime;

		while( 1 )
		{
			if( IfCanSendPkt(pktlen,waittime,dir) )
			{
				return;
			}
			else
			{
				ACE_OS::sleep(waittime);
			}
		}	

		return;
	}

public:
	void GetSpeed(ACE_UINT64& up_speed,ACE_UINT64& down_speed) const
	{
		up_speed = m_UpPktSpeedCtrl.GetSpeed();
		down_speed = m_DownPktSpeedCtrl.GetSpeed();

	}

	unsigned int GetUnitNums() const
	{
		return m_UpPktSpeedCtrl.GetUnitNums();
	}

protected:
	PktSpeedCtrl_TYPE m_UpPktSpeedCtrl;
	PktSpeedCtrl_TYPE m_DownPktSpeedCtrl;


};

