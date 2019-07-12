//////////////////////////////////////////////////////////////////////////
//ACE_Log_File_Msg.cpp

#include "cpf/ACE_Log_File_Msg.h"
#include "cpf/other_tools.h"
#include "cpf/path_tools.h"
#include "ace/Log_Msg_Callback.h"
#include "ace/Log_Record.h"
#include "fstream"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Guard_T.h"

class CPeriod_File_Callback : public ACE_Log_Msg_Callback
{
public:
	CPeriod_File_Callback();
	virtual ~CPeriod_File_Callback();

public:
	BOOL init(const char * filename,BOOL bClearAll=false,unsigned int interval = 0,BOOL bEnterThreadMutex= false, int iHistory=0);
	void fini();

	BOOL init_oneday_log(const char * filename,BOOL bClearAll=false,BOOL bEnterThreadMutex= false, int iHistory=0)
	{
		return init(filename,bClearAll,24*60,bEnterThreadMutex, iHistory);
	}

	BOOL init_onehour_log(const char * filename,BOOL bClearAll=false,BOOL bEnterThreadMutex= false, int iHistory=0)
	{
		return init(filename,bClearAll,60,bEnterThreadMutex, iHistory);
	}

protected:
	virtual void log(ACE_Log_Record& log_record);

private:
	BOOL Create_New_Log();
	BOOL Create_New_Log(time_t curtime);

private:
	std::string		m_str_filename;//文件名，不带后缀
	std::string		m_str_sufname;//文件名的后缀

	time_t			m_last_time;

	unsigned int	m_interval;
	BOOL			m_bClearAll;
	BOOL			m_bEnterThreadMutex;

	std::ofstream * m_pLogOutFile;

	int				m_iHistoryDay;	// 保留多少天的文件，<=0为不删除
private:
	ACE_Recursive_Thread_Mutex	m_tm;

};

CPeriod_File_Callback::CPeriod_File_Callback()
{
	m_interval = 0;
	m_bClearAll = false;

	m_last_time = 0;

	m_pLogOutFile = NULL;

	m_iHistoryDay = 0;
}

CPeriod_File_Callback::~CPeriod_File_Callback()
{
	fini();

	return;
}


BOOL CPeriod_File_Callback::init(const char * filename,BOOL bClearAll,unsigned int interval,BOOL bEnterThreadMutex, int iHistory)
{
	const char * temp1 = CPF::GetFileNameFromFullName(filename);

	if( !temp1 )
	{
		return false;
	}

	const char * temp2 = strchr(temp1,'.');

	if( !temp2 )
	{
		m_str_filename = filename;
	}
	else
	{
		m_str_filename.assign(filename,temp2-filename);
	}

	CPF::GetSufFromFileName(filename,m_str_sufname);

	m_interval = interval*60;

	m_bClearAll = bClearAll;
	m_bEnterThreadMutex = bEnterThreadMutex;
	m_iHistoryDay = iHistory;

	m_last_time = 0;

	if( !Create_New_Log() )
	{
		return false;
	}

	return true;

}
void CPeriod_File_Callback::fini()
{
	if( m_pLogOutFile )
	{
		m_pLogOutFile->close();
		delete m_pLogOutFile;

		m_pLogOutFile = NULL;
	}
	
	return;
}

BOOL CPeriod_File_Callback::Create_New_Log()
{
	if( 0 == m_interval )
	{
		 if( m_last_time != 0 )
		 {//已经创建过了的

			 return true;
		 }
		 else
		 {
			 m_last_time = ACE_OS::gettimeofday().sec();

			 return Create_New_Log(0);
		 }

		return true;
	}
	else
	{
		time_t cur_time = ACE_OS::gettimeofday().sec();

		if( cur_time / m_interval == m_last_time / m_interval )
		{//时间段相同，不需要生成新的文件名

			return true;
		}

		cur_time = cur_time / m_interval * m_interval;

		if( Create_New_Log(cur_time) )
		{
			m_last_time = cur_time;

			return true;
		}
	}

	return false;
}
	
BOOL CPeriod_File_Callback::Create_New_Log(time_t curtime)
{
	char buf_filename[MAX_PATH];

	if( 0 == curtime )
	{
		if( m_str_sufname.empty() )
		{
			sprintf(buf_filename,"%s",m_str_filename.c_str());
		}
		else
		{
			sprintf(buf_filename,"%s.%s",m_str_filename.c_str(),m_str_sufname.c_str());
		}
	}
	else
	{
		// 需要创建文件之前，检查一下是否需删除以前的就文件
		if (m_iHistoryDay > 0)
		{
			//从一个完整文件名中,得到除了文件名之外的全路径名,不包含最后一个 '\\'
			std::string str_path = CPF::GetPathNameFromFullName(m_str_filename.c_str());

			//从一个完整文件名中,得到没有路径的文件名,
			//得到的指针还是指着strPathFile里面的内容
			std::string str_file = CPF::GetFileNameFromFullName(m_str_filename.c_str());
			str_file += "_";

			CPF::DeleteTimerLogFile(str_path.c_str(), str_file.c_str(), m_str_sufname.c_str(), m_iHistoryDay*24*3600);

		}
		const char * ptemp = CPF::FormatTime(5,curtime);

		if( m_str_sufname.empty() )
		{
			sprintf(buf_filename,"%s_%s",m_str_filename.c_str(),ptemp);
		}
		else
		{
			sprintf(buf_filename,"%s_%s.%s",m_str_filename.c_str(),ptemp,m_str_sufname.c_str());
		}
	}


	if( m_pLogOutFile )
	{
		m_pLogOutFile->close();
		delete m_pLogOutFile;

		m_pLogOutFile = NULL;
	}

	std::ofstream * pLogOutFile = NULL;
	
	if( m_bClearAll )
	{
		pLogOutFile = new std::ofstream(buf_filename,std::ios::out|std::ios::trunc);
	}
	else
	{
		pLogOutFile = new std::ofstream(buf_filename,std::ios::out|std::ios::app);		
	}

	if( !pLogOutFile )
	{
		return false;
	}

	if( !pLogOutFile->good() )
	{
		delete pLogOutFile;
		return false;
	}

	m_pLogOutFile = pLogOutFile;
	
	return true;
}

void CPeriod_File_Callback::log(ACE_Log_Record& log_record)
{
	if( m_bEnterThreadMutex )
	{
		m_tm.acquire();
	}
	
	if( Create_New_Log() )
	{
		if( m_pLogOutFile )
		{
			log_record.print("",0,*m_pLogOutFile);
		}
	}

	if( m_bEnterThreadMutex )
	{
		m_tm.release();
	}

	return;

}


//////////////////////////////////////////////////////////////////////////
//ACE_Log_File_Msg

ACE_Log_File_Msg::ACE_Log_File_Msg()
{
	m_pPeriod_File_Callback = NULL;
}

ACE_Log_File_Msg::~ACE_Log_File_Msg()
{
	fini();
}

BOOL ACE_Log_File_Msg::init(const char * filename,BOOL bClearAll,unsigned int interval,BOOL bEnterThreadMutex, int iHistory)
{
	if( filename==NULL || filename[0] == 0 )
	{
		return init_use_appname(CPF::GetModulePathName(),"log",bClearAll,interval,bEnterThreadMutex, iHistory);
	}
	else
	{		
		if( !CPF::IsFullPathName(filename) )
		{
			filename = CPF::GetModuleFullFileName(filename);
		}

		return Inner_init(filename,bClearAll,interval,bEnterThreadMutex,iHistory);

	}

	ACE_ASSERT(FALSE);

	return false;
}


//生成path/appname.suf的log文件
BOOL ACE_Log_File_Msg::init_use_appname(const char * path,const char * suf,BOOL bClearAll,unsigned int interval,BOOL bEnterThreadMutex, int iHistory)
{
	if( path == NULL )
	{
		path = CPF::GetModulePathName();
	}

	const char * pFullName = CPF::GetModuleFileName();

	const char * pFileName = CPF::GetFileNameFromFullName(pFullName);

	const char * ptempName = CPF::JoinPathToPath(path,pFileName);

	std::string strLogName;

	CPF::ReplaceSufFromFileName(ptempName,suf,strLogName);

	return Inner_init(strLogName.c_str(),bClearAll,interval,bEnterThreadMutex,iHistory);

}


BOOL ACE_Log_File_Msg::Inner_init(const char * filename,BOOL bClearAll,unsigned int interval,BOOL bEnterThreadMutex, int iHistory)
{
//	ACE_ASSERT(!m_pPeriod_File_Callback);

	if( m_pPeriod_File_Callback )
	{
		delete m_pPeriod_File_Callback;
		m_pPeriod_File_Callback = NULL;
	}

	m_pPeriod_File_Callback = new CPeriod_File_Callback;

	if( !m_pPeriod_File_Callback )
	{
		return false;
	}

	if( !m_pPeriod_File_Callback->init(filename,bClearAll,interval,bEnterThreadMutex, iHistory) )
	{
		delete m_pPeriod_File_Callback;
		m_pPeriod_File_Callback = NULL;

		return false;
	}

	this->set_flags(ACE_Log_Msg::MSG_CALLBACK);
	this->clr_flags(ACE_Log_Msg::STDERR);

	this->msg_callback(m_pPeriod_File_Callback);

	return true;
}

void ACE_Log_File_Msg::fini()
{
	this->clr_flags(ACE_Log_Msg::MSG_CALLBACK);

	if( m_pPeriod_File_Callback )
	{
		delete m_pPeriod_File_Callback;
		m_pPeriod_File_Callback = NULL;
	}

	return;
}



bool ACE_Log_File_Msg_Singleton::m_bFirst = true;

ACE_Log_File_Msg* ACE_Log_File_Msg_Singleton::instance()
{
	if (m_bFirst)
		return instance_init_use_appname(false,24*60);
	else
		return ACE_Singleton<ACE_Log_File_Msg, ACE_Null_Mutex>::instance();
}

ACE_Log_File_Msg* ACE_Log_File_Msg_Singleton::instance_init_use_appname(BOOL bClearAll,unsigned int interval, BOOL bEnterThreadMutex, int iHistoryDay)
{
	ACE_Log_File_Msg* pInstance = ACE_Singleton<ACE_Log_File_Msg, ACE_Null_Mutex>::instance();

	if (m_bFirst)
	{
		char log_path[MAX_PATH];

		CPF::GetModuleFullFileName(log_path, sizeof(log_path), "log");
		CPF::CreateFullDirecory(log_path);
	
		pInstance->init_use_appname(log_path, "log", bClearAll, interval, bEnterThreadMutex, iHistoryDay);

		m_bFirst = false;
	}

	return pInstance;
}

ACE_Log_File_Msg* ACE_Log_File_Msg_Singleton::instance_init_file(const char* pszFilePath,BOOL bClearAll,unsigned int interval, BOOL bEnterThreadMutex, int iHistoryDay)
{
	if (NULL == pszFilePath)
		return ACE_Log_File_Msg_Singleton::instance();


	ACE_Log_File_Msg* pInstance = ACE_Singleton<ACE_Log_File_Msg, ACE_Null_Mutex>::instance();
	if (m_bFirst)
	{
		m_bFirst = false;

		pInstance->init(pszFilePath,bClearAll,interval,bEnterThreadMutex,iHistoryDay);
	}

	return pInstance;
}

