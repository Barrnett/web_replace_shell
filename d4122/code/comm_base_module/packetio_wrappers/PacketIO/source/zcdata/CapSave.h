/*******************************************************************************
  CapSave.h

	创 建 者:	孙向光
	创建时间:	2003/5/22 
	内容描述：  连续多个CPT文件记录类(通过文件头中的前、后文件名连接)。
				支持在多个进程间和多个读CCapDataRead类对同一系列CPT文件的读。

	修改记录：
	修改时间	    修改者		修改内容

*********************************************************************************/

#if !defined(AFX_CAPSAVE_H__6CC18264_0FC4_4C4F_97ED_5D5609846165__INCLUDED_)
#define AFX_CAPSAVE_H__6CC18264_0FC4_4C4F_97ED_5D5609846165__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CapFileHead.h"
#include "CPTSingleWrite.h"
#include "CapDataRead.h"
#include "CAPOPSTRUCT.h"
#include "ShareMem.h"

class CCapSave  
{
public:
	CCapSave();
	virtual ~CCapSave();
	
	// 初始化，要求szFileName中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
	BOOL Init(LPCSTR szFileName,
		DWORD dwMaxFileLengthMb					= 50,
		DWORD dwBufSize							= 1*1024*1024,	// 缓冲大小
		int nMultiFiles							= 1,
		unsigned int ncaptime					= 0,//捕获多长时间，０表示不限制
		GET_NEXT_FILE_NAME_FUNC pfnGetNextFile	= NULL,			// 如果连续记录，获取下一个文件名，不含路径，最长FILENAME_LEN
		void * pParam							= NULL			// pfnGetNextFile回调的 void * 参数
		);

	void CloseAll();

	// 获取当前记录的包数
	inline ULONGLONG GetTotalFrames(){return m_pCapShareOpBuf->m_ullTotalFrame;}
	// 获取当前使用的空间数
	ULONGLONG GetTotalUsage();
	
	// 获取初始化文件名
	LPCSTR GetInitFileName();
	// 获取正在记录的当前文件名
	LPCSTR GetCurrentFileName();

	// 获取记录方式
	inline BOOL GetMultiFiles() const {return m_nMultiFiles;}


	/*
	*	记录包数据,0表示正常,-1表示设备出错，-2表示空间磁盘满，-3表示文件写完成了。
	成功时的ullPos，可用于GetPacket,但不可用于SetToIndexPacket	
	*/
	int RecordPacket(BYTE * pPacket,int nPacketLen,HEADER_INFO * pHeadInfo,
		ULONGLONG * pullIndexFrame = NULL, ULONGLONG * pullPos = NULL);
	
	// 通过帧号设置内部读取指针，调用后，可连续使用GetNextPacket获取后其他的帧或者GetPrevPacket
	// 该函数要求BuildIndex已经调用
	inline BOOL SetToIndexPacket(const ULONGLONG & ullIndexFrame){return m_ReadObj.SetToIndexPacket(ullIndexFrame);}
	
	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	inline int GetNextPacket(PACKET_CONTEXT& theContext){return m_ReadObj.GetNextPacket(theContext);}
	
	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	inline int GetPrevPacket(PACKET_CONTEXT& theContext){return m_ReadObj.GetPrevPacket(theContext);}
	
	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	inline int GetPacketFromIndex(const ULONGLONG & ullIndexFrame,PACKET_CONTEXT& theContext){return m_ReadObj.GetPacketFromIndex(ullIndexFrame,theContext);}

	// 提取包信息，数据部分直接指向数据包部分的缓冲区
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	inline int GetPacketFromPos(const ULONGLONG & ullPos,PACKET_CONTEXT& theContext){return m_ReadObj.GetPacketFromPos(ullPos,theContext);}
	
protected:
	static 	LPCSTR GetNextFileName(void * pParam);
	LPCSTR MakeFullPathName(LPCSTR szFileName,CStr& strFullName);

	//0表示正常,-1表示设备出错，-2表示空间磁盘满，-3表示文件写完成了。
	int CreateNextFile();
protected:
	BOOL		m_bInit;
	CStr		m_strInitFileName;		// 初始存盘文件名,全路径
	CStr		m_strPrevFileName;		// 前一个文件名，无路径
	CStr		m_strNextFileName;		// 下一个文件名,无路径
	
	int			m_nMultiFiles;;			// 是否记录一系列的文件

	ACE_Time_Value	m_start_captime;
	unsigned int	m_ncaptime;			//从打开文件开始捕获多长时间，单位为秒

	DWORD		m_dwBufSize;			// 缓冲大小

	DWORD		m_dwMaxSize;		    // 每个文件最大值

	GET_NEXT_FILE_NAME_FUNC m_pfnGetNextFile;
	void *		m_pParam;				

	CShareMem	*m_pShareMem;					// 操作数据的共享内存
	CAP_OP_STRUCT * m_pCapShareOpBuf;			// 共享内存指针
	

	CCPTSingleWrite m_SingleWriteObj;	// 写入的对象
	CCapDataRead m_ReadObj;
};

#endif // !defined(AFX_CAPSAVE_H__6CC18264_0FC4_4C4F_97ED_5D5609846165__INCLUDED_)
