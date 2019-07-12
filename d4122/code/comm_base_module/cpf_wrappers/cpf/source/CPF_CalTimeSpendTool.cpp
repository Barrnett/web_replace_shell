//////////////////////////////////////////////////////////////////////////
//CPF_CalTimeSpendTool.cpp

#include "cpf/CPF_CalTimeSpendTool.h"


void CPF_CalTimeSpendTool::OnData(const CTimeStamp32& ts)
{
	int diff_sec = ts.GetSEC() - m_lasttime.GetSEC();

	if( diff_sec > 0 )
	{
		if( diff_sec >= (ACE_INT32)m_max_interval )
		{//连续m_max_interval没有数据，认为这次聊天结束。

			++m_nTimers;

			m_nTimeSpend += 1;
		}
		else
		{
			m_nTimeSpend +=  diff_sec;
		}

		m_lasttime = ts;
	}

	return;

}

