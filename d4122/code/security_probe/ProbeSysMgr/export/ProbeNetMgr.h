/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  NetMgr.h
 *
 *  TITLE      :  Probeϵͳ�������������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  ������������ṩ�����һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_NET_MANAGER_H__
#define __PROBE_NET_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetInfo.h"

// Probeϵͳ��Դ������
class EXPORT_CLASS CProbeNetMgr
{
public:
	CProbeNetMgr();
	~CProbeNetMgr();

	// ��ʼ��
	bool Init( IN const SNetCfgInfo *pCfgInfo = NULL );

	// ��ȡ��������������Ŀǰ���ã�
	int GetCpuCount() const
	{	return m_vectInfo.size();	}

	// ����Ż�ȡ������Ϣ��Ŀǰ����ȡ�������һ����Ϣ�����nNth����Ŀǰ���ã�
	const CNetInfo* GetNetInfoByNth( IN int nNth = 0 );
	
	// �Ƿ��������ʳ����޸澯
	bool IsNetUsePercentAlarm();
	
private:

private:
	// ����������Ϣ
	SNetCfgInfo					*m_pCfgInfo;

	// ������Ϣ���������
	std::vector< CNetInfo* >	m_vectInfo;

};

#endif // __PROBE_NET_MANAGER_H__

