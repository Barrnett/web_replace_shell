// CapDataRead.cpp: implementation of the CCapDataRead class.
//
//////////////////////////////////////////////////////////////////////

#include "CapDataRead.h"
#include "ShareMem.h"
#include "MapViewAccess.h"
#include "InnerCommon.h"
#include "cpf/path_tools.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCapDataRead::CCapDataRead()
:m_buffer(2048)
{
	m_bInit				= FALSE;
	m_strOrgFileName	= "";
	m_dwBufSize			= 0;
	m_nCurFile			= 0;
	m_pShareMem			= new CShareMem;
}

CCapDataRead::~CCapDataRead()
{
	CloseAll();
	delete m_pShareMem;
}

BOOL CCapDataRead::Init(LPCSTR szFileName,DWORD nReadNums,DWORD dwBufSize)
{
	ACE_ASSERT(m_bInit == FALSE);

	m_strOrgFileName = szFileName;
	
	if(!TransToFullPath(m_strOrgFileName))
	{
		return FALSE;
	}

	m_nReadNums = nReadNums;

	m_dwBufSize	= dwBufSize;
	
	CStr strShareName;

	CAPMakeShareMemName(m_strOrgFileName,strShareName);

	BOOL bCreate = FALSE;
	if(!m_pShareMem->Init(strShareName,sizeof(CAP_OP_STRUCT),bCreate))
		return FALSE;
	
	m_pCapShareOpBuf = (CAP_OP_STRUCT *)m_pShareMem->GetMem();
	if(bCreate)
	{
		if(!InitShareMem(m_pCapShareOpBuf))
		{
			CloseAll();
			return FALSE;
		}
	}
	
	if(m_pCapShareOpBuf->m_nFileCount<1)
	{
		CloseAll();
		return FALSE;
	}
	
	CStr strFullName;
	
	MakeFullPathName(m_pCapShareOpBuf->m_Files[0].szFileName,strFullName);

	// 打开第一个文件
	if(!m_SingleReadObj.Init(strFullName,dwBufSize))
	{
		CloseAll();
		return FALSE;
	}

	m_nCurFile = 0;

	m_bInit = TRUE;
	return TRUE;
}

BOOL CCapDataRead::InitShareMem(CAP_OP_STRUCT *pCapShareOpBuf)
{
	memset(pCapShareOpBuf,0,sizeof(CAP_OP_STRUCT));

	pCapShareOpBuf->m_bWriting = FALSE;
	strcpy(pCapShareOpBuf->m_strOrgFilePathName,m_strOrgFileName);

	// 建立文件列表
	CStr  strFullFileName = (LPCTSTR)m_strOrgFileName;
	ULONGLONG ullFirstPos = 0;
	ULONGLONG ullFrameNo = 0;
	while(1)
	{
		// 打开下一个文件
		if(!m_SingleReadObj.Init(strFullFileName))
		{
			break;
		}
		
		// 加入到文件表
		SINGLE_FILE_INFO & sfi = m_pCapShareOpBuf->m_Files[m_pCapShareOpBuf->m_nFileCount];
		strcpy(sfi.szFileName,CPF::GetFileNameFromFullName(strFullFileName));
		sfi.ullPktFirstPos = ullFirstPos;
		sfi.ullFirstFrame = ullFrameNo;
		
		sfi.dwFrameCount = m_SingleReadObj.GetTotalFrames();
		m_pCapShareOpBuf->m_ullTotalFrame += sfi.dwFrameCount;
		
		ullFrameNo		+= sfi.dwFrameCount;
		
		ullFirstPos += m_SingleReadObj.GetCurFileSize();
		
		sfi.ullPktEndPos = ullFirstPos;

        m_pCapShareOpBuf->m_ullTotalSize = sfi.ullPktEndPos;
        
		m_pCapShareOpBuf->m_nFileCount ++;
		
		if(!m_SingleReadObj.IsSerialFile())	// 不是连续记录,只需要一个文件
		{
			// 关闭
			m_SingleReadObj.CloseAll();
			break;
		}
		else
		{
			MakeFullPathName(m_SingleReadObj.GetNextFileName(),strFullFileName);
		}
		
		// 关闭
		m_SingleReadObj.CloseAll();
	}
	
	// 判断文件表是否为空
	if(m_pCapShareOpBuf->m_nFileCount==0)
	{
		CloseAll();
		return FALSE;
	}

	return TRUE;
}


void CCapDataRead::CloseAll()
{
	m_strOrgFileName	= "";
	m_dwBufSize			= 0;
	m_nCurFile			= 0;
	m_SingleReadObj.CloseAll();
	m_pShareMem->Close();
	m_pCapShareOpBuf = NULL;


	m_bInit = FALSE;
}

LPCSTR CCapDataRead::GetCurFile()
{
	if(!m_bInit)
		return NULL;
	
	ACE_ASSERT(m_nCurFile < m_pCapShareOpBuf->m_nFileCount);
	return m_pCapShareOpBuf->m_Files[m_nCurFile].szFileName;
}

//通过帧号设置内部读取指针，调用后，可连续使用GetPacket获取后续的帧
//该函数要求BuildIndex已经调用
BOOL CCapDataRead::SetToIndexPacket(const ULONGLONG & ullIndexFrame)
{
	if(!m_bInit)
		return FALSE;

	ACE_ASSERT(m_nCurFile<m_pCapShareOpBuf->m_nFileCount);

	SINGLE_FILE_INFO & sfi = m_pCapShareOpBuf->m_Files[m_nCurFile];
	if(ullIndexFrame>=sfi.ullFirstFrame && 
		ullIndexFrame<sfi.ullFirstFrame+sfi.dwFrameCount) //在当前文件
	{
		DWORD dwFrameIndex = (DWORD)(ullIndexFrame-sfi.ullFirstFrame);
		return m_SingleReadObj.SetToIndexPacket(dwFrameIndex);
	}
		
	CStr strFullName;

	// 定位在哪个文件
	for(int i=0; i<m_pCapShareOpBuf->m_nFileCount; i++)
	{
		SINGLE_FILE_INFO & sfi = m_pCapShareOpBuf->m_Files[i];
		
		if(ullIndexFrame>=sfi.ullFirstFrame && 
			ullIndexFrame<sfi.ullFirstFrame+sfi.dwFrameCount)
		{
			DWORD dwFrameIndex = (DWORD)(ullIndexFrame-sfi.ullFirstFrame);

			ACE_ASSERT(m_nCurFile != i);
			m_SingleReadObj.CloseAll();

			
			MakeFullPathName(sfi.szFileName,strFullName);

			// 初始化新的文件
			if(!m_SingleReadObj.Init(strFullName,m_dwBufSize))
			{
				MakeFullPathName(m_pCapShareOpBuf->m_Files[m_nCurFile].szFileName,strFullName);

				// 不成功再恢复原来打开的文件
				m_SingleReadObj.Init(strFullName,m_dwBufSize);

				return FALSE;
			}
			
			// 设置当前文件
			m_nCurFile = i;
			return m_SingleReadObj.SetToIndexPacket(dwFrameIndex);
		}
	}
	return FALSE;
}	

// 提取包信息，数据部分直接指向数据包部分的缓冲区
// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
int CCapDataRead::InnerGetNextPacket(PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
		return -1;

	int nRet = m_SingleReadObj.GetNextPacket(theContext);
	
	if(nRet == +1)
		return nRet;

	ACE_ASSERT(m_nCurFile<m_pCapShareOpBuf->m_nFileCount);
	
	if(m_nCurFile >= m_pCapShareOpBuf->m_nFileCount-1)
	{
		return -1;
	}


	if(m_nReadNums != 0 && m_nCurFile + 1 >= (int)m_nReadNums)
	{
		return -1;
	}

	CStr strFullName;

	MakeFullPathName(m_pCapShareOpBuf->m_Files[m_nCurFile+1].szFileName,strFullName);

	m_SingleReadObj.CloseAll();
	if(!m_SingleReadObj.Init(strFullName,m_dwBufSize))
	{
		return -1;
	}

	m_nCurFile ++;

	return m_SingleReadObj.GetNextPacket(theContext);
}

// 提取包信息，数据部分直接指向数据包部分的缓冲区
// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
int CCapDataRead::InnerGetPrevPacket(PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
		return -1;
	
	int nRet = m_SingleReadObj.GetPrevPacket(theContext);
	
	if(nRet == +1)
		return nRet;
	
	ACE_ASSERT(m_nCurFile<m_pCapShareOpBuf->m_nFileCount);
	
	if(m_nCurFile == 0)
	{
		return -1;
	}
	
	m_SingleReadObj.CloseAll();

	CStr strFullName;

	MakeFullPathName(m_pCapShareOpBuf->m_Files[m_nCurFile-1].szFileName,strFullName);

	if(!m_SingleReadObj.Init(strFullName,m_dwBufSize))
	{
		return -1;
	}
	
	m_nCurFile --;

	DWORD dwFrames = m_SingleReadObj.GetTotalFrames();
	m_SingleReadObj.SetToIndexPacket(dwFrames-1);
	return m_SingleReadObj.GetPrevPacket(theContext);
}

// 提取包信息，数据部分直接指向数据包部分的缓冲区
// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
int CCapDataRead::InnerGetPacketFromIndex(const ULONGLONG & ullIndexFrame,PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
		return -1;
	
	ACE_ASSERT(m_nCurFile<m_pCapShareOpBuf->m_nFileCount);

	// 判断是否在当前文件
	SINGLE_FILE_INFO & sfi= m_pCapShareOpBuf->m_Files[m_nCurFile];
	if(ullIndexFrame >= sfi.ullFirstFrame && 
		ullIndexFrame < sfi.ullFirstFrame + sfi.dwFrameCount) // 在当前文件
	{
		DWORD dwFrameIndex = (DWORD)(ullIndexFrame-sfi.ullFirstFrame);
		return m_SingleReadObj.GetPacketFromIndex(dwFrameIndex,theContext);
	}

	CStr strFullName;
		
	// 定位在哪个文件
	for(int i=0; i<m_pCapShareOpBuf->m_nFileCount; i++)
	{
		SINGLE_FILE_INFO & sfi= m_pCapShareOpBuf->m_Files[i];

		if(ullIndexFrame >= sfi.ullFirstFrame && 
			ullIndexFrame < sfi.ullFirstFrame + sfi.dwFrameCount)
		{
			ACE_ASSERT(ullIndexFrame-sfi.ullFirstFrame<MAXDWORD);
			DWORD dwFrameIndex = (DWORD)(ullIndexFrame-sfi.ullFirstFrame);

			ACE_ASSERT(m_nCurFile != i);

			m_SingleReadObj.CloseAll();

			// 初始化新的文件
			if(!m_SingleReadObj.Init(MakeFullPathName(sfi.szFileName,strFullName),m_dwBufSize))
			{
				MakeFullPathName(m_pCapShareOpBuf->m_Files[m_nCurFile].szFileName,strFullName);

				// 不成功再恢复原来打开的文件
				m_SingleReadObj.Init(strFullName,m_dwBufSize);

				return -1;
			}

			// 设置当前文件
			m_nCurFile = i;
			return m_SingleReadObj.GetPacketFromIndex(dwFrameIndex,theContext);
		}
	}
	return -1;
}

// 提取包信息，数据部分直接指向数据包部分的缓冲区
// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
int CCapDataRead::GetPacketFromPos(const ULONGLONG & ullPos,PACKET_CONTEXT& theContext)
{	
	if(!m_bInit)
		return -1;
	
	ACE_ASSERT(m_nCurFile<m_pCapShareOpBuf->m_nFileCount);
	
	// 判断是否在当前文件
	SINGLE_FILE_INFO & sfi= m_pCapShareOpBuf->m_Files[m_nCurFile];
	if(ullPos>=sfi.ullPktFirstPos && ullPos<sfi.ullPktEndPos) // 在当前文件
	{
		DWORD dwPos = (DWORD)(ullPos-sfi.ullPktFirstPos);
		return m_SingleReadObj.GetPacketFromPos(dwPos,theContext);
	}

	CStr strFullName;
	
	// 定位在哪个文件
	for(int i=0; i<m_pCapShareOpBuf->m_nFileCount; i++)
	{
		SINGLE_FILE_INFO & sfi= m_pCapShareOpBuf->m_Files[i];
		
		if(ullPos>=sfi.ullPktFirstPos && ullPos<sfi.ullPktEndPos)
		{
			ACE_ASSERT(ullPos-sfi.ullPktFirstPos<MAXDWORD);
			DWORD dwPos = (DWORD)(ullPos-sfi.ullPktFirstPos);
			
			ACE_ASSERT(m_nCurFile != i);
			
			m_SingleReadObj.CloseAll();
			
			// 初始化新的文件
			if(!m_SingleReadObj.Init(MakeFullPathName(sfi.szFileName,strFullName),m_dwBufSize))
			{
				MakeFullPathName(m_pCapShareOpBuf->m_Files[m_nCurFile].szFileName,strFullName);

				// 不成功再恢复原来打开的文件
				m_SingleReadObj.Init(strFullName,m_dwBufSize);
				
				return -1;
			}
			
			// 设置当前文件
			m_nCurFile = i;
			return m_SingleReadObj.GetPacketFromPos(dwPos,theContext);
		}
	}
	return -1;
}

LPCSTR CCapDataRead::MakeFullPathName(LPCSTR szFileName,CStr& strFullName)
{
	strFullName = CPF::GetPathNameFromFullName(m_strOrgFileName);

	strFullName = strFullName+FILENAME_SEPERATOR+szFileName;
	return (LPCSTR)strFullName;
}

ULONGLONG CCapDataRead::GetCurTotalFrames()
{
	if(!m_bInit)
	{
		return 0;
	}
	
	return m_pCapShareOpBuf->m_ullTotalFrame;
}

// 获取当前的大小（字节）
ULONGLONG CCapDataRead::GetCurTotalSize()
{
	if(!m_bInit)
	{
		return 0;
	}
    
    return m_pCapShareOpBuf->m_ullTotalSize;
}


