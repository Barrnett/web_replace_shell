#include "StdAfx.h"
#include "RegAccess.h"

#ifdef OS_WINDOWS


//
// 防止在帮定一个注册表以后，还有部分机器无法找到网卡，先在该外帮定所有的注册表信息
//
#define REG_ROOT_CUR_NETWORK	"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_001_NETWORK	"SYSTEM\\ControlSet001\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_002_NETWORK	"SYSTEM\\ControlSet002\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_CUR_CLASS		"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_001_CLASS		"SYSTEM\\ControlSet001\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"
#define REG_ROOT_002_CLASS		"SYSTEM\\ControlSet002\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"

// 把原来的多个适用REG_ROOT_CLASS的地方，改为使用全局字符串，这样代码可以尽量少的修改
const char*	g_pszClassStr = NULL;
const char * g_pszNetworkStr = NULL;
char g_pszConnectionName[256];

// 这个是网卡列表下边的一个非网卡的字段
// #define	REG_DESC			"Descriptions"

#define REG_DEVICE_ID		"NetCfgInstanceId"
#define REG_DEVICE_DESC		"DriverDesc"

//#define REG_LINKAGE		"Linkage"
//#define REG_UPPERBIND	"UpperBind"

const char*	g_pszCompStr = NULL;


// 回调部分
// 函数声明
typedef int (*FN_PROCESS_FUNCION)(void* pPara1, void* pPara2, void* pPara3);
// 返回值
enum{
	EN_RTN_BREAK = -1,
	EN_RTN_COUTINUE = 0,
};

//
// 枚举注册表子目录函数，同时提供一个回调函数，将遍历的每一个子目录都传入回调函数处理
//		  IN HKEY hRootKey - 注册表的根键值
//		  IN LPCTSTR lpSubKey - 要进行遍历的注册表的子键值
//		  IN FN_PROCESS_FUNCION fnCallBack - 需要执行的回调函数
//		  IN void* pPara1 - 传入给回调函数的参数1
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

	// 获取所有网卡的GUID
	ULONG uRet = RegOpenKeyEx(hRootKey, lpSubKey, 0, dwAccessType, &keyRoot);
	if (ERROR_SUCCESS != uRet)
	{
		// 无法打开注册表
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
			// 没有了
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

	// 关闭
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

	// 比较这个枚举出来的网络设备是不是我们需要的网卡
	CHAR pszNewSubKey[MAX_PATH];
	sprintf(pszNewSubKey, "%s\\%s", g_pszClassStr/*REG_ROOT_CLASS*/, pszDevice);

	ULONG uRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszNewSubKey, 0, dwAccessType, &keyRoot);
	if (ERROR_SUCCESS != uRet)
	{		
		return EN_RTN_COUTINUE;	// 无法打开注册表
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

	// 设备找到了，保存下来

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	ST_DEVICE_INFO* pInfo = NULL;
	bool bFind = false;
	for (int i=0; i<(int)p_vec_out->size(); i++)
	{
	//	ST_DEVICE_INFO info = p_vec_out->at();
		pInfo = &((*p_vec_out)[i]);
		if(0 == pInfo->pszGuid.compare(pszNCGUID))	// 已经存在，添加到位置里
		{
			pInfo->pszRegPosi[pInfo->iRegCount ++] = pszNewSubKey;
			bFind = true;
			break;
		}
	}

	if (!bFind)	// 第一次添加
	{
		ST_DEVICE_INFO info;
		info.pszConnectName = g_pszConnectionName;
		info.pszGuid = pszNCGUID;
		info.pszRegPosi[0] = pszNewSubKey;
		info.iRegCount = 1;
		// 取描述信息
		dwDataLen = sizeof(pData);
		uRet = RegQueryValueEx(keyRoot, REG_DEVICE_DESC, NULL, &dwType, pData, &dwDataLen);
		if (ERROR_SUCCESS == uRet)
		{
			info.pszDesc = (char*)pData;
		}

		// 取vlan控制信息,这里随便查询一个，如果是用我们程序进行控制，那么下一次打开就会正确了
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

	{//看是否有PnpInstanceID的值，如果没有则不是ether网卡
		CHAR pszNewSubKey[MAX_PATH];
		sprintf(pszNewSubKey, "%s\\%s\\Connection", g_pszNetworkStr, pszNCGUID);

		HKEY keyRoot;
		ULONG uRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszNewSubKey, 0, dwAccessType, &keyRoot);
		if (ERROR_SUCCESS != uRet)
		{		
			return EN_RTN_COUTINUE;	// 无法打开注册表
		}

		BYTE pData[MAX_PATH];
		DWORD dwDataLen = sizeof(pData);
		DWORD dwType = 0;
		uRet = RegQueryValueEx(keyRoot, "PnpInstanceID", NULL, &dwType, pData, &dwDataLen);
		if(ERROR_SUCCESS != uRet)
		{
			return EN_RTN_COUTINUE;
		}

		// 取name纪录下来
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

// 这里是外部唯一的入口，直接将这里的一个注册调用修改为3个注册表调用，就可以添加所有的字段信息
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
		info.iValue = 1;	// 这个还没有实现

		p += strlen(p) +1;

		vec_out.push_back(info);

	}

	return true;
}


#endif // OS_WINDOWS