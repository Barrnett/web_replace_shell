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

	//������������ύ��Ӳ��
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
	BYTE * m_pMapBuffer;	// �ڴ�ӳ���ַ = �ڴ��������ַ ��64K��ַ���¶���
	ULONGLONG m_ullOffset;	// m_pMapBuffer��ӳ��ƫ��λ��
	ULONGLONG m_ullMapSize;	// m_hMap ��ʵ�ʴ�С
	int m_nMemfSize;        // msync and munmap need the real mapped size
};

#endif // __cplusplus
#endif // _MAPVIEW_ACCESS_H_


