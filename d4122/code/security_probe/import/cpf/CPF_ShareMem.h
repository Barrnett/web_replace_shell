//////////////////////////////////////////////////////////////////////////
//CPF_ShareMem.h

//封装windows和linux的共享内存

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CPF_ShareMem
{
public:
	CPF_ShareMem(void);
	virtual ~CPF_ShareMem(void);

public:
	void * init(const char * name,size_t mem_size,int& first_time);
	void * map(const char * name);
	void fin();

public:
	inline size_t GetMemSize() const
	{
		return m_memsize;
	}

	inline void * GetMemBuffer() const
	{
		return m_mem_buffer;
	}

private:
	size_t			m_memsize;

#ifdef OS_WINDOWS
	HANDLE			m_hMap;
#else
	int				m_hMap;
#endif

	void *			m_mem_buffer;
};
