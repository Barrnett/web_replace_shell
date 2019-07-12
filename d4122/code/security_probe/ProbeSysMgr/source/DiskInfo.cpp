//////////////////////////////////////////////////////////////////////////
//DiskInfo.cpp

#include "DiskInfo.h"
#include "cpf/strtools.h"

CDiskInfo::CDiskInfo()
{
}

CDiskInfo::~CDiskInfo()
{

}
// 初始化
bool CDiskInfo::Init()
{
#ifdef OS_WINDOWS
	char buf[1024];

	size_t neln = (size_t)GetLogicalDriveStrings(sizeof(buf),buf);

	int nums = CPF::GetWordNums<char>(buf,neln,(char)0,true);

	for( int i = 0; i < nums; ++i)
	{
		const char * pRootname = NULL;

		int name_len = CPF::GetWord_Pointer<char>(i,buf,neln,pRootname,0);

		if( name_len > 0 && pRootname  )
		{
			if( DRIVE_FIXED == GetDriveType(pRootname) )
			{
				DISK_INFO diskinfo;

				ULARGE_INTEGER FreeBytesAvailable;
				ULARGE_INTEGER TotalNumberOfBytes;
				ULARGE_INTEGER TotalNumberOfFreeBytes;

				GetDiskFreeSpaceEx(pRootname,&FreeBytesAvailable
					,&TotalNumberOfBytes,&TotalNumberOfFreeBytes);

				diskinfo.name = pRootname;
				diskinfo.nCapacity = (size_t)(TotalNumberOfBytes.QuadPart/(1024*1024));
				diskinfo.nUsed = diskinfo.nCapacity-(size_t)(TotalNumberOfFreeBytes.QuadPart/(1024*1024));

				m_vtDisk.push_back(diskinfo);

			}
			
		}
	}
#else
	Refresh();
#endif

	return true;
}

bool CDiskInfo::Refresh()
{
#ifdef OS_WINDOWS
	for( size_t i = 0; i < m_vtDisk.size(); ++i )
	{
		ULARGE_INTEGER TotalNumberOfFreeBytes;

		GetDiskFreeSpaceEx(m_vtDisk[i].name.c_str(),NULL
			,NULL,&TotalNumberOfFreeBytes);
		m_vtDisk[i].nUsed = m_vtDisk[i].nCapacity 
			- (size_t)(TotalNumberOfFreeBytes.QuadPart/(1024*1024));
	}

#else

	m_vtDisk.clear();

	DISK_INFO diskinfo;

	FILE *fpTemp = NULL;
	char strBuff[512];
	char strPath[512];
	int nTotalSize, nUsedSize;

	char* pRtn = NULL;

	if (NULL == (fpTemp = popen("df -m", "r")))
	{
		//printf("CDiskInfo::Refresh()	读磁盘剩余空间：popen错误, err=%d\n", errno);
		printf("CDiskInfo::Refresh()	get disk size of rest：popen fail! err=%d\n", errno);
		return false;
	}

	// 跳过标题行
	fgets(strBuff, 512, fpTemp);

	while(NULL !=(pRtn =  fgets(strBuff, 512, fpTemp)) )
	{
//		Filesystem		1M-blocks		Used		Available	Use%	Mounted on
		sscanf(strBuff, "%*s %d %d %*s %*s %s", &nTotalSize, &nUsedSize, strPath);
//		printf("%-15s, nTotalSize=%d, nFreeSize=%d\n", strPath, nTotalSize, nUsedSize);
		diskinfo.name = strPath;
		diskinfo.nCapacity = nTotalSize;
		diskinfo.nUsed = nUsedSize;

		m_vtDisk.push_back(diskinfo);
	}
	pclose(fpTemp);
#endif

	return true;
}

