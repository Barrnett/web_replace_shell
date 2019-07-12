//////////////////////////////////////////////////////////////////////////
//config_fc_server_xml.h

#pragma once

#include "cpf/TinyXmlEx.h"
#include "fc_server_export.h"


class ACE_Log_Msg;
class CConfigFcServer;

FCSERVERFACADE_API bool FCServerConfig_Initialize(ACE_Log_Msg *pLogInstance);
FCSERVERFACADE_API void FCServerConfig_UnInitialize();

FCSERVERFACADE_API CConfigFcServer* FCServerConfig_Obj();


class FCSERVERFACADE_CLASS CConfigFcServer
{
	typedef struct _servant_info
	{
		ACE_UINT32		ip;
		ACE_UINT16		port;
		int				cmd_servant_id;
		int				live_servant_id;
	}st_servant_info;

public:
	CConfigFcServer(void);
	virtual ~CConfigFcServer(void);

	bool init(ACE_Log_Msg *pLogInstance);
	void fini();


private:
	BOOL GetAllSaveParam(CTinyXmlEx& xml_reader);

	BOOL get_fc_probe_config(CTinyXmlEx& xml_reader);

	void get_servant_info(CTinyXmlEx& xml_reader, TiXmlNode * probe_config_node, st_servant_info &servant, const char* pitem);


public:

	ACE_Log_Msg*	m_pLogInstance;

	// fc_server exe
	std::string		exe_m_strCmdLine;
	int				exe_b_auto_start;
	int				exe_sys_repotr_interval;

	int				g_test_todb_performance;

	int				multi_thread;
	int				max_client_nums;

	int				recv_timeout_ms;
	int				send_timeout_ms;

	// fc_probe and fc_monitor
	st_servant_info	fc_probe;
	st_servant_info	fc_monitor;

	// pa_probe and pa_monitor
	st_servant_info	pa_probe;
	st_servant_info	pa_monitor;
	st_servant_info	pa_reptile;

};

