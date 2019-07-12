/*******************************************************************************
  CapDataRead.h 

	创 建 者:	
	创建时间:	2002/8/16 
	内容描述：  捕获数据文件读取类

				该类支持连续记录的多个文件连续读取。

				提供与物理接口相类似的函数接口
				以下内容引自《接口管理部分提取数据包的接口函数》文档

				提取数据的接口函数定义
				//只提取包头信息，数据部分直接指向原来数据包部分的缓冲区
				int GetPacketHeaderInfo(PACKET_CONTEXT* pContext);

				提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）


	修改记录：  

	修改时间	2003.6.20
    修改者		
    修改内容    
                1.加入GetCurSize()函数
                2.修改GetTotalFrames函数名为GetCurTotalFrames

*********************************************************************************/


#if !defined(AFX_CAPDATAREAD3_H__D64BD075_5A76_4969_ACA3_51DA5844282C__INCLUDED_)
#define AFX_CAPDATAREAD3_H__D64BD075_5A76_4969_ACA3_51DA5844282C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cpf/Octets.h"
#include "CapFileHead.h"
#include "CPTSingleRead.h"
#include "CAPOPSTRUCT.h"


class CCapDataRead  
{
public:
	CCapDataRead();
	virtual ~CCapDataRead();

	LPCSTR GetOrgFile(){return m_strOrgFileName.GetString();}
	LPCSTR GetCurFile();

    // 初始化，要求szFileName中不能包括 . .. 之类的相对路径，可以直接指定 abc\\def, 不能使用 / 作为路径。
    BOOL Init(LPCSTR szFileName,DWORD nReadNums,DWORD dwBufSize = 1*1024*1024);

	void CloseAll();
	
	// 获取当前记录的包数
	ULONGLONG GetCurTotalFrames();
    // 获取当前的大小（字节）
    ULONGLONG GetCurTotalSize();
	
	// 通过帧号设置当前包和内部读取指针，调用后，可连续使用GetNextPacket获取后其他的帧或者GetPrevPacket
	BOOL SetToIndexPacket(const ULONGLONG & ullIndexFrame);
	
	// 提取当前包信息，数据部分直接指向数据包部分的缓冲区，移动内部读取指针到下一个包
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int GetNextPacket(PACKET_CONTEXT& theContext)
	{
		int nRes = InnerGetNextPacket(theContext);

		if( nRes == +1 )
		{
			m_buffer.SetData(theContext.pPacket,theContext.dwLengthPacket);
			theContext.pPacket = m_buffer.GetData();
		}

		return nRes;
		
	}
	
	// 提取当前包信息，数据部分直接指向数据包部分的缓冲区，移动内部读取指针到上一个包
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int GetPrevPacket(PACKET_CONTEXT& theContext)
	{
		int nRes = InnerGetPrevPacket(theContext);
		
		if( nRes == +1 )
		{
			m_buffer.SetData(theContext.pPacket,theContext.dwLengthPacket);
			theContext.pPacket = m_buffer.GetData();
		}
		
		return nRes;
	}
	
	// 提取包信息，数据部分直接指向数据包部分的缓冲区,移动内部读取指针到下一个包
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int GetPacketFromIndex(const ULONGLONG & ullIndexFrame,PACKET_CONTEXT& theContext)
	{
		int nRes = InnerGetPacketFromIndex(ullIndexFrame,theContext);
		
		if( nRes == +1 )
		{
			m_buffer.SetData(theContext.pPacket,theContext.dwLengthPacket);
			theContext.pPacket = m_buffer.GetData();
		}
		
		return nRes;
	}

	// 提取指定位置包信息，数据部分直接指向数据包部分的缓冲区，调用后，当前包位置发生不确定性变化。
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	// 注意，该函数调用后会使GetNextPacket和GetPrevPacket调用不确定，
	// 应该再次使用SetToIndexPacket或者GetPacketFromIndex后再调用GetNextPacket和GetPrevPacket
	int GetPacketFromPos(const ULONGLONG & ullPos,PACKET_CONTEXT& theContext);
	
public:
	LPCSTR MakeFullPathName(LPCSTR szFileName,CStr& strFullName);

protected:
	// 提取当前包信息，数据部分直接指向数据包部分的缓冲区，移动内部读取指针到下一个包
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int InnerGetNextPacket(PACKET_CONTEXT& theContext);
	
	// 提取当前包信息，数据部分直接指向数据包部分的缓冲区，移动内部读取指针到上一个包
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int InnerGetPrevPacket(PACKET_CONTEXT& theContext);
	
	// 提取包信息，数据部分直接指向数据包部分的缓冲区,移动内部读取指针到下一个包
	// 提取包的返回值：－1（已没有数据），0（该包无效，可继续取），＋1（该包有效）
	int InnerGetPacketFromIndex(const ULONGLONG & ullIndexFrame,PACKET_CONTEXT& theContext);
	
	BOOL InitShareMem(CAP_OP_STRUCT *pCapShareOpBuf);
	BOOL DetectPrevVer(LPCSTR szFileName);
protected:
	CCPFOctets	m_buffer;

	DWORD	m_nReadNums;//读几个文件，0表示读无数个
	BOOL m_bInit;

	CStr m_strOrgFileName;			// 原始的文件名字
	int		m_nCurFile;					// 当前播放的文件

	DWORD m_dwBufSize;					// 读缓冲的大小
	CCPTSingleRead m_SingleReadObj;		// 读一个文件

	CAP_OP_STRUCT * m_pCapShareOpBuf;	// 共享内存指针

	CShareMem	*m_pShareMem;				// 共享内存对象
};

#endif // !defined(AFX_CAPDATAREAD3_H__D64BD075_5A76_4969_ACA3_51DA5844282C__INCLUDED_)
