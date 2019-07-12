//////////////////////////////////////////////////////////////////////////
//ACE_Log_File_Msg.h

#pragma once

#include "cpf/cpf.h"
#include "ace/Log_Msg.h"

class CPeriod_File_Callback;

//将log到文件中
class CPF_CLASS ACE_Log_File_Msg : public ACE_Log_Msg
{
public:
	ACE_Log_File_Msg(void);
	virtual ~ACE_Log_File_Msg(void);

	//interval：多长时间生成一个新的文件（单位是分钟，新的文件名在以前的文件名基础上加上时间）。
	//如果interval==0，表示不生成新的文件。一天一个:interval=24*60
	//bClearAll:是否清除以前的log文件。
	//如果filename==NULL,则使用执行程序名字+".log".
	BOOL init(const char * filename,BOOL bClearAll=false,unsigned int interval = 0, BOOL bEnterThreadMutex=false, int iHistory = 0);

	//生成path/appname.suf的log文件, 如果path=NULL, 表示在当前目录。如果suf=NULL,表示不要后缀
	BOOL init_use_appname(const char * path,const char * suf,BOOL bClearAll=false,unsigned int interval = 0,BOOL bEnterThreadMutex=false , int iHistory = 30);

	void fini();

	BOOL init_oneday_log(const char * filename,BOOL bClearAll=false,BOOL bEnterThreadMutex=false, int iHistory = 30)
	{
		return init(filename,bClearAll,24*60,bEnterThreadMutex, iHistory);
	}

	BOOL init_onehour_log(const char * filename,BOOL bClearAll=false,BOOL bEnterThreadMutex=false, int iHistory = 15)
	{
		return init(filename,bClearAll,60,bEnterThreadMutex, iHistory);
	}

private:
	BOOL Inner_init(const char * filename,BOOL bClearAll,unsigned int interval,BOOL bEnterThreadMutex=false, int iHistory=0);

private:
	CPeriod_File_Callback *	m_pPeriod_File_Callback;

};



#include "ace/Singleton.h"
#include "ace/Null_Mutex.h"

class CPF_CLASS ACE_Log_File_Msg_Singleton : public ACE_Singleton<ACE_Log_File_Msg, ACE_Null_Mutex>
{
public:
	static bool m_bFirst;

	static ACE_Log_File_Msg* instance();
	static ACE_Log_File_Msg* instance_init_use_appname(BOOL bClearAll=false,unsigned int interval = 0, BOOL bEnterThreadMutex=true, int iHistory = 30);
	static ACE_Log_File_Msg* instance_init_file(const char* pszFilePath,BOOL bClearAll=false,unsigned int interval = 0, BOOL bEnterThreadMutex=true, int iHistory = 30);
};

