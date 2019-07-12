/*******************************************************************************
  CapSave.cpp

	�� �� ��:	�����
	����ʱ��:	2001/11/07 

	����������  һ��������̵���

	�޸ļ�¼��
	�޸�ʱ��	    �޸���		�޸�����

*********************************************************************************/


#include "CapSave.h"
#include "MapViewAccess.h"
#include "ShareMem.h"
#include "InnerCommon.h"
#include "cpf/path_tools.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// �����ļ���һһ��Ӧ�Ĺ����ڴ湲������Ҫ�����в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
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


// ��ʼ����Ҫ��szFileName�в��ܰ��� . .. ֮������·��������ֱ��ָ�� abc\\def, ����ʹ�� / ��Ϊ·����
BOOL CCapSave::Init(LPCSTR szFileName,
		  DWORD dwMaxFileLengthMb,
		  DWORD dwBufSize,
		  int nMultiFiles,
		  unsigned int ncaptime,
		  GET_NEXT_FILE_NAME_FUNC pfnGetNextFile, // ���������¼����ȡ��һ���ļ���������·�����FILENAME_LEN
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

	// ����һ���ļ�������ڴ���
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
 *	��¼������,0��ʾ����,-1��ʾ�豸����-2��ʾ�ռ��������-3��ʾ�ļ�д����ˡ�
 �ɹ�ʱ��ullPos��������GetPacket,����������SetToIndexPacket	
 */
int CCapSave::RecordPacket(BYTE * pPacket,int nPacketLen,HEADER_INFO * pHeadInfo,
							ULONGLONG * pullIndexFrame /*= NULL*/, ULONGLONG * pullPos /*= NULL*/)
{
	if( m_ncaptime != 0 )
	{
		if( (ACE_OS::gettimeofday()-m_start_captime).sec() > m_ncaptime )
		{//�����ʱ�䵽�ˡ�
			return -3;
		}
	}

	DWORD dwIndexFrame;
	DWORD dwPos;

	int nRet = m_SingleWriteObj.RecordPacket(pPacket,nPacketLen,pHeadInfo,&dwIndexFrame,&dwPos);
	
	switch(nRet)
	{
	case +1:	// ����д��

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
 *	��ȡ��ʼ���ļ���
 */
LPCSTR CCapSave::GetInitFileName()
{
	return (LPCSTR)m_strInitFileName;
}

/*
 *  ��ȡ���ڼ�¼�ĵ�ǰ�ļ���
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

	if(pCapSave->m_pfnGetNextFile != NULL)	// ʹ�ûص���ȡ��һ���ļ���
	{
		LPCSTR pName = (*(pCapSave->m_pfnGetNextFile))(pCapSave->m_pParam);
		if(pName != NULL)
		{
			pCapSave->m_strNextFileName = pName;
		}
		else	// �Զ�����һ��
		{
			MakeNextFileName(pCapSave->m_strInitFileName,
				pCapSave->m_pCapShareOpBuf->m_nFileCount,
				pCapSave->m_strNextFileName);
		}
	}
	else // �Զ�����һ��
	{
		MakeNextFileName(pCapSave->m_strInitFileName,
			pCapSave->m_pCapShareOpBuf->m_nFileCount,
			pCapSave->m_strNextFileName);
	}

	return (LPCSTR) pCapSave->m_strNextFileName;
}

int CCapSave::CreateNextFile()
{
	//m_nMultiFiles==0��ʾ���޸��ļ�
	if( m_nMultiFiles > 0 && m_pCapShareOpBuf->m_nFileCount >= m_nMultiFiles )
		return -3;

	DWORD dwFileSize = m_SingleWriteObj.GetWritedSize();
	DWORD dwTotalFrame = m_SingleWriteObj.GetTotalFrames();
	m_SingleWriteObj.CloseAll();

	CStr strFullName;
	
	// �������ļ�,m_strPrevFileName�ھ�̬�ص�����GetNextFileName���Ѿ����
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
	
	// ���������ڴ��ϸ��ļ���
	SINGLE_FILE_INFO & SFInfo1 = m_pCapShareOpBuf->m_Files[m_pCapShareOpBuf->m_nFileCount-1];
	ACE_ASSERT(SFInfo1.dwFrameCount == dwTotalFrame);
	ACE_ASSERT(m_pCapShareOpBuf->m_ullTotalFrame == dwTotalFrame + SFInfo1.ullFirstFrame);
	SFInfo1.dwFrameCount = dwTotalFrame;
	SFInfo1.ullPktEndPos = SFInfo1.ullPktFirstPos + dwFileSize;
	
	// ����һ���ļ�������ڴ���
	SINGLE_FILE_INFO & SFInfo2 = m_pCapShareOpBuf->m_Files[m_pCapShareOpBuf->m_nFileCount];
	SFInfo2.dwFrameCount = 0;
	strcpy(SFInfo2.szFileName,CPF::GetFileNameFromFullName(m_strNextFileName));
	SFInfo2.ullFirstFrame = m_pCapShareOpBuf->m_ullTotalFrame;
	SFInfo2.ullPktFirstPos = SFInfo1.ullPktEndPos;
	SFInfo2.ullPktEndPos = SFInfo1.ullPktEndPos;
	m_pCapShareOpBuf->m_nFileCount ++;
	
	return 0;
}

// ��ȡ��ǰʹ�õĿռ���
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


