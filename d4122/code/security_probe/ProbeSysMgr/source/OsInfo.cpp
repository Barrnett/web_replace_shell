//////////////////////////////////////////////////////////////////////////
//OsInfo.cpp

#include "OsInfo.h"

COsInfo::COsInfo()
{

}

COsInfo::~COsInfo()
{

}

bool COsInfo::Init()
{
#ifdef OS_WINDOWS
	char name[1024];

	if( 0 == gethostname(name,sizeof(name)) )
	{
		m_strHostName = name;
	}

#else
	char strBuff[512];
	int nTotalSize, nUsedSize;
	FILE *fpTemp;

	if (NULL == (fpTemp = popen("cat /proc/version", "r")))
	{
		printf("读系统：popen错误\n");
		return -1;
	}
	fgets(strBuff, 512, fpTemp);
	pclose(fpTemp);

	char szOS[64];
	char szVersion[64];
	char szHost[64];
	sscanf(strBuff, "%s %s %s", szOS, szHost, szVersion);

	// 品牌
	m_strTrademark = szOS;
	// 版本
	m_strVersion = szVersion;
	m_strHostName = szHost;

#endif
	return true;
}
