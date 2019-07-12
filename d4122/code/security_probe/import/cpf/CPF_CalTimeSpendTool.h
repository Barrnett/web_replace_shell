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
	unsigned int		m_nTimers;//�������
	unsigned int		m_nTimeSpend;//���ѵ�ʱ��
	unsigned int		m_max_interval;//�೤ʱ������һ�Ρ�
	CTimeStamp32		m_lasttime;//���һ������ʱ��
};


