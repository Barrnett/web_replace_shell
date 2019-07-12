//////////////////////////////////////////////////////////////////////////
//MyNTService.h

#pragma once

#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS

#include "ace/Event_Handler.h"
#include "ace/NT_Service.h"
#include "ace/Singleton.h"
#include "ace/Mutex.h"
#include <string>
#include <map>
#include "cpf/cpf.h"

class CSubNTServiceBase;

class CPF_CLASS CMyNTService : public ACE_NT_Service
{
public:
  CMyNTService ();
  ~CMyNTService (void);

  //we must re-implement this func in order to add our layer
  virtual int open(void *args = 0);

  // We override <handle_control> to dispatch control handles
  virtual void handle_control (DWORD control_code);
  /*
  virtual int handle_timeout(const ACE_Time_Value &current_time, const void *act = 0)
  {
	  return 0;
  }
  

  virtual int handle_exception(ACE_HANDLE);
*/
  virtual int svc (void);
  // This is a virtual method inherited from ACE_NT_Service.


  //TODO:
  int init(ACE_Log_Msg * pLogInstance);
  int close();

  //Some map related operations 
  int insert_subsvc(const char * svcName, CSubNTServiceBase * pSubSvc);
  int remove_subsvc(const char * svcName);
  int remove_all();

protected:
	void stop_all_sub_service();

protected:
  typedef ACE_NT_Service inherited;

  int stop_;

  typedef std::map<std::string, CSubNTServiceBase *> SUBSVC_MAP;

  typedef SUBSVC_MAP::iterator SUBSVC_MAP_ITERATOR;

  SUBSVC_MAP		subSvcMap;
  ACE_Log_Msg  *	m_pLogInstance;
};


#endif//OS_WINDOWS



