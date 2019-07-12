/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  MemInfo.cpp
 *               
 *  TITLE      :  Probe系统内存信息类实现
 *               
 *  CREATE TIME:  2005-10-27 15:38:30
 *               
 *  AUTHOR     :  
 *               
 *  DESCRIPTION:  
 * 				
 *****************************************************************************/

#include "MemInfo.h"

/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CMemInfo::CMemInfo()
 *       
 * DESCRIPTION : 
 *       -- 构造函数
 *       
 * PARAMETER   :
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CMemInfo::CMemInfo()
:	m_uPhysicalMem( 0 ), m_uFreePhysicalMem( 0 ), 
	m_uPageFile( 0 ), m_uFreePageFile( 0 ), m_uVirtualMem( 0 ), 
	m_uFreeVirtualMem( 0 ), m_uCurrentUsage( 0 )
{
}

/******************************************************************************
 * FUNCTION    : ProbeMemInfo::~CMemInfo(
 *       
 * DESCRIPTION : 析构函数
 *       
 *       
 * PARAMETER   : 
 *       
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CMemInfo::~CMemInfo()
{
	
}


/******************************************************************************
 * FUNCTION    : ProbeMemInfo::Init(
 *       
 * DESCRIPTION : 初始化
 *       
 *       
 * PARAMETER   : 
 *       
 *       
 * RETURN     : 
 *       BOOL -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CMemInfo::Init()
{	
#ifdef OS_WINDOWS

	MEMORYSTATUSEX sMem;
	sMem.dwLength = sizeof(sMem);

	if( !GlobalMemoryStatusEx( &sMem ) )
	{
		DWORD dwError = GetLastError();

		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I调用GlobalMemoryStatus失败！！！\n" ) ) );
		return false;
	}

	m_uPhysicalMem = (unsigned int)(sMem.ullTotalPhys / MEM_SIZE_DIV); 
	m_uPageFile = (unsigned int)(sMem.ullTotalPageFile / MEM_SIZE_DIV); 
	m_uVirtualMem = (unsigned int)(sMem.ullTotalVirtual / MEM_SIZE_DIV); 

	ACE_ASSERT( 0 < m_uPhysicalMem && 0 < m_uPageFile && 0 < m_uVirtualMem );
	if (	0 == m_uPhysicalMem
		||	0 == m_uPageFile
		||	0 == m_uVirtualMem )
	{
		ASSERT_INFO;
		return false;
	}
#else
	Refresh();
#endif
	
	return true;
}


/******************************************************************************
 * FUNCTION    : CMemInfo::Refresh()
 *       
 * DESCRIPTION : 
 *       -- 更新内存状态信息
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CMemInfo::Refresh()
{
#ifdef OS_WINDOWS

	MEMORYSTATUSEX sMem;
	sMem.dwLength = sizeof(sMem);

	if( !GlobalMemoryStatusEx( &sMem ) )
	{
		DWORD dwError = GetLastError();

		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I调用GlobalMemoryStatus失败！！！\n" ) ) );
		return false;
	}

	m_uPhysicalMem = (unsigned int)(sMem.ullTotalPhys / MEM_SIZE_DIV); 
	m_uPageFile = (unsigned int)(sMem.ullTotalPageFile / MEM_SIZE_DIV); 
	m_uVirtualMem = (unsigned int)(sMem.ullTotalVirtual / MEM_SIZE_DIV); 

	m_uFreePhysicalMem = (unsigned int)(sMem.ullAvailPhys / MEM_SIZE_DIV); 
	m_uFreePageFile = (unsigned int)(sMem.ullAvailPageFile / MEM_SIZE_DIV); 
	m_uFreeVirtualMem = (unsigned int)(sMem.ullAvailVirtual / MEM_SIZE_DIV);

	m_uCurrentUsage = sMem.dwMemoryLoad;

#else

	char strBuff[512];
//	int nTotalSize, nUsedSize;
	FILE *fpTemp;

	if (NULL == (fpTemp = popen("free -k", "r")))
	{
		printf("读内存空间：popen错误, err=%d\n", errno);
		return -1;
	}

	// 跳过标题行
	fgets(strBuff, 512, fpTemp);

	fgets(strBuff, 512, fpTemp);// Mem行
	sscanf(strBuff, "%*s %d ", &m_uPhysicalMem);
	fgets(strBuff, 512, fpTemp);	// -/+ buffers/cache:
	sscanf(strBuff, "%*s %*s %*d %d",  &m_uFreePhysicalMem);

	// 交换分区没价值，
	// fgets(strBuff, 512, fpTemp);	// Swap:
	// sscanf(strBuff, "%*s %d %*d %d", &m_uPageFile,  &m_uFreePageFile);
	m_uPageFile = 0;
	m_uFreePageFile = 0;

	pclose(fpTemp);


	m_uVirtualMem = 0;
	m_uFreeVirtualMem = 0;
	m_uCurrentUsage = 0;


#endif

	return true;
}
























