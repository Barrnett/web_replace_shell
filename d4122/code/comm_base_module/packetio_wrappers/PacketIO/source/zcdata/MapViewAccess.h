#ifndef _MAPVIEW_ACCESS_H_
#define _MAPVIEW_ACCESS_H_


#include "cpf/cpf.h"

#ifdef __cplusplus
/*
 *	
 */
class CMapViewAccess
{
public:
	CMapViewAccess();
	virtual ~CMapViewAccess();

	BOOL SetMaxBufferSize(int nMaxBufSize);
	BOOL SetMapHandle(HANDLE hMap,ULONGLONG ullMapSize,DWORD dwDesiredAccess = FILE_MAP_ALL_ACCESS);
	HANDLE GetMapHandle(){return m_hMap;}
	void Close();

	BYTE * GetMemory(ULONGLONG ullOffset,int nSize);

	//将缓存的数据提交到硬盘
	void Commit();

private:
	void CommitAndUnMap()
	{
		Commit();
		UnMap();
	}

	void UnMap();

private:
	HANDLE m_hMap;
	DWORD m_dwDesiredAccess;
	int m_nMaxBufSize;
	BYTE * m_pMapBuffer;	// 内存映射地址 = 内存虚拟基地址 按64K地址向下对齐
	ULONGLONG m_ullOffset;	// m_pMapBuffer的映射偏移位置
	ULONGLONG m_ullMapSize;	// m_hMap 的实际大小
	int m_nMemfSize;        // msync and munmap need the real mapped size
};

#endif // __cplusplus
#endif // _MAPVIEW_ACCESS_H_


