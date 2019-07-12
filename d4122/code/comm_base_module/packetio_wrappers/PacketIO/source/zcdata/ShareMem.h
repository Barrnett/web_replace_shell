#ifndef _CAP_SHAREMEM_H_
#define _CAP_SHAREMEM_H_
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"

#if defined(OS_LINUX)
#include <sys/ipc.h>
#include <sys/shm.h>
#endif



/*
 *	�����ڴ� CShareMem
 */
class CShareMem
{
public:
	CShareMem();
	~CShareMem();
	
	// ��ʼ��,bCreate�����Ƿ��´���
	BOOL Init(LPCSTR szShareName, DWORD dwLength, 
		BOOL& bCreate, DWORD dwDesiredAccess = FILE_MAP_READ|FILE_MAP_WRITE);
	void Close();

	DWORD GetMemSize() const	{return m_dwSize;}
	void* GetMem()	const		{return ((BYTE*)m_pBuffer)+sizeof(int);}
	LPCSTR GetShareName() const {return m_szSharName;}
	int GetRefCount() const{return *(int *)m_pBuffer;}

private:
	BOOL   m_bInit;
#if defined(OS_WINDOWS)
	HANDLE m_hMap;
#elif defined(OS_LINUX)
	int m_hMap;
#endif
	DWORD m_dwSize;

	// �ڴ�ӳ���ַ,����ǰsizeof(int) 4���ֽ�Ϊ���ü���,�Ժ�Ϊ�û�ʹ�õ��ڴ�
	void * m_pBuffer;		
	char * m_szSharName; 
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // _CAP_SHAREMEM_H_

