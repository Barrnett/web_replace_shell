//////////////////////////////////////////////////////////////////////////
// EtherDataSourceBase.cpp 

#include "cpf/ostypedef.h"


#include "PacketIO/EtherDataSourceBase.h"
#include "cpf/path_tools.h"


CEtherDataSourceBase::CEtherDataSourceBase(void)
:m_indexarray(0),m_dwRecvHandle(0),m_nMemSize(0)
,m_bEnableTimerPacket(TRUE),m_nDevCount(0)
,m_nTimeStampType(3),m_bErrorRecover(TRUE)
{
}

CEtherDataSourceBase::~CEtherDataSourceBase(void)
{
}

//相当于init+OpenDataSource
BOOL CEtherDataSourceBase::Open(int drv_load_type,const char * sysfilename,const char *drivername,
							  const char * macaddr,unsigned int memsize,
							  BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
							  int * error_code)
{
	if( !this->init(drv_load_type,sysfilename,drivername,macaddr,memsize,
		bEnableTimer,nTimeStampType,bErrorRecover,error_code) )
	{
		return false;
	}

	return (this->OpenDataSource() == 0);
}

BOOL CEtherDataSourceBase::Open(int drv_load_type,const char * sysfilename,const char *drivername,int macindex_array,
		  unsigned int memsize,BOOL bEnableTimer,int nTimeStampType,BOOL bErrorRecover,
		  int * error_code)
{
	if( !this->init(drv_load_type,sysfilename,drivername,macindex_array,memsize,
		bEnableTimer,nTimeStampType,bErrorRecover,error_code) )
	{
		return false;
	}

	return (this->OpenDataSource() == 0);
}


//相当于CloseDataSource()+fini
void CEtherDataSourceBase::Close()
{
	this->CloseDataSource();

	this->fini();

	m_indexarray = 0;

	return;
}

bool CEtherDataSourceBase::GetDataSourceName(std::string& name)
{
	char buf[256] = {0};

	if( m_indexarray == -1 )
	{
		sprintf(buf,"%s %s",m_strDriverName.c_str(),"all card(s)");
	}
	else
	{
		sprintf(buf,"%s %s",m_strDriverName.c_str(),m_strMacaddrInfo.c_str());
	}

	name = buf;

	return true;		
}










BOOL CEtherDataSourceBase::EnableTimerPacket(BOOL bEnable)
{
	if( m_dwRecvHandle != 0 )
	{
		m_bEnableTimerPacket = bEnable;

		return true;
	}
	
	return false;
}

//询问这个设备是否可以产生定时包。
BOOL CEtherDataSourceBase::IsEnableTimerPacket() const
{
	return m_bEnableTimerPacket;
}


void CEtherDataSourceBase::GetSysAndDriverName(const char * sysfilename,const char *drivername)
{
	if( sysfilename&&strlen(sysfilename)>0 )
	{
		if( CPF::IsFullPathName(sysfilename) )
			m_strSysFileName = sysfilename;
		else
			m_strSysFileName = CPF::GetModuleFullFileName(sysfilename);
	}
	else
	{
		m_strSysFileName = CPF::GetModuleFullFileName(GetConstDriverName());
	}

	if(drivername&&strlen(drivername)>0)
	{
		m_strDriverName = drivername;
	}
	else
	{
		std::string tempfilename = CPF::GetFileNameFromFullName(m_strSysFileName.c_str());

		char * ptemp = (char *)strchr(tempfilename.c_str(),'.');

		if( ptemp )
			*ptemp = 0;

		m_strDriverName = tempfilename;		
	}

	return;
}



//#endif//OS_WINDOWS

