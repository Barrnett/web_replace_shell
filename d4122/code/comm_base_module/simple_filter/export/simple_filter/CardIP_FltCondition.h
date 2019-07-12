//////////////////////////////////////////////////////////////////////////
//CardIP_FltCondition.h

#pragma once

#include "simple_filter/simple_filter.h"
#include "cpf/cpf.h"
#include <vector>

class SIMPLE_FILTER_CLASS CCardIP_FltCondition
{
public:
	CCardIP_FltCondition(void);
	~CCardIP_FltCondition(void);


public:
	typedef struct tag_MY_IP_Pair
	{
		tag_MY_IP_Pair()
		{
			card_no = -1;
			cap_notip = 1;
		}

		tag_MY_IP_Pair& operator = (const tag_MY_IP_Pair& other)
		{
			if( this != &other )
			{
				vt_ip_pair = other.vt_ip_pair;

				card_no = other.card_no;
				cap_notip = other.cap_notip;
			}

			return *this;
		}

		inline BOOL flt_ip(ACE_UINT32 ip) const
		{
			for(size_t i = 0; i < vt_ip_pair.size(); ++i)
			{
				if( ip >= vt_ip_pair[i].first && ip <= vt_ip_pair[i].second )
					return true;
			}

			return false;
		}

		//first=from,second=to(host_order)
		std::vector<std::pair<ACE_UINT32,ACE_UINT32> >	vt_ip_pair;

		int						card_no;
		BOOL					cap_notip;

	}MY_IP_Pair;

	MY_IP_Pair	m_pIPFilter[32];
};
