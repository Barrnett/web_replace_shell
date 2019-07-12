//////////////////////////////////////////////////////////////////////////
//AdapterInfo.cpp

#include "AdapterInfo.h"
#include "cpf/adapter_info_query.h"

CAdapterInfo::CAdapterInfo(void)
{
#ifdef OS_WINDOWS
	m_pIP_ADAPTER_INFO = NULL;
	m_dwAdapterInfoLen = 0;
#endif
}

CAdapterInfo::~CAdapterInfo(void)
{
#ifdef OS_WINDOWS
	if( m_pIP_ADAPTER_INFO )
	{
		delete []((BYTE *)m_pIP_ADAPTER_INFO);
		m_dwAdapterInfoLen = 0;
	}
#endif
	return;
}


bool CAdapterInfo::init()
{
	query_adapter_info(m_vtAdapterInfo);

	return true;
}

void CAdapterInfo::Refresh()
{
	query_adapter_info(m_vtAdapterInfo);

	return;
}

BOOL CAdapterInfo::query_adapter_info(std::vector<MyAdapterInfo>& vtInfo)
{
#ifdef OS_WINDOWS

	do 
	{
		DWORD dwSize = 0;

		DWORD dwRtn = GetAdaptersInfo(NULL,&dwSize);

		PIP_ADAPTER_INFO pIP_ADAPTER_INFO = NULL;

		if( dwRtn == ERROR_BUFFER_OVERFLOW )
		{
			pIP_ADAPTER_INFO = (PIP_ADAPTER_INFO)new BYTE[dwSize];

			dwRtn = GetAdaptersInfo(pIP_ADAPTER_INFO,&dwSize);

			if( dwRtn != NO_ERROR )
			{
				delete []((BYTE *)pIP_ADAPTER_INFO);
				break;
			}
		}

		if( !pIP_ADAPTER_INFO )
		{
			break;
		}

		if( m_dwAdapterInfoLen == dwSize 
			&& memcmp(m_pIP_ADAPTER_INFO,pIP_ADAPTER_INFO,dwSize) == 0 )
		{//信息和上次相同，则不需要进行处理
			delete []((BYTE *)pIP_ADAPTER_INFO);
			return true;
		}

		//释放老的数据
		if( m_pIP_ADAPTER_INFO )
		{
			delete []((BYTE *)m_pIP_ADAPTER_INFO);
			m_dwAdapterInfoLen = 0;
		}

		//获取新的信息
		m_pIP_ADAPTER_INFO = (PIP_ADAPTER_INFO)new BYTE[dwSize];
		m_dwAdapterInfoLen = dwSize;
		memcpy(m_pIP_ADAPTER_INFO,pIP_ADAPTER_INFO,dwSize);

		turn_info(m_pIP_ADAPTER_INFO,vtInfo);

		return true;

	} while(0);


	if( m_pIP_ADAPTER_INFO )
	{
		delete []((BYTE *)m_pIP_ADAPTER_INFO);
		m_dwAdapterInfoLen = 0;
	}

	turn_info(m_pIP_ADAPTER_INFO,vtInfo);

	return false;
#else
	char szInterfaceName[16];
	char szBuffer[128];
	int iRtn = -1;
	MyAdapterInfo  adapter_info;
	unsigned int dwIP = 0;
	unsigned int dwMask = 0;
	unsigned int gatewayAddr = 0;	// 拿不到

	vtInfo.clear();

	char szBuf[1024];
	char* p = szBuf;
	int iCount = adapter_query_dev_name(szBuf, 1024);
	for (int idx=0; idx<iCount; ++idx)
	{
		strcpy(szInterfaceName, p);
		p += strlen(p) +1;

		iRtn = adapter_query_mac(szInterfaceName, szBuffer);
		if (iRtn < 0 )
			continue;

		adapter_query_ip(szInterfaceName, dwIP);
		adapter_query_mask(szInterfaceName, dwMask);

		adapter_info.vt_netorder_ip.push_back(std::make_pair(dwIP,dwMask));
		adapter_info.vt_netorder_router.push_back(gatewayAddr);

		memcpy(adapter_info.macaddr, szBuffer, 6);
		strcpy(adapter_info.AdapterName, szInterfaceName);
		strcpy(adapter_info.Description, szInterfaceName);

		vtInfo.push_back(adapter_info);
	}
#endif


	return false;
}

#ifdef OS_WINDOWS
void CAdapterInfo::turn_info(PIP_ADAPTER_INFO pInfo,
			   std::vector<MyAdapterInfo>& vtInfo)
{
	vtInfo.clear();

	while( pInfo )
	{
		if( pInfo->AddressLength != 6 )
		{
			pInfo = pInfo->Next;
			continue;
		}

		MyAdapterInfo adapter_info;

		memcpy(adapter_info.macaddr,pInfo->Address,6);
		memcpy(adapter_info.AdapterName,pInfo->AdapterName,sizeof(pInfo->AdapterName));
		memcpy(adapter_info.Description,pInfo->Description,sizeof(pInfo->Description));

		IP_ADDR_STRING * pIpAddress = &pInfo->IpAddressList;

		while( pIpAddress )
		{
			DWORD dwIP = inet_addr((char *)pIpAddress->IpAddress.String);
			DWORD dwMask = inet_addr((char *)pIpAddress->IpMask.String);

			adapter_info.vt_netorder_ip.push_back(std::make_pair(dwIP,dwMask));

			pIpAddress = pIpAddress->Next;
		}

		IP_ADDR_STRING * pRouterAddr = &pInfo->GatewayList;

		while( pRouterAddr )
		{
			DWORD dwRouter = inet_addr(pRouterAddr->IpAddress.String);

			adapter_info.vt_netorder_router.push_back(dwRouter);

			pRouterAddr = pRouterAddr->Next;
		}

		vtInfo.push_back(adapter_info);

		pInfo = pInfo->Next;	
	}

	return;
}

#endif 