//////////////////////////////////////////////////////////////////////////
//ModuleForRestoreOrig.h


#pragma once

#include "cpf/ConnectToServerMgr.h"

class CModuleForRestoreOrig
{
public:
	CModuleForRestoreOrig(void);
	~CModuleForRestoreOrig(void);


public:
	void init();
	void close();

public:
	void DoRestoreOrig();

public:
	//����֪ͨ�ܿس���ֹͣ��������
	CConnectToServerMgr		m_fc_monitor_sock;

};
