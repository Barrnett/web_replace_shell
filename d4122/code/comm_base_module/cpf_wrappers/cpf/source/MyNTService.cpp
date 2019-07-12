//////////////////////////////////////////////////////////////////////////
//MyNTService.cpp

#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS

#include "ace/Reactor.h"
#include "cpf/MyNTService.h"
#include "cpf/SubNTServiceBase.h"

CMyNTService::CMyNTService ()
{
	m_pLogInstance = NULL;

	stop_ = 0;
}

CMyNTService::~CMyNTService (void) 
{ 
	this->close();
}

//add some subsvc
int CMyNTService::init(ACE_Log_Msg * pLogInstance)
{
	m_pLogInstance = pLogInstance;
	return 0;
}

//co-oper with above init() to do some cleanup work 
int CMyNTService::close()
{
	this->remove_all(); 

	return 0;
}

//
//re-implement open of NT_Service
//
int CMyNTService::open(void *args)
{
	SUBSVC_MAP_ITERATOR itr = subSvcMap.begin();

	for (; itr != subSvcMap.end(); itr++)
	{
		int nRet = ((*itr).second)->start_svc();
	}

	return ACE_NT_Service::open();
}


// This method is called when the service gets a control request.  It
// handles requests for stop and shutdown by calling terminate ().
// All others get handled by calling up to inherited::handle_control.

void
CMyNTService::handle_control (DWORD control_code)
{
	//report state right now!!
	switch (control_code)
	{
	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		//this->stop_requested (control_code);
		report_status (SERVICE_STOP_PENDING);
		this->stop_ = 1;
		break;

	case SERVICE_CONTROL_PAUSE:
	case SERVICE_CONTROL_CONTINUE:
		{
			SUBSVC_MAP_ITERATOR itr = subSvcMap.begin();
			for (; itr != subSvcMap.end(); itr++)
			{
				((*itr).second)->handle_control(control_code);
			}   
		}
		break;

	case SERVICE_CONTROL_INTERROGATE:
		//this->interrogate_requested (control_code);
		report_status(0);
		break;
	}

	MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG, ACE_TEXT ("[Info][%D]CMyNTService control: %d\n"), control_code));

}

int
CMyNTService::svc (void)
{
	MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG,
		ACE_TEXT ("[Info][%D]CMyNTService::svc CMyNTService begin running\n")));


	report_status (SERVICE_RUNNING);

	this->stop_ = 0;

	while (!this->stop_)
	{
		ACE_OS::sleep(ACE_Time_Value(2,0));
	}

	stop_all_sub_service();

	MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG, ACE_TEXT ("[Info][%D]CMyNTService Shutting down\n")));

	return 0;
}

int CMyNTService::insert_subsvc(const char * svcName, CSubNTServiceBase * pSubSvc)
{
	ACE_ASSERT(pSubSvc);
	if (pSubSvc)
	{
		SUBSVC_MAP_ITERATOR itr = subSvcMap.find(((std::string)svcName));
		if (itr != subSvcMap.end())
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG,
				ACE_TEXT ("[Err][%D]insert sub_service '%s' failed\n"),svcName));

			//this key string has existed!!
			return -1;
		}
		else
		{
			MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG,
				ACE_TEXT ("[Info][%D]insert sub_service '%s' success\n"),svcName));

			subSvcMap[(std::string)svcName] = pSubSvc;

			return 0;
		}
	}

	//Null ptr
	return -1;
}

int CMyNTService::remove_subsvc(const char *svcName)
{
	SUBSVC_MAP_ITERATOR itr = subSvcMap.find((std::string)svcName);
	if (itr != subSvcMap.end())
	{
		MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG,
			ACE_TEXT ("[Info][%D]remove sub_service '%s' success\n"),svcName));

		subSvcMap.erase(itr);
		return 0;
	}

	MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG,
		ACE_TEXT ("[Err][%D]remove sub_service '%s' failed\n"),svcName));

	return -1;
}

int CMyNTService::remove_all()
{
	subSvcMap.clear();

	return 0;
}


void CMyNTService::stop_all_sub_service()
{
	SUBSVC_MAP_ITERATOR itr = subSvcMap.begin();
	for (; itr != subSvcMap.end(); itr++)
	{
		((*itr).second)->stop_svc();
	}   

	return;
}

#endif//OS_WINDOWS


