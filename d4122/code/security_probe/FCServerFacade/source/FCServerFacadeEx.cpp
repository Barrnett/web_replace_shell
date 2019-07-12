//////////////////////////////////////////////////////////////////////////
//FCServerFacadeEx.cpp

#include "StdAfx.h"
#include "FCServerFacadeEx.h"
#include "FCServerFacade.h"
#include "ModuleForRestoreOrig.h"

CFCServerFacadeEx::CFCServerFacadeEx()
{
	m_pFCServerFacade = NULL;
	m_pModuleForRestoreOrig = NULL;
	
}

CFCServerFacadeEx::~CFCServerFacadeEx()
{

}


BOOL CFCServerFacadeEx::init_normal(
							 ACE_Log_File_Msg * m_pLogInstance,
							 const char * monitor_event_name,
							 int monitor_event_interval)
{
	m_pFCServerFacade = new CFCServerFacade;
	
	return m_pFCServerFacade->init(m_pLogInstance,monitor_event_name,monitor_event_interval);
}

BOOL CFCServerFacadeEx::init_for_restor_orig(
	ACE_Log_File_Msg * m_pLogInstance)
{
	m_pModuleForRestoreOrig = new CModuleForRestoreOrig;

	m_pModuleForRestoreOrig->init();

	m_pModuleForRestoreOrig->DoRestoreOrig();

	return TRUE;
}


void CFCServerFacadeEx::fini()
{
	if( m_pFCServerFacade )
	{
		m_pFCServerFacade->fini();
		delete m_pFCServerFacade;
		m_pFCServerFacade = NULL;
	}
}

BOOL CFCServerFacadeEx::Start()
{
	if( m_pFCServerFacade )
		return m_pFCServerFacade->Start();

	return FALSE;
}

void CFCServerFacadeEx::Stop()
{
	if( m_pFCServerFacade )
		m_pFCServerFacade->Stop();
}

BOOL CFCServerFacadeEx::BackupSysCfgFile(const char * sys_cfg_file)
{
	if( m_pFCServerFacade )
		return m_pFCServerFacade->BackupSysCfgFile(sys_cfg_file);

	return false;

}

BOOL CFCServerFacadeEx::LoadSysCfgFile(const char * sys_cfg_file, int incude_local_machine)
{
	if( m_pFCServerFacade )
		return m_pFCServerFacade->LoadSysCfgFile(sys_cfg_file, incude_local_machine);

	return false;

}

BOOL CFCServerFacadeEx::RestoreOrig()
{
	if( m_pFCServerFacade )
		return m_pFCServerFacade->RestoreOrig();

	return false;

}
