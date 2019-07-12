//////////////////////////////////////////////////////////////////////////
//AdapterInfo.h

#pragma once

#include "cpf/cpf.h"

#include "ProbeSysCommon.h"

#ifdef OS_WINDOWS
#include <Iphlpapi.h>
#endif


// Õ¯ø®–≈œ¢¿‡
class PROBESYSMGR_CLASS CAdapterInfo
{
public:

#ifndef OS_WINDOWS
	enum{ MAX_ADAPTER_NAME_LENGTH = 256 ,
		MAX_ADAPTER_DESCRIPTION_LENGTH = 128
	};
#endif

	typedef struct tagMyAdapterInfo
	{
		tagMyAdapterInfo()
		{
			AdapterName[0] = 0;
			Description[0] = 0;

			memset(macaddr,0x00,sizeof(macaddr));
		}

		tagMyAdapterInfo(const tagMyAdapterInfo& other )
		{
			*this = other;
		}


		tagMyAdapterInfo& operator=(const tagMyAdapterInfo& other)
		{
			if( this != &other )
			{
				vt_netorder_ip = other.vt_netorder_ip;
				vt_netorder_router = other.vt_netorder_router;

				memcpy(macaddr,other.macaddr,sizeof(macaddr));
				memcpy(AdapterName,other.AdapterName,sizeof(AdapterName));
				memcpy(Description,other.Description,sizeof(Description));
			}

			return *this;

		}

		std::vector<std::pair<ACE_UINT32,ACE_UINT32> >	vt_netorder_ip;
		std::vector<ACE_UINT32>							vt_netorder_router;
		BYTE											macaddr[6];	
		char											AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
		char											Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];

	}MyAdapterInfo;

public:
	CAdapterInfo(void);
	~CAdapterInfo(void);

public:
	bool init();

	size_t get_adapter_nums() const
	{
		return m_vtAdapterInfo.size();
	}

	const MyAdapterInfo* get_adapter_info(size_t index) const
	{
		if( index >= m_vtAdapterInfo.size() )
			return NULL;

		return &m_vtAdapterInfo[index];
	}

	void Refresh();

private:
	BOOL query_adapter_info(std::vector<MyAdapterInfo>& vtInfo);

#ifdef OS_WINDOWS
	void turn_info(PIP_ADAPTER_INFO pIP_ADAPTER_INFO,
		std::vector<MyAdapterInfo>& vtInfo);
	PIP_ADAPTER_INFO	m_pIP_ADAPTER_INFO;
	DWORD						m_dwAdapterInfoLen;
#endif

private:
	std::vector<MyAdapterInfo>	m_vtAdapterInfo;
};
