//////////////////////////////////////////////////////////////////////////
//FCServerFacadeEx.h

#pragma once

#include "fc_server_export.h"
#include "cpf/ACE_Log_File_Msg.h"

class CFCServerFacade;
class CModuleForRestoreOrig;

class FCSERVERFACADE_CLASS CFCServerFacadeEx
{
public:
	CFCServerFacadeEx(void);
	virtual ~CFCServerFacadeEx(void);

public:
	BOOL init_normal(
		ACE_Log_File_Msg * m_pLogInstance,
		const char * monitor_event_name,
		int monitor_event_interval);

	BOOL init_for_restor_orig(
		ACE_Log_File_Msg * m_pLogInstance);


public:
	void fini();

	BOOL Start();
	void Stop();

public:
	BOOL BackupSysCfgFile(const char * sys_cfg_file);
	BOOL LoadSysCfgFile(const char * sys_cfg_file, int incude_local_machine);

	BOOL RestoreOrig();

private:
	CFCServerFacade *	m_pFCServerFacade;
	CModuleForRestoreOrig *	m_pModuleForRestoreOrig;
};


