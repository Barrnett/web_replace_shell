//////////////////////////////////////////////////////////////////////////
//FltCondition.cpp

#include "simple_filter/SimpleFltCondition.h"

void CSimpleFltCondition::cal_flt()
{
	std::vector<ONE_FILTER>::const_iterator it;

	for(it = m_vt_filter.begin(); it != m_vt_filter.end(); ++it)
	{
		if( (*it).vt_ip.size() > 0 )
		{
			m_bIPFilter = TRUE;
		}

		if( !(*it).if_filter.IsEmpty() )
		{
			m_bCardFilter = TRUE;
		}

		if( (*it).vt_port.size() > 0 )
		{
			m_bPortFilter = TRUE;
		}

		if( m_bIPFilter && m_bPortFilter && m_bCardFilter )
		{
			break;
		}
	}
}

BOOL CSimpleFltCondition::IsEmpty() const
{
	if( m_vt_filter.empty() )
	{
		return true;
	}

	for(size_t i = 0; i < m_vt_filter.size(); ++i)
	{
		if( !m_vt_filter[i].IsEmpty() )
		{
			return false;
		}
	}

	return true;
}

