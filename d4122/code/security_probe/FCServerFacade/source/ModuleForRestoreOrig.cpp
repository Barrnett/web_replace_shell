//////////////////////////////////////////////////////////////////////////
//ModuleForRestoreOrig.cpp

#include "StdAfx.h"
#include "config_fc_server_xml.h"
#include "ModuleForRestoreOrig.h"
#include "SysMaintenanceDoCmd.h"
#include "cpf/SleepAndDoThread.h"
#include "cpf/os_syscall.h"

CModuleForRestoreOrig::CModuleForRestoreOrig(void)
{
}

CModuleForRestoreOrig::~CModuleForRestoreOrig(void)
{
}


void CModuleForRestoreOrig::init()
{
	m_fc_monitor_sock.init(FCServerConfig_Obj()->fc_monitor.ip,FCServerConfig_Obj()->fc_monitor.port,5);
	m_fc_monitor_sock.ConnectToServer();
	m_fc_monitor_sock.StartReConnectMonitor();

	return;
}

void CModuleForRestoreOrig::close()
{
	m_fc_monitor_sock.close();

	return;

}

void CModuleForRestoreOrig::DoRestoreOrig()
{
	//停止流控程序，确保系统处于bypass状态
	CSysMaintenanceDoCmd::Ctrl_Prg_By_Unierm(m_fc_monitor_sock,_DEF_TESTCONTROL_NAME,0,1);
	CSysMaintenanceDoCmd::Ctrl_Prg_By_Unierm(m_fc_monitor_sock,_DEF_FCSERVER_NAME,0,1);

	//恢复到出厂配置
	int nerror = CSysMaintenanceDoCmd::RestoreOrig();

	CPF::MySystemShutdown(0x02);

	while( 1 )
	{
		ACE_OS::sleep(5);
	}

	return;
}
