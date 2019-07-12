// CPTSingleWrite.h: interface for the CCPTSingleWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPTSINGLEWRITE_H__4F620C2F_AEE6_4762_A12D_60D550F0A748__INCLUDED_)
#define AFX_CPTSINGLEWRITE_H__4F620C2F_AEE6_4762_A12D_60D550F0A748__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*******************************************************************************
  CPTSingleWrite.h

	创 建 者:	孙向光
	创建时间:	2003/05/22 
	内容描述：  单个CPT文件读写类：
				
				CPT文件最大为 4GB-CPTFILE_MIN_FREE
				支持读和写接口，可以在进程间与CCPTSingleRead类共享同一个CPT文件.

	修改记录：
	修改时间	    修改者		修改内容

*********************************************************************************/


#include "CPTOPSTRUCT.h"
#include "CPTSingleRead.h"


class CShareMem;
class CMapViewAccess;

class CCPTSingleWrite  
{
public:
	CCPTSingleWrite();
	virtual ~CCPTSingleWrite();

	// 初始化，要求szFileName中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
	BOOL Init(LPCSTR szFileName,									// 文件名
		DWORD dwMaxSize								= 50*1024*1024, // 文件最大长度
		DWORD dwBufSize								= 1*1024*1024,	// 读缓冲的大小
		BOOL bSerialFile							= FALSE,		// 是否连续记录中的一个
		int nFileNo									= 0,			// 如果连续记录，文件顺序号，从0开始
		LPCSTR szPrevFileName						= NULL,			// 如果连续记录，传入上一个文件名,不带路径，将存入文件头中
		GET_NEXT_FILE_NAME_FUNC pfnGetNextFile		= NULL,			// 如果连续记录，获取下一个文件名，不含路径，最长FILENAME_LEN
		void * pParam								= NULL			// pfnGetNextFile回调的 void * 参数
		); 

	void CloseAll();
	
	// 获取文件名
	LPCSTR GetFileName();
	// 获取最大长度
	DWORD GetMaxSize(){return m_pShareOpBuf->m_dwMapSize;}

	// 获取当前写入大小
	DWORD GetWritedSize(){return m_pShareOpBuf->m_dwContentEnd;};
	
	// 获取包结束的位置,只有RecordPacket导致文件结束时可以调用
	DWORD GetPktEndPos(){return m_pShareOpBuf->m_filehead.dwIndexPosInFile;};

	// 获取当前记录的包数
	inline DWORD GetTotalFrames(){return m_pShareOpBuf->m_dwPacketCount;}

	// 记录包数据,返回 1 正常，0 文件满，－1错误,dwIndexFrame返回
	int RecordPacket(BYTE * pPacket,int nPacketLen,HEADER_INFO * pHeadInfo,
		DWORD * pdwIndexFrame = NULL, DWORD * pdwPos = NULL);
	
	// 通过帧号设置内部读取指针，调用后，可连续使用GetNextPacket获取后其他的帧或者GetPrevPacket
	inline BOOL SetToIndexPacket(DWORD dwIndexFrame){return m_ReadObj.SetToIndexPacket(dwIndexFrame);}

	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	inline int GetNextPacket(PACKET_CONTEXT& theContext){return m_ReadObj.GetNextPacket(theContext);}

	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	inline int GetPrevPacket(PACKET_CONTEXT& theContext){return m_ReadObj.GetPrevPacket(theContext);}

	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	inline int GetPacketFromIndex(DWORD dwIndexFrame,PACKET_CONTEXT& theContext){return m_ReadObj.GetPacketFromIndex(dwIndexFrame,theContext);}

	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	inline int GetPacketFromPos(DWORD dwPos,PACKET_CONTEXT& theContext){return m_ReadObj.GetPacketFromPos(dwPos,theContext);}
	
	// 将捕获到内存的内容写入到文件中
	BOOL SaveMemToDiskFile(LPCSTR szFileName);
	
protected:

	// 重写文件头,和索引信息TRUE表示成功
	BOOL EndFile();
	
	// 创建文件、Map对象，并初始化写缓冲
	BOOL CreateMapFile(BOOL bInMem,DWORD dwBufSize);
	
	// 在文件尾部写入索引,nPos为结束标志后的位置
	BOOL WriteIndexAtEnd(DWORD dwPos);

	// 写入,返回写入的字节数，-1错误
	int Write(const void * pData,int nSize);
	// 写入,返回写入的字节数，-1错误
	int Write(DWORD dwPos,const void * pData,int nSize);
	
private:
	BOOL		m_bInit;						// 是否初始化的标志
	BOOL		m_bCPTMem;						// 是否是在内存中
	BOOL		m_bFileEnd;						// 文件写入是否结束
	CPT_OP_STRUCT * m_pShareOpBuf;				// 操作数据结构

	DWORD		m_dwCurWritePos;				// 写指针
	
	FILE_ID		m_hFile;						// 文件句柄
	HANDLE		m_hMap;							// 文件内存映射句柄

	CShareMem	* m_pShareMem;						// 操作数据的共享内存
	CMapViewAccess * m_pWriteBuf;					// 写缓冲
	CCPTSingleRead m_ReadObj;					// 读对象

	GET_NEXT_FILE_NAME_FUNC m_pfnGetNextFile;	// 获取下一个文件名，结束时调用
	void *		m_pParam;						// m_pfnGetNextFile的参数

	DWORD m_dwOpen ;
};

#endif // !defined(AFX_CPTSINGLEWRITE_H__4F620C2F_AEE6_4762_A12D_60D550F0A748__INCLUDED_)
