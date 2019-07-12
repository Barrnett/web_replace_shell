// CPTSingleRead.cpp: implementation of the CCPTSingleRead class.
//
//////////////////////////////////////////////////////////////////////

#include "CPTSingleRead.h"
#include "ShareMem.h"
#include "MapViewAccess.h"
#include "InnerCommon.h"
#include "cpf/ostypedef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#if !defined(OS_WINDOWS)

DWORD GetFileSize(
	int hFile,
	DWORD* lpFileSizeHigh)
{
	struct stat64 file_info;
	fstat64(hFile, &file_info);
	if( lpFileSizeHigh )
		*lpFileSizeHigh = HILONG(file_info.st_size);

	return LOLONG(file_info.st_size);
}
#endif

CCPTSingleRead::CCPTSingleRead()
{
	m_bInit				= FALSE;
	m_pShareOpBuf		= NULL;
	m_dwCurReadPos		= 0;
	m_hFile				= (FILE_ID)-1;
	m_hMap				= NULL;
	m_bCPTMem			= FALSE;
	m_pShareMem			= new CShareMem;
	m_pReadBuf			= new CMapViewAccess;
	m_dwCurReadIndex	= 0;

	m_dwOpen = 0;
	m_dwFileSize = 0;
}

CCPTSingleRead::~CCPTSingleRead()
{
	CloseAll();
	delete m_pShareMem;
	delete m_pReadBuf;
}

BOOL CCPTSingleRead::Init(LPCSTR szFileName,DWORD dwBufSize) //szFileNameΪ�ļ��ľ���·��
{
	ACE_ASSERT(m_bInit == FALSE);

	CStr strPathName;
	CStr strShareName;

	if(szFileName != NULL)	// ���ļ��ж�ȡ
	{
		// ���ȼ���ļ��Ƿ����
		FILE * fin = fopen(szFileName,"rb");
		if(fin == NULL)
		{
			return FALSE;
		}
		fclose(fin);
		
		// Ҫ��Ϊ����·��
		strPathName = szFileName;
		if(!TransToFullPath(strPathName))
		{
			ACE_ASSERT(0);
			return FALSE;
		}

		CPTMakeShareMemName(strPathName,strShareName);
	}
	else // ���ڴ��ж�ȡ
	{
		strPathName = "IN_MEMORY_CPT";
		strShareName.Format("CPTOP_IN_MEMORY_CPT%u",GetCurrentProcessId());
		m_bCPTMem = TRUE;
	}
	
	
	DWORD dwFileSize = 0;

	// ���������ڴ�
	BOOL bCreate = FALSE;
	if(!m_pShareMem->Init(strShareName,sizeof(CPT_OP_STRUCT),bCreate))
	{
		return FALSE;
	}

	m_pShareOpBuf = (CPT_OP_STRUCT*)m_pShareMem->GetMem();
	if(m_pShareOpBuf == NULL)
	{
		CloseAll();
		return FALSE;
	}
	else
	{
		if(bCreate)		// ��һ�δ�
		{
			if(m_bCPTMem)	// ���ڴ��д�
			{
				return FALSE;	// ����Ӧ��д������ˡ�
			}
		}
		else // ���������Ѿ����ļ�
			dwFileSize = m_pShareOpBuf->m_dwMapSize;
	}
	

	// ����ӳ��ʹ򿪶�����,���Ҫ���ļ��������ļ���С
	if(!CreateMapFile(bCreate,strPathName,&dwFileSize,dwBufSize))
	{
		CloseAll();
		return FALSE;
	}
	
	m_dwFileSize = dwFileSize;
	
	// ��һ�δ�,��ʼ�������ڴ�
	if(bCreate)	
	{
		if(!InitShareMem(m_pShareOpBuf,strPathName,dwFileSize))
		{
			CloseAll();
			return FALSE;
		}
	}

	// ����ָ�����õ���һ����λ��
	SetReadPos(sizeof(CAPFILE_HEAD));
	
	m_bInit = TRUE;
	return TRUE;
}

void CCPTSingleRead::CloseAll()
{
	m_dwCurReadPos		= 0;
	m_dwCurReadIndex	= 0;

	m_pReadBuf->Close();

	if(m_hMap != NULL)
	{
		CLOSE_MAP(m_hMap);
		m_hMap = NULL;
	}
	
	if(m_hFile != ZCTT_INVALID_HANDLE)
	{
		CLOSEFILE(m_hFile);
		m_dwOpen --;
		
		m_hFile = ZCTT_INVALID_HANDLE;
	}
	
	if(m_bInit && !m_bCPTMem && m_pShareOpBuf!=NULL 
		&& m_pShareOpBuf->m_bWriting) // �����д���󴴽��Ĺ���,���Ҳ������ڴ���
	{
		// ����Լ����һ��ʹ�ø��ļ��Ķ���
		if(m_pShareMem->GetRefCount() == 1) 
		{
#ifdef OS_WINDOWS
			FILE_ID hFile = CreateFile(m_pShareOpBuf->m_strFilePathName,
				GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,NULL);
#elif defined(OS_LINUX)
			FILE_ID hFile = open(m_pShareOpBuf->m_strFilePathName, O_WRONLY, 0660);
#endif
				
			if(hFile == ZCTT_INVALID_HANDLE)
			{
				//ACE_ASSERT(0);
			}
			else
			{
				m_dwOpen ++;
				
				ftruncate64(hFile,m_pShareOpBuf->m_dwContentEnd);
                		CLOSEFILE(hFile);
				m_dwOpen --;
			}
		}
	}
	
	m_pShareMem->Close();
	m_pShareOpBuf = NULL;
	
	m_bCPTMem = FALSE;

	m_bInit = FALSE;
	ACE_ASSERT(m_dwOpen == 0);
}


BOOL CCPTSingleRead::InitShareMem(CPT_OP_STRUCT * pOPStruct,LPCSTR szFileName,DWORD dwFileSize)
{
	memset(pOPStruct,0,sizeof(CPT_OP_STRUCT));
	
	pOPStruct->m_bWriting = TRUE;
	pOPStruct->m_dwMapSize = dwFileSize;
	pOPStruct->m_dwContentEnd = dwFileSize;
	strcpy(pOPStruct->m_strFilePathName,szFileName);
	
	// ���벶���ļ�ͷ
	int nRet = Read(0,&pOPStruct->m_filehead,sizeof(CAPFILE_HEAD));
	if(nRet == -1)
		return FALSE;

	// ����ļ�ͷУ���
	if(pOPStruct->m_filehead.dwHeadCheckSum != 0xFFFF)
	{
		WORD wSum = CPTFileHeadComputeCheckSum((BYTE *)&pOPStruct->m_filehead,sizeof(pOPStruct->m_filehead));
		if(wSum != 0x0000)
			return FALSE;
	}
	
	// ��ʼ��OPStruct��������
	pOPStruct->m_dwPacketCount = pOPStruct->m_filehead.dwPacketCount;
	pOPStruct->m_bWriting = FALSE;


	// ����������
	pOPStruct->m_dwIndexCount = pOPStruct->m_filehead.dwIndexCount;
	if(pOPStruct->m_dwIndexCount == 0)
		return FALSE;
	ACE_ASSERT(pOPStruct->m_filehead.dwIndexPosInFile !=0);
	if(pOPStruct->m_filehead.dwIndexPosInFile == 0)
		return FALSE;
	
	nRet = Read(pOPStruct->m_filehead.dwIndexPosInFile,
		pOPStruct->m_adwPktIndex,
		pOPStruct->m_filehead.dwIndexCount * sizeof(DWORD));
	if(nRet == -1)
		return FALSE;

	return TRUE;
}

// ͨ��֡�������ڲ���ȡָ�룬���ú󣬿�����ʹ��GetNextPacket��ȡ��������֡����GetPrevPacket
// �ú���Ҫ��BuildIndex�Ѿ�����
BOOL CCPTSingleRead::SetToIndexPacket(DWORD dwIndexFrame)
{
	if(!m_bInit)
		return FALSE;

	if(m_pShareOpBuf->m_dwIndexCount == 0)
	{
		ACE_ASSERT(0);
		return FALSE;
	}
	
	if(dwIndexFrame >= m_pShareOpBuf->m_dwPacketCount)
	{
		ACE_ASSERT(0);
		return FALSE;
	}
	

	
	// �����±�
	DWORD dwInnerIndexSub = dwIndexFrame/m_pShareOpBuf->m_filehead.dwIndexStep;
	
	if(dwInnerIndexSub > m_pShareOpBuf->m_dwIndexCount-1)
	{
		ACE_ASSERT(0);
		return FALSE;
	}
	
	DWORD dwFrameSkip = dwIndexFrame - 
		(dwInnerIndexSub)*m_pShareOpBuf->m_filehead.dwIndexStep;
	
	ACE_ASSERT(dwFrameSkip>=0);

	// ���ٶ�ȡ,��������λ�ö�ȡ�����İ��໹�Ǵӵ�ǰλ�ö�ȡ�����İ���
	int nSkip1 = dwIndexFrame - m_dwCurReadIndex ; 

	if(IsValidCurIndex() && abs(nSkip1) < (int) dwFrameSkip) // �ӵ�ǰλ�ö���
	{
		// nSkip�п���������������ľ���ֵԶ����1000
		if(nSkip1 > 0)	// �����
		{
			for(int i = 0; i < nSkip1; i++)
			{
				PACKET_CONTEXT context;
				int nRet = GetNextPacket(context);
				if(nRet != +1)
					return FALSE;
			}
		}
		else	// ��ǰ��
		{
			for(int i = 0; i < (-nSkip1); i++)
			{
				PACKET_CONTEXT context;
				int nRet = GetPrevPacket(context);
				if(nRet != +1)
					return FALSE;
			}
		}

	}
	else // ������λ�ö���
	{
		// ���ö�ȡλ��
		ACE_ASSERT(m_pShareOpBuf->m_adwPktIndex[dwInnerIndexSub]<m_pShareOpBuf->m_dwContentEnd);
		SetReadPos(m_pShareOpBuf->m_adwPktIndex[dwInnerIndexSub]);
		// ����һ����Ŀ��֡
		for(DWORD dw=0; dw<dwFrameSkip; dw++)
		{
			PACKET_CONTEXT context;
			int nRet = GetNextPacket(context);
			if(nRet != +1)
				return FALSE;
		}
	}

		

	m_dwCurReadIndex = dwIndexFrame;
	return TRUE;
}

// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
int CCPTSingleRead::GetNextPacket(PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
	{
		return -1;
	}

	// ���濪ʼ�����λ��
	DWORD dwPosBeforeRead = GetReadPos();

	if(dwPosBeforeRead<sizeof(CAPFILE_HEAD))
	{
		return -1;
	}

	int nError = 0;

	do{
		const void * pbuf = NULL;

		//��ȡ������
		pbuf = Get(2) ;

		if(pbuf == NULL)
		{
			nError = 1;
			break;
		}
		
		USHORT uLen = *((USHORT *)pbuf);

		if(CAPFILE_FRAME_END_FLAG == uLen)	// ���ֽ�����־
		{
			DWORD dwCurPos = GetReadPos();
			char * pEndStr = (char *)Get(CPTFILE_FRAME_END_STRING_LEN);
			if(0==strcmp(pEndStr,CPTFILE_FRAME_END_STRING)) //����
			{
				nError = 2;
				break;
			}
			else 
			{
				SetReadPos(dwCurPos);
			}
		}

		//ACE_ASSERT(uLen<1024*10);	//��ʱ�����˶��ԣ����ִ�����ݰ�Ӧ���Ժ����ȥ��

		//ʵ�ʴ洢Ϊ������4�ֽڶ���,���Ѿ���������2�ֽڣ��ο���֡�ṹ�ı��ĵ���
		int nReadLen = (((DWORD)uLen-1)/4+1)*4 +2 ;	
		pbuf = Get(nReadLen); 

		if(pbuf==NULL)
		{
			nError = 3;
			break;
		}

		memcpy(&m_headInfo,(HEADER_INFO*)pbuf,sizeof(m_headInfo));

		theContext.pHeaderInfo = &m_headInfo;

		if(theContext.pHeaderInfo->btHeaderLength != sizeof(HEADER_INFO)) //�ļ�������
		{
			ACE_ASSERT(false);
			nError = 4;
			break;
		}

		ACE_ASSERT(theContext.pHeaderInfo->btHeaderLength<uLen);

		theContext.dwLengthPacket = uLen-theContext.pHeaderInfo->btHeaderLength-2;

		theContext.pPacket = (BYTE*)((BYTE *)pbuf+theContext.pHeaderInfo->btHeaderLength);

		m_dwCurReadIndex ++;

		return +1;

	}while(0);

	ACE_ASSERT(nError != 0);

	// �����ָ���ָ��
	SetReadPos(dwPosBeforeRead);

	return -1;
}

// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
int CCPTSingleRead::GetPrevPacket(PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
		return -1;
	
	DWORD dwCurPos = GetReadPos();
	if(dwCurPos < sizeof(CAPFILE_HEAD))
		return -1;

	USHORT uLen;

	// ����ǰһ������λ��
	DWORD dwPrevPos = 0;
	if(dwCurPos > sizeof(CAPFILE_HEAD))
	{
		void * pBuf = GetPrev(2);
		uLen = *(USHORT *)pBuf;
		int nSkipLen = (((DWORD)uLen-1)/4+1)*4 +2 ;	
		dwPrevPos = dwCurPos-nSkipLen-2;
		if(dwPrevPos < sizeof(CAPFILE_HEAD))
		{
			ACE_ASSERT(0);
			return -1;
		}
	}

	// ����
	SetReadPos(dwCurPos);
	
	//����ԭ����m_dwCurReadIndex, GetNextPacket���޸���
	DWORD dwOldCurReadIndex = m_dwCurReadIndex;

	int nRet = GetNextPacket(theContext);
	if(nRet == +1)	// ����
	{
		m_dwCurReadIndex = dwOldCurReadIndex-1;
		SetReadPos(dwPrevPos);
	}
	else			// ����
	{
		m_dwCurReadIndex = dwOldCurReadIndex;
		SetReadPos(dwCurPos);
	}
	
	return nRet;
}

// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
int CCPTSingleRead::GetPacketFromIndex(DWORD dwIndexFrame,PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
		return -1;
	
	if(dwIndexFrame >= m_pShareOpBuf->m_dwPacketCount)
		return -1;

	// ���ٰ���ȡ

	if(m_dwCurReadIndex==dwIndexFrame) // ���������
		return GetNextPacket(theContext);
	
	if(m_dwCurReadIndex==dwIndexFrame+1) // ��ǰһ����
	{
		if(+1 == GetPrevPacket(theContext))	// ��һ�ζ�����ǰ��
		{
			return GetPrevPacket(theContext); // ����ǰһ����
		}
	}

	if(!SetToIndexPacket(dwIndexFrame))	
		return -1;

	return GetNextPacket(theContext);

}

// ��ȡ����Ϣ�����ݲ���ֱ��ָ�����ݰ����ֵĻ�����
// ��ȡ���ķ���ֵ����1����û�����ݣ���0���ð���Ч���ɼ���ȡ������1���ð���Ч��
int CCPTSingleRead::GetPacketFromPos(DWORD dwPos,PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
		return -1;
	
	if(dwPos < sizeof(CAPFILE_HEAD) || dwPos >= m_pShareOpBuf->m_dwContentEnd)
		return -1;
	
	// ���濪ʼ�����λ��
	DWORD dwPosBeforeRead = GetReadPos();
	
	SetReadPos(dwPos);

	int nError = 0;

	do
	{
		//��ȡ������
		const void * pbuf = Get(2) ;
		
		if(pbuf == NULL)
		{
			nError = 1;
			break;
		}
		
		USHORT uLen = *((USHORT *)pbuf);
		
		if(CAPFILE_FRAME_END_FLAG == uLen)	// ���ֽ�����־
		{
			DWORD dwCurPos = GetReadPos();
			char * pEndStr = (char *)Get(CPTFILE_FRAME_END_STRING_LEN);
			if(0==strcmp(pEndStr,CPTFILE_FRAME_END_STRING)) //����
			{
				nError = 2;
				break;
			}
			else
			{
				SetReadPos(dwCurPos);
			}
		}
		
		ACE_ASSERT(uLen<1024*10);	//��ʱ�����˶��ԣ����ִ�����ݰ�Ӧ���Ժ����ȥ��
		
		//ʵ�ʴ洢Ϊ������4�ֽڶ���,���Ѿ���������2�ֽڣ��ο���֡�ṹ�ı��ĵ���
		int nReadLen = (((DWORD)uLen-1)/4+1)*4 +2 ;	
		pbuf = Get(nReadLen); 
		
		if(pbuf==NULL)
		{
			nError = 3;
			break;
		}
		
		memcpy(&m_headInfo,(HEADER_INFO*)pbuf,sizeof(m_headInfo));
		
		theContext.pHeaderInfo = &m_headInfo;
		
		if(theContext.pHeaderInfo->btHeaderLength != sizeof(HEADER_INFO)) //�ļ�������
		{
			ACE_ASSERT(false);
			nError = 4;
			break;
		}
		
		ACE_ASSERT(theContext.pHeaderInfo->btHeaderLength<uLen);
		
		theContext.dwLengthPacket = uLen-theContext.pHeaderInfo->btHeaderLength-2;
		
		theContext.pPacket = (BYTE*)((BYTE *)pbuf+theContext.pHeaderInfo->btHeaderLength);

		// �ָ���ָ��,��Ӱ����������֡������������״̬
		SetReadPos(dwPosBeforeRead);

		return +1;

	}while(0);
	
	ACE_ASSERT(nError != 0);

	// �����ָ���ָ��
	SetReadPos(dwPosBeforeRead);

	return -1;

}

// �����ļ���Map���󣬲���ʼ��������
BOOL CCPTSingleRead::CreateMapFile(BOOL bCreateFile,LPCSTR szFileName,DWORD * pdwFileSize,DWORD dwBufSize)
{	
	if(bCreateFile)
	{
#ifdef OS_WINDOWS
		// �����ļ�
		m_hFile = CreateFile (szFileName,
			GENERIC_READ ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
#elif defined(OS_LINUX)
		m_hFile = open(szFileName, O_RDONLY, 0660);
#endif
		
		if(m_hFile == ZCTT_INVALID_HANDLE)
		{
			return FALSE;
		}

		m_dwOpen ++;
		// ��ȡ�ļ���С
		DWORD dwHiOrderSize = 0;
		*pdwFileSize = GetFileSize(m_hFile,&dwHiOrderSize);
		if(dwHiOrderSize != 0)	// �ļ����� 4G
		{
			CLOSEFILE(m_hFile);
			m_dwOpen --;
			
			return FALSE;
		}
	}
	
	// ���������ļ�ӳ��
	CStr strMapHandleName ;
	if(m_bCPTMem)
		strMapHandleName.Format("CPTMH_IN_MEMORY_CPT%u",GetCurrentProcessId());
	else
		CPTMakeMapHandleName(szFileName,strMapHandleName);

	if(bCreateFile) // ��һ�δ��ļ�,���ļ����
	{
		ACE_ASSERT(!m_bCPTMem);
		
#if defined(OS_WINDOWS)
			m_hMap = CreateFileMapping(m_hFile,NULL,
				PAGE_READONLY,0,*pdwFileSize,strMapHandleName);
#elif defined(OS_LINUX)
			ftruncate64(m_hFile,(LONGLONG)(*pdwFileSize));
			m_hMap = HANDLE(m_hFile);
#endif
	}
	else	
	{
#if defined(OS_WINDOWS)
		// ���������Ѿ���
		m_hMap = OpenFileMapping(FILE_MAP_READ,FALSE,strMapHandleName);
#else
		m_hFile = open(szFileName, O_RDONLY, 0660);		
		m_hMap = HANDLE(m_hFile);		
#endif	
	}
	
	if(m_hMap == NULL)
	{		
		CLOSEFILE(m_hFile);
		m_dwOpen --;
		
		m_hFile = ZCTT_INVALID_HANDLE;

		return FALSE;
	}
	
	// ���ö�����

	if(m_bCPTMem)
		dwBufSize = *pdwFileSize;

	m_pReadBuf->SetMaxBufferSize(dwBufSize);
	if(!m_pReadBuf->SetMapHandle(m_hMap,*pdwFileSize,FILE_MAP_READ))
	{
		CLOSE_MAP(m_hMap);
		CLOSEFILE(m_hFile);
		m_dwOpen --;
		
		return FALSE;
	}

	
	return TRUE;
}


// ����,���ض�����ֽ�����-1����
int CCPTSingleRead::Read(void * pData,int nSize)
{
	if(nSize <0)
		return -1;
	
	if(m_dwCurReadPos+nSize>m_pShareOpBuf->m_dwContentEnd)
		return 0;
	
	void * pMem = m_pReadBuf->GetMemory(m_dwCurReadPos,nSize);
	if(pMem == NULL)
		return -1;
	
	memcpy(pData,pMem,nSize);
	
	m_dwCurReadPos += nSize;
	return nSize;
}

// ����,���ض�����ֽ�����-1����
int CCPTSingleRead::Read(DWORD dwPos,void * pData,int nSize)
{
	m_dwCurReadPos = dwPos;
	return Read(pData,nSize);
}

// ���룬����ָ�����ݵ�ָ��
void * CCPTSingleRead::Get(int nSize)
{
	if(nSize <0)
		return NULL;
	
	if(m_dwCurReadPos+nSize>m_pShareOpBuf->m_dwContentEnd)
		return NULL;

	void * pMem = m_pReadBuf->GetMemory(m_dwCurReadPos,nSize);
	if(pMem == NULL)
	{
		return NULL;
	}

	m_dwCurReadPos += nSize;

	return pMem;
}

// ����ǰ����ֽڣ�����ָ�����ݵ�ָ��
void * CCPTSingleRead::GetPrev(int nSize)
{
	if(nSize <0)
		return NULL;
	
	void * pMem = m_pReadBuf->GetMemory(m_dwCurReadPos-nSize,nSize);
	if(pMem == NULL)
		return NULL;
	
	m_dwCurReadPos -= nSize;
	
	return pMem;
}

// ��ȡ��������λ��,ֻ��û��д���ļ�ʱ�ſ��Ե���
DWORD CCPTSingleRead::GetPktEndPos()
{
	ACE_ASSERT(!m_pShareOpBuf->m_bWriting);
	if(m_pShareOpBuf->m_bWriting)
		return _MAX_DWORD;
	return m_pShareOpBuf->m_filehead.dwIndexPosInFile;
}




