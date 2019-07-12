//////////////////////////////////////////////////////////////////////////
//FltCondition.cpp

#include "cpf/simple_filter/CPF_SimpleFltCondition.h"

void CPF_SimpleFltCondition::cal_flt()
{
	m_bIPFilter = false;
	m_bPortFilter = false;

	for(size_t i = 0; i < m_vt_filter.size(); ++i)
	{
		if( !m_vt_filter[i].ip_flt.IsEmpty() )
		{
			m_bIPFilter = TRUE;
		}

		if( !m_vt_filter[i].port_flt.IsEmpty() )
		{
			m_bPortFilter = TRUE;
		}
	}

	return;
}


void CPF_SimpleFltCondition::close()
{
	m_bIPFilter = false;
	m_bPortFilter = false;

	m_vt_filter.clear();
}

const CPF_ONE_FILTER * CPF_SimpleFltCondition::find(const char * name) const
{
	return (CPF_ONE_FILTER *)(const_cast<CPF_SimpleFltCondition *>(this)->find(name));
}

CPF_ONE_FILTER * CPF_SimpleFltCondition::find(const char * name)
{
	for(size_t i = 0; i < m_vt_filter.size(); ++i)
	{
		if( m_vt_filter[i].name.compare(name) == 0 )
		{
			return &m_vt_filter[i];
		}
	}

	return NULL;		
}