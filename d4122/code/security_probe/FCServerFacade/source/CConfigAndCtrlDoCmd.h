//////////////////////////////////////////////////////////////////////////
//CConfigAndCtrlDoCmd.h

#pragma once

#include "ace/Recursive_Thread_Mutex.h"
#include "BaseDoCmd.h"
#include <vector>
#include "cpf/CommonQueueManager.h"
#include "LMB_serial_class.h"

class CFCSockManager;
class CFCServerFacade;

class CCConfigAndCtrlDoCmd : public CBaseDoCmd
{
public:
	CCConfigAndCtrlDoCmd(void);
	virtual ~CCConfigAndCtrlDoCmd(void);

public:
	void init(ACE_Log_File_Msg* pLogInstance,
		CFCSockManager* pFCSockManager,
		CFCServerFacade * pFCServerFacade);

	void fini();

public:
	int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	//web服务器已经把xml文件写好了，通知fcserver配置发生变化。
	BOOL OnCmd_SetConfigFile(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

	//web服务器通知fcserver准备来读取某个文件，请先做好准备
	BOOL OnCmd_GetConfigFile(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);

	BOOL OnCmd_clear_service_statistic(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_EnableMgmtTunnel(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_DisableMgmtTunnel(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_ApplyUpdateCfg(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_CheckUpdateCfg(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock);

private:
	BOOL OnCmd_SetOneXMLFileConfig_ErrorByProbe(int trans_to_probe,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock,
		const char * log_string,const char * xml_file_name,const char * db_log_string);

	BOOL OnCmd_SetOneXMLFileConfig_ErrorByFCServer(int trans_to_probe,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock,
		const char * log_string,const char * xml_file_name,const char * db_log_string);

private:
	BOOL MyTransRequestToFCProbe(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	BOOL SendOneXMLFileToFCProbe(const char * filename,int operation);

	size_t GetProbeSysTime(MSG_BLOCK * pResponMsgBlock);

public:
	CFCSockManager *	m_pFCSockManager;

	CFCServerFacade *	m_pFCServerFacade;


};



