
#include "StdAfx.h"

#include "config_fc_server_xml.h"
#include "cpf/path_tools.h"
#include "cpf/strtools.h"
#include "cpf/other_tools.h"
#include "cpf/File_Data_Stream.h"

#include "config_center.h"

CConfigFcServer::CConfigFcServer()
{
	multi_thread = 1;
	max_client_nums = 10;

	exe_b_auto_start = 1;
	exe_sys_repotr_interval = 2*3600;//2个小时报告一次，为0表示不报告

	recv_timeout_ms = 50;
	send_timeout_ms = 3*1000;
}


CConfigFcServer::~CConfigFcServer()
{

}

bool CConfigFcServer::init(ACE_Log_Msg *pLogInstance)
{
	ACE_ASSERT(pLogInstance);
	m_pLogInstance = pLogInstance;

	char xml_file_name[MAX_PATH] = {0};

	CPF::GetModuleOtherFileName(xml_file_name,sizeof(xml_file_name),"ini","fc_server.xml");

	CTinyXmlEx xml_reader;

	if( 0 != xml_reader.open(xml_file_name) )
	{
		MY_ACE_DEBUG(pLogInstance, (LM_ERROR,
			ACE_TEXT("[Err][%D]Config_Center: File load failed [%s]\n"), xml_file_name ));
		return FALSE;
	}

	do 
	{
		if( !xml_reader.GetRootNode() )
		{
			break;
		}

		// common
		TiXmlNode * pcommon_node = xml_reader.GetRootNode()->FirstChildElement("common");
		ACE_ASSERT(pcommon_node);
		if( !pcommon_node )
		{
			break;
		}
		xml_reader.GetValueLikeIni(pcommon_node,"auto_start", exe_b_auto_start);
		xml_reader.GetValueLikeIni(pcommon_node,"cmd_line",exe_m_strCmdLine);
		xml_reader.GetValueLikeIni(pcommon_node,"sys_report_interval", exe_sys_repotr_interval);

		xml_reader.GetValueLikeIni(pcommon_node,"test_todb_performance",g_test_todb_performance);		

		// net_com
		TiXmlNode * net_com_node = xml_reader.GetRootNode()->FirstChildElement("net_com");
		ACE_ASSERT(net_com_node);
		if( !net_com_node )
		{
			break;
		}
		xml_reader.GetValueLikeIni(net_com_node,"multi_thread",multi_thread);
		xml_reader.GetValueLikeIni(net_com_node,"max_client_nums",max_client_nums);

		xml_reader.GetValueLikeIni(net_com_node,"recv_timeout_ms",recv_timeout_ms);
		xml_reader.GetValueLikeIni(net_com_node,"send_timeout_ms",send_timeout_ms);


		//读取probe文件存储配置
		return GetAllSaveParam(xml_reader);

	}while(0);


	MY_ACE_DEBUG(pLogInstance, (LM_ERROR,
		ACE_TEXT("[Err][%D]Config_Center: Configuration file format error  [%s]\n"), xml_file_name ));

	return false;
}

void CConfigFcServer::fini()
{

}

BOOL CConfigFcServer::GetAllSaveParam(CTinyXmlEx& xml_reader)
{
	get_fc_probe_config(xml_reader);

	return TRUE;
}

BOOL CConfigFcServer::get_fc_probe_config(CTinyXmlEx& xml_reader)
{
	TiXmlNode * probe_config_node = xml_reader.GetRootNode()->FirstChildElement("fc_probe_config");

	if( !probe_config_node )
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	get_servant_info(xml_reader, probe_config_node, fc_probe, "fc_probe");
	get_servant_info(xml_reader, probe_config_node, fc_monitor, "fc_monitor");


	return true;

}

void CConfigFcServer::get_servant_info(CTinyXmlEx& xml_reader, 
					  TiXmlNode * probe_config_node,
					  st_servant_info &servant,
					  const char* pitem)
{
	TiXmlNode * probe_node = probe_config_node->FirstChildElement(pitem);
	if( !probe_node )
	{
		ACE_ASSERT(FALSE);
		return;
	}
	xml_reader.GetValueLikeIni_HostOrderIP(probe_node,"ip_addr", servant.ip);
	xml_reader.GetValueLikeIni(probe_node,"listen_port",servant.port);
	xml_reader.GetValueLikeIni(probe_node,"servant_id",servant.cmd_servant_id);

	servant.live_servant_id = -1;
	xml_reader.GetValueLikeIni(probe_node,"live_servant_id",servant.live_servant_id);

	return;
}


CConfigFcServer	*	g_configFcServer = NULL;
bool				g_bInited =	false;

bool FCServerConfig_Initialize(ACE_Log_Msg *pLogInstance)
{
	if (g_bInited)
		return true;

	g_bInited = true;

	ConfigCenter_Initialize();

	g_configFcServer = new CConfigFcServer;

	g_configFcServer->init(pLogInstance);

	return true;
}

void FCServerConfig_UnInitialize()
{
	if( g_configFcServer )
	{
		delete g_configFcServer;
		g_configFcServer = NULL;
	}

	ConfigCenter_UnInitialize();

	g_bInited = false;

	return;
}

CConfigFcServer* FCServerConfig_Obj()
{
	if (!g_bInited)
	{
		ACE_ASSERT(false);
		return NULL;
	}

	return g_configFcServer;
}