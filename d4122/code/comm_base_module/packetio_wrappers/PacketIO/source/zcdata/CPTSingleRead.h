// CPTSingleRead.h: interface for the CCPTSingleRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPTSINGLEREAD_H__6A7EADF8_46DD_4B55_AB55_8FD44D5A3B26__INCLUDED_)
#define AFX_CPTSINGLEREAD_H__6A7EADF8_46DD_4B55_AB55_8FD44D5A3B26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*******************************************************************************
  CPTSingleRead.h

	创 建 者:	孙向光
	创建时间:	2003/05/22 
	内容描述：  单个CPT文件读类：
				
				CPT文件最大为 4GB-CPTFILE_MIN_FREE
				可以在进程间与CCPTSingleWrite类共享同一个CPT文件.

	修改记录：
	修改时间	    修改者		修改内容

*********************************************************************************/

#include "CPTOPSTRUCT.h"

class CShareMem;
class CMapViewAccess;

class CCPTSingleRead  
{
public:
	CCPTSingleRead();
	virtual ~CCPTSingleRead();


	// 初始化，要求szFileName中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
	BOOL Init(LPCSTR szFileName,DWORD dwBufSize = 1*1024*1024); //szFileName为文件的绝对路径

	void CloseAll();
	
	// 判断是否是一个正在写的文件
	BOOL IsWritingFile(){return m_pShareOpBuf->m_bWriting;}
	// 判断是否是连续文件中的一个
	BOOL IsSerialFile(){return m_pShareOpBuf->m_filehead.bSerialFiles;}
	// 获取下一个文件名
	LPCSTR GetNextFileName(){return m_pShareOpBuf->m_filehead.NextFileName;}
	
	// 获取包结束的位置,只有没有写此文件时才可以调用,如果文件被写，返回MAXDWORD。
	DWORD GetPktEndPos();
	
	// 获取记录的包数
	inline DWORD GetTotalFrames(){return m_pShareOpBuf->m_dwPacketCount;}

    // 获取文件当前的大小
    DWORD GetCurFileSize(){return m_pShareOpBuf->m_dwContentEnd;};
    
	// 通过帧号设置内部读取指针，调用后，可连续使用GetNextPacket获取后其他的帧或者GetPrevPacket
	// 该函数要求BuildIndex已经调用
	BOOL SetToIndexPacket(DWORD dwIndexFrame);
	
	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int GetNextPacket(PACKET_CONTEXT& theContext);
	
	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int GetPrevPacket(PACKET_CONTEXT& theContext);
	
	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int GetPacketFromIndex(DWORD dwIndexFrame,PACKET_CONTEXT& theContext);

	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int GetPacketFromPos(DWORD dwPos,PACKET_CONTEXT& theContext);
	
protected:

	BOOL InitShareMem(CPT_OP_STRUCT *pOPStruct ,LPCSTR szFileName,DWORD dwFileSize);
	
	// 创建文件、Map对象，并初始化读缓冲
	BOOL CreateMapFile(BOOL bCreateFile,LPCSTR szFileName,DWORD * pdwFileSize,DWORD dwBufSize);
	
	inline void SetReadPos(DWORD dwPos){m_dwCurReadPos = dwPos;}
	inline DWORD GetReadPos(){return m_dwCurReadPos;}
	// 读入,返回读入的字节数，-1错误
	int Read(void * pData,int nSize);
	// 读入,返回读入的字节数，-1错误
	int Read(DWORD dwPos,void * pData,int nSize);
	// 读入，返回指向数据的指针
	void * Get(int nSize);
	// 读入前面的字节，返回指向数据的指针,移动当前读指针到刚读出的数据始位
	void * GetPrev(int nSize);

	BOOL IsValidCurIndex(){ return m_dwCurReadIndex>=m_pShareOpBuf->m_dwIndexCount?FALSE:TRUE;}
	
private:
	BOOL		m_bInit;						// 是否初始化的标志
	BOOL		m_bCPTMem;						// 是否在内存中读取
	CPT_OP_STRUCT * m_pShareOpBuf;				// 操作数据结构
	
	DWORD		m_dwCurReadPos;					// 读指针
	DWORD		m_dwCurReadIndex;				// 读包序号指针
	
	FILE_ID			m_hFile;						// 文件句柄
	ACE_HANDLE		m_hMap;							// 文件内存映射句柄
	
	CShareMem	*m_pShareMem;					// 操作数据的共享内存
	CMapViewAccess *m_pReadBuf;					// 读缓冲

	DWORD		m_dwOpen;//测试
	DWORD		m_dwFileSize;
	HEADER_INFO m_headInfo;
	
};

#endif // !defined(AFX_CPTSINGLEREAD_H__6A7EADF8_46DD_4B55_AB55_8FD44D5A3B26__INCLUDED_)
