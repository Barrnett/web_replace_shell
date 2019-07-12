// CPTSingleWrite.cpp: implementation of the CCPTSingleWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "CPTSingleWrite.h"
#include "ShareMem.h"
#include "MapViewAccess.h"
#include "InnerCommon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CCPTSingleWrite::CCPTSingleWrite()
{
	m_bInit				= FALSE;
	m_bCPTMem			= FALSE;
	m_pShareOpBuf		= NULL;
	m_pfnGetNextFile	= NULL;
	m_hFile				= ZCTT_INVALID_HANDLE;
	m_hMap				= NULL;
	m_dwCurWritePos		= 0;
	m_bFileEnd			= FALSE;
	m_pParam			= NULL;

	m_pShareMem			= new CShareMem;
	m_pWriteBuf			= new CMapViewAccess;

	m_dwOpen = 0;
}

CCPTSingleWrite::~CCPTSingleWrite()
{
	CloseAll();
	delete m_pShareMem;
	delete m_pWriteBuf;
	ACE_ASSERT(m_dwOpen == 0);
}

BOOL CCPTSingleWrite::Init(LPCSTR szFileName,						// �ļ�����NULL��ʾ���ڴ��в���
						   DWORD dwMaxSize,							// �ļ���󳤶�
						   DWORD dwBufSize,							// ������Ĵ�С,��������ڴ��в��񣬸�ֵ��Ч
						   BOOL bSerialFile,						// �Ƿ�������¼�е�һ��
						   int nFileNo,								// ���������¼���ļ�˳��ţ���0��ʼ
						   LPCSTR szPrevFileName,					// ���������¼��������һ���ļ���,����·�����������ļ�ͷ��
						   GET_NEXT_FILE_NAME_FUNC pfnGetNextFile,	// ���������¼����ȡ��һ���ļ���������·�����FILENAME_LEN
						   void * pParam							// pfnGetNextFile�ص��� void * ����
						   ) 
{
	// �����ļ�����
	if(dwMaxSize>_MAX_DWORD - CPTFILE_MAX_ADDTION_DATA)
	{
		return FALSE;
	}


	CStr strShareName;
	CStr strFullPathName;
	if(szFileName != NULL) //���ļ��в���
	{
		strFullPathName = szFileName;
		if(!TransToFullPath(strFullPathName))
		{
			ACE_ASSERT(0);
			return FALSE;
		}
		CPTMakeShareMemName(strFullPathName,strShareName);
	}
	else // ���ڴ��в���
	{
		strFullPathName = "IN_MEMORY_CPT";
		strShareName.Format("CPTOP_IN_MEMORY_CPT%u",GetCurrentProcessId());
		bSerialFile = FALSE;
		pfnGetNextFile = NULL;
		nFileNo = 0;
		m_bCPTMem = TRUE;
	}
	
	

	BOOL bCreate = FALSE;
	if(!m_pShareMem->Init(strShareName,sizeof(CPT_OP_STRUCT),bCreate))
		return FALSE;
	if(!bCreate)	//������ǰ����
	{
		CloseAll();
		return FALSE;
	}
	m_pShareOpBuf = (CPT_OP_STRUCT*)m_pShareMem->GetMem();
	if(m_pShareOpBuf == NULL)
	{
		CloseAll();
		return FALSE;
	}

	m_pfnGetNextFile = pfnGetNextFile;
	m_pParam = pParam;
		
	memset(m_pShareOpBuf,0,sizeof(CPT_OP_STRUCT));

	m_pShareOpBuf->m_bWriting = TRUE;
	m_pShareOpBuf->m_dwMapSize = dwMaxSize;
	strcpy(m_pShareOpBuf->m_strFilePathName,strFullPathName);
	
	// ���ò����ļ�ͷ
	memset(&m_pShareOpBuf->m_filehead,0,sizeof(CAPFILE_HEAD));
	memcpy(m_pShareOpBuf->m_filehead.ID,CAPFILE_ID4,10);
	m_pShareOpBuf->m_filehead.bSerialFiles = bSerialFile;
	m_pShareOpBuf->m_filehead.nNo = nFileNo;
	m_pShareOpBuf->m_filehead.dwIndexStep = DEFAULT_INDEX_STEP;
	time_t timebegin;
	time(&timebegin);
	m_pShareOpBuf->m_filehead.timeBegin = (ACE_UINT32)timebegin;
	//time(&m_pShareOpBuf->m_filehead.timeBegin);
	if(bSerialFile)
	{
		ACE_ASSERT(NULL==strchr(szPrevFileName,FILENAME_SEPERATOR));	// ��Ҫ·��
		strcpy(m_pShareOpBuf->m_filehead.PrevFileName,szPrevFileName);
	}
	
	// ����ӳ��ͻ���
	if(!CreateMapFile(m_bCPTMem,dwBufSize))
	{
		CloseAll();
		return FALSE;
	}
    
	if(!m_ReadObj.Init(szFileName))
	{
		CloseAll();
		return FALSE;
	}


	m_bInit = TRUE;
	// д���ļ�ͷ
	int nRet = Write(0,&m_pShareOpBuf->m_filehead,sizeof(CAPFILE_HEAD));
	if(nRet == -1)
	{
		CloseAll();
		return FALSE;
	}
	return TRUE;
}

// ��д�ļ�ͷ,TRUE��ʾ�ɹ�
BOOL CCPTSingleWrite::EndFile()
{
	if(!m_bInit || m_bFileEnd)
		return FALSE;

	m_bFileEnd = TRUE;

    //д��������־
    USHORT endFlag = CAPFILE_FRAME_END_FLAG;
    if(-1==Write(&endFlag,2))
        return FALSE;

    if(-1==Write(CPTFILE_FRAME_END_STRING,CPTFILE_FRAME_END_STRING_LEN))
        return FALSE;
    
	m_pShareOpBuf->m_filehead.dwIndexPosInFile = m_pShareOpBuf->m_dwContentEnd;
	m_pShareOpBuf->m_filehead.dwPacketCount = m_pShareOpBuf->m_dwPacketCount;

	//���ò����ļ�ͷ
	time_t timeend;
	time(&timeend);
	m_pShareOpBuf->m_filehead.timeEnd = (ACE_UINT32)timeend;

	//time(&m_pShareOpBuf->m_filehead.timeEnd);
	
	if(m_pShareOpBuf->m_filehead.bSerialFiles)
	{
		if(m_pfnGetNextFile != NULL)
		{
			LPCSTR pFileName = (*m_pfnGetNextFile)(m_pParam);
			if(pFileName != NULL)
			{
				ACE_ASSERT(NULL==strchr(pFileName,FILENAME_SEPERATOR));	//��·�����ļ���

				strcpy(m_pShareOpBuf->m_filehead.NextFileName,pFileName);				
			}
			
		}
	}

	// �����ļ�ͷ�е�У���

	m_pShareOpBuf->m_filehead.dwHeadCheckSum = 0x0000;
	WORD wTemp = CPTFileHeadComputeCheckSum(
		(BYTE *)&m_pShareOpBuf->m_filehead,sizeof(m_pShareOpBuf->m_filehead));

	m_pShareOpBuf->m_filehead.dwHeadCheckSum = ACE_SWAP_WORD(wTemp);

	if(-1==Write(0,&m_pShareOpBuf->m_filehead,sizeof(CAPFILE_HEAD)))
		return FALSE;
	
	
	if(!WriteIndexAtEnd(m_pShareOpBuf->m_dwContentEnd))
		return FALSE;

	return TRUE;
}

// �����ļ���Map���󣬲���ʼ��д����
BOOL CCPTSingleWrite::CreateMapFile(BOOL bInMem,DWORD dwBufSize)
{
	ACE_ASSERT(m_pShareOpBuf != NULL);
	if(m_pShareOpBuf == NULL)
		return FALSE;

	if(!bInMem)
	{
		// �����ļ�
#ifdef OS_WINDOWS
		m_hFile = CreateFile (m_pShareOpBuf->m_strFilePathName,
			GENERIC_WRITE|GENERIC_READ ,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
#elif defined(OS_LINUX)
		m_hFile = open(m_pShareOpBuf->m_strFilePathName, O_CREAT|O_RDWR, 0660 );
#endif
		
		if(m_hFile == ZCTT_INVALID_HANDLE)
			return FALSE;
		m_dwOpen ++;
	}
	else
        m_hFile = ZCTT_INVALID_HANDLE;

	// ���������ļ�ӳ��
	CStr strMapHandleName ;
	
	if(m_bCPTMem)
		strMapHandleName.Format("CPTMH_IN_MEMORY_CPT%u",GetCurrentProcessId());
	else
		CPTMakeMapHandleName(m_pShareOpBuf->m_strFilePathName,strMapHandleName);
			
#if defined(OS_WINDOWS)
	m_hMap = CreateFileMapping(m_hFile,NULL,
				PAGE_READWRITE,0,m_pShareOpBuf->m_dwMapSize,strMapHandleName);
#elif defined(OS_LINUX)
	ftruncate64(m_hFile,(LONGLONG)m_pShareOpBuf->m_dwMapSize);
	m_hMap = HANDLE(m_hFile);				
#endif
	
	if(m_hMap == NULL)
	{
#if defined(OS_WINDOWS)
		DWORD dwErr = GetLastError();
#endif
		
		if(m_hFile != ZCTT_INVALID_HANDLE)
		{
			CLOSEFILE(m_hFile);
			m_dwOpen--;
			m_hFile = ZCTT_INVALID_HANDLE;
		}
		return FALSE;
	}

	// ����д����
	if(bInMem)
		dwBufSize = m_pShareOpBuf->m_dwMapSize;
		
	m_pWriteBuf->SetMaxBufferSize(dwBufSize);
	if(!m_pWriteBuf->SetMapHandle(m_hMap,m_pShareOpBuf->m_dwMapSize))
	{
		CLOSE_MAP(m_hMap);
		m_hMap = NULL;

        	if(m_hFile != ZCTT_INVALID_HANDLE)
        	{
            		CLOSEFILE(m_hFile);
			m_dwOpen --;
        	}
		return FALSE;
	}
	
	return TRUE;
}

// ���ļ�β��д������,nPosΪ������־���λ��
BOOL CCPTSingleWrite::WriteIndexAtEnd(DWORD dwPos)
{

	ACE_ASSERT(m_pShareOpBuf->m_dwIndexCount<=MAX_ONE_CPT_INDEX);
	if(m_pShareOpBuf->m_dwPacketCount>0)
	{
		ACE_ASSERT(m_pShareOpBuf->m_dwIndexCount == 
			(m_pShareOpBuf->m_dwPacketCount-1)/m_pShareOpBuf->m_filehead.dwIndexStep+1);
	}
	int nRet = Write(dwPos,m_pShareOpBuf->m_adwPktIndex,
		m_pShareOpBuf->m_dwIndexCount*sizeof(DWORD));

	if(nRet == -1)
		return FALSE;
	return TRUE;
}


// ��ȡ�ļ���
LPCSTR CCPTSingleWrite::GetFileName()
{
	if(m_pShareOpBuf == NULL)
		return NULL;
	return m_pShareOpBuf->m_strFilePathName;
}


// ��¼������,���� 1 ������0 �ļ�������1����,�ɹ�ʱ��dwPos��������GetPacket,����������SetToIndexPacket
int CCPTSingleWrite::RecordPacket(BYTE * pPacket,int nPacketLen,HEADER_INFO * pHeadInfo,
								  DWORD * pdwIndexFrame/* = NULL*/, DWORD * pdwPos /*= NULL*/)
{
	if(!m_bInit || m_bFileEnd)
		return -1;

	ACE_ASSERT(nPacketLen >0 && nPacketLen < MAX_RAW_PACKETLEN);
	
	static const char FillBytes[6] = {0,0,0,0,0,0};

	int nError = 0;

	do
	{
		USHORT uLen = nPacketLen+pHeadInfo->btHeaderLength+2;


		DWORD dwSpaceSize = m_pShareOpBuf->m_dwMapSize - m_pShareOpBuf->m_dwContentEnd;
		if(uLen + CPTFILE_MIN_FREE + m_pShareOpBuf->m_dwIndexCount*sizeof(DWORD) > dwSpaceSize)	//û�пռ���
		{
            ACE_ASSERT(dwSpaceSize>=CPTFILE_MIN_FREE);
            if(! EndFile())
			{
				nError = 6;
				break;
			}

			return 0;	//�ļ���
		}

		if(0 == m_pShareOpBuf->m_dwPacketCount % m_pShareOpBuf->m_filehead.dwIndexStep)
		{
			m_pShareOpBuf->m_adwPktIndex[m_pShareOpBuf->m_dwIndexCount++]=m_pShareOpBuf->m_dwContentEnd;
			m_pShareOpBuf->m_filehead.dwIndexCount  = m_pShareOpBuf->m_dwIndexCount;
			ACE_ASSERT(m_pShareOpBuf->m_dwIndexCount<MAX_ONE_CPT_INDEX);
		}

		DWORD dwBeginPos = m_pShareOpBuf->m_dwContentEnd;

		//д�볤��
		int nRet = Write(&uLen,2);
		if(nRet == -1)
		{
			nError = 1;
			break;
		}
		
		//д��ͷ
		nRet = Write(pHeadInfo,sizeof(HEADER_INFO));
		if(nRet == -1)
		{			
			nError = 2;
			break;
		}
		if(pHeadInfo->btHeaderLength>sizeof(HEADER_INFO))
		{
			ACE_ASSERT(0);
			nError = 2;
			break;
		}

		//д���
		nRet = Write(pPacket,nPacketLen);
		if(nRet == -1)
		{
			nError = 3;
			break;
		}

		//д������ֽ�
		int nFill = (((DWORD)uLen-1)/4+1)*4-uLen;
		if(nFill>0)
		{
			nRet = Write(FillBytes,nFill);
			if(nRet == -1)
			{
				nError = 4;
				break;
			}
		}

		//д������ֽ�
		nRet = Write(FillBytes,2);
		if(nRet == -1)
		{
			nError = 4;
			break;
		}

		//�ٴ�д�볤��
		nRet = Write(&uLen,2);
		if(nRet == -1)
		{
			nError = 5;
			break;
		}

		if(pdwIndexFrame != NULL)
		{
			*pdwIndexFrame = m_pShareOpBuf->m_dwPacketCount;
		}

		if(pdwPos != NULL)
		{
			*pdwPos = dwBeginPos;
		}

		m_pShareOpBuf->m_dwPacketCount ++;
		
		return +1; // ���� 

	}while(0);

	ACE_ASSERT(FALSE);

	return -1; // ����

}

void CCPTSingleWrite::CloseAll()
{
	EndFile();

	m_ReadObj.CloseAll();
	m_pfnGetNextFile	= NULL;
	m_pParam			= NULL;
	m_dwCurWritePos		= 0;
	
	m_pWriteBuf->Close();
	
	if(m_hMap != NULL)
	{
		CLOSE_MAP(m_hMap);

		m_hMap = NULL;
	}
	
	if(m_hFile != ZCTT_INVALID_HANDLE)
	{
		ACE_ASSERT(m_pShareOpBuf->m_bWriting);
		if(m_bInit && !m_bCPTMem && m_pShareMem->GetRefCount() == 1) // ���һ��ʹ�ø��ļ��Ķ���
		{
			LONGLONG llEnd =m_pShareOpBuf->m_dwContentEnd; 
			
			// ���ļ�д������β���ض�
			ftruncate64(m_hFile,llEnd);
			
		}
		CLOSEFILE(m_hFile);
		m_dwOpen--;
			
		m_hFile = ZCTT_INVALID_HANDLE;
	}

	m_pShareMem->Close();
	m_pShareOpBuf = NULL;
	m_bFileEnd = FALSE;
	m_bInit = FALSE;
	ACE_ASSERT(m_dwOpen==0);
}

// д��,����д����ֽ�����-1����
int CCPTSingleWrite::Write(const void * pData,int nSize)
{
	if(nSize <0 || !m_bInit)
		return -1;

	void * pMem = m_pWriteBuf->GetMemory(m_dwCurWritePos,nSize);
	if(pMem == NULL)
		return -1;

	memcpy(pMem,pData,nSize);

	m_dwCurWritePos += nSize;
	if(m_pShareOpBuf->m_dwContentEnd<m_dwCurWritePos)
		m_pShareOpBuf->m_dwContentEnd = m_dwCurWritePos;
	return nSize;
}

// д��,����д����ֽ�����-1����
int CCPTSingleWrite::Write(DWORD dwPos,const void * pData,int nSize)
{
	m_dwCurWritePos = dwPos;
	return Write(pData,nSize);
}

// �������ڴ������д�뵽�ļ���
BOOL CCPTSingleWrite::SaveMemToDiskFile(LPCSTR szFileName)
{
	ACE_ASSERT(m_bCPTMem);
	ACE_ASSERT(m_bInit);
	if(!m_bCPTMem || !m_bInit)
		return FALSE;

	if(!GetTotalFrames())
		return FALSE;
	
	if(!m_bFileEnd)
	{
		if(!EndFile())
			return FALSE;
	}

	ACE_ASSERT(m_pShareOpBuf->m_dwContentEnd < _MAX_LONG);
	if(m_pShareOpBuf->m_dwContentEnd >= _MAX_LONG)
		return FALSE;

	// ���ļ�
	FILE * fout = fopen(szFileName,"wb");
	if(fout == NULL)
		return FALSE;

	// ��ȡ�ڴ�
	BYTE * pByte = m_pWriteBuf->GetMemory(0,(int)m_pShareOpBuf->m_dwContentEnd);

	// д������
	size_t nRet = fwrite(pByte,1,(int)m_pShareOpBuf->m_dwContentEnd,fout);

	if(nRet != (size_t)m_pShareOpBuf->m_dwContentEnd)
	{
		fclose(fout);
		ACE_OS::unlink(szFileName);
		return FALSE;
	}

	fclose(fout);

	return TRUE;
}


