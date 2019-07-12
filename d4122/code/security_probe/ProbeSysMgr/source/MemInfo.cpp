/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  MemInfo.cpp
 *               
 *  TITLE      :  Probeϵͳ�ڴ���Ϣ��ʵ��
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
 *       -- ���캯��
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
 * DESCRIPTION : ��������
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
 * DESCRIPTION : ��ʼ��
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

		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I����GlobalMemoryStatusʧ�ܣ�����\n" ) ) );
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
 *       -- �����ڴ�״̬��Ϣ
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

		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I����GlobalMemoryStatusʧ�ܣ�����\n" ) ) );
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
		printf("���ڴ�ռ䣺popen����, err=%d\n", errno);
		return -1;
	}

	// ����������
	fgets(strBuff, 512, fpTemp);

	fgets(strBuff, 512, fpTemp);// Mem��
	sscanf(strBuff, "%*s %d ", &m_uPhysicalMem);
	fgets(strBuff, 512, fpTemp);	// -/+ buffers/cache:
	sscanf(strBuff, "%*s %*s %*d %d",  &m_uFreePhysicalMem);

	// ��������û��ֵ��
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
























