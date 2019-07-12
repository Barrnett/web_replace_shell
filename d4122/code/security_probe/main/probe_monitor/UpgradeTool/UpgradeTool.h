///////////////////////////////////////////////////////////////////////////
//UpgradeTool.h

#include "MonitorTask.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/ACE_Log_File_Msg.h"

class CUpgradeTool
{
public:
	CUpgradeTool();
	~CUpgradeTool();

	BOOL init(ACE_Log_File_Msg& log_file);
	void close();
	
public:
	//返回-1表示出错，不能升级
	//返回0表示可以升级
	//返回1表示没有符合的版本，不能升级	
	int CheckVersion();

	int StopOldProgram();
	int DeleteOldFile();
	int UpdateDB();
	int SetupProgram();
	int CopyProgram();
	int StartProgram();

	int UpdateDB(TiXmlNode * p_one_update);//, std::string &str_full_update_cmd);
	bool UpdateDB_oneline(TiXmlNode* update_db_list, int i_type);//, std::string& str_full_update_cmd);

private:
	CTinyXmlEx		m_xml_reader;
	TiXmlNode *		m_pCurUpdateNode;
	TiXmlNode *		m_pCommonUpdateNode;

private:
	//m_str_cur_run_version当前运行的版本，要从这个版本升级或者回退到m_str_new_version
	//如果m_str_cur_run_version<=m_str_new_version表示要正向升级
	//如果m_str_cur_run_version>m_str_new_version表示要反向升级
	std::string m_str_cur_run_version;
	std::string m_str_new_version;

public:
	const std::string& GetCurRunVersion() const
	{
		return m_str_cur_run_version;
	}

	const std::string& GetNewVersion() const
	{
		return m_str_new_version;
	}

private:
	TiXmlNode *	GetCurrFuncNode(const char * func_name);

private:
	int SetupOneProgram(const char * prg_name);
	int StartOneProgram(const char * prg_name);


private:
#ifdef OS_WINDOWS
	int RunProcess(const char * szAppName, const char *szCmd, const char *szWorkingDirectory);
	int RunBat(const char * szAppName, const char *szCmd, const char *szWorkingDirectory);
#else
	int SetupOneProgram_linux(const char * prg_name);
	int StartOneProgram_linux(const char * prg_name);
#endif

private:
	typedef struct tag_VERSION_RANGE
	{
		tag_VERSION_RANGE()
		{
			but_from = but_to = 0;
		}

		std::string from_version;
		int			but_from;//是否是不包含from_version,即是[from_version  还是(from_version

		std::string	to_verison;
		int			but_to;//是否是不包含to_version,即是[to_verison  还是(to_verison

	}VERSION_RANGE;

private:
	void ReadSupportVersionList(
		CTinyXmlEx& xml_reader,
		TiXmlNode * parent_node,
		std::vector< VERSION_RANGE >& vt_supp_version,
		const std::string& this_version);//this_version表示upgrade_tool.xml本身的代表的软件版本

	int ReadVersionAttr(CTinyXmlEx& xml_reader,
		TiXmlNode * node,const char * att,
		std::string& version,
		const std::string& this_version);//this_version表示upgrade_tool.xml本身的代表的软件版本

	int ReadVersion(const char * version_file_pos,std::string& str_version);

	BOOL CheckOldVersion(
		const std::string& old_version,
		const std::vector< VERSION_RANGE >& vt_supp_version);


private:
	typedef struct tagdb_param
	{
		std::string	strServer;
		std::string strDBName;
		std::string	strUsr;
		std::string strPwd;
		std::string	strCharset;
		int			nPort;
	}db_param;

private:
	BOOL ReadDBParam(CTinyXmlEx& xml_reader,TiXmlNode * parent_node,
		db_param& param);

private:
	int GetUpDateConfigDbCmdString(const char * config_db_update_sql_file_name,std::string& cmd_string);
	int GetUpDateDataDbCmdString(const char * data_db_update_sql_file_name,std::string& cmd_string);
	int GetUpDateMysqlDbCmdString(const char * data_db_update_sql_file_name,std::string& cmd_string);

	int GetUpdateDbCmdString(const db_param& param,const char * sql_script_file,std::string& cmd_string);

private:
	int RunUpdateCmd(const char * full_cmd_string);

	BOOL CheckOSAttr(CTinyXmlEx& xml_reader,TiXmlNode * node);
	int CheckPresetup(CTinyXmlEx& xml_reader, TiXmlNode * p_program_node);

private:
	ACE_Log_File_Msg *	m_pLogInstance;

};

