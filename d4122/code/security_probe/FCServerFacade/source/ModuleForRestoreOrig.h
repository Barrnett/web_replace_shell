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
	//用于通知总控程序停止其他程序
	CConnectToServerMgr		m_fc_monitor_sock;

};
