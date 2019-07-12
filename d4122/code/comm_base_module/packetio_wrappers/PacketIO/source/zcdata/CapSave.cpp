/*******************************************************************************
  CapSave.cpp

	创 建 者:	孙向光
	创建时间:	2001/11/07 

	内容描述：  一个捕获存盘的类

	修改记录：
	修改时间	    修改者		修改内容

*********************************************************************************/


#include "CapSave.h"
#include "MapViewAccess.h"
#include "ShareMem.h"
#include "InnerCommon.h"
#include "cpf/path_tools.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 创建文件的一一对应的共享内存共享名，要求其中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
void CAPMakeShareMemName(LPCSTR szFileName,CStr& strSMName)
{
	CStr strPathName = szFileName;
	
	if(!TransToFullPath(strPathName))
	{
		ACE_ASSERT(0);
	}
	
	strPathName.MakeUpper();

	strSMName.Format("CAPOP_%s",(LPCSTR)strPathName);
	strSMName.Replace(FILENAME_SEPERATOR,'_');

	return;
}

CCapSave::CCapSave()
{
	m_bInit				= FALSE;
	m_nMultiFiles		= 1;
	m_ncaptime			= 0;
	m_dwMaxSize			= 0;
	m_pfnGetNextFile	= NULL;
	m_pParam			= NULL;
	m_pShareMem			= new CShareMem;
}

CCapSave::~CCapSave()
{
	CloseAll();
	
	if( m_pShareMem )
	{
		delete m_pShareMem;
		m_pShareMem = NULL;
	}
}


// 初始化，要求szFileName中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
BOOL CCapSave::Init(LPCSTR szFileName,
		  DWORD dwMaxFileLengthMb,
		  DWORD dwBufSize,
		  int nMultiFiles,
		  unsigned int ncaptime,
		  GET_NEXT_FILE_NAME_FUNC pfnGetNextFile, // 如果连续记录，获取下一个文件名，不含路径，最长FILENAME_LEN
		  void * pParam
		  )
{
	ACE_ASSERT(m_bInit == FALSE);

	CStr strFullPathName = szFileName;

	if(!TransToFullPath(strFullPathName))
	{
		ACE_ASSERT(0);
		return FALSE;
	}

	CStr strShareName;
	
	CAPMakeShareMemName(strFullPathName,strShareName);
	BOOL bCreate = FALSE;
	if(!m_pShareMem->Init(strShareName,sizeof(CAP_OP_STRUCT),bCreate))
		return FALSE;
	
	if(!bCreate)
	{
		CloseAll();
		return FALSE;
	}

	m_pCapShareOpBuf = (CAP_OP_STRUCT *)m_pShareMem->GetMem();

	memset(m_pCapShareOpBuf,0,sizeof(CAP_OP_STRUCT));
	m_pCapShareOpBuf->m_bWriting = TRUE;
	strcpy(m_pCapShareOpBuf->m_strOrgFilePathName,strFullPathName);

	m_strInitFileName	= strFullPathName;
	m_dwMaxSize			= dwMaxFileLengthMb*1024*1024;
	m_nMultiFiles		= nMultiFiles;
	m_pfnGetNextFile	= pfnGetNextFile;
	m_pParam			= pParam;
	m_dwBufSize			= dwBufSize;
	m_ncaptime			= ncaptime;
	m_start_captime		= ACE_OS::gettimeofday();
	
	if(!m_SingleWriteObj.Init(szFileName,m_dwMaxSize,m_dwBufSize,
		(nMultiFiles!=1),0,
		m_strPrevFileName,GetNextFileName,this))
	{
		CloseAll();
		return FALSE;
	}

	// 加入一个文件项到共享内存中
	SINGLE_FILE_INFO & SFInfo = m_pCapShareOpBuf->m_Files[m_pCapShareOpBuf->m_nFileCount];
	SFInfo.dwFrameCount = 0;
	strcpy(SFInfo.szFileName,CPF::GetFileNameFromFullName(strFullPathName));
	SFInfo.ullFirstFrame = 0;
	SFInfo.ullPktFirstPos = 0;
	SFInfo.ullPktEndPos = 0;
	m_pCapShareOpBuf->m_nFileCount ++;

	m_strNextFileName	= szFileName;
	m_strPrevFileName  = CPF::GetFileNameFromFullName(strFullPathName);

	if(!m_ReadObj.Init(strFullPathName,0,m_dwBufSize))
	{
		CloseAll();
		return FALSE;
	}

	m_bInit = TRUE;
	return TRUE;
}

void CCapSave::CloseAll()
{
	m_ReadObj.CloseAll();
	m_SingleWriteObj.CloseAll();

	m_pShareMem->Close();
	m_pCapShareOpBuf = NULL;
	
	m_strPrevFileName	= "";
	m_strNextFileName	= "";
	m_strInitFileName	= "";
	m_dwMaxSize			= 0;
	m_nMultiFiles		= 1;
	m_pfnGetNextFile	= NULL;
	m_pParam			= NULL;
	

	m_bInit = FALSE;
}

/*
 *	记录包数据,0表示正常,-1表示设备出错，-2表示空间磁盘满，-3表示文件写完成了。
 成功时的ullPos，可用于GetPacket,但不可用于SetToIndexPacket	
 */
int CCapSave::RecordPacket(BYTE * pPacket,int nPacketLen,HEADER_INFO * pHeadInfo,
							ULONGLONG * pullIndexFrame /*= NULL*/, ULONGLONG * pullPos /*= NULL*/)
{
	if( m_ncaptime != 0 )
	{
		if( (ACE_OS::gettimeofday()-m_start_captime).sec() > m_ncaptime )
		{//捕获的时间到了。
			return -3;
		}
	}

	DWORD dwIndexFrame;
	DWORD dwPos;

	int nRet = m_SingleWriteObj.RecordPacket(pPacket,nPacketLen,pHeadInfo,&dwIndexFrame,&dwPos);
	
	switch(nRet)
	{
	case +1:	// 正常写入

		ACE_ASSERT(m_pCapShareOpBuf->m_nFileCount>0);

		if(pullIndexFrame != NULL)
			*pullIndexFrame = m_pCapShareOpBuf->m_ullTotalFrame;

		m_pCapShareOpBuf->m_ullTotalFrame ++;

		{
			SINGLE_FILE_INFO & SFInfo = m_pCapShareOpBuf->m_Files[m_pCapShareOpBuf->m_nFileCount-1];
			SFInfo.dwFrameCount ++;
			SFInfo.ullPktEndPos = SFInfo.ullPktFirstPos + m_SingleWriteObj.GetWritedSize();
            m_pCapShareOpBuf->m_ullTotalSize = SFInfo.ullPktEndPos;
            ACE_ASSERT(SFInfo.ullFirstFrame+SFInfo.dwFrameCount == m_pCapShareOpBuf->m_ullTotalFrame);
			if(pullPos != NULL)
				*pullPos = SFInfo.ullPktFirstPos + dwPos;
		}

		return 0;
		break;

	case 0:	
		{
			int nrnt = CreateNextFile();

			if( nrnt == 0 )
			{
				return RecordPacket(pPacket,nPacketLen,pHeadInfo,pullIndexFrame,pullPos);
			}

			return nrnt;				
		}
		break;

	case -1:
	default:
		return -1;
		break;
	}

	return -1;
}

/*
 *	获取初始化文件名
 */
LPCSTR CCapSave::GetInitFileName()
{
	return (LPCSTR)m_strInitFileName;
}

/*
 *  获取正在记录的当前文件名
 */
LPCSTR CCapSave::GetCurrentFileName()
{
	return (LPCSTR)m_strNextFileName;
}

void MakeNextFileName(LPCSTR strFileName,int nCurFileCount,CStr& strNextName)
{
	CStr strName = CPF::GetFileNameFromFullName(strFileName);

	int nStat = strName.ReverseFind('.');

	ACE_ASSERT(nStat != -1);

	strNextName.Format("%s_e%04d.CPT",strName.Left(nStat).GetString(),nCurFileCount);

	return;
}

LPCSTR CCapSave::GetNextFileName(void * pParam)
{
	CCapSave * pCapSave = (CCapSave *)pParam;
	ACE_ASSERT(pCapSave!=NULL);

	if(pCapSave == NULL)
		return NULL;

	if(pCapSave->m_pfnGetNextFile != NULL)	// 使用回调获取下一个文件名
	{
		LPCSTR pName = (*(pCapSave->m_pfnGetNextFile))(pCapSave->m_pParam);
		if(pName != NULL)
		{
			pCapSave->m_strNextFileName = pName;
		}
		else	// 自动创建一个
		{
			MakeNextFileName(pCapSave->m_strInitFileName,
				pCapSave->m_pCapShareOpBuf->m_nFileCount,
				pCapSave->m_strNextFileName);
		}
	}
	else // 自动创建一个
	{
		MakeNextFileName(pCapSave->m_strInitFileName,
			pCapSave->m_pCapShareOpBuf->m_nFileCount,
			pCapSave->m_strNextFileName);
	}

	return (LPCSTR) pCapSave->m_strNextFileName;
}

int CCapSave::CreateNextFile()
{
	//m_nMultiFiles==0表示无限个文件
	if( m_nMultiFiles > 0 && m_pCapShareOpBuf->m_nFileCount >= m_nMultiFiles )
		return -3;

	DWORD dwFileSize = m_SingleWriteObj.GetWritedSize();
	DWORD dwTotalFrame = m_SingleWriteObj.GetTotalFrames();
	m_SingleWriteObj.CloseAll();

	CStr strFullName;
	
	// 建立新文件,m_strPrevFileName在静态回调函数GetNextFileName中已经获得
	if(!m_SingleWriteObj.Init(
			MakeFullPathName(m_strNextFileName,strFullName),
			m_dwMaxSize,
			m_dwBufSize,(m_nMultiFiles!=1),
			m_pCapShareOpBuf->m_nFileCount,m_strPrevFileName,
			CCapSave::GetNextFileName,this))
	{
		return -1;
	}

	m_strPrevFileName = m_strNextFileName;
	
	// 结束共享内存上个文件项
	SINGLE_FILE_INFO & SFInfo1 = m_pCapShareOpBuf->m_Files[m_pCapShareOpBuf->m_nFileCount-1];
	ACE_ASSERT(SFInfo1.dwFrameCount == dwTotalFrame);
	ACE_ASSERT(m_pCapShareOpBuf->m_ullTotalFrame == dwTotalFrame + SFInfo1.ullFirstFrame);
	SFInfo1.dwFrameCount = dwTotalFrame;
	SFInfo1.ullPktEndPos = SFInfo1.ullPktFirstPos + dwFileSize;
	
	// 加入一个文件项到共享内存中
	SINGLE_FILE_INFO & SFInfo2 = m_pCapShareOpBuf->m_Files[m_pCapShareOpBuf->m_nFileCount];
	SFInfo2.dwFrameCount = 0;
	strcpy(SFInfo2.szFileName,CPF::GetFileNameFromFullName(m_strNextFileName));
	SFInfo2.ullFirstFrame = m_pCapShareOpBuf->m_ullTotalFrame;
	SFInfo2.ullPktFirstPos = SFInfo1.ullPktEndPos;
	SFInfo2.ullPktEndPos = SFInfo1.ullPktEndPos;
	m_pCapShareOpBuf->m_nFileCount ++;
	
	return 0;
}

// 获取当前使用的空间数
ULONGLONG CCapSave::GetTotalUsage()
{
	if(!m_bInit)
		return 0;
	SINGLE_FILE_INFO & SFInfo = m_pCapShareOpBuf->m_Files[m_pCapShareOpBuf->m_nFileCount-1];
	return SFInfo.ullPktEndPos;
}

LPCSTR CCapSave::MakeFullPathName(LPCSTR szFileName,CStr& strFullName)
{
	strFullName = CPF::GetPathNameFromFullName(m_strInitFileName);
	strFullName = strFullName+FILENAME_SEPERATOR_STR+szFileName;
	return (LPCSTR)strFullName;
}


