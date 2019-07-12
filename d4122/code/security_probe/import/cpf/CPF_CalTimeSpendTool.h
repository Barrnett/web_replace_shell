//////////////////////////////////////////////////////////////////////////
//CPF_CalTimeSpendTool.h

#pragma once

#include "cpf/TimeStamp32.h"

class CPF_CLASS CPF_CalTimeSpendTool
{
public:
	CPF_CalTimeSpendTool(unsigned int max_interval=5*60)
		:m_max_interval(max_interval)
		,m_nTimeSpend(0)
		,m_nTimers(0)
	{
	}

	~CPF_CalTimeSpendTool()
	{
		reset();
	}

	void reset()
	{
		m_nTimeSpend = 0;
		m_nTimers = 0;

		m_lasttime = CTimeStamp32::zero;
	}

	void OnData(const CTimeStamp32& ts);

	unsigned int GetTimeSpend() const
	{
		return m_nTimeSpend;
	}

	unsigned int GetTimers() const
	{
		return m_nTimers;
	}

private:
	unsigned int		m_nTimers;//聊天次数
	unsigned int		m_nTimeSpend;//花费的时间
	unsigned int		m_max_interval;//多长时间间隔算一次。
	CTimeStamp32		m_lasttime;//最后一个包的时间
};


