#ifndef  MEMMGR_H_2006_05_20
#define  MEMMGR_H_2006_05_20
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"

namespace CPF
{
//多线程安全
CPF_CLASS BYTE *MyMemAlloc(UINT bytes);
CPF_CLASS BYTE *MyMemAlloc(UINT bytes,const char * filename,unsigned int lineno);
CPF_CLASS BYTE *MyMemReAlloc(void* block,UINT bytes);
CPF_CLASS BYTE *MyMemReAlloc(void* block,UINT bytes,const char * filename,unsigned int lineno);
CPF_CLASS void MyMemFree(void* block);
CPF_CLASS void MyMemReserve(size_t blocksize,size_t blocknums);

template<typename DATA_TYPE>
class CPF_allocator
{
public:
	inline DATA_TYPE * allocate(size_t count,const void *pbegin)
	{
		return (DATA_TYPE *) CPF::MyMemAlloc((UINT)count);
	}
	inline DATA_TYPE * allocate(size_t count)
	{
		return allocate(count,(const void *)0);
	}

	inline void deallocate(DATA_TYPE *pdata,size_t datalen)
	{
		CPF::MyMemFree((void *)pdata);
	}

	inline void deallocate(DATA_TYPE *pdata)
	{
		deallocate(pdata,0);
	}
};

template<typename DATA_TYPE>
class CPF_Null_allocator
{
public:
	inline DATA_TYPE * allocate(size_t count,const void *pbegin)
	{
		return NULL;
	}
	inline DATA_TYPE * allocate(size_t count)
	{
		return NULL;
	}

	inline void deallocate(DATA_TYPE *pdata,size_t datalen)
	{
		return;
	}

	inline void deallocate(DATA_TYPE *pdata)
	{
		return;
	}
};


template<typename DATA_TYPE>
class CPF_malloc_allocator
{
public:
	inline DATA_TYPE * allocate(size_t count,const void *pbegin)
	{
		return (BYTE *)malloc(count);
	}
	inline DATA_TYPE * allocate(size_t count)
	{
		return (BYTE *)malloc(count);
	}

	inline void deallocate(DATA_TYPE *pdata,size_t datalen)
	{
		free((void *)pdata);
	}

	inline void deallocate(DATA_TYPE *pdata)
	{
		free((void *)pdata);
	}
};




}//end namespace CPF

#define CPF_MEM_ALLOC_NOTRACE(nsize)				CPF::MyMemAlloc(nsize)
#define CPF_MEM_ALLOC_TRACE(nsize)					CPF::MyMemAlloc(nsize,__FILE__,__LINE__)
#define CPF_MEM_REALLOC_NOTRACE(x,nsize)			CPF::MyMemReAlloc(x,nsize)
#define CPF_MEM_REALLOC_TRACE(x,nsize)				CPF::MyMemReAlloc(x,nsize,__FILE__,__LINE__)
#define CPF_MEM_FREE(x)								CPF::MyMemFree(x)

//用于一个类的重载new和delete
#define CLASS_CPF_MEM_NOTRACE(T) \
	public :\
	void* operator new(size_t sz){ return CPF::MyMemAlloc(sizeof(T));} \
	void  operator delete(void * p){CPF::MyMemFree(p);}	


#define CLASS_CPF_MEM_TRACE(T)	\
	public :					\
	void* operator new(size_t sz)												\
	{		return CPF::MyMemAlloc((UINT)sz);	}										\
	void* operator new(size_t sz,const char * filename,unsigned int lineno)		\
	{		return CPF::MyMemAlloc((UINT)sz, filename, lineno);	}	\
	void *operator new[](size_t count)											\
	{		return CPF::MyMemAlloc((UINT)count);	}									\
	void* operator new[](size_t count,const char * filename,unsigned int lineno)	\
	{		return CPF::MyMemAlloc((UINT)count, filename, lineno);	}				\
	void  operator delete[](void * p)											\
	{		CPF::MyMemFree(p);	}												\
	void  operator delete(void * p){CPF::MyMemFree(p);}	


#define _ENABLE_CPF_MEM_TRACE_


#ifdef _ENABLE_CPF_MEM_TRACE_

#define CPF_MEM_ALLOC(nsize)					CPF_MEM_ALLOC_TRACE(nsize)
#define CPF_MEM_REALLOC(x,nsize)				CPF_MEM_REALLOC_TRACE(x,nsize)
#define CLASS_CPF_MEM(T)						CLASS_CPF_MEM_TRACE(T)

#else//_ENABLE_CPF_MEM_TRACE_


#define CPF_MEM_ALLOC(nsize)					CPF_MEM_ALLOC_NOTRACE(nsize)
#define CPF_MEM_REALLOC(x,nsize)				CPF_MEM_REALLOC_NOTRACE(x,nsize)
//#define CLASS_CPF_MEM(T)						CLASS_CPF_MEM_NOTRACE(T)
#define CLASS_CPF_MEM(T)						CLASS_CPF_MEM_TRACE(T)

#endif//_ENABLE_CPF_MEM_TRACE_







//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif // MEMMGR_H_2006_05_20


