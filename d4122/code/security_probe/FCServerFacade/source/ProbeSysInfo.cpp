
#include "stdafx.h"

#include "ProbeSysInfo.h"
#include "ProbeProcMgr.h"

CProbeSysInfo::CProbeSysInfo(void)
{
}

CProbeSysInfo::~CProbeSysInfo(void)
{
}

void CProbeSysInfo::init(const char * probe_program_name)
{
	m_cpuInfo.Init();
	m_memInfo.Init();
	m_diskInfo.Init();
	m_adapterInfo.init();
	m_osInfo.Init();

	if( probe_program_name )
	{
		m_probe_program_name = probe_program_name;
		m_procInfo.Init(probe_program_name,&m_memInfo);
	}
	else
	{
		m_probe_program_name.clear();
	}


}

void CProbeSysInfo::Refresh()
{
	m_cpuInfo.Refresh();
	m_memInfo.Refresh();
	m_diskInfo.Refresh();
	m_adapterInfo.Refresh();

	if( !m_probe_program_name.empty() )
	{
		LONGLONG llKernalTime = 0;
		LONGLONG llUsrTime = 0;

		CProbeProcMgr::getTotalCpuUsage(llKernalTime,llUsrTime);

		m_procInfo.Refresh(llKernalTime,llUsrTime);
	}

	return;
}
