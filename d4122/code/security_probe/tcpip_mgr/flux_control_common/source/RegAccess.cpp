#include "StdAfx.h"
#include "RegAccess.h"

#ifdef OS_WINDOWS


//
// ��ֹ�ڰﶨһ��ע����Ժ󣬻��в��ֻ����޷��ҵ����������ڸ���ﶨ���е�ע�����Ϣ
//
#define REG_ROOT_CUR_NETWORK	"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_001_NETWORK	"SYSTEM\\ControlSet001\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_002_NETWORK	"SYSTEM\\ControlSet002\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_CUR_CLASS		"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_001_CLASS		"SYSTEM\\ControlSet001\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_002_CLASS		"SYSTEM\\ControlSet002\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"

// ��ԭ���Ķ������REG_ROOT_CLASS�ĵط�����Ϊʹ��ȫ���ַ���������������Ծ����ٵ��޸�
const char*	g_pszClassStr = NULL;
const char * g_pszNetworkStr = NULL;
char g_pszConnectionName[256];

// ����������б��±ߵ�һ�����������ֶ�
// #define	REG_DESC			"Descriptions"

#define REG_DEVICE_ID		"NetCfgInstanceId"
#define REG_DEVICE_DESC		"DriverDesc"

//#define REG_LINKAGE		"Linkage"
//#define REG_UPPERBIND	"UpperBind"

const char*	g_pszCompStr = NULL;


// �ص�����
// ��������
typedef int (*FN_PROCESS_FUNCION)(void* pPara1, void* pPara2, void* pPara3);
// ����ֵ
enum{
	EN_RTN_BREAK = -1,
	EN_RTN_COUTINUE = 0,
};

//
// ö��ע�����Ŀ¼������ͬʱ�ṩһ���ص���������������ÿһ����Ŀ¼������ص���������
//		  IN HKEY hRootKey - ע���ĸ���ֵ
//		  IN LPCTSTR lpSubKey - Ҫ���б�����ע�����Ӽ�ֵ
//		  IN FN_PROCESS_FUNCION fnCallBack - ��Ҫִ�еĻص�����
//		  IN void* pPara1 - ������ص������Ĳ���1
//
bool reg_EnumRegister(
		  IN HKEY hRootKey,
		  IN LPCTSTR lpSubKey,
		  IN FN_PROCESS_FUNCION fnCallBack,
		  IN void* pPara2,
		  IN void* pPara3
		  )
{
	HKEY	keyRoot;

	DWORD dwAccessType = KEY_READ;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}

	// ��ȡ����������GUID
	ULONG uRet = RegOpenKeyEx(hRootKey, lpSubKey, 0, dwAccessType, &keyRoot);
	if (ERROR_SUCCESS != uRet)
	{
		// �޷���ע���
		return false;
	}

	DWORD dwIndex = 0;
	ULONG uEnumRtn = ERROR_SUCCESS;
	TCHAR strSubName[MAX_PATH] = "";
	DWORD dwSubNamelen = MAX_PATH;

	while (true)
	{
		uEnumRtn = RegEnumKey(keyRoot, dwIndex, strSubName, MAX_PATH);
		dwIndex ++;
		if(ERROR_NO_MORE_ITEMS == uEnumRtn)
		{
			// û����
			break;
		}

		int iRet = fnCallBack(strSubName, pPara2, pPara3);
		if(EN_RTN_COUTINUE==iRet)
		{
			continue;
		}
		else if(EN_RTN_BREAK==iRet)
		{
			break;	
		}

	}

	// �ر�
	RegCloseKey(keyRoot);

	return true;
}

int reg_IsNetCard(void* pPara1, void* pPara2, void* pPara3)
{
//	ASSERT(NULL!=pPara1 && NULL!=pPara2 && NULL!=pPara3);

	const char* pszDevice = (const char*)pPara1;
	const char* pszNCGUID = (const char*)pPara2;
	std::vector<ST_DEVICE_INFO>* p_vec_out = (std::vector<ST_DEVICE_INFO>*)pPara3;

	HKEY	keyRoot;
	DWORD	dwType;
	BYTE	pData[MAX_PATH];
	DWORD	dwDataLen = sizeof(pData);


	DWORD dwAccessType = KEY_READ;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}

	// �Ƚ����ö�ٳ����������豸�ǲ���������Ҫ������
	CHAR pszNewSubKey[MAX_PATH];
	sprintf(pszNewSubKey, "%s\\%s", g_pszClassStr/*REG_ROOT_CLASS*/, pszDevice);

	ULONG uRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszNewSubKey, 0, dwAccessType, &keyRoot);
	if (ERROR_SUCCESS != uRet)
	{		
		return EN_RTN_COUTINUE;	// �޷���ע���
	}

	dwDataLen = sizeof(pData);
	uRet = RegQueryValueEx(keyRoot, g_pszCompStr, NULL, &dwType, pData, &dwDataLen);
	if(ERROR_SUCCESS != uRet)
	{
		RegCloseKey(keyRoot);
		return EN_RTN_COUTINUE;
	}
	if(0 != strcmp((const char *)pData, pszNCGUID))
	{
		RegCloseKey(keyRoot);
		return EN_RTN_COUTINUE;
	}

	// �豸�ҵ��ˣ���������

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	ST_DEVICE_INFO* pInfo = NULL;
	bool bFind = false;
	for (int i=0; i<(int)p_vec_out->size(); i++)
	{
	//	ST_DEVICE_INFO info = p_vec_out->at();
		pInfo = &((*p_vec_out)[i]);
		if(0 == pInfo->pszGuid.compare(pszNCGUID))	// �Ѿ����ڣ���ӵ�λ����
		{
			pInfo->pszRegPosi[pInfo->iRegCount ++] = pszNewSubKey;
			bFind = true;
			break;
		}
	}

	if (!bFind)	// ��һ�����
	{
		ST_DEVICE_INFO info;
		info.pszConnectName = g_pszConnectionName;
		info.pszGuid = pszNCGUID;
		info.pszRegPosi[0] = pszNewSubKey;
		info.iRegCount = 1;
		// ȡ������Ϣ
		dwDataLen = sizeof(pData);
		uRet = RegQueryValueEx(keyRoot, REG_DEVICE_DESC, NULL, &dwType, pData, &dwDataLen);
		if (ERROR_SUCCESS == uRet)
		{
			info.pszDesc = (char*)pData;
		}

		// ȡvlan������Ϣ,��������ѯһ��������������ǳ�����п��ƣ���ô��һ�δ򿪾ͻ���ȷ��
//		dwType = REG_DWORD;
//		dwDataLen = sizeof(info.pszDesc);
//		uRet = RegQueryValueEx(keyRoot, "MonitorMode", NULL, &dwType, (LPBYTE)&info.iCheck, &dwDataLen);

		p_vec_out->push_back(info);
	}

	RegCloseKey(keyRoot);

	return EN_RTN_BREAK;
}



int reg_BindSpecialNetcard(void* pPara1, void* pPara2, void* pPara3)
{
//	ASSERT(NULL != pPara1);

	DWORD dwAccessType = KEY_READ;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}

	const char* pszNCGUID = (const char*)pPara1;

	{//���Ƿ���PnpInstanceID��ֵ�����û������ether����
		CHAR pszNewSubKey[MAX_PATH];
		sprintf(pszNewSubKey, "%s\\%s\\Connection", g_pszNetworkStr, pszNCGUID);

		HKEY keyRoot;
		ULONG uRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszNewSubKey, 0, dwAccessType, &keyRoot);
		if (ERROR_SUCCESS != uRet)
		{		
			return EN_RTN_COUTINUE;	// �޷���ע���
		}

		BYTE pData[MAX_PATH];
		DWORD dwDataLen = sizeof(pData);
		DWORD dwType = 0;
		uRet = RegQueryValueEx(keyRoot, "PnpInstanceID", NULL, &dwType, pData, &dwDataLen);
		if(ERROR_SUCCESS != uRet)
		{
			return EN_RTN_COUTINUE;
		}

		// ȡname��¼����
		dwDataLen = sizeof(g_pszConnectionName);
		uRet = RegQueryValueEx(keyRoot, "Name", NULL, &dwType, (LPBYTE)g_pszConnectionName, &dwDataLen);
		if(ERROR_SUCCESS != uRet)
		{
			strcpy(g_pszConnectionName, "");
		}
	}
	

	reg_EnumRegister(
		HKEY_LOCAL_MACHINE, 
		g_pszClassStr,	// REG_ROOT_CLASS, 
		reg_IsNetCard,
		(void*)pszNCGUID,
		pPara3
		);

	return EN_RTN_COUTINUE;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// �������ⲿΨһ����ڣ�ֱ�ӽ������һ��ע������޸�Ϊ3��ע�����ã��Ϳ���������е��ֶ���Ϣ
bool REG_SearchAllDevice(std::vector<ST_DEVICE_INFO>& vec_out)
{
	g_pszCompStr = REG_DEVICE_ID;
	
	g_pszClassStr = REG_ROOT_CUR_CLASS;
	g_pszNetworkStr = REG_ROOT_CUR_NETWORK;
	reg_EnumRegister(
		HKEY_LOCAL_MACHINE, 
		REG_ROOT_CUR_NETWORK, 
		reg_BindSpecialNetcard,
		NULL,
		&vec_out);

	g_pszClassStr = REG_ROOT_001_CLASS;
	g_pszNetworkStr = REG_ROOT_001_NETWORK;
	reg_EnumRegister(
		HKEY_LOCAL_MACHINE, 
		REG_ROOT_001_NETWORK, 
		reg_BindSpecialNetcard,
		NULL,
		&vec_out);


	g_pszClassStr = REG_ROOT_002_CLASS;
	g_pszNetworkStr = REG_ROOT_002_NETWORK;
	reg_EnumRegister(
		HKEY_LOCAL_MACHINE, 
		REG_ROOT_002_NETWORK, 
		reg_BindSpecialNetcard,
		NULL,
		&vec_out);

	return true;
}


#else

#include "cpf/adapter_info_query.h"
bool REG_SearchAllDevice(std::vector<ST_DEVICE_INFO>& vec_out)
{
	char szBuf[1024];
	char* p = szBuf;
	int iCount = adapter_query_dev_name(szBuf, 1024);
	for (int idx=0; idx<iCount; ++idx)
	{

		ST_DEVICE_INFO info;
		info.pszConnectName = p;
		info.pszDesc = p;
		info.pszGuid = "";
		info.iRegCount = 0;

		info.iEnable = 1;
		info.iValue = 1;	// �����û��ʵ��

		p += strlen(p) +1;

		vec_out.push_back(info);

	}

	return true;
}


#endif // OS_WINDOWS