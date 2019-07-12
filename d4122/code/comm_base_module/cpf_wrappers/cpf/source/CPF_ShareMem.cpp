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
	m_memsize = memsize/(64*1024)*(64*1024);//64KB����
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

//��������ڴ��Ѿ������з������Ľ��̶Ͽ������ӣ������IPC_RMID�������ϵͳ������ɾ�������ڴ�ı�ʶ����
//��ɾ���ù����ڴ������Լ�������ص����ݽṹ��

//������б�Ľ�����ù����ڴ汣�����ӣ������IPC_RMID�������
//�ù����ڴ沢���ᱻ������ϵͳ��ɾ�������Ǳ�����ΪIPC_PRIVATE״̬���������Ϊ���ѱ�ɾ����
//��ʹ��ipcs������Կ���dest�ֶΣ���ֱ����������ȫ���Ͽ����ù����ڴ�Ż����մ�ϵͳ����ʧ��

//��Ҫ˵�����ǣ�һ��ͨ��shmctl�Թ����ڴ������ɾ��������
//��ù����ڴ潫�����ٽ����κ��µ����ӣ���ʹ����Ȼ������ϵͳ�У�
//���ԣ�����ȷ֪�� �ڶԹ����ڴ�ɾ��֮�󲻿��������µ����ӣ���ִ��ɾ�������ǰ�ȫ�ģ�
//������ɾ������֮���������µ����ӷ���������Щ���Ӷ�������ʧ�ܣ�

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
		// ����ڴ��Ƿ�û��ʹ����
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
