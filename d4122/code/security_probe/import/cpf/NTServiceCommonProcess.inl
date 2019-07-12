//////////////////////////////////////////////////////////////////////////
//CNTServiceCommonProcess.inl

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/streams.h"
#include "cpf/path_tools.h"
#include "ace/Mutex.h"
#include "ace/Init_ACE.h"

// Default for the -i (install) option
#define DEFAULT_SERVICE_INIT_STARTUP     SERVICE_AUTO_START

static BOOL __stdcall
ConsoleHandler (DWORD ctrlType)
{
	ACE_UNUSED_ARG (ctrlType);
	//PROCESS::instance()->GetService()->handle_control (SERVICE_CONTROL_STOP);
	if( PROCESS::instance()->service )
	{
		PROCESS::instance()->service->handle_control (SERVICE_CONTROL_STOP);
	}
	
	return TRUE;
}

CNTServiceCommonProcess::CNTServiceCommonProcess ()
			: opt_install (0),
			opt_remove (0),
			opt_start (0),
			opt_kill (0),
			opt_type (0),
			opt_debug (0),
			opt_startup (0),
			service(0),
			m_pLogInstance(NULL)
{
	ACE_OS::strcpy (progname,"service");
	
	ACE::init ();
}

CNTServiceCommonProcess::~CNTServiceCommonProcess (void)
{
	ACE::fini ();
}

int CNTServiceCommonProcess::init(ACE_Log_Msg *pLogInstance,
								  ACE_NT_Service* psvc,
								  const char * name,
								  const char * descname)
{
	m_pLogInstance = pLogInstance;

	ACE_ASSERT(psvc);

	this->service = psvc;
	this->svcname = name;
	this->svcdscname = descname;

	return 0;
}

void CNTServiceCommonProcess::print_usage_and_die (void)
{
	MY_ACE_DEBUG (m_pLogInstance,(LM_INFO,
		"Usage: %s"
		" -in -r -s -k -tn -d\n"
		"  -i: Install this program as an NT service, with specified startup\n"
		"  -r: Remove this program from the Service Manager\n"
		"  -s: Start the service\n"
		"  -k: Kill the service\n"
		"  -t: Set startup for an existing service\n"
		"  -d: Debug; run as a regular application\n",
		progname,
		0));
	ACE_OS::exit(1);
}

void CNTServiceCommonProcess::parse_args (int argc, ACE_TCHAR* argv[])
{
	ACE_Get_Opt get_opt (argc, argv, ACE_TEXT ("i:rskt:d"));
	int c;

	while ((c = get_opt ()) != -1)
		switch (c)
	{
		case 'i':
			opt_install = 1;
			opt_startup = ACE_OS::atoi (get_opt.opt_arg ());
			if (opt_startup <= 0)
				print_usage_and_die ();
			break;
		case 'r':
			opt_remove = 1;
			break;
		case 's':
			opt_start = 1;
			break;
		case 'k':
			opt_kill = 1;
			break;
		case 't':
			opt_type = 1;
			opt_startup = ACE_OS::atoi (get_opt.opt_arg ());
			if (opt_startup <= 0)
				print_usage_and_die ();
			break;
		case 'd':
			opt_debug = 1;
			break;
		default:
			// -i can also be given without a value - if so, it defaults
			// to defined value.
			if (ACE_OS::strcmp (get_opt.argv ()[get_opt.opt_ind () - 1], ACE_TEXT ("-i")) == 0)
			{
				opt_install = 1;
				opt_startup = DEFAULT_SERVICE_INIT_STARTUP;
			}
			else
			{
				print_usage_and_die ();
			}
			break;
	}
}

int CNTServiceCommonProcess::run (int argc, ACE_TCHAR* argv[])
{
	service->name (svcname.c_str(),svcdscname.c_str());

	parse_args (argc, argv);

	if (opt_install && !opt_remove)
	{
		if (-1 == service->insert (opt_startup))
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_ERROR, ACE_TEXT ("insert servece '%s' error %p\n"), svcname.c_str(),ACE_TEXT ("insert")));
			int nError = ACE_OS::last_error();
			return -1;
		}
		else
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_INFO,ACE_TEXT("insert service '%s' success\n"),svcname.c_str()));
		}

		return 0;
	}

	if (opt_remove && !opt_install)
	{
		if (-1 == service->remove ())
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_ERROR, ACE_TEXT ("remove service '%s' error %p\n"), svcname.c_str(),ACE_TEXT ("remove")));
			return -1;
		}
		else
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_INFO,ACE_TEXT("remove service '%s' success\n"),svcname.c_str()));
		}

		return 0;
	}

	if (opt_start && opt_kill)
		print_usage_and_die ();

	if (opt_start)
	{
		ACE_OS::last_error(0);

		DWORD svc_state = 0;

		if (-1 == service->start_svc (NULL,&svc_state))
		{
			DWORD dwError = ACE_OS::last_error();
			MY_ACE_DEBUG (m_pLogInstance,(LM_ERROR, ACE_TEXT ("start service '%s' error ,error id:(%u) %p\n"),svcname.c_str(),dwError,ACE_TEXT ("start")));
			return -1;
		}
		else
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_INFO,ACE_TEXT("start service '%s' success\n"),svcname.c_str()));
		}
		return 0;
	}

	if (opt_kill)
	{
		if (-1 == service->stop_svc ())
		{
			DWORD dwError = ACE_OS::last_error();

			MY_ACE_DEBUG (m_pLogInstance,(LM_ERROR, ACE_TEXT ("stop service '%s' error,error id:(%u) %p\n"),svcname.c_str(),dwError,ACE_TEXT ("stop")));
			return -1;
		}
		else
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_INFO,ACE_TEXT("stop service '%s' success\n"),svcname.c_str()));
		}
		return 0;
	}

	if (opt_type)
	{
		if (-1 == service->startup (opt_startup))
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("set startup")));
			return -1;
		}
		return 0;
	}

	// If we get here, we either run the app in debug mode (-d) or are
	// being called from the service manager to start the service.

	if (opt_debug)
	{
		SetConsoleCtrlHandler (&ConsoleHandler, 1);
		service->open ();
	}
	else
	{
		MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG, ACE_TEXT ("(%t): Starting service.\n")));

		ACE_NT_SERVICE_REFERENCE(Fixed_Service_Name_20060824);
		MY_NT_SERVICE_RUN ((LPSTR)svcname.c_str(),
			                Fixed_Service_Name_20060824,
			                service,
			                ret);
		if (ret == 0)
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_ERROR,
			ACE_TEXT ("%p\n"),
			ACE_TEXT ("Couldn't start service")));
		}
		else
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG, ACE_TEXT ("(%t): Service start success.\n")));
		}
	}

	return 0;
}

