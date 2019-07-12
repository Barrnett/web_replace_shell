//////////////////////////////////////////////////////////////////////////
//HostEtherCardMgr.cpp 一个主机的以太网卡的管理


#include "cpf/HostEtherCardMgr.h"


#ifdef OS_WINDOWS
bool REG_GetNetCardShowName(const char * link_name,char * showname);
#endif

CHostEtherCardMgr::CHostEtherCardMgr(void)
{
	m_card_count = 0;

	memset(m_array_card_info,0x00,sizeof(m_array_card_info));

	return;
}

CHostEtherCardMgr::~CHostEtherCardMgr(void)
{
}


int CHostEtherCardMgr::Init()
{
	m_card_count = 0;

	memset(m_array_card_info,0x00,sizeof(m_array_card_info));

#ifdef OS_WINDOWS
	QuaryCardInfoFromReg_Win();
#else
	QuaryCardInfoFromReg_Linux();
#endif
	
	return m_card_count;

}

int CHostEtherCardMgr::GetOneIndexByMAC(const BYTE * mac_addr) const
{
	for(int i = 0; i < m_card_count; ++i)
	{
		if( memcmp(m_array_card_info[i]._szMacAddress,mac_addr,6) == 0 )
		{
			return i;
		}
	}

	return -1;
}

int CHostEtherCardMgr::GetOneIndexByShowName(const char * shoname) const
{
	for(int i = 0; i < m_card_count; ++i)
	{
		if( stricmp(m_array_card_info[i]._szShowName,shoname) == 0 )
		{
			return i;
		}
	}

	return -1;
}


#ifdef OS_WINDOWS

#include "NtDDNdis.h"
#include "winioctl.h"


#define REG_ROOT_CUR_NETWORK	"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_001_NETWORK	"SYSTEM\\ControlSet001\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_002_NETWORK	"SYSTEM\\ControlSet002\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_CUR_CLASS		"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_001_CLASS		"SYSTEM\\ControlSet001\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_002_CLASS		"SYSTEM\\ControlSet002\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"


#define REG_ROOT_NetworkCards   "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards"

//查注册表，得到网卡的显示名字
bool REG_GetNetCardShowName(const char * link_name,char * showname)
{
	CHAR pszNewSubKey[MAX_PATH];
	sprintf(pszNewSubKey, "%s\\%s\\Connection", REG_ROOT_CUR_NETWORK, link_name);

	HKEY keyRoot;

	DWORD dwAccessType = KEY_READ;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}

	ULONG uRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszNewSubKey, 0, dwAccessType, &keyRoot);
	if(ERROR_SUCCESS != uRet)
	{
		RegCloseKey(keyRoot);
		return false;
	}

	BYTE pData[MAX_PATH];
	DWORD dwDataLen = sizeof(pData);
	DWORD dwType = 0;
	uRet = RegQueryValueEx(keyRoot, "Name", NULL, &dwType, pData, &dwDataLen);
	if(ERROR_SUCCESS != uRet)
	{
		RegCloseKey(keyRoot);
		return false;
	}

	RegCloseKey(keyRoot);

	strcpy(showname,(char *)pData);

	return true;

}

bool get_interface_mac_win(char *NetCardName, BYTE * macAddr)
{
	char file_name[MAX_PATH] = "\\\\.\\";
	strcat(file_name,NetCardName);

	HANDLE hFile = CreateFile(file_name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,0);

	if(hFile == INVALID_HANDLE_VALUE || hFile == NULL )
	{
		DWORD dwErr = GetLastError();

		return false;
	}

	DWORD fuc_id = OID_802_3_PERMANENT_ADDRESS;

	DWORD BytesReturned = 0;

	//OID_802_3_PERMANENT_ADDRESS :Integer = $01010101;
	//OID_802_3_CURRENT_ADDRESS :Integer = $01010102;
	//IOCTL_NDIS_QUERY_GLOBAL_STATS :Integer = $00170002;

	BOOL bOK = DeviceIoControl(hFile,
		IOCTL_NDIS_QUERY_GLOBAL_STATS,
		&fuc_id,4,macAddr,6,&BytesReturned,NULL);

	CloseHandle(hFile);

	return bOK;
}

int CHostEtherCardMgr::QuaryCardInfoFromReg_Win()
{
	HKEY keyRoot;

	DWORD dwAccessType = KEY_READ;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}


	ULONG uRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_ROOT_NetworkCards, 0, dwAccessType, &keyRoot);
	if(ERROR_SUCCESS != uRet)
	{
		RegCloseKey(keyRoot);
		return 0;
	}

	DWORD dwIndex = 0;

	while (true)
	{
		char strSubName[MAX_PATH] = {0};

		ULONG uEnumRtn = RegEnumKey(keyRoot, dwIndex, strSubName, MAX_PATH);
		
		if(ERROR_NO_MORE_ITEMS == uEnumRtn)
		{
			// 没有了
			break;
		}


		if( ERROR_SUCCESS != uEnumRtn )
		{
			break;
		}

		++dwIndex;

		char pszNewSubKey[MAX_PATH] = {0};

		sprintf(pszNewSubKey,"%s\\%s",REG_ROOT_NetworkCards,strSubName);

		HKEY keySubItem;

		DWORD dwAccessType = KEY_READ;

		if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
		{
			dwAccessType |= KEY_WOW64_64KEY;
		}

		ULONG uRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszNewSubKey, 0, dwAccessType, &keySubItem);

		if( ERROR_SUCCESS != uRet )
		{
			continue;
		}
		
		char service_name[MAX_PATH] = {0};

		char desc_name[MAX_PATH] = {0};

		{
			DWORD dwType = 0;

			DWORD dwDataLen = sizeof(service_name);

			uRet = RegQueryValueEx(keySubItem, "ServiceName", NULL, &dwType, (BYTE *)service_name, &dwDataLen);

			if( ERROR_SUCCESS != uRet )
			{
				RegCloseKey(keySubItem);
				continue;
			}
		}

		{
			DWORD dwType = 0;

			DWORD dwDataLen = sizeof(desc_name);

			uRet = RegQueryValueEx(keySubItem, "Description", NULL, &dwType, (BYTE *)desc_name, &dwDataLen);
		}

		if (!get_interface_mac_win(service_name, m_array_card_info[m_card_count]._szMacAddress))
		{
			RegCloseKey(keySubItem);
			continue;
		}
	
		strcpy(m_array_card_info[m_card_count]._szLinkName,service_name);
		strcpy(m_array_card_info[m_card_count]._szDevDesc,desc_name);
		
		REG_GetNetCardShowName(service_name,m_array_card_info[m_card_count]._szShowName);

		m_card_count++;

		RegCloseKey(keySubItem);		
	}
	
	RegCloseKey(keyRoot);
	
	return m_card_count;
}

#endif

#ifdef OS_LINUX

#include "cpf/adapter_info_query.h"

bool get_interface_mac_linux(char *interface, BYTE * macAddr)
{
	int inet_sock;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, interface);

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ioctl(inet_sock, SIOCGIFHWADDR, &ifr)< 0)
	{
		close(inet_sock);
		return false;
	}

	memcpy(macAddr,ifr.ifr_ifru.ifru_hwaddr.sa_data,6);

	close(inet_sock);

	return true;
}

int CHostEtherCardMgr::QuaryCardInfoFromReg_Linux()
{
	char pInterface[32];

	char szBuf[1024];
	char* p = szBuf;
	int iCount = adapter_query_dev_name(szBuf, 1024);
	for (int idx=0; idx<iCount&&idx<32; ++idx)
	{
		strcpy(pInterface, p);

		p += strlen(p) +1;

		if (get_interface_mac_linux(pInterface, m_array_card_info[m_card_count]._szMacAddress))
		{
			strcpy(m_array_card_info[m_card_count]._szLinkName,pInterface);
			strcpy(m_array_card_info[m_card_count]._szDevDesc,pInterface);
			strcpy(m_array_card_info[m_card_count]._szShowName,pInterface);

			m_card_count++;		
		}
	}
}
#endif



