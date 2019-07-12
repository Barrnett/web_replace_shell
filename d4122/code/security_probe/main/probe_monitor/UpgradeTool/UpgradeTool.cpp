// UpgradeTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "UpgradeTool.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/path_tools.h"
#include "cpf/other_tools.h"
#include "MonitorTaskRW.h"
#include "MonitorManager.h"
#ifdef OS_WINDOWS
#	include "ace/Process.h"
#	include "shellapi.h"
#endif
#include "cpf/os_syscall.h"
#include "cpf/VersionCompCheck.h"

#ifdef OS_WINDOWS
#	define CONFIG_DB_CONFIG_FILE_NAME	"C:/unierm/probe/ini/fc_server.xml"
#	define DATA_DB_CONFIG_FILE_NAME		"D:/unierm_data/fc_server_config/data_db_config.xml"
#	define MYSQL_EXE_PATHNAME			"c:/unierm/mysql/bin/mysql"
#	define OUT_FILE_PATHNAME			"c:/unierm/mysql_update.out"
#else
#	define CONFIG_DB_CONFIG_FILE_NAME	"/home/unierm/probe/ini/fc_server.xml"
#	define DATA_DB_CONFIG_FILE_NAME		"/unierm_data/fc_server_config/data_db_config.xml"
#	define MYSQL_EXE_PATHNAME			"mysql"	// linux下应该不需要加绝对路径
#	define OUT_FILE_PATHNAME			"/tmp/mysql_update.out"
#endif


CUpgradeTool::CUpgradeTool()
{
	m_pCurUpdateNode = NULL;

	m_pCommonUpdateNode = NULL;

	m_pLogInstance = NULL;
}

CUpgradeTool::~CUpgradeTool()
{

}

BOOL CUpgradeTool::init(ACE_Log_File_Msg& log_file)
{
	const char xml_file_name[] = "Upgrade_Tool.xml";

	if( 0 != m_xml_reader.open_for_readonly(CPF::GetModuleFullFileName(xml_file_name)) )
	{
		return false;
	}

	m_pLogInstance = &log_file;

	m_pCurUpdateNode = NULL;
	m_pCommonUpdateNode = NULL;

	return true;
}

void CUpgradeTool::close()
{
	m_xml_reader.close();

	return;
}


int CUpgradeTool::CheckVersion()
{
	//读取需要停止程序的配置
	
	{
		std::string old_unierm_version_file_pos;

		m_xml_reader.GetValueLikeIni(m_xml_reader.GetRootNode(),
			"old_unierm_version_file_pos",
			old_unierm_version_file_pos);

		ReadVersion(old_unierm_version_file_pos.c_str(),m_str_cur_run_version);

		if( m_str_cur_run_version.empty() )
		{
			return -1;
		}

		std::string new_unierm_version_file_pos;

		m_xml_reader.GetValueLikeIni(m_xml_reader.GetRootNode(),
			"new_unierm_version_file_pos",
			new_unierm_version_file_pos);

		if( new_unierm_version_file_pos.empty() )
		{
			new_unierm_version_file_pos = "unierm_version.xml";
		}

		if( !CPF::IsFullPathName(new_unierm_version_file_pos.c_str()) )
		{
			new_unierm_version_file_pos = CPF::GetModuleFullFileName(new_unierm_version_file_pos.c_str());
		}

		ReadVersion(new_unierm_version_file_pos.c_str(),m_str_new_version);

		if( m_str_new_version.empty() )
		{
			return -1;
		}
	}

	int upgrade_dir = 1;

	int cmp_veriosn = CVersionCompInfo::CompareVersion(m_str_cur_run_version,m_str_new_version);

	if( cmp_veriosn > 0 )
	{
		upgrade_dir = -1;

		std::string old_anti_upgrade_tool_xml_pos;

		m_xml_reader.GetValueLikeIni(m_xml_reader.GetRootNode(),
			"old_anti_upgrade_tool_xml_pos",
			old_anti_upgrade_tool_xml_pos);

		if( old_anti_upgrade_tool_xml_pos.empty() )
		{
			return -1;
		}

		m_xml_reader.close();

		if( 0 != m_xml_reader.open_for_readonly(old_anti_upgrade_tool_xml_pos.c_str()) )
		{
			return -1;
		}
	}
	else
	{
		upgrade_dir = 1;
	}

	m_pCommonUpdateNode =  m_xml_reader.GetRootNode()->FirstChildElement("common_update");

	if( !m_pCommonUpdateNode )
	{
		return -2;
	}

	for(TiXmlNode * pCurUpdateNode = m_xml_reader.GetRootNode()->FirstChildElement("one_update");
		pCurUpdateNode;
		pCurUpdateNode = pCurUpdateNode->NextSiblingElement("one_update") )
	{
		std::vector< VERSION_RANGE > vt_supp_version;

		if( upgrade_dir == 1 )
			ReadSupportVersionList(m_xml_reader,pCurUpdateNode,vt_supp_version,m_str_new_version);
		else
			ReadSupportVersionList(m_xml_reader,pCurUpdateNode,vt_supp_version,m_str_cur_run_version);

		if( vt_supp_version.empty() )
		{
			return -1;
		}

		if( upgrade_dir == 1 )
		{
			if( CheckOldVersion(m_str_cur_run_version,vt_supp_version) )
			{
				m_pCurUpdateNode = pCurUpdateNode;
				break;
			}
		}
		else
		{
			if( CheckOldVersion(m_str_new_version,vt_supp_version) )
			{
				m_pCurUpdateNode = pCurUpdateNode;
				break;
			}
		}		
	}

	if( m_pCurUpdateNode  )
	{
		return 0;
	}

	return 1;
}


void CUpgradeTool::ReadSupportVersionList(CTinyXmlEx& xml_reader,
										  TiXmlNode * parent_node,
										  std::vector< VERSION_RANGE >& vt_supp_version,
										  const std::string& this_version)
{
	TiXmlNode * support_version_list_node = 
		parent_node->FirstChildElement("support_version_list");

	if( !support_version_list_node )
	{
		return;
	}

	for(TiXmlNode * version_node = support_version_list_node->FirstChildElement("version");
		version_node;
		version_node=version_node->NextSibling("version"))
	{
		std::string str_version;

		xml_reader.GetValue(version_node,str_version);

		if( !str_version.empty() )
		{
			if( 0 == stricmp(str_version.c_str(),"THIS_VERSION") )
			{
				str_version = this_version;
			}

			VERSION_RANGE version_range;
			version_range.from_version = version_range.to_verison = str_version;
			version_range.but_from = version_range.but_to = 0;

			vt_supp_version.push_back(version_range);
		}
	}

	for(TiXmlNode * version_range_node = support_version_list_node->FirstChildElement("version_range");
		version_range_node;
		version_range_node=version_range_node->NextSibling("version_range"))
	{
		VERSION_RANGE version_range;

		std::string from_verison;

		ReadVersionAttr(xml_reader,version_range_node,"from_version",from_verison,this_version);

		if( !from_verison.empty() )
		{
			version_range.but_from = 0;
			version_range.from_version = from_verison;
		}
		else
		{
			ReadVersionAttr(xml_reader,version_range_node,"from_version_bu_this",from_verison,this_version);

			version_range.but_from = 1;
			version_range.from_version = from_verison;
		}

		if( version_range.from_version.empty() )
		{
			ACE_ASSERT(FALSE);
			continue;
		}

		std::string to_verison;
		ReadVersionAttr(xml_reader,version_range_node,"to_version",to_verison,this_version);

		if( !to_verison.empty() )
		{
			version_range.but_to = 0;
			version_range.to_verison = to_verison;
		}
		else
		{	
			ReadVersionAttr(xml_reader,version_range_node,"to_version_but_this",to_verison,this_version);

			version_range.but_to = 1;
			version_range.to_verison = to_verison;
		}

		if( version_range.to_verison.empty() )
		{
			ACE_ASSERT(FALSE);
			continue;
		}

		vt_supp_version.push_back(version_range);
	}

	return;
}

BOOL CUpgradeTool::CheckOldVersion(
					 const std::string& old_version,
					 const std::vector< VERSION_RANGE >& vt_supp_version)
{
	for(size_t i = 0; i  <vt_supp_version.size(); ++i)
	{
		const VERSION_RANGE& version_range = vt_supp_version[i];

		int cmp1 = CVersionCompInfo::CompareVersion(version_range.from_version,old_version);

		if( cmp1 > 0 )
			continue;

		if( cmp1 == 0  )
		{
			if( version_range.but_from == 0 )
				return true;
			else
				continue;
		}

		int cmp2 = CVersionCompInfo::CompareVersion(version_range.to_verison,old_version);

		if( cmp2 < 0 )
			continue;

		if( cmp2 == 0 )
		{
			if( version_range.but_to == 0 )
				return true;
			else
				continue;
		}

		return true;
	}

	return false;
}


int CUpgradeTool::ReadVersion(const char * version_file_pos,std::string& str_version)
{
	str_version = "";

	CTinyXmlEx xml_file;

	if( 0 != xml_file.open_for_readonly(version_file_pos) )
	{
		return -1;
	}

	for(TiXmlElement* pversion_infoNode = xml_file.GetRootNode()->FirstChildElement("version_info");
		pversion_infoNode;
		pversion_infoNode = pversion_infoNode->NextSiblingElement("version_info"))
	{
		std::string model_name;
		xml_file.GetAttr(pversion_infoNode,"self_name",model_name);

		if( model_name.compare("unierm_system") == 0 )
		{
			xml_file.GetAttr(pversion_infoNode,"self_verion",str_version);	

			break;
		}
	}

	return 0;

}

int CUpgradeTool::ReadVersionAttr(CTinyXmlEx& xml_reader,TiXmlNode * node,const char * att,std::string& version,const std::string& this_version)
{
	if( !xml_reader.GetAttr(node,att,version) )
	{
		return false;
	}

	if( 0 == stricmp(version.c_str(),"THIS_VERSION") )
	{
		version = this_version;
	}

	return true;
}

int CUpgradeTool::StopOldProgram()
{
	TiXmlNode * service_node = GetCurrFuncNode("MonitorService");

	if( !service_node )
		return -1;

	std::vector<MONITOR_TASK> aMonitorTask;

	CMonitorTaskRW::GetMonitorTasks(m_xml_reader,service_node,aMonitorTask);

	if( aMonitorTask.size() == 0 )
	{
		return 0;
	}

	//停止程序
	CMonitorManager monitor_mgr(true,m_pLogInstance);

	monitor_mgr.AddMonitorTasks(aMonitorTask);

	monitor_mgr.StopAll(1);

	return 0;
}

int CUpgradeTool::DeleteOldFile()
{
	TiXmlNode * delete_file_node = GetCurrFuncNode("delete_file");

	if( !delete_file_node )
		return 0;

	for(TiXmlNode * path_node = delete_file_node->FirstChildElement("path");
		path_node;
		path_node = path_node->NextSiblingElement("path"))
	{
		int onoff = 1;
		m_xml_reader.GetAttr(path_node,"onoff",onoff);

		int delete_self = 0;
		m_xml_reader.GetAttr(path_node,"delete_self",delete_self);

		std::string str_path_name;
		m_xml_reader.GetValue(path_node,str_path_name);

		if(onoff && !str_path_name.empty())
		{//删除文件

			CPF::DeleteDirectory(str_path_name.c_str(),delete_self);

		}
	}

	for(TiXmlNode * file_node = delete_file_node->FirstChildElement("file");
		file_node;
		file_node = file_node->NextSiblingElement("file"))
	{
		int onoff = 1;
		m_xml_reader.GetAttr(file_node,"onoff",onoff);

		std::string str_file_name;
		m_xml_reader.GetValue(file_node,str_file_name);


		if(onoff && !str_file_name.empty())
		{//删除文件

			ACE_OS::unlink(str_file_name.c_str());
		}
	}

	return 0;
}

bool CUpgradeTool::UpdateDB_oneline(TiXmlNode* update_db_list, int i_type)//, std::string& str_full_update_cmd)
{
	ACE_ASSERT(i_type < 3);
	const char* str_db_type[] = {"mysql_db", "config_db", "data_db"};
	TiXmlNode * mysql_db_node = update_db_list->FirstChildElement(str_db_type[i_type]);

	if(mysql_db_node)
	{
		if( !CheckOSAttr(m_xml_reader,mysql_db_node) )
			return false;

		CheckPresetup(m_xml_reader, mysql_db_node);

		std::string str_sql_db_sql_file_name;
		m_xml_reader.GetAttr(mysql_db_node,"script_file_name",str_sql_db_sql_file_name);

		if( !str_sql_db_sql_file_name.empty() )
		{
			std::string update_cmd_string;

			switch (i_type)
			{
			case 0:
				if( GetUpDateMysqlDbCmdString(str_sql_db_sql_file_name.c_str(),update_cmd_string) != 0 )
					return false;
				break;
			case 1:
				if( GetUpDateConfigDbCmdString(str_sql_db_sql_file_name.c_str(),update_cmd_string) != 0 )
					return false;
				break;
			case 2:
				if( GetUpDateDataDbCmdString(str_sql_db_sql_file_name.c_str(),update_cmd_string) != 0 )
					return false;
				break;
			default:
				return false;
			}

//			str_full_update_cmd += update_cmd_string;
			if (!update_cmd_string.empty())
			{
				printf("执行数据库脚本 [%s] ....\n", update_cmd_string.c_str());

				MY_ACE_DEBUG(m_pLogInstance,
					(LM_ERROR,
					ACE_TEXT("\n%D:执行数据库脚本 [%s] ....\n"),update_cmd_string.c_str()
					));

				ACE_OS::system(update_cmd_string.c_str());
			}

// #ifndef OS_LINUX
// 			str_full_update_cmd += "\r";
// #endif
// 			str_full_update_cmd += "\n";
		}
	}

	return true;

}


int CUpgradeTool::UpdateDB(TiXmlNode * p_one_update)//, std::string &str_full_update_cmd)
{
	TiXmlNode * update_db_node = p_one_update;

	if( !update_db_node )
	{
		return 0;
	}

	for(TiXmlNode * update_db_list = update_db_node->FirstChildElement("list");
		update_db_list;
		update_db_list = update_db_list->NextSiblingElement("list"))
	{
		UpdateDB_oneline(update_db_list, 0);//, str_full_update_cmd);	// "mysql_db"
		UpdateDB_oneline(update_db_list, 1);//, str_full_update_cmd);	// "config_db"
		UpdateDB_oneline(update_db_list, 2);//, str_full_update_cmd);	// "data_db"
	}
	
	return 0;
}

int CUpgradeTool::SetupProgram()
{
	TiXmlNode * setup_proram_node = GetCurrFuncNode("setup_proram");

	if( !setup_proram_node )
	{
		return -1;
	}

	for(TiXmlNode * program_node = setup_proram_node->FirstChildElement("program");
		program_node;
		program_node = program_node->NextSiblingElement("program"))
	{
		int onoff = 1;

		m_xml_reader.GetAttr(program_node,"onoff",onoff);

		if( !onoff )
			continue;

		if( !CheckOSAttr(m_xml_reader,program_node) )
			continue;
		
		CheckPresetup(m_xml_reader, program_node);

		{
			std::string str_program;

			m_xml_reader.GetValue(program_node,str_program);

			if( !str_program.empty() )
			{
				//执行安装程序
				//...
				SetupOneProgram(str_program.c_str());
			}
		}

		{
			std::string setup_cmd;
			m_xml_reader.GetAttr(program_node,"setup",setup_cmd);

			if( !setup_cmd.empty() )
			{
				SetupOneProgram(setup_cmd.c_str());
			}
		}
		
	}

	return 0;
}

int CUpgradeTool::CopyProgram()
{
	TiXmlNode * copy_proram_node = GetCurrFuncNode("copy_proram");

	if( !copy_proram_node )
	{
		return -1;
	}

	for(TiXmlNode * program_node = copy_proram_node->FirstChildElement("program");
		program_node;
		program_node = program_node->NextSiblingElement("program"))
	{
		int onoff = 1;

		m_xml_reader.GetAttr(program_node,"onoff",onoff);

		if( !onoff )
			continue;

		if( !CheckOSAttr(m_xml_reader,program_node) )
			continue;

		CheckPresetup(m_xml_reader,program_node);


		std::string strDstDir;
		std::string strProgram;
		bool bRet = 0; 
		bRet = m_xml_reader.GetAttr(program_node, "dst_dir", strDstDir);
		if (!bRet)
			continue;
		bRet = m_xml_reader.GetValue(program_node, strProgram);
		if (!bRet)
			continue;

		int over_write = 1;
		m_xml_reader.GetAttr(program_node, "over_write", over_write);

		// 文件都放在解压缩后的CopyFile目录下
		char src_file[MAX_PATH];
		CPF::GetModuleOtherFileName(src_file, MAX_PATH, "CopyFile", strProgram.c_str());

		strDstDir += "/" ;
		strDstDir += strProgram.c_str();

		std::string str_dst_file_backup;
		str_dst_file_backup = strDstDir + "." + CPF::get_curtime_string(5);

		BOOL bFailIfExists = over_write ? FALSE : TRUE;
		BOOL b = CopyFile(strDstDir.c_str(), str_dst_file_backup.c_str(), bFailIfExists);
		b = CopyFile(src_file, strDstDir.c_str(), bFailIfExists);
#ifdef OS_WINDOWS
		if (!b)
		{
			DWORD err = GetLastError();
		}
/*#else
	
		bool bExist = true;
		FILE* fp = fopen(strDstDir.c_str(), "r");
		if (NULL == fp)
			bExist = false;
		else
			fclose(fp);

		if (!over_write && bExist)
			continue;

		char str_cmd[MAX_PATH];
		sprintf(str_cmd, "cp %s %s %s", over_write?"-R":" ", strDstDir.c_str(), str_dst_file_backup.c_str());
		system(str_cmd);
		
		sprintf(str_cmd, "cp %s %s %s", over_write?"-R":" ", src_file, strDstDir.c_str());
		system(str_cmd);
*/
#endif

		
	}

	return 0;
}

int CUpgradeTool::StartProgram()
{
	TiXmlNode * start_proram_node = GetCurrFuncNode("start_proram");

	if( !start_proram_node )
	{
		return -1;
	}

	for(TiXmlNode * program_node = start_proram_node->FirstChildElement("program");
		program_node;
		program_node = program_node->NextSiblingElement("program"))
	{
		int onoff = 1;

		m_xml_reader.GetAttr(program_node,"onoff",onoff);

		if( !onoff )
			continue;

		if( !CheckOSAttr(m_xml_reader,program_node) )
			continue;		

		CheckPresetup(m_xml_reader, program_node);

		{
			std::string str_program;

			m_xml_reader.GetValue(program_node,str_program);

			if( str_program.empty() )
				continue;

			//启动程序
			//...
			StartOneProgram(str_program.c_str());
		}

		{
			std::string setup_cmd;
			m_xml_reader.GetAttr(program_node,"setup",setup_cmd);

			if( !setup_cmd.empty() )
			{
				SetupOneProgram(setup_cmd.c_str());
			}
		}
	}

	int reboot = 0;
	m_xml_reader.GetAttr(start_proram_node,"reboot",reboot);

	if( reboot == 1 )
	{
		//重新启动机器
		CPF::MySystemShutdown(0x02);
	}

	return 0;
}


int CUpgradeTool::SetupOneProgram(const char * prg_name)
{
	MY_ACE_DEBUG(m_pLogInstance,
		(LM_ERROR,
		ACE_TEXT("\t SetupOneProgram = %s\n"), prg_name
		));
#ifdef OS_LINUX
	return SetupOneProgram_linux(prg_name);
#else
	std::string str_suf;

	CPF::GetSufFromFileName(prg_name,str_suf);

	if(stricmp(str_suf.c_str(),"bat") == 0 )
	{
		RunBat(prg_name,NULL,NULL);
	}
	else
	{
		int process_id = RunProcess(prg_name,NULL,NULL);

		if( process_id == -1 )
			return -1;

		while( 1 )
		{
			if( ACE::process_active(process_id) )
			{//等待程序结束

				ACE_OS::sleep(1);

				continue;
			}
			else
			{
				break;
			}

		}	
	}

	return 0;
#endif
}

int CUpgradeTool::StartOneProgram(const char * prg_name)
{
	MY_ACE_DEBUG(m_pLogInstance,
		(LM_ERROR,
		ACE_TEXT("\t StartOneProgram = %s\n"), prg_name
		));
#ifdef OS_LINUX
	return StartOneProgram_linux(prg_name);
#else
	std::string str_suf;

	CPF::GetSufFromFileName(prg_name,str_suf);

	if(stricmp(str_suf.c_str(),"bat") == 0 )
	{
		RunBat(prg_name,NULL,NULL);
	}
	else
	{
		int process_id = RunProcess(prg_name,NULL,NULL);

		if( process_id == -1 )
			return -1;
	}

	return 0;
#endif
}

#ifdef OS_WINDOWS

int CUpgradeTool::RunProcess(const char * szAppName, const char *szCmd, const char *szWorkingDirectory)
{
	if( !szAppName && !szCmd )
		return -1;

	ACE_Process rProcess;
	ACE_Process_Options rOpt;

	rOpt.handle_inheritance(false);

	if( szWorkingDirectory )
	{
		rOpt.working_directory(szWorkingDirectory);
	}

#ifdef OS_WINDOWS
	rOpt.creation_flags( CREATE_NEW_CONSOLE );
#endif

	if( szAppName && szCmd )
		rOpt.command_line("%s %s",szAppName,szCmd);
	else if( szAppName )
		rOpt.command_line("%s",szAppName);
	else if( szCmd )
		rOpt.command_line("%s",szCmd);

	return rProcess.spawn( rOpt );
}

int CUpgradeTool::RunBat(const char * szAppName, const char *szCmd, const char *szWorkingDirectory)
{
	if( !szAppName && !szCmd )
		return -1;

	SHELLEXECUTEINFO sei;
	memset(&sei, 0, sizeof(SHELLEXECUTEINFO)); 

	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpFile = szAppName;
	sei.nShow = SW_SHOWNORMAL;

	const char * path_name = NULL;
	if( CPF::IsFullPathName(szAppName) )
	{
		sei.lpDirectory = CPF::GetPathNameFromFullName(szAppName);
	}
	BOOL bRet = ShellExecuteEx(&sei);
	MY_ACE_DEBUG(m_pLogInstance,
		(LM_ERROR,
		ACE_TEXT("\t RunBat = %s, bret = %d, handle=%d\n"), szAppName, bRet, sei.hProcess
		));

	// 15秒超时
	DWORD timeout = 120 * 1000;
	DWORD dwRet = WaitForSingleObject(sei.hProcess, timeout);	//INFINITE);
	MY_ACE_DEBUG(m_pLogInstance,
		(LM_ERROR,
		ACE_TEXT("\t RunBat WaitForSingleObject OK = %s\n", (WAIT_OBJECT_0==dwRet)? "finished":"timeout" )
		));
	CloseHandle(sei.hProcess);
	return 0;
}
#endif

int CUpgradeTool::GetUpDateConfigDbCmdString(const char * config_db_update_sql_file_name,std::string& cmd_string)
{
	CTinyXmlEx xml_reader;

	db_param param;

	if( 0 != xml_reader.open_for_readonly(CONFIG_DB_CONFIG_FILE_NAME) )
	{
		param.strServer = "127.0.0.1";
		param.strDBName = "netems_config";
		param.strUsr = "TESTDB";
		param.strPwd = "dbtestok";
		param.strCharset = "GBK";
		param.nPort = 61000;
	}
	else
	{
		TiXmlNode * db_config_node = xml_reader.GetRootNode()->FirstChildElement("db_config");

		if( !db_config_node )
		{
			return -1;
		}

		if( !ReadDBParam(xml_reader,db_config_node,param) )
		{
			return -1;
		}
	}

	return GetUpdateDbCmdString(param,config_db_update_sql_file_name,cmd_string);

}

int CUpgradeTool::GetUpDateMysqlDbCmdString(const char * data_db_update_sql_file_name,std::string& cmd_string)
{
	CTinyXmlEx xml_reader;

	db_param param;

	if( 0 != xml_reader.open_for_readonly(DATA_DB_CONFIG_FILE_NAME) )
	{
		param.strServer = "127.0.0.1";
		param.strDBName = "mysql";
		param.strUsr = "root";
		param.strPwd = "dbtestok";
		param.strCharset = "GBK";
		param.nPort = 61000;
	}
	else
	{
		TiXmlNode * db_config_node = xml_reader.GetRootNode()->FirstChildElement("db_config");

		if( !db_config_node )
		{
			return -1;
		}

		if( !ReadDBParam(xml_reader,db_config_node,param) )
		{
			return -1;
		}

		param.strDBName = "mysql";
		param.strUsr = "root";

		if( stricmp(data_db_update_sql_file_name,"DB_UPDATE_MYSQL_MODIFY_ROOT_PASSWORD.SQL") != 0 )
			param.strPwd = "dbtestok";
		else
			param.strPwd = "";
	}

	return GetUpdateDbCmdString(param,data_db_update_sql_file_name,cmd_string);
}


int CUpgradeTool::GetUpDateDataDbCmdString(const char * data_db_update_sql_file_name,std::string& cmd_string)
{
	CTinyXmlEx xml_reader;

	db_param param;

	if( 0 != xml_reader.open_for_readonly(DATA_DB_CONFIG_FILE_NAME) )
	{
		param.strServer = "127.0.0.1";
		param.strDBName = "netems";
		param.strUsr = "TESTDB";
		param.strPwd = "dbtestok";
		param.strCharset = "GBK";
		param.nPort = 61000;
	}
	else
	{
		TiXmlNode * db_config_node = xml_reader.GetRootNode()->FirstChildElement("db_config");

		if( !db_config_node )
		{
			return -1;
		}

		if( !ReadDBParam(xml_reader,db_config_node,param) )
		{
			return -1;
		}
	}

	return GetUpdateDbCmdString(param,data_db_update_sql_file_name,cmd_string);
}


BOOL CUpgradeTool::ReadDBParam(CTinyXmlEx& xml_reader,TiXmlNode * parent_node,
						db_param& param)
{
	xml_reader.GetValueLikeIni(parent_node,"db_server",param.strServer);

	if( param.strServer.empty() )
	{
		param.strServer = "127.0.0.1";
	}

	xml_reader.GetValueLikeIni(parent_node,"db_name",param.strDBName);
	xml_reader.GetValueLikeIni(parent_node,"user",param.strUsr);
	xml_reader.GetValueLikeIni(parent_node,"password",param.strPwd);
	xml_reader.GetValueLikeIni(parent_node,"char_set",param.strCharset);

	if( param.strCharset.empty() )
	{
		param.strCharset = "GBK";
	}

	xml_reader.GetValueLikeIni(parent_node,"port",param.nPort);

	return true;
}

int CUpgradeTool::GetUpdateDbCmdString(const db_param& param,const char * sql_script_file,std::string& cmd_string)
{
	//c:\unierm\mysql\bin\mysql --host=127.0.0.1   --database=NetEMS     --port=61000  -u TESTDB --password=dbtestok  < db_update_netems_v1.0.0.43.sql > c:/unierm/mysql.out

	char full_sql_script_file_name[MAX_PATH] = {0};

	CPF::GetModuleOtherFileName(full_sql_script_file_name,
		sizeof(full_sql_script_file_name),
		"data_migration",sql_script_file);

	char cmd_line[2048]={0};

	if( param.strPwd.empty() )
	{
		sprintf(cmd_line,
			"%s --host=%s --database=%s --port=%d  -u%s < \"%s\" > \"%s\"",
			MYSQL_EXE_PATHNAME,
			param.strServer.c_str(),
			param.strDBName.c_str(),
			param.nPort,
			param.strUsr.c_str(),
			full_sql_script_file_name,
			OUT_FILE_PATHNAME
			);
	}
	else
	{
		sprintf(cmd_line,
			"%s --host=%s --database=%s --port=%d  -u%s -p%s  < \"%s\" > \"%s\"",
			MYSQL_EXE_PATHNAME,
			param.strServer.c_str(),
			param.strDBName.c_str(),
			param.nPort,
			param.strUsr.c_str(),
			param.strPwd.c_str(),
			full_sql_script_file_name,
			OUT_FILE_PATHNAME
			);
	}


	cmd_string = cmd_line;

	return 0;

}

int CUpgradeTool::RunUpdateCmd(const char * full_cmd_string)
{
	{
		char szFullName[MAX_PATH];
		strcpy(szFullName, CPF::GetModuleFullFileName("update_db.bat"));

		FILE * myfile = fopen(szFullName,"wb");

		fwrite(full_cmd_string,1,strlen(full_cmd_string),myfile);

		fclose(myfile);
		myfile = NULL;

		CPF::RunBat(szFullName, NULL, NULL);

		ACE_OS::sleep(10);
	}

	return 0;

}
TiXmlNode *	CUpgradeTool::GetCurrFuncNode(const char * func_name)
{
	TiXmlNode * func_node_this_version = 
		m_pCurUpdateNode->FirstChildElement(func_name);

	TiXmlNode * func_node = NULL;

	if( func_node_this_version )
	{
		int same_as_common = 0;

		m_xml_reader.GetAttr(func_node_this_version,"same_as_common",same_as_common);

		if( same_as_common )
		{
			func_node = m_pCommonUpdateNode->FirstChildElement(func_name);
		}
		else
		{
			func_node = func_node_this_version;
		}		
	}
	else//如果本节点没有，则调用common中对应的节点
	{
		func_node = m_pCommonUpdateNode->FirstChildElement(func_name);
	}

	return func_node;
}

BOOL CUpgradeTool::CheckOSAttr(CTinyXmlEx& xml_reader,TiXmlNode * node)
{
	int blinux=1;
	xml_reader.GetAttr(node,"linux",blinux);

	if( blinux == 0 )
	{
#ifdef OS_LINUX
		return false;
#endif
	}

	int bwindows=1;
	xml_reader.GetAttr(node,"windows",bwindows);
	if( bwindows ==0 )
	{
#ifdef OS_WINDOWS
		return false;
#endif
	}

	return true;
}


#ifdef OS_LINUX
int CUpgradeTool::SetupOneProgram_linux(const char * prg_name)
{
//	在当前创建一个setup_tmp目录，将压缩包解进去，然后运行probe_autorun.sh脚本，在将setup_tmp目录删除

	if (NULL == prg_name)
		return -1;
	FILE *fp = fopen(prg_name, "r");
	if (NULL == fp)
	{
		printf("\n SetupOneProgram_linux, [%s] not exist!!!\n", prg_name);
		return -1;
	}
	fclose(fp);

	printf("\n SetupOneProgram_linux, ready instal [%s]\n", prg_name);

/*
	传入进来的文件，有可能是真实的可执行程序，也可能是我们用tar压缩仿造的程序，
	在这里先用file对文件属性进行判断，如果是executable，则直接执行，否则在解压缩运行脚本
*/
	char strFileCmd[512];
	sprintf(strFileCmd, "file %s | grep executable | cut -f1 -d:", prg_name);
	FILE* fp1 = popen(strFileCmd, "r");
	if (!fp)
		return -1;
	bool bExe = ( 0 != fgets(strFileCmd, 512, fp1));
	fclose(fp1);

	if (bExe)
	{
		// 直接执行
		sprintf(strFileCmd, "./%s", prg_name);
		system(strFileCmd);
		return 0;
	}
	

	char strPath[MAX_PATH];
	char strOldPath[MAX_PATH];

	// 记录当前工作路径
	ACE_OS::getcwd((char*)strOldPath, MAX_PATH);

	// 删除临时目录，将压缩包解压缩到临时路径中
	system("rm -rf /tmp/setup_tmp");
	ACE_OS::mkdir("/tmp/setup_tmp");
	sprintf(strPath, "tar zxf %s -C /tmp/setup_tmp/", prg_name);
	system(strPath);
	ACE_OS::chdir("/tmp/setup_tmp");

	// 运行自动安装脚本，返回原工作路径
	system("./probe_autorun.sh");

	ACE_OS::chdir(strOldPath);	// 返回开始的目录
	return 0;

}



int CUpgradeTool::StartOneProgram_linux(const char * prg_name)
{
	printf("\n StartOneProgram = %s\n", prg_name);
	char strPath[512];
	char strFile[128];
	char strOldPath[128];
	if (NULL == prg_name)
		return -1;
	strcpy(strPath, prg_name);

	ACE_OS::getcwd((char*)strOldPath, 128);
	// 先进入目录，在启动程序
	char* pPos = strrchr(strPath, '/');
	if (NULL != pPos)
	{
		*pPos = '\0';
		int iRtn = chdir(strPath);
		printf("chdir = %s, Rtn = %d\n", strPath, iRtn);

		// 将名字copy过去
		pPos ++;
		sprintf(strFile, "./%s", pPos);
	}
	else
	{
		sprintf(strFile, "./%s", strPath);
	}

	/*return*/ system(strFile); // 这个返回值并不是pid

	chdir(strOldPath);	// 返回开始的目录
	return 0;

}

#endif


int CUpgradeTool::UpdateDB()
{
	TiXmlNode * update_db_root = m_xml_reader.GetRootNode()->FirstChildElement("sql_update");
	if( !update_db_root )
	{
		return 0;
	}

	for(TiXmlNode * p_one_update = update_db_root->FirstChildElement("one_update");
		p_one_update;
		p_one_update = p_one_update->NextSiblingElement("one_update"))
	{
		// 读版本和include属性
		int i_include = 0;
		std::string str_version;
		m_xml_reader.GetAttr(p_one_update, "include", i_include);
		m_xml_reader.GetAttr(p_one_update, "version", str_version);
		if (str_version.empty())
		{
			ACE_ASSERT(false);
			continue;
		}

		// 小于则运行，等于并且include也运行
		int i_cmp = CVersionCompInfo::CompareVersion(m_str_cur_run_version, str_version);
		bool b_install =  (i_cmp<0) || ( (i_cmp==0) && (0!=i_include));
		if (!b_install)
			continue;

		UpdateDB(p_one_update);	
	}

	return 1;
}

int CUpgradeTool::CheckPresetup(CTinyXmlEx& xml_reader, TiXmlNode * p_program_node)
{
	std::string presetup_cmd;
	xml_reader.GetAttr(p_program_node, "presetup", presetup_cmd);

	if( !presetup_cmd.empty() )
	{
		SetupOneProgram(presetup_cmd.c_str());
		return 1;
	}

	return 0;
}
