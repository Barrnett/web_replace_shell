//////////////////////////////////////////////////////////////////////////
//ProbeDiskMgr.cpp

#include "ProbeDiskMgr.h"
#include "cpf/Common_Func_Macor.h"

CProbeDiskMgr::CProbeDiskMgr()
{
	m_pCfgInfo = NULL;
	m_pDiskInfo = NULL;

	m_pCurAlaramLevel = NULL;

	m_nInitDiskNums = 0;

	return;
}

CProbeDiskMgr::~CProbeDiskMgr()
{
	if( m_pCurAlaramLevel )
	{
		delete m_pCurAlaramLevel;
		m_pCurAlaramLevel = NULL;
	}

}

// 初始化
bool CProbeDiskMgr::Init(IN const CDiskInfo * pDiskInfo, IN const SDiskCfgInfo *pCfgInfo )
{
	m_pCfgInfo = pCfgInfo;

	m_pDiskInfo = pDiskInfo;

	m_nInitDiskNums = m_pDiskInfo->m_vtDisk.size();

	m_pCurAlaramLevel = new int[m_nInitDiskNums];

	if( m_pCurAlaramLevel )
	{
		for(size_t i = 0; i < m_nInitDiskNums; ++i )
		{
			m_pCurAlaramLevel[i] = -1;
		}
	}

	return true;
}

// 获取被监控磁盘个数
int CProbeDiskMgr::GetDiskCount() const
{	
	return m_pDiskInfo->m_vtDisk.size();	
}

// 按序号获取磁盘信息
const CDiskInfo::DISK_INFO* CProbeDiskMgr::GetDiskInfoByNth( IN int nNth )
{
	if(nNth>= (int)m_pDiskInfo->m_vtDisk.size())
	{
		ACE_ASSERT(FALSE);
		return NULL;
	}

	return  &(m_pDiskInfo->m_vtDisk[nNth]);
}

int	CProbeDiskMgr::GetDiskAlarmLevelByNth( IN int nNth )
{
	if(nNth>= (int)m_pDiskInfo->m_vtDisk.size())
	{
		ACE_ASSERT(FALSE);
		return NULL;
	}

	return  m_pCurAlaramLevel[nNth];
}

void CProbeDiskMgr::genrate_disk_alarm()
{
	for(size_t i = 0; i < mymin(m_pDiskInfo->m_vtDisk.size(),m_nInitDiskNums); ++i)
	{
		int percent = m_pDiskInfo->m_vtDisk[i].nUsed*100/m_pDiskInfo->m_vtDisk[i].nCapacity;

		int j = GET_NUMS_OF_ARRAY(m_pCfgInfo->aUsePercentLimit_level)-1;

		for( ; j >= 0; --j)
		{
			if(m_pCfgInfo->aUsePercentLimit_level[j] <= 0)
			{
				continue;
			}

			if( percent >= m_pCfgInfo->aUsePercentLimit_level[j] )
			{				
				m_pCurAlaramLevel[i] = j;

				break;
			}
		}

		if( j < 0 )
		{
			m_pCurAlaramLevel[i] = j;
		}
	}

	return;
}

