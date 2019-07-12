//////////////////////////////////////////////////////////////////////////
//ProbeSysInfo.h

#pragma once

#include "DiskInfo.h"
#include "CpuInfo.h"
#include "MemInfo.h"
#include "ProcInfo.h"
#include "AdapterInfo.h"
#include "OsInfo.h"

class CProbeSysInfo
{
public:
	CProbeSysInfo(void);
	~CProbeSysInfo(void);

	void init(const char * probe_program_name);

	void Refresh();

public:
	CCpuInfo		m_cpuInfo;
	CDiskInfo		m_diskInfo;
	CMemInfo		m_memInfo;
	CProcInfo		m_procInfo;
	CAdapterInfo	m_adapterInfo;
	COsInfo			m_osInfo;

private:
	std::string		m_probe_program_name;
};
