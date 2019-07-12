//////////////////////////////////////////////////////////////////////////
//CSysMaintenanceDoCmd.cpp

#include "StdAfx.h"
#include "SysMaintenanceDoCmd.h"
#include "cpf/path_tools.h"
#include "cpf/other_tools.h"
#include "FCServerFacade.h"
#include "SockOperationId.h"
#include "SockManager.h"
#include "ace/Process.h"
#include "cpf/XMLFileSock.h"
#include "cpf2/unrar.h"
#include "cpf/SleepAndDoThread.h"

#include "config_fc_server_xml.h"
#include "config_center.h"


CSysMaintenanceDoCmd::CSysMaintenanceDoCmd(void)
{
	m_pLogInstance = NULL;
	m_pFCSockManager = NULL;
	m_pFCServerFacade = NULL;


	m_bSysUpgrading = false;
}

CSysMaintenanceDoCmd::~CSysMaintenanceDoCmd(void)
{
}


void CSysMaintenanceDoCmd::init(ACE_Log_File_Msg* pLogInstance,
								CFCSockManager * pFCSockManager,
								CFCServerFacade * pFCServerFacade)
{
	
	m_pLogInstance = pLogInstance;

	m_pFCSockManager = pFCSockManager;

	m_pFCServerFacade = pFCServerFacade;

	return;
}


void CSysMaintenanceDoCmd::fini()
{
	return;
}

int CSysMaintenanceDoCmd::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	switch(pMsgBlock->pPduHdr->nOperation)
	{
	case OP_ID_RESTOR_ORIGN:
		OnCmd_RestorOrig(pRemoteAddr, pMsgBlock, pRetMsgBlock);
		break;	

	case OP_ID_UPGRADE_SYS_PRG:
		OnCmd_UpgradeSysPrg(pRemoteAddr, pMsgBlock, pRetMsgBlock);
		break;	

	default:
		MY_ACE_DEBUG(m_pLogInstance,(LM_ERROR, ACE_TEXT("[Err][%D]CSysMaintenanceDoCmd::OnRequest Opera[%d] not found!!\n"), pMsgBlock->pPduHdr->nOperation) );
		ACE_ASSERT(FALSE);
		return -1;
		break;

	}

	if( pRetMsgBlock )
	{
		return pRetMsgBlock->pPduHdr->nResponseCode;
	}

	return -1;
}

BOOL CSysMaintenanceDoCmd::OnCmd_RestorOrig(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	CBaseDoCmdTool::log_request(m_pLogInstance,pRemoteAddr,"RestorOrig");

	//停止流控程序，确保系统处于bypass状态	
	CSysMaintenanceDoCmd::Ctrl_Prg_By_Unierm(m_pFCSockManager->m_fc_monitor_sock,_DEF_TESTCONTROL_NAME,0,1);

	int nerror = RestoreOrig();
	
	if( nerror )
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,nerror,RESPONSE_CODE_OTHER);
	else
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,0);
	
	CRebootTask * restart_task = new CRebootTask(CRebootTask::REBOOT_COMPUTER);

	restart_task->sleep_and_do(5000);

	return true;
}

int CSysMaintenanceDoCmd::RestoreOrig()
{
	{
		//恢复程序配置
		CPF::DeleteDirectory(ConfigCenter_Obj()->m_cfg_base_path.c_str(),false);

		char bak_program_config[MAX_PATH] = {0};

		CPF::JoinPathToPath(bak_program_config,sizeof(bak_program_config),
			ConfigCenter_Obj()->m_prg_base_path2.c_str(),"ts_config");

		//覆盖
		CPF::CopySubDirectory(bak_program_config,
			ConfigCenter_Obj()->m_cfg_base_path.c_str());
	}
	
	return 0;
}


int CSysMaintenanceDoCmd::Ctrl_Prg_By_Unierm(CConnectToServerMgr& fc_monitor_sock,const char * prg_name,int status,BOOL bTempStat)
{
	char buf[256];

	char * pdata = buf;

	CData_Stream_LE::PutUint32(pdata,bTempStat);
	CData_Stream_LE::PutString(pdata,prg_name);

	MSG_BLOCK * pRspMsgBlock = NULL;

	unsigned short operation = 0;

	if( status )
	{
		operation = 60001;
	}
	else
	{
		operation = 60002;
	}

	int nerror = fc_monitor_sock.SendRequest(
		1,(short)operation,
		buf,pdata-buf,
		pRspMsgBlock,10);

	if( nerror != 0 )
	{
		return nerror;
	}

	const char * read_data = CMsgBlockManager::ReadPtr(*pRspMsgBlock,0,2);

	int code = (int)CData_Stream_LE::GetUint16(read_data);

	CMsgBlockManager::Free(pRspMsgBlock);

	if( code != 0 )
	{
		return code;
	}

	return 0;
}


BOOL CSysMaintenanceDoCmd::BackupSysCfgFile(const char * sys_cfg_file)
{
	CTinyXmlEx xml_writter;

	if( 0 != xml_writter.open_ex(sys_cfg_file,CTinyXmlEx::OPEN_MODE_CREATE_TRUNC) )
	{
		return 1;
	}

	BOOL bOK = 0;

	const int cur_version = 2;

	xml_writter.InsertRootElement("unierm_sys_cfg");
	xml_writter.SetAttr(xml_writter.GetRootNode(),"version", cur_version);

	do 
	{
		TiXmlNode * probe_config_node = xml_writter.InsertEndChildElement(xml_writter.GetRootNode(),"ts_config");
		if( !probe_config_node )
		{
			bOK = 3;
			break;
		}

		if( !xml_writter.PacketOneFolderIntoOneNode(probe_config_node,
			ConfigCenter_Obj()->m_cfg_base_path.c_str()) )
		{
			bOK = 4;
			break;
		}
	} while (0);
	
	xml_writter.savefile();
	xml_writter.close();

	return bOK;
}

//导入db和和配置信息
BOOL CSysMaintenanceDoCmd::LoadSysCfgFile(const char * sys_cfg_file,int incude_local_machine)
{
	CTinyXmlEx xml_reader;

	if( 0 != xml_reader.open_for_readonly(sys_cfg_file) )
		return 1;

	{//删除临时文件
		/*
		CPF::DeleteDirectory(ConfigCenter_Obj()->m_str_probe_buffer_path.c_str(), false);
		CPF::DeleteDirectory(ConfigCenter_Obj()->m_str_probe_auth_result_path.c_str(), false);
		CPF::DeleteDirectory(ConfigCenter_Obj()->m_udata_path.c_str(),false);
		*/
	}

	int version = 2;
	
	{
		TiXmlNode * pRootNode = xml_reader.GetRootNode();

		if (!xml_reader.GetAttr(pRootNode, "version", version))
		{
			return 2;
		}
	}

	{
		TiXmlNode * ts_config_node = xml_reader.GetRootNode()->FirstChildElement("ts_config");
		if( !ts_config_node)
		{
			return 3;
		}

		BOOL result = xml_reader.UnPacketOneFolderFromOneNode(ts_config_node, ConfigCenter_Obj()->m_cfg_base_path.c_str());
		if (false == result)
		{
			return 4;
		}
	}

	return 0;
}


#ifdef OS_LINUX
class CUpgradeThread : public ACE_Task_Base
{
public:
	CUpgradeThread(const char* pszFilename)
	{
		strcpy(full_file_name, pszFilename);
	}

	virtual ~CUpgradeThread(void){};

	virtual int svc(void)
	{
		ACE_OS::sleep(2);	// 让客户端可以将结果返回，在杀掉当前进程

		// 记录当前工作路径
		char strOldPath[MAX_PATH];
		ACE_OS::getcwd((char*)strOldPath, MAX_PATH);

		std::string path_name = CPF::GetPathNameFromFullName(full_file_name);

		ACE_OS::chdir(path_name.c_str());

		//执行升级包程序
		system(full_file_name);
		
		ACE_OS::chdir(strOldPath);	// 返回开始的目录	
	}

private:
	//安装包的文件名字(是一个完整的压缩包)
	char    full_file_name[MAX_PATH];
};
#endif // OS_LINUX


BOOL CSysMaintenanceDoCmd::OnCmd_UpgradeSysPrg(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	if (m_bSysUpgrading)
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -4);
		return true;
	}

	const char * pdata = CMsgBlockManager::ReadPtr(*pMsgBlock,0,4);
	if( !pdata )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,-2);
		return true;
	}

	char full_file_name[MAX_PATH] = { 0 };

	char * short_file_name = (char *)CData_Stream_LE::GetString(pdata);

	if (CPF::IsFullPathName(short_file_name))
	{
		strcpy(full_file_name, short_file_name);
	}
	else
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -3);
		return false;
	}

	m_bSysUpgrading = true;

	// 加入执行参数,add by xumx 2016-10-31
	int len = strlen(full_file_name);
	strcpy(full_file_name+len, " update");
	
	CUpgradeThread* pUpThread = new CUpgradeThread(full_file_name);

	if (pUpThread)
	{
		pUpThread->activate();
	}

	pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,0);

	return true;

}


int CSysMaintenanceDoCmd::StartOneProgram(const char * prg_name,
										  const char *szCmd, 
										  const char *szWorkingDirectory,
										  int wait_end)
{
	std::string str_suf;

	CPF::GetSufFromFileName(prg_name,str_suf);

	if(stricmp(str_suf.c_str(),"bat") == 0 )
	{
		CPF::RunBat(prg_name,szCmd,szWorkingDirectory);
	}
	else
	{
		if( wait_end )			
		{
			CPF::RunProcessAndWaitDone(prg_name,szCmd,szWorkingDirectory);
		}
		else
		{
			CPF::RunProcess(prg_name,szCmd,szWorkingDirectory);
		}
	}

	return 0;
}

void CSysMaintenanceDoCmd::restart_all_program_by_unierm(CFCSockManager * pFCSockManager,BOOL bTempStat)
{
	if (!pFCSockManager)
		return;

	restart_one_program_by_unierm(pFCSockManager, 1, _DEF_TESTCONTROL_NAME, (short)60004,bTempStat);
	restart_one_program_by_unierm(pFCSockManager, 1, _DEF_FCSERVER_NAME, (short)60004,bTempStat);

}

void CSysMaintenanceDoCmd::restart_one_program_by_unierm(CFCSockManager * pFCSockManager, int iServantid, const char* pszName, short wOperation,BOOL bTempStat)
{
	char szBuf[256];
	char * pData = szBuf;

	CData_Stream_LE::PutUint32(pData, bTempStat);
	CData_Stream_LE::PutString(pData, pszName);

	int nerror = pFCSockManager->m_fc_monitor_sock.SendNotify(
		iServantid, wOperation,
		szBuf, pData-szBuf);
}