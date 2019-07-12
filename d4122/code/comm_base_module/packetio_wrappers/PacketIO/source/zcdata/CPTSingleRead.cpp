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

BOOL CCPTSingleRead::Init(LPCSTR szFileName,DWORD dwBufSize) //szFileName为文件的绝对路径
{
	ACE_ASSERT(m_bInit == FALSE);

	CStr strPathName;
	CStr strShareName;

	if(szFileName != NULL)	// 在文件中读取
	{
		// 首先检测文件是否存在
		FILE * fin = fopen(szFileName,"rb");
		if(fin == NULL)
		{
			return FALSE;
		}
		fclose(fin);
		
		// 要求为绝对路径
		strPathName = szFileName;
		if(!TransToFullPath(strPathName))
		{
			ACE_ASSERT(0);
			return FALSE;
		}

		CPTMakeShareMemName(strPathName,strShareName);
	}
	else // 在内存中读取
	{
		strPathName = "IN_MEMORY_CPT";
		strShareName.Format("CPTOP_IN_MEMORY_CPT%u",GetCurrentProcessId());
		m_bCPTMem = TRUE;
	}
	
	
	DWORD dwFileSize = 0;

	// 建立共享内存
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
		if(bCreate)		// 第一次打开
		{
			if(m_bCPTMem)	// 在内存中打开
			{
				return FALSE;	// 至少应该写对象打开了。
			}
		}
		else // 其他对象已经打开文件
			dwFileSize = m_pShareOpBuf->m_dwMapSize;
	}
	

	// 创建映射和打开读缓冲,如果要打开文件，返回文件大小
	if(!CreateMapFile(bCreate,strPathName,&dwFileSize,dwBufSize))
	{
		CloseAll();
		return FALSE;
	}
	
	m_dwFileSize = dwFileSize;
	
	// 第一次打开,初始化共享内存
	if(bCreate)	
	{
		if(!InitShareMem(m_pShareOpBuf,strPathName,dwFileSize))
		{
			CloseAll();
			return FALSE;
		}
	}

	// 将读指针设置到第一个包位置
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
		&& m_pShareOpBuf->m_bWriting) // 如果是写对象创建的共享,并且不是在内存中
	{
		// 如果自己最后一个使用该文件的对象
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
	
	// 读入捕获文件头
	int nRet = Read(0,&pOPStruct->m_filehead,sizeof(CAPFILE_HEAD));
	if(nRet == -1)
		return FALSE;

	// 检查文件头校验和
	if(pOPStruct->m_filehead.dwHeadCheckSum != 0xFFFF)
	{
		WORD wSum = CPTFileHeadComputeCheckSum((BYTE *)&pOPStruct->m_filehead,sizeof(pOPStruct->m_filehead));
		if(wSum != 0x0000)
			return FALSE;
	}
	
	// 初始化OPStruct其他数据
	pOPStruct->m_dwPacketCount = pOPStruct->m_filehead.dwPacketCount;
	pOPStruct->m_bWriting = FALSE;


	// 读入索引表
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

// 通过帧号设置内部读取指针，调用后，可连续使用GetNextPacket获取后其他的帧或者GetPrevPacket
// 该函数要求BuildIndex已经调用
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
	

	
	// 计算下标
	DWORD dwInnerIndexSub = dwIndexFrame/m_pShareOpBuf->m_filehead.dwIndexStep;
	
	if(dwInnerIndexSub > m_pShareOpBuf->m_dwIndexCount-1)
	{
		ACE_ASSERT(0);
		return FALSE;
	}
	
	DWORD dwFrameSkip = dwIndexFrame - 
		(dwInnerIndexSub)*m_pShareOpBuf->m_filehead.dwIndexStep;
	
	ACE_ASSERT(dwFrameSkip>=0);

	// 加速读取,看从索引位置读取跳过的包多还是从当前位置读取跳过的包多
	int nSkip1 = dwIndexFrame - m_dwCurReadIndex ; 

	if(IsValidCurIndex() && abs(nSkip1) < (int) dwFrameSkip) // 从当前位置读快
	{
		// nSkip有可能溢出，但是它的绝对值远大于1000
		if(nSkip1 > 0)	// 向后跳
		{
			for(int i = 0; i < nSkip1; i++)
			{
				PACKET_CONTEXT context;
				int nRet = GetNextPacket(context);
				if(nRet != +1)
					return FALSE;
			}
		}
		else	// 向前跳
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
	else // 从索引位置读快
	{
		// 设置读取位置
		ACE_ASSERT(m_pShareOpBuf->m_adwPktIndex[dwInnerIndexSub]<m_pShareOpBuf->m_dwContentEnd);
		SetReadPos(m_pShareOpBuf->m_adwPktIndex[dwInnerIndexSub]);
		// 跳过一定数目的帧
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

// 提取包信息，数据部分直接指向数据包部分的缓冲区
int CCPTSingleRead::GetNextPacket(PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
	{
		return -1;
	}

	// 保存开始读入的位置
	DWORD dwPosBeforeRead = GetReadPos();

	if(dwPosBeforeRead<sizeof(CAPFILE_HEAD))
	{
		return -1;
	}

	int nError = 0;

	do{
		const void * pbuf = NULL;

		//获取包长度
		pbuf = Get(2) ;

		if(pbuf == NULL)
		{
			nError = 1;
			break;
		}
		
		USHORT uLen = *((USHORT *)pbuf);

		if(CAPFILE_FRAME_END_FLAG == uLen)	// 出现结束标志
		{
			DWORD dwCurPos = GetReadPos();
			char * pEndStr = (char *)Get(CPTFILE_FRAME_END_STRING_LEN);
			if(0==strcmp(pEndStr,CPTFILE_FRAME_END_STRING)) //结束
			{
				nError = 2;
				break;
			}
			else 
			{
				SetReadPos(dwCurPos);
			}
		}

		//ACE_ASSERT(uLen<1024*10);	//暂时保留此断言，出现大的数据包应用以后可以去掉

		//实际存储为包长按4字节对齐,且已经包含对齐2字节，参考《帧结构改变文档》
		int nReadLen = (((DWORD)uLen-1)/4+1)*4 +2 ;	
		pbuf = Get(nReadLen); 

		if(pbuf==NULL)
		{
			nError = 3;
			break;
		}

		memcpy(&m_headInfo,(HEADER_INFO*)pbuf,sizeof(m_headInfo));

		theContext.pHeaderInfo = &m_headInfo;

		if(theContext.pHeaderInfo->btHeaderLength != sizeof(HEADER_INFO)) //文件有问题
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

	// 出错后恢复读指针
	SetReadPos(dwPosBeforeRead);

	return -1;
}

// 提取包信息，数据部分直接指向数据包部分的缓冲区
// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
int CCPTSingleRead::GetPrevPacket(PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
		return -1;
	
	DWORD dwCurPos = GetReadPos();
	if(dwCurPos < sizeof(CAPFILE_HEAD))
		return -1;

	USHORT uLen;

	// 计算前一个包的位置
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

	// 读包
	SetReadPos(dwCurPos);
	
	//保存原来的m_dwCurReadIndex, GetNextPacket会修改它
	DWORD dwOldCurReadIndex = m_dwCurReadIndex;

	int nRet = GetNextPacket(theContext);
	if(nRet == +1)	// 正常
	{
		m_dwCurReadIndex = dwOldCurReadIndex-1;
		SetReadPos(dwPrevPos);
	}
	else			// 出错
	{
		m_dwCurReadIndex = dwOldCurReadIndex;
		SetReadPos(dwCurPos);
	}
	
	return nRet;
}

// 提取包信息，数据部分直接指向数据包部分的缓冲区
// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
int CCPTSingleRead::GetPacketFromIndex(DWORD dwIndexFrame,PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
		return -1;
	
	if(dwIndexFrame >= m_pShareOpBuf->m_dwPacketCount)
		return -1;

	// 加速包读取

	if(m_dwCurReadIndex==dwIndexFrame) // 就是这个包
		return GetNextPacket(theContext);
	
	if(m_dwCurReadIndex==dwIndexFrame+1) // 是前一个包
	{
		if(+1 == GetPrevPacket(theContext))	// 第一次读到当前包
		{
			return GetPrevPacket(theContext); // 读到前一个包
		}
	}

	if(!SetToIndexPacket(dwIndexFrame))	
		return -1;

	return GetNextPacket(theContext);

}

// 提取包信息，数据部分直接指向数据包部分的缓冲区
// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
int CCPTSingleRead::GetPacketFromPos(DWORD dwPos,PACKET_CONTEXT& theContext)
{
	if(!m_bInit)
		return -1;
	
	if(dwPos < sizeof(CAPFILE_HEAD) || dwPos >= m_pShareOpBuf->m_dwContentEnd)
		return -1;
	
	// 保存开始读入的位置
	DWORD dwPosBeforeRead = GetReadPos();
	
	SetReadPos(dwPos);

	int nError = 0;

	do
	{
		//获取包长度
		const void * pbuf = Get(2) ;
		
		if(pbuf == NULL)
		{
			nError = 1;
			break;
		}
		
		USHORT uLen = *((USHORT *)pbuf);
		
		if(CAPFILE_FRAME_END_FLAG == uLen)	// 出现结束标志
		{
			DWORD dwCurPos = GetReadPos();
			char * pEndStr = (char *)Get(CPTFILE_FRAME_END_STRING_LEN);
			if(0==strcmp(pEndStr,CPTFILE_FRAME_END_STRING)) //结束
			{
				nError = 2;
				break;
			}
			else
			{
				SetReadPos(dwCurPos);
			}
		}
		
		ACE_ASSERT(uLen<1024*10);	//暂时保留此断言，出现大的数据包应用以后可以去掉
		
		//实际存储为包长按4字节对齐,且已经包含对齐2字节，参考《帧结构改变文档》
		int nReadLen = (((DWORD)uLen-1)/4+1)*4 +2 ;	
		pbuf = Get(nReadLen); 
		
		if(pbuf==NULL)
		{
			nError = 3;
			break;
		}
		
		memcpy(&m_headInfo,(HEADER_INFO*)pbuf,sizeof(m_headInfo));
		
		theContext.pHeaderInfo = &m_headInfo;
		
		if(theContext.pHeaderInfo->btHeaderLength != sizeof(HEADER_INFO)) //文件有问题
		{
			ACE_ASSERT(false);
			nError = 4;
			break;
		}
		
		ACE_ASSERT(theContext.pHeaderInfo->btHeaderLength<uLen);
		
		theContext.dwLengthPacket = uLen-theContext.pHeaderInfo->btHeaderLength-2;
		
		theContext.pPacket = (BYTE*)((BYTE *)pbuf+theContext.pHeaderInfo->btHeaderLength);

		// 恢复读指针,不影响其他基于帧索引读函数的状态
		SetReadPos(dwPosBeforeRead);

		return +1;

	}while(0);
	
	ACE_ASSERT(nError != 0);

	// 出错后恢复读指针
	SetReadPos(dwPosBeforeRead);

	return -1;

}

// 创建文件、Map对象，并初始化读缓冲
BOOL CCPTSingleRead::CreateMapFile(BOOL bCreateFile,LPCSTR szFileName,DWORD * pdwFileSize,DWORD dwBufSize)
{	
	if(bCreateFile)
	{
#ifdef OS_WINDOWS
		// 创建文件
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
		// 获取文件大小
		DWORD dwHiOrderSize = 0;
		*pdwFileSize = GetFileSize(m_hFile,&dwHiOrderSize);
		if(dwHiOrderSize != 0)	// 文件超过 4G
		{
			CLOSEFILE(m_hFile);
			m_dwOpen --;
			
			return FALSE;
		}
	}
	
	// 创建命名文件映射
	CStr strMapHandleName ;
	if(m_bCPTMem)
		strMapHandleName.Format("CPTMH_IN_MEMORY_CPT%u",GetCurrentProcessId());
	else
		CPTMakeMapHandleName(szFileName,strMapHandleName);

	if(bCreateFile) // 第一次打开文件,有文件句柄
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
		// 其他对象已经打开
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
	
	// 设置读缓冲

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


// 读入,返回读入的字节数，-1错误
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

// 读入,返回读入的字节数，-1错误
int CCPTSingleRead::Read(DWORD dwPos,void * pData,int nSize)
{
	m_dwCurReadPos = dwPos;
	return Read(pData,nSize);
}

// 读入，返回指向数据的指针
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

// 读入前面的字节，返回指向数据的指针
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

// 获取包结束的位置,只有没有写此文件时才可以调用
DWORD CCPTSingleRead::GetPktEndPos()
{
	ACE_ASSERT(!m_pShareOpBuf->m_bWriting);
	if(m_pShareOpBuf->m_bWriting)
		return _MAX_DWORD;
	return m_pShareOpBuf->m_filehead.dwIndexPosInFile;
}




