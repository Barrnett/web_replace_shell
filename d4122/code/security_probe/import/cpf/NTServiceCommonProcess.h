//////////////////////////////////////////////////////////////////////////
//NTServiceCommonProcess.h

#pragma once



#include "ace/NT_Service.h"
#include <string>

#define NT_SERVICE_DEFINE(SVCCLASS, SVCDESC)  \
		ACE_NT_SERVICE_DEFINE (Fixed_Service_Name_20060824, SVCCLASS, SVCDESC);


#define MY_NT_SERVICE_RUN(MY_UNIQUE_NAME,SVCNAME, SVCINSTANCE, RET)                      \
	ACE_TEXT_SERVICE_TABLE_ENTRY _ace_nt_svc_table[2] =                      \
  {                                                                        \
  ACE_NT_SERVICE_ENTRY(MY_UNIQUE_NAME, SVCNAME),                           \
	{ 0, 0 }                                                               \
  };                                                                       \
  _ace_nt_svc_obj_##SVCNAME = SVCINSTANCE;                                 \
  _ace_nt_svc_obj_##SVCNAME->capture_log_msg_attributes ();                \
  ACE_OS::last_error (0);                                                  \
  int RET = ACE_TEXT_StartServiceCtrlDispatcher(_ace_nt_svc_table);



//#define NT_SERVICE_DEFINE(SVCCLASS,SVCDESC) \
//	ACE_NT_SERVICE_DEFINE (Beeper, SVCCLASS, SVCDESC);

class CNTServiceCommonProcess
{
public:
	CNTServiceCommonProcess ();
	~CNTServiceCommonProcess (void);

	int init(ACE_Log_Msg *pLogInstance,
		ACE_NT_Service* psvc,
		const char * name,
		const char * descname);

	int run(int argc, ACE_TCHAR* argv[]);

	ACE_NT_Service * GetService()
	{
		return service;
	}

public:
	ACE_NT_Service* service;


public:
	void parse_args (int argc, ACE_TCHAR* argv[]);
	void print_usage_and_die (void);

public:
	char progname[128];
	std::string svcdscname;
	std::string svcname;

	int opt_install;
	int opt_remove;
	int opt_start;
	int opt_kill;
	int opt_type;
	int opt_debug;
	int opt_startup;
	ACE_Log_Msg *	m_pLogInstance;

};


typedef ACE_Singleton< CNTServiceCommonProcess, ACE_Mutex > PROCESS;

#include"cpf/NTServiceCommonProcess.inl"




#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Singleton<CNTServiceCommonProcess, ACE_Mutex>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Singleton<CNTServiceCommonProcess, ACE_Mutex>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */


