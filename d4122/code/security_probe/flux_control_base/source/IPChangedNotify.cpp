//////////////////////////////////////////////////////////////////////////
//IPChangedNotify.cpp

#include "stdafx.h"
#include "IPChangedNotify.h"
#include <stdio.h>
#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS
#	include <iphlpapi.h>
#	include <windows.h>
#	pragma comment(lib, "iphlpapi.lib")
#else
#	include "cpf/adapter_info_query.h"
#endif

CIPChangedNotify::CIPChangedNotify(void)
{
	m_pObserver = NULL;
}

CIPChangedNotify::~CIPChangedNotify(void)
{
}

int CIPChangedNotify::Start(CIPChangeNofityObserver * pObserver)
{
	m_pObserver = pObserver;

	m_bUserStop = false;

#ifdef OS_LINUX
	m_iDevCount = 0;

	// ��ȡ�豸�б�
	char szBuf[1024];
	char* p = szBuf;

	m_iDevCount = adapter_query_dev_name(szBuf, 1024);
	for (int idx=0; idx<m_iDevCount; ++idx)
	{
		strcpy(m_stDevInfo[idx].szDevName, p);

		// ��¼IP��ַ
		adapter_query_ip_str(m_stDevInfo[idx].szDevName, m_stDevInfo[idx].szDevIP);

		p += strlen(p) +1;
	}

#endif
	
	activate();

	return 0;
}

int CIPChangedNotify::Stop()
{
	m_bUserStop = true;

	wait();

	return 0;
}

#ifdef OS_WINDOWS
int CIPChangedNotify::svc(void)
{
	OVERLAPPED overlap;

	HANDLE hand = NULL;
	ZeroMemory(&overlap,sizeof(overlap));
	overlap.hEvent = WSACreateEvent();

	DWORD ret = NotifyAddrChange(&hand, &overlap);

	while( !m_bUserStop )
	{
		if ( WaitForSingleObject(overlap.hEvent, 5*1000) == WAIT_OBJECT_0 )
		{
			if( m_pObserver )
			{
				m_pObserver->OnIPChanged();
			}
			
			WSAResetEvent(overlap.hEvent);
			
			NotifyAddrChange(&hand, &overlap);
		}
	}

	return 0;
}

#else

int CIPChangedNotify::svc(void)
{
	char szBuffer[16];
	bool bChange = false;

 	m_bUserStop = false;

 	while( !m_bUserStop )
	{
		// ѭ����ѯIP��ַ�����ֱ仯��֪ͨ
		// ��ǰ��������һ����ַ��IP�ı�󣬾Ͳ���֪ͨ��Ȼ���ٴβ�ѯ�����û�иı䣬��˯��
		bChange = false;
		for (int i=0; i<m_iDevCount; i++)
		{
			adapter_query_ip_str(m_stDevInfo[i].szDevName, szBuffer);
			bChange = (0 != strcmp(m_stDevInfo[i].szDevIP, szBuffer));
			if (bChange)
			{
				strcpy(m_stDevInfo[i].szDevIP, szBuffer);
				if (m_pObserver)
					m_pObserver->OnIPChanged();
				break;
			}
		}
		if (!bChange)	// û�иı䣬sleep
		{
			ACE_OS::sleep(5);
		}

	}

	return 0;
}
#endif

