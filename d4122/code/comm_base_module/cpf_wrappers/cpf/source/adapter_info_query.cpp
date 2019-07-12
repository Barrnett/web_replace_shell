//////////////////////////////////////////////////////////////////////////
// adapter_info_query.cpp 

#include "cpf/adapter_info_query.h"
#include "string.h"
#include <stdio.h>
#include "net/if.h"
#include <arpa/inet.h>
#include <sys/ioctl.h>



// 将设备的名字以字符串形式放入到pszOut缓冲区中，名字之间为0，最后再带一个0，返回值为设备个数
// lo被排除在外
int adapter_query_dev_name(char* pszBufOut, int iBufLen)
{
	if (NULL == pszBufOut)
		return -1;

	int iUsedLen = 0, iCount = 0;
	char* pszUsed = pszBufOut;
	*pszUsed = '\0';


	struct if_nameindex* pIndex = if_nameindex();
	while(pIndex && pIndex->if_index && pIndex->if_name)
	{
//		printf("%d, %s\n", pIndex->if_index, pIndex->if_name);
		if (0 == strcmp("lo", pIndex->if_name))
		{
			pIndex ++;
			continue;
		}

		// 缓冲区长度检查
		if (pszUsed - pszBufOut + strlen(pIndex->if_name) + 2 > iBufLen)
			return iCount;

		strcpy(pszUsed, pIndex->if_name);
		pszUsed += strlen(pszUsed) + 1;

		pIndex ++;
		iCount ++;
	}

	*pszUsed = '\0';

	return iCount;
}




int adapter_query_mac(const char* pszInterface, char* pMac)
{
	int inet_sock;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, pszInterface);

	pMac[0] = pMac[1] = pMac[2] = pMac[3] = pMac[4] = pMac[5] = 0;

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ioctl(inet_sock, SIOCGIFHWADDR, &ifr)< 0)
	{
		close(inet_sock);
		return -1;
	}
	for (int i=0; i<6; i++)
	{
		pMac[i] = ifr.ifr_ifru.ifru_hwaddr.sa_data[i];
	}
	close(inet_sock);
	return 1;
}
int adapter_query_mac_str(const char* pszInterface, char* pszOutMac, char chSec)
{
	char szMac[6];
	int iRtn = adapter_query_mac(pszInterface, szMac);
	if (iRtn < 0)
		return iRtn;

	sprintf(pszOutMac, "%02X%c%02X%c%02X%c%02X%c%02X%c%02X", 
				szMac[0], chSec, szMac[1], chSec, szMac[2], chSec, 
				szMac[3], chSec, szMac[4], chSec, szMac[5]);
	return iRtn;
}

int adapter_query_ip(const char* pszInterface, unsigned int& ipAddr)
{
	int inet_sock;
	struct ifreq ifr;

	ipAddr = 0;
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, pszInterface);

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ioctl(inet_sock, SIOCGIFADDR, &ifr)< 0)
	{
		close(inet_sock);
		return -1;
	}

	ipAddr = *(int*)&(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
	close(inet_sock);
	return 1;
}

int adapter_query_ip_str(const char* pszInterface, char* pszOutIP)
{
	int inet_sock;
	struct ifreq ifr;

	pszOutIP[0] = '\0';
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, pszInterface);

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ioctl(inet_sock, SIOCGIFADDR, &ifr)< 0)
	{
		close(inet_sock);
		return -1;
	}

	strcpy(pszOutIP, inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));
	close(inet_sock);
	return 1;
}


int adapter_query_mask(const char* pszInterface, unsigned int& maskAddr)
{
	int inet_sock;
	struct ifreq ifr;

	maskAddr = 0;
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, pszInterface);

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ioctl(inet_sock, SIOCGIFNETMASK, &ifr)< 0)
	{
		close(inet_sock);
		return -1;
	}

	maskAddr = *(int*)&(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
	close(inet_sock);
	return 1;
}

int adapter_query_gateway(const char* pszInterface, unsigned int& iGateWay)
{
	char strBuff[512];
	FILE *fpTemp = NULL;
	char* pTemp = NULL;

	iGateWay = 0;

	sprintf(strBuff, "cat /proc/net/route | grep %s", pszInterface);
	if (NULL == (fpTemp = popen(strBuff, "r")))
	{
		printf("读路由IP错误：popen错误\n");
		return -1;
	}

	char szName[16];
	char szIP[16];
	while( (pTemp = fgets(strBuff, 512, fpTemp)) != NULL)
	{
		sscanf(strBuff, "%s %s %x", szName, szIP, &iGateWay);
		if ( (0 == strcmp(szName, pszInterface))
			&& (0 == strcmp(szIP, "00000000")) )
		{
			pclose(fpTemp);
			return 1;
		}
	}
	pclose(fpTemp);
	return -1;
}

int adapter_query_mtu(const char* pszInterface, unsigned int& mtu)
{
	struct ifreq ifr;
	int sockfd, err;

	if( (sockfd = socket(PF_INET, SOCK_DGRAM, 0)) <= 0 )
	{
		return -1;
	}

	strcpy(ifr.ifr_name, pszInterface);
	err = ioctl(sockfd, SIOCGIFMTU, (void*)&ifr);
	close(sockfd);

	if (err == -1)
	{
		mtu = 0;
		return -1;
	}

	mtu = ifr.ifr_mtu; 
	return 1;


	/*

	ETHERIO_OID_DATA* pOidData = (ETHERIO_OID_DATA*)new
	BYTE[sizeof(ETHERIO_OID_DATA)];
	if (NULL == pOidData)
	return -1;

	pOidData->CardIndex = _iIndex;
	pOidData->Oid = OID_GEN_MAXIMUM_TOTAL_SIZE;
	pOidData->Length = sizeof(ULONG);
	*(int*)(pOidData->Data) = 0;

	bool bRet = CDeviceAccess::GetOid(*pOidData);
	int iValue = *(int*)(pOidData->Data);

	delete pOidData;

	if(bRet)
	return iValue;
	return -1;
	*/
}



/*
int main(int argc, char* argv[])
{
	unsigned char mac[120];
	unsigned int ip;
	if (argc < 2)
	{
		printf("not eth\n");
		return 0;
	}

	int iRtn = adapter_query_mac(argv[1], &mac[0]);
	if (iRtn <0)
	{
		printf("adapter_query_mac [%s] error\n", argv[1]);
		return 0;
	}
	else
	{
		printf("[%s] mac addr is %02X:%02X:%02X:%02X:%02X:%02X\n", 
			argv[1], mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

		adapter_query_mac_str(argv[1], (char*)mac);
		printf("[%s] mac addr is [%s]\n", argv[1], mac);
	}
	iRtn = adapter_query_ip(argv[1], ip);
	if (iRtn <0)
	{
		printf("adapter_query_ip [%s] error\n", argv[1]);
		return 0;
	}
	else
	{
		unsigned char* pip = (unsigned char*)&ip;
		printf("[%s] ip addr is %d.%d.%d.%d\n", argv[1], pip[0], pip[1], pip[2], pip[3]);
		adapter_query_ip_str(argv[1], (char*)mac);
		printf("[%s] ip addr is [%s]\n", argv[1], mac);
	}

	return 0;
}
*/
