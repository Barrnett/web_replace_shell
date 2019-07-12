//////////////////////////////////////////////////////////////////////////
//CPF_ShareMem.cpp

#include "cpf/CPF_ShareMem.h"

#ifdef OS_LINUX
#	include "cpf/crc_tools.h"
#	include <sys/ipc.h>
#	include <sys/shm.h>
#endif

CPF_ShareMem::CPF_ShareMem(void)
{
	m_memsize = 0;

#ifdef OS_WINDOWS
	m_hMap = 0;
#else
	m_hMap = -1;
#endif

	m_mem_buffer = NULL;
}

CPF_ShareMem::~CPF_ShareMem(void)
{
}


void * CPF_ShareMem::init(const char * name,size_t memsize,int& first_time)
{
#ifdef OS_WINDOWS
	m_memsize = memsize/(64*1024)*(64*1024);//64KB对齐
#else
	m_memsize = memsize;
#endif

#ifdef OS_WINDOWS
	m_hMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,m_memsize,name);

	if( !m_hMap )
	{
		DWORD dwErr = GetLastError ();
		return NULL;
	}

	first_time =
		::GetLastError () == ERROR_ALREADY_EXISTS
		? 0
		: 1;

	m_mem_buffer = MapViewOfFile(m_hMap,FILE_MAP_WRITE | FILE_MAP_READ,0,0,0);

	if( !m_mem_buffer )
	{
		CloseHandle(m_hMap);
		m_hMap = NULL;
	}

#else
	key_t key = CPF::GetFCS32(name, ACE_OS::strlen(name) );
	m_hMap = shmget(key, m_memsize, IPC_CREAT|IPC_EXCL|0660);
	if(m_hMap  == -1)
	{
		first_time = 0;
#if 0//modified by zhongjh
		m_hMap = shmget(key, m_memsize, 0660);
#else
		m_hMap = shmget(key, 0, 0);
#endif
		if(m_hMap == -1)
		{
			return NULL;
		}
	}
	else
	{
		first_time = 1;
	}

	m_mem_buffer = (int*)shmat(m_hMap,0,0);

#endif

	return m_mem_buffer;
}

/*add by zhongjh *begin*/
void * CPF_ShareMem::map(const char * name)
{
	if ((-1 == m_mem_buffer) || (NULL == m_mem_buffer))
	{
		key_t key = CPF::GetFCS32(name, ACE_OS::strlen(name) );
		
		m_hMap = shmget(key, 0, 0);
		if(m_hMap == -1)
		{
			return NULL;
		}

		m_mem_buffer = shmat(m_hMap,0,0);
	}

	return m_mem_buffer;
}
/*add by zhongjh *end*/

//如果共享内存已经与所有访问它的进程断开了连接，则调用IPC_RMID子命令后，系统将立即删除共享内存的标识符，
//并删除该共享内存区，以及所有相关的数据结构；

//如果仍有别的进程与该共享内存保持连接，则调用IPC_RMID子命令后，
//该共享内存并不会被立即从系统中删除，而是被设置为IPC_PRIVATE状态，并被标记为”已被删除”
//（使用ipcs命令可以看到dest字段）；直到已有连接全部断开，该共享内存才会最终从系统中消失。

//需要说明的是：一旦通过shmctl对共享内存进行了删除操作，
//则该共享内存将不能再接受任何新的连接，即使它依然存在于系统中！
//所以，可以确知， 在对共享内存删除之后不可能再有新的连接，则执行删除操作是安全的；
//否则，在删除操作之后如仍有新的连接发生，则这些连接都将可能失败！

void CPF_ShareMem::fin()
{
#ifdef OS_WINDOWS
	if( m_mem_buffer )
	{
		UnmapViewOfFile(m_mem_buffer);
		m_mem_buffer = NULL;
	}

	if( m_hMap )
	{
		CloseHandle(m_hMap);
		m_hMap = NULL;
	}
#else
	if (-1 != m_hMap)
	{
		shmdt(m_mem_buffer);
/*
		FILE *fpTemp = NULL;
		char strBuff[512];
		sprintf(strBuff, "ipcs | grep %d|awk '{print $6}'", m_hMap);


		if (NULL == (fpTemp = popen(strBuff, "r")))
		{
			printf("read share memory error!!\n");
			return -1;
		}
		int count = 0;
		fscanf(fpTemp, "%d", &count);
printf("id=%d, count=%d\n", m_hMap, count);
		if (0 == count)
		{
			shmctl(m_hMap, IPC_RMID, 0);
		}
*/
		// 检测内存是否没人使用了
		struct shmid_ds buf;
		shmctl(m_hMap, IPC_STAT, &buf);
                if (0 == buf.shm_nattch)
                {
                        shmctl(m_hMap, IPC_RMID, 0);
                }
	}

	m_mem_buffer = NULL;
	m_hMap = -1;

#endif

	return;
}
