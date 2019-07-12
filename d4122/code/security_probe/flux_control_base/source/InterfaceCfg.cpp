//////////////////////////////////////////////////////////////////////////
//InterfaceCfg.cpp

#include "stdafx.h"
#include "InterfaceCfg.h"
#include "cpf/IfIPTool.h"
#include "cpf/HostEtherCardMgr.h"

#ifdef OS_WINDOWS

#include "Iphlpapi.h"
#pragma comment(lib,"Iphlpapi.lib")
#include "../../../comm_base_module/packetio_wrappers/PacketIO/source/etherio/EtherioDllExport.h"
#else
#include "PacketIO/DataSourceHelper.h"
#include "cpf/adapter_info_query.h"
#include "../../../comm_base_module/packetio_wrappers/PacketIO/source/etherio/EtherioDllExport.h"
#endif



int CInterfaceCfg::SetDevManagerIP()
{
	

	return 0;
}

/*

int CInterfaceCfg::SetDevManagerIP(const std::vector<CInterfaceCfg::IP_CARD_INFO>& vt_ip_card_info,
								   const CFCNetIfConfig::DEV_MANAGER_IP& dev_ip_card)
{
	if( !dev_ip_card.onoff )
	{
		return 0;
	}

	if( dev_ip_card.ipaddr == 0 )
	{
		return 0;
	}

	if( m_pSignalEventFunc )
	{
		m_pSignalEventFunc(m_pSignalParam);
	}

	if( !JudegSameIP(vt_ip_card_info,dev_ip_card.cardname.c_str(),
		dev_ip_card.ipaddr,dev_ip_card.mask,dev_ip_card.gateway) )
	{
		CPF::SetIfIPByCmd_Static(dev_ip_card.cardname.c_str(),
			dev_ip_card.ipaddr,dev_ip_card.mask,dev_ip_card.gateway,1);
	}

	if( m_pSignalEventFunc )
	{
		m_pSignalEventFunc(m_pSignalParam);
	}

	if( !JudegSameDNS(vt_ip_card_info,dev_ip_card.cardname.c_str(),
		dev_ip_card.dns1,dev_ip_card.dns2) )
	{
		CPF::SetIfDNSByCmd_Static(dev_ip_card.cardname.c_str(),dev_ip_card.dns1,dev_ip_card.dns2);
	}

	if( m_pSignalEventFunc )
	{
		m_pSignalEventFunc(m_pSignalParam);
	}

	return 0;
}

*/

BOOL CInterfaceCfg::JudegSameIP(const std::vector<CInterfaceCfg::IP_CARD_INFO>&	vt_ip_card_info,
								const char * card_name,ACE_UINT32 ipaddr,ACE_UINT32 ipmask,ACE_UINT32 ipgw)
{
	for(size_t i = 0; i < vt_ip_card_info.size(); ++i)
	{
		if( stricmp(vt_ip_card_info[i].friend_name,card_name) == 0 )
		{
			if( vt_ip_card_info[i].host_ip == ipaddr
				&& vt_ip_card_info[i].host_mask == ipmask
				&& 
				( (vt_ip_card_info[i].host_gateway == ipgw)
				|| ((vt_ip_card_info[i].host_gateway==(ACE_UINT32)-1) && ipgw == 0)
				)
				)
			{
				return true;
			}
		}
	}

	return false;
}

BOOL CInterfaceCfg::JudegSameDNS(const std::vector<CInterfaceCfg::IP_CARD_INFO>& vt_ip_card_info,
								 const char * card_name,ACE_UINT32 dns1,ACE_UINT32 dns2)
{
	for(size_t i = 0; i < vt_ip_card_info.size(); ++i)
	{
		if( stricmp(vt_ip_card_info[i].friend_name,card_name) == 0 )
		{
			if( vt_ip_card_info[i].dns1 == dns1
				&& vt_ip_card_info[i].dns2 == dns2 )
			{
				return true;
			}
		}
	}

	return false;
}


#ifdef OS_WINDOWS
BOOL CInterfaceCfg::GetAllIPCardInfo(std::vector<CInterfaceCfg::IP_CARD_INFO>& vt_ip_cardinfo)
{
	vt_ip_cardinfo.clear();

	// Declare and initialize variables
	IP_ADAPTER_INFO * pAdapterInfo = NULL;
	ULONG ulOutBufLen = 0;

	DWORD dwRetVal = GetAdaptersInfo(NULL, &ulOutBufLen);
	if (dwRetVal == ERROR_BUFFER_OVERFLOW) 
	{
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
	}

	if (pAdapterInfo == NULL) 
	{
		return false;
	}

	dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

	if( dwRetVal != NO_ERROR )
	{
		free(pAdapterInfo);
		return false;
	}

	CHostEtherCardMgr host_card_mgr;

	int etherio_count = host_card_mgr.Init();

	for(PIP_ADAPTER_INFO pCurAdapter = pAdapterInfo;
		pCurAdapter;
		pCurAdapter = pCurAdapter->Next)
	{
		if( pCurAdapter->AddressLength != 6 )
			continue;

		if( pCurAdapter->Type != 6 )
			continue;

		ACE_UINT32 hostorder_if_ip = ACE_NTOHL(ACE_OS::inet_addr(pCurAdapter->IpAddressList.IpAddress.String));
		ACE_UINT32 hostorder_if_mask = ACE_NTOHL(ACE_OS::inet_addr(pCurAdapter->IpAddressList.IpMask.String));
		ACE_UINT32 hostorder_if_gateway = ACE_NTOHL(ACE_OS::inet_addr(pCurAdapter->GatewayList.IpAddress.String));

		if( hostorder_if_ip == 0 )
		{
			continue;
		}

		int index = host_card_mgr.GetOneIndexByMAC(pCurAdapter->Address);

		if( index == -1 )
			continue;

		CInterfaceCfg::IP_CARD_INFO ip_card_info;

		GetCardDNS(pCurAdapter->Index,ip_card_info.dns1,ip_card_info.dns2);

		memcpy(ip_card_info.mac_addr,pCurAdapter->Address,6);
		ip_card_info.host_ip = hostorder_if_ip;
		ip_card_info.host_mask = hostorder_if_mask;
		ip_card_info.host_gateway = hostorder_if_gateway;

		strncpy(ip_card_info.friend_name,
			host_card_mgr.m_array_card_info[index]._szShowName,
			sizeof(ip_card_info.friend_name)-1);

		vt_ip_cardinfo.push_back(ip_card_info);
	}

	if( pAdapterInfo )
	{
		free(pAdapterInfo);
	}

	return true;
}
#else
BOOL CInterfaceCfg::GetAllIPCardInfo(std::vector<IP_CARD_INFO>& vt_ip_cardinfo)
{
	vt_ip_cardinfo.clear();

	char szInterface[32];
	char szMac[12];
	int iIP;
	int iRtn;

	char szBuf[1024];
	char* p = szBuf;
	int iCount = adapter_query_dev_name(szBuf, 1024);

	for (int idx=0; idx<iCount; ++idx)
	{
		strcpy(szInterface, p);
		p += strlen(p) +1;

		iRtn = adapter_query_mac(szInterface, szMac);
		if (iRtn < 0)
			continue;

		ACE_UINT32 hostorder_if_ip = 0;
		ACE_UINT32 hostorder_if_mask = 0;
		ACE_UINT32 hostorder_if_gateway = 0;


		adapter_query_ip(szInterface, hostorder_if_ip);
		adapter_query_mask(szInterface, hostorder_if_mask);
		adapter_query_gateway(szInterface, hostorder_if_gateway);

		hostorder_if_ip = ACE_NTOHL(hostorder_if_ip);
		hostorder_if_mask = ACE_NTOHL(hostorder_if_mask);
		hostorder_if_gateway = ACE_NTOHL(hostorder_if_gateway);

		if( hostorder_if_ip == 0 )
		{
			continue;
		}

		IP_CARD_INFO ip_card_info;

		ip_card_info.dns1 = ip_card_info.dns2 = 0;

		GetCardDNS(idx,ip_card_info.dns1,ip_card_info.dns2);

		memcpy(ip_card_info.mac_addr, szMac, 6);

		ip_card_info.host_ip = hostorder_if_ip;
		ip_card_info.host_mask = hostorder_if_mask;
		ip_card_info.host_gateway = hostorder_if_gateway;

		strncpy(ip_card_info.friend_name,szInterface,sizeof(ip_card_info.friend_name)-1);

		vt_ip_cardinfo.push_back(ip_card_info);
	}
	return true;
}
#endif

#ifdef OS_WINDOWS

BOOL CInterfaceCfg::GetCardDNS(int adapter_index,ACE_UINT32& dns1,ACE_UINT32& dns2)
{
	//Dns服务器  
	IP_PER_ADAPTER_INFO* pPerAdapt    = NULL;  
	ULONG ulLen = 0;  
	int err = GetPerAdapterInfo( adapter_index, pPerAdapt, &ulLen);  
	if( err == ERROR_BUFFER_OVERFLOW )  
	{  
		pPerAdapt = ( IP_PER_ADAPTER_INFO* ) new char[ulLen];

		err = GetPerAdapterInfo( adapter_index, pPerAdapt, &ulLen ); 

		if( err == ERROR_SUCCESS )  
		{  
			IP_ADDR_STRING* pNext = &( pPerAdapt->DnsServerList );  

			if (pNext && strcmp(pNext->IpAddress.String, "") != 0)//自动获取为false  
			{  
				dns1 = ACE_NTOHL(ACE_OS::inet_addr(pNext->IpAddress.String));

				if (pNext=pNext->Next)  
				{
					dns2 = ACE_NTOHL(ACE_OS::inet_addr(pNext->IpAddress.String));
				}
			}  
			else //dns自动获取为true  
			{  
				dns1 = dns2 = 0;
			}  
		}
	}

	return true;
}

#else

/**
 * get dns.
 * @param dns1 a pointer to save first dns.
 * @param dns2 a pointer to save second dns.
 * @return 0 success, or fail.
 */
int CInterfaceCfg::GetCardDNS(int adapter_index,ACE_UINT32& dns1,ACE_UINT32& dns2)
{
	char str_dns1[64];
	char str_dns2[64];

    int fd = -1;
    int size = 0;
	char strBuf[1024] = {0};
    int buf_size = sizeof(strBuf);
    char *p = NULL;
    char *q = NULL;
    int i = 0;
    int j = 0;
    int count = 0;
    
    fd = open("/etc/resolv.conf", O_RDONLY);
    if(-1 == fd)
    {
        return -1;
    }
    size = read(fd, strBuf, buf_size);
    if(size < 0)
    {
        close(fd);
        return -1;
    }
    strBuf[buf_size] = '\0';
    close(fd);

    while(i < buf_size)
    {
        if((p = strstr(&strBuf[i], "nameserver")) != NULL)
        {
            j++;
            p += 1;
            count = 0;            
          
            q = p;
            while(*q != '\n')
            {
                q++;
                count++;
            }
            i += (sizeof("nameserver") + count);
            
            if(1 == j)
            {
                memcpy(str_dns1, p, count);
                str_dns1[count]='\0';
            }
            else if(2 == j)
            {
                memcpy(str_dns2, p, count);
                str_dns2[count]='\0';

				break;
            }
        }
        else
        {
            i++;
        }
    }

	if( j >= 1 )
	{
		dns1 = CPF::string_to_hip(str_dns1);
	}

	if( j >= 2 )
	{
		dns2 = CPF::string_to_hip(str_dns2);
	}

    return 0;
}

#endif