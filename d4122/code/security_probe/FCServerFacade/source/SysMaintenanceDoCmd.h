//////////////////////////////////////////////////////////////////////////
//CSysMaintenanceDoCmd.h

#pragma once

#include "BaseDoCmd.h"
#include "cpf/TinyXmlEx.h"

#ifdef OS_WINDOWS
#	define _DEF_TESTCONTROL_NAME		"TestControl.exe"
#	define _DEF_FCSERVER_NAME			"FCServer.exe"
#	define _DEF_UNIERM_CTRL_NAME		"Unierm_Ctrl.exe"
#else
#	define _DEF_TESTCONTROL_NAME		"testcontrol"
#	define _DEF_FCSERVER_NAME			"fcserver"
#	define _DEF_UNIERM_CTRL_NAME		"unierm_ctrl"
#endif

class CFCServerFacade;
class CFCSockManager;


class CSysMaintenanceDoCmd : public CBaseDoCmd
{
public:
	CSysMaintenanceDoCmd(void);
	virtual ~CSysMaintenanceDoCmd(void);

public:
	void init(ACE_Log_File_Msg* pLogInstance,
		CFCSockManager * pFCSockManager,
		CFCServerFacade * pFCServerFacade);

	void fini();

public:
	int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	BOOL OnCmd_RestorOrig(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_UpgradeSysPrg(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

public:
	static int StartOneProgram(const char * prg_name,
		const char *szCmd, 
		const char *szWorkingDirectory,
		int wait_end);

public:
	static void restart_all_program_by_unierm(CFCSockManager* pFCSockManager,BOOL bTempStat);
	static void restart_one_program_by_unierm(CFCSockManager* pFCSockManager, int iServantid, const char* pszName, short wOperation,BOOL bTempStat);

public:
	static int Ctrl_Prg_By_Unierm(CConnectToServerMgr& fc_monitor_sock,const char * prg_name,int status,BOOL bTempStat);

public:
	static BOOL BackupSysCfgFile(const char * sys_cfg_file);
	static BOOL LoadSysCfgFile(const char * sys_cfg_file,int incude_local_machine);

	static int RestoreOrig();

private:
	CFCSockManager *	m_pFCSockManager;
	CFCServerFacade *	m_pFCServerFacade;

	BOOL				m_bSysUpgrading;

};
