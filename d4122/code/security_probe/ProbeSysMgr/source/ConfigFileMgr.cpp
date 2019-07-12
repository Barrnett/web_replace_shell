/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ConfigFileMgr.cpp
 *
 *  TITLE      :  Probeϵͳ���ά��ϵͳ�����ļ�������ʵ��
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:
 *
 *****************************************************************************/

#include "LogFileMgr.h"
#include "cpf/cpf.h"
#include "cpf/IniFile.h"
#include "cpf/path_tools.h"

#include "ProbeSysCommon.h"
#include "FileDirInfo.h"

#include "ConfigFileMgr.h"



/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::CConfigFileMgr()
 *       
 * DESCRIPTION : 
 *       -- ���캯��
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CConfigFileMgr::CConfigFileMgr()
 : 	m_pSnmpCfgInfo( NULL ), m_pCfgFile( NULL ), m_pLogCfgInfo( NULL ), 
	m_pCpuCfgInfo( NULL ), m_pMemCfgInfo( NULL ), m_pCtlCfgInfo( NULL ),
	m_pDiskCfgInfo( NULL ), m_pFileSysCfgInfo( NULL ), m_pCardCfgInfo( NULL ),
	m_pNetCfgInfo( NULL ), m_pProcCfgInfo( NULL )
{

}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::~CConfigFileMgr()
 *       
 * DESCRIPTION : 
 *       -- ��������
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CConfigFileMgr::~CConfigFileMgr()
{
	// �����ļ����ʶ���
	if ( NULL != m_pCfgFile )
	{
		delete m_pCfgFile;
		m_pCfgFile = NULL;
	}

	// ����CPU������Ϣ����
	if ( NULL != m_pCpuCfgInfo )
	{
		delete m_pCpuCfgInfo;
		m_pCpuCfgInfo = NULL;
	}

	// �����ڴ�������Ϣ����
	if ( NULL != m_pMemCfgInfo )
	{
		delete m_pMemCfgInfo;
		m_pMemCfgInfo = NULL;
	}

	// ���ٴ���������Ϣ����
	if ( NULL != m_pDiskCfgInfo )
	{
		delete m_pDiskCfgInfo;
		m_pDiskCfgInfo = NULL;
	}

	// �����ļ�ϵͳ������Ϣ����
	if ( NULL != m_pFileSysCfgInfo )
	{
		delete m_pFileSysCfgInfo;
		m_pFileSysCfgInfo = NULL;
	}

	// ���ٰ忨������Ϣ����
	if ( NULL != m_pCardCfgInfo )
	{
		delete m_pCardCfgInfo;
		m_pCardCfgInfo = NULL;
	}

	// ��������������Ϣ����
	if ( NULL != m_pNetCfgInfo )
	{
		delete m_pNetCfgInfo;
		m_pNetCfgInfo = NULL;
	}

	// ���ٽ���������Ϣ����
	if ( NULL != m_pProcCfgInfo )
	{
		delete m_pProcCfgInfo;
		m_pProcCfgInfo = NULL;
	}

	if ( NULL != m_pSnmpCfgInfo )
	{
		delete m_pSnmpCfgInfo;
		m_pSnmpCfgInfo = NULL;
	}

	if ( NULL != m_pCtlCfgInfo )
	{
		delete m_pCtlCfgInfo;
		m_pCtlCfgInfo = NULL;
	}

	
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::Init(IN const std::string &strCfgFileName)
 *       
 * DESCRIPTION : 
 *       -- ��ʼ��
 *       
 * PARAMETER   : 
 *       1: strCfgFileName -- IN, �����ļ�ȫ·������
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::Init( IN const std::string &strCfgFileName )
{
	// �����ļ����ʶ���
	char strPath[MAX_PATH];
	if ( !CPF::GetModulePathName( strPath, sizeof( strPath ) ) )
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ�����ļ�·��ʧ�ܣ�����\n" ) ) );
		ACE_ASSERT( 0 );
		return false;
	}
	
	std::string strFullPath( strPath );
	strFullPath += FILENAME_SEPERATOR_STR;
	strFullPath += "ini";
	if ( !CFileDirInfo::IsFileExist( strFullPath, strCfgFileName ) )
	{
		ACE_ERROR( (	LM_ERROR, 
						ACE_TEXT( "\n�ļ�·��[%s]��\n�����������ļ�[%s]��\n�޷���ȡ������Ϣ����ȫ��ʹ��ȱʡ���ã�\n\n" ), 
						ACE_TEXT( strFullPath.c_str() ),
						ACE_TEXT( strCfgFileName.c_str() ) ) );
	}
	else
	{
		strFullPath += FILENAME_SEPERATOR_STR;
		strFullPath += strCfgFileName;
	}

	m_strFileName = strFullPath;
	m_pCfgFile = new CIniFile;
	ACE_ASSERT( m_pCfgFile );
	if ( NULL == m_pCfgFile )
	{
		ASSERT_INFO;
		return false;
	}
	m_pCfgFile->Open(m_strFileName.c_str());

	//ACE_DEBUG( ( LM_INFO, ACE_TEXT( "%I�����ļ�����[%s]\n" ), ACE_TEXT( m_pCfgFile->GetFileName() ) ) );
	
	
	// ��ȡSNMP������Ϣ
	if ( !getSnmpCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡSNMP������Ϣʧ�ܣ�����\n" ) ) );
	}

	// ��ȡ����������Ϣ
	if ( !getCtlCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ����������Ϣʧ�ܣ�����\n" ) ) );
	}

	// ��ȡ��־�ļ�����
	if ( !getLogCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ��־������Ϣʧ�ܣ�����\n" ) ) );
	}


	// ��ȡCPU������Ϣ
	if ( !getCpuCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡCPU������Ϣʧ�ܣ�����\n" ) ) );
	}
	
	// ��ȡ�ڴ�������Ϣ
	if ( !getMemCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ�ڴ�������Ϣʧ�ܣ�����\n" ) ) );
	}

	// ��ȡ����������Ϣ
	if ( !getProcCfgInfo() )
	{
		//ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ����������Ϣʧ�ܣ�����\n" ) ) );
	}

	return true;
}


/********************************************************************
Private Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getSnmpCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡSNMP������Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getSnmpCfgInfo()
{
	m_pSnmpCfgInfo = new SSnmpCfgInfo;
	ACE_ASSERT( m_pSnmpCfgInfo );
	if ( NULL == m_pSnmpCfgInfo )
	{
		return false;
	}
	
	// �Ƿ���SNMP����
	unsigned int nOpenSvr = 0;
	if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
								ITEM_SNMP_OPEN_SNMP, 
								nOpenSvr ) )
	{
		m_pSnmpCfgInfo->bOpenSvr = DEFAULT_OPEN_SNMP;
	}
	else
	{
		m_pSnmpCfgInfo->bOpenSvr = ( 0 == nOpenSvr ) ? false : true;
	}

	// ��ʱ��ѯ���ʱ�䣨��λ���룩
	unsigned int uPort = 0;
	if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
								ITEM_SNMP_PORT, 
								uPort )
		|| uPort < 1 )
	{
		uPort = DEFAULT_SNMP_PORT;
	}
	m_pSnmpCfgInfo->uSnmpPort = ( unsigned short )uPort;
	
	// Mib������
	{
		CStr strKey(ITEM_SYSMONITOR_MIB_NAME);
		
		if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
									strKey, 
									m_pSnmpCfgInfo->strMibName )
			|| m_pSnmpCfgInfo->strMibName.IsEmpty() )
		{
			m_pSnmpCfgInfo->strMibName.AddString( DEFAULT_SYSMONITOR_MIB_NAME );
		}
	}
	
	// Trap ��Դ��ʶ
	{
		CStr strKey(ITEM_SYS_TRAP_SOURCE);
		
		if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
			strKey, 
			m_pSnmpCfgInfo->strTarpSource )
			|| m_pSnmpCfgInfo->strTarpSource.IsEmpty() )
		{
			m_pSnmpCfgInfo->strTarpSource.AddString( DEFAULT_SYS_TRAP_SOURCE );
		}
		
	}
	// Trap Ŀ�ĵ�ַ
	{
		CStr strKey(ITEM_SYS_TRAP_DADDRESS);
		
		if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
			strKey, 
			m_pSnmpCfgInfo->strTrapAddr )
			|| m_pSnmpCfgInfo->strTrapAddr.IsEmpty() )
		{
			m_pSnmpCfgInfo->strTrapAddr.AddString( DEFAULT_SYS_TRAP_DADDRESS );
		}
	}

	// Trap��ѯ���ʱ�䣨��λ���룩
	{
		int lTrapInterval = 0;
		if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
			ITEM_SYS_TRAP_INTERVAL, 
			lTrapInterval )
			|| lTrapInterval < DEFAULT_SYS_TRAP_INTERVAL )
		{
			lTrapInterval = DEFAULT_SYS_TRAP_INTERVAL;
		}
		m_pSnmpCfgInfo->lTrapInterval = lTrapInterval;
	}
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getCtlCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡ����������Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getCtlCfgInfo()
{
	m_pCtlCfgInfo = new SCtlCfgInfo;
	ACE_ASSERT( m_pCtlCfgInfo );
	if ( NULL == m_pCtlCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	CStr strConsoleTitle;
	// ����̨���ڱ��⣬����ָ����ȱʡΪ"ProbeSysMonitor"
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_CONSOLE_TITLE, 
								strConsoleTitle )
		|| strConsoleTitle.IsEmpty() )
	{
		m_pCtlCfgInfo->strConsoleTitle = DEFAULT_CONSOLE_TITLE;
	}
	else
	{
		m_pCtlCfgInfo->strConsoleTitle = strConsoleTitle.GetString();
	}
	
	// ��ʱ��ѯ���ʱ�䣨��λ���룩
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_TIMER_INTERVAL, 
								m_pCtlCfgInfo->uTimerInterval )
		|| m_pCtlCfgInfo->uTimerInterval < 1 )
	{
		m_pCtlCfgInfo->uTimerInterval = DEFAULT_TIMER_INTERVAL;
	}
	
	// ��ʱ��ѯ���ʱ�䣨��λ���룩
	unsigned int uShowTimes = 0;
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_SHOW_TIMES, 
								uShowTimes )
		|| uShowTimes < 1 )
	{
		uShowTimes = DEFAULT_SHOW_TIMES;
	}
	m_pCtlCfgInfo->uShowInterval = m_pCtlCfgInfo->uTimerInterval * uShowTimes;
	
	// ��ʱ��������ϵͳ���ʱ�䣨��λ���룩��Ϊ0��ʾ����ʱ����
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_REBOOT_OS_INTERVAL, 
								m_pCtlCfgInfo->uRebootOsInterval ) )
	{
		m_pCtlCfgInfo->uRebootOsInterval = DEFAULT_REBOOT_OS_INTERVAL;
	}
	m_pCtlCfgInfo->uRebootOsInterval *= 3600;
	
	// ��ʱ�������̼��ʱ�䣨��λ���룩��Ϊ0��ʾ����ʱ����
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_RESTART_RPOC_INTERVAL, 
								m_pCtlCfgInfo->uRestartProcInterval ) )
	{
		m_pCtlCfgInfo->uRestartProcInterval = DEFAULT_REBOOT_OS_INTERVAL;
	}
	m_pCtlCfgInfo->uRestartProcInterval *= 3600;
	
	// �Ƿ��Զ�ע�ᵽ����ϵͳ
	unsigned int nAutoRegist = 0;
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_AUTO_REGIST, 
								nAutoRegist ) )
	{
		m_pCtlCfgInfo->bAutoRegist = DEFAULT_AUTO_REGIST;
	}
	else
	{
		m_pCtlCfgInfo->bAutoRegist = ( 0 == nAutoRegist ) ? false : true;
	}

	// �Ƿ��ڸ澯ʱ��������Trap
	unsigned int nSendTrap = 0;
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_SEND_TRAP, 
								nSendTrap ) )
	{
		m_pCtlCfgInfo->bSendTrap = DEFAULT_SEND_TRAP;
	}
	else
	{
		m_pCtlCfgInfo->bSendTrap = ( 0 == nSendTrap ) ? false : true;
	}

	
#ifdef OS_WINDOWS
	std::string strPrefixItem( OS_WINDOWS_ID );
#else
	std::string strPrefixItem( OS_LINUX_ID );
#endif
	std::string strItem;

	CStr strTmp;

	// CPU�����ʳ����޸澯������
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_CPUUSEALARMHANDLE_DEFAULT, 
								m_pCtlCfgInfo->uCpuUseAlarmHandle_default )
		|| E_HANDLE_REBOOT_OS > m_pCtlCfgInfo->uCpuUseAlarmHandle_default
		|| E_HANDLE_CLOSE_OS < m_pCtlCfgInfo->uCpuUseAlarmHandle_default )
	{
		m_pCtlCfgInfo->uCpuUseAlarmHandle_default = E_HANDLE_REBOOT_OS;
	}

	strItem = strPrefixItem + ITEM_CTL_CPUUSEALARMHANDLE_CUSTOM;
	if ( m_pCfgFile->GetValue(	SECTION_CONTROL, 
								strItem.c_str(), 
								strTmp ) )
	{
		strTmp.AllTrim();
		m_pCtlCfgInfo->strCpuUseAlarmHandle_custom = strTmp.GetString();
	}

	// �ڴ������ʳ����޸澯������
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_MEMUSEALARMHANDLE_DEFAULT, 
								m_pCtlCfgInfo->uMemUseAlarmHandle_default )
		|| E_HANDLE_REBOOT_OS > m_pCtlCfgInfo->uMemUseAlarmHandle_default
		|| E_HANDLE_CLOSE_OS < m_pCtlCfgInfo->uMemUseAlarmHandle_default )
	{
		m_pCtlCfgInfo->uMemUseAlarmHandle_default = E_HANDLE_REBOOT_OS;
	}

	strItem = strPrefixItem + ITEM_CTL_MEMUSEALARMHANDLE_CUSTOM;
	if ( m_pCfgFile->GetValue(	SECTION_CONTROL, 
								strItem.c_str(), 
								strTmp ) )
	{
		strTmp.AllTrim();
		m_pCtlCfgInfo->strMemUseAlarmHandle_custom = strTmp.GetString();
	}

	// ����-----------------------------------------------------------------------
	// ����ռ��CPU�����ʳ����޸澯������
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_PROCCPUUSEALARMHANDLE_DEFAULT, 
								m_pCtlCfgInfo->uProcCpuUseAlarmHandle_default )
		|| E_HANDLE_RESTART_PROCESS > m_pCtlCfgInfo->uProcCpuUseAlarmHandle_default
		|| E_HANDLE_CLOSE_OS < m_pCtlCfgInfo->uProcCpuUseAlarmHandle_default )
	{
		m_pCtlCfgInfo->uProcCpuUseAlarmHandle_default = E_HANDLE_RESTART_PROCESS;
	}

	strItem = strPrefixItem + ITEM_CTL_PROCCPUUSEALARMHANDLE_CUSTOM;
	if ( m_pCfgFile->GetValue(	SECTION_CONTROL, 
								strItem.c_str(), 
								strTmp ) )
	{
		strTmp.AllTrim();
		m_pCtlCfgInfo->strProcCpuUseAlarmHandle_custom = strTmp.GetString();
	}

	// ����ռ���ڴ������ʳ����޸澯������
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_PROCMEMUSEALARMHANDLE_DEFAULT, 
								m_pCtlCfgInfo->uProcMemUseAlarmHandle_default )
		|| E_HANDLE_RESTART_PROCESS > m_pCtlCfgInfo->uProcMemUseAlarmHandle_default
		|| E_HANDLE_CLOSE_OS < m_pCtlCfgInfo->uProcMemUseAlarmHandle_default )
	{
		m_pCtlCfgInfo->uProcMemUseAlarmHandle_default = E_HANDLE_RESTART_PROCESS;
	}

	strItem = strPrefixItem + ITEM_CTL_PROCMEMUSEALARMHANDLE_CUSTOM;
	if ( m_pCfgFile->GetValue(	SECTION_CONTROL, 
								strItem.c_str(), 
								strTmp ) )
	{
		strTmp.AllTrim();
		m_pCtlCfgInfo->strProcMemUseAlarmHandle_custom = strTmp.GetString();
	}

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getLogCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡ��־������Ϣ
 *       
 * PARAMETER   : 
 *       NONE
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getLogCfgInfo()
{
#ifdef OS_WINDOWS
	std::string strItem( OS_WINDOWS_ID );
#else
	std::string strItem( OS_LINUX_ID );
#endif
	strItem.append( ITEM_LOG_FILE_PATH );

	m_pLogCfgInfo = new SLogCfgInfo;
	ACE_ASSERT( m_pLogCfgInfo );
	if ( NULL == m_pLogCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	// �Ƿ���־��Ϣͬʱ���������̨����
	int nBool = 0;
	if ( !m_pCfgFile->GetValue( SECTION_LOG, ITEM_LOG_PUTOUT_TO_CONSOLE, nBool ) )
	{
		m_pLogCfgInfo->bIsLogToConsole = DEFAULT_LOG_PUTOUT_TO_CONSOLE;
	}
	else
	{
		if ( 0 == nBool )
		{
			m_pLogCfgInfo->bIsLogToConsole = false;
		}
		else
		{
			m_pLogCfgInfo->bIsLogToConsole = true;
		}
	}
	
	// ��ʱ��־ʱ����
	unsigned int uTimes = 0;
	if ( m_pCfgFile->GetValue( SECTION_LOG, ITEM_LOG_TIMES, uTimes ) )
	{
		m_pLogCfgInfo->uLogInterval = uTimes * m_pCtlCfgInfo->uTimerInterval;
	}
	else
	{
		m_pLogCfgInfo->uLogInterval = DEFAULT_LOG_TIMES * m_pCtlCfgInfo->uTimerInterval;
	}
	
	// ��ʱ�ı���־������µ��ļ���ʱ����
	unsigned int uInterval = 0;
	if ( m_pCfgFile->GetValue( SECTION_LOG, ITEM_CHANGE_LOGFILE_INTERVAL, uInterval ) )
	{
		m_pLogCfgInfo->uChangeLogFileInterval = uInterval * 3600;
	}
	else
	{
		m_pLogCfgInfo->uChangeLogFileInterval = DEFAULT_CHANGE_LOGFILE_INTERVAL * 3600;
	}
	
	// ��־�ļ�·��
	CStr strFile;
	if ( m_pCfgFile->GetValue( SECTION_LOG, strItem.c_str(), strFile ) )
	{
		m_pLogCfgInfo->strFilePath = strFile.GetString();
	}
	else
	{
		char strPath[MAX_PATH];
		CPF::GetModulePathName( strPath, MAX_PATH );
		m_pLogCfgInfo->strFilePath = strPath;
	}
	
	int nEndPos = m_pLogCfgInfo->strFilePath.size() - 1;
	if ( FILENAME_SEPERATOR != m_pLogCfgInfo->strFilePath[ nEndPos ] )
	{
		m_pLogCfgInfo->strFilePath += FILENAME_SEPERATOR;
	}
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getCpuCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡCPU������Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getCpuCfgInfo()
{
	// ���������ޣ�ֵ��100*�ٷֱȣ�0��ʾ����ظ�ָ��
	int nUsePercent = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_CPU, ITEM_CPU_USEPERCENTLIMIT, nUsePercent );

	// �����������������޵�ʱ�����ޣ���λ���룬0��ʾ����ظ�ָ��
	int nUseAlarmDuration = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_CPU, ITEM_CPU_USEALARMDURATION, nUseAlarmDuration );

	m_pCpuCfgInfo = new SCpuCfgInfo;
	ACE_ASSERT( m_pCpuCfgInfo );
	if ( NULL == m_pCpuCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	m_pCpuCfgInfo->nUsePercentLimit = nUsePercent;
	m_pCpuCfgInfo->nUseAlarmDuration = nUseAlarmDuration;

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getMemCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡ�ڴ�������Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getMemCfgInfo()
{
	// ���������ޣ�ֵ��100*�ٷֱȣ�0��ʾ����ظ�ָ��
	int nUsePercent = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_MEM, ITEM_MEM_USEPERCENTLIMIT, nUsePercent );

	// �����������������޵�ʱ�����ޣ���λ���룬0��ʾ����ظ�ָ��
	int nUseAlarmDuration = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_MEM, ITEM_MEM_USEALARMDURATION, nUseAlarmDuration );

	m_pMemCfgInfo = new SMemCfgInfo;
	ACE_ASSERT( m_pMemCfgInfo );
	if ( NULL == m_pMemCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	m_pMemCfgInfo->nUsePercentLimit = nUsePercent;
	m_pMemCfgInfo->nUseAlarmDuration = nUseAlarmDuration;

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getProcessCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡ����������Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getProcCfgInfo()
{
	// CPU���������ޣ�ֵ��100*�ٷֱȣ�0��ʾ����ظ�ָ��
	int nCpuUsePercent = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_CPU_USEPERCENTLIMIT, nCpuUsePercent );

	// CPU�����������������޵�ʱ�����ޣ���λ���룬0��ʾ����ظ�ָ��
	int nCpuUseAlarmDuration = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_CPU_USEALARMDURATION, nCpuUseAlarmDuration );

	// ����ռ���ڴ������ʳ����޸澯�����������ڴ滹�������ڴ����ѡ�1:�����ڴ棬2:�����ڴ棩��ȱʡΪ�����ڴ�
	int nMEMUseFlag = DEFAULT_PROC_MEM_PHY_FLAG;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_MEM_MEMUSEFLAG, nMEMUseFlag );

	// �ڴ����������ޣ�ֵ��100*�ٷֱȣ�0��ʾ����ظ�ָ��
	int nMemUsePercent = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_MEM_USEPERCENTLIMIT, nMemUsePercent );

	// �ڴ�����������������޵�ʱ�����ޣ���λ���룬0��ʾ����ظ�ָ��
	int nMemUseAlarmDuration = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_MEM_USEALARMDURATION, nMemUseAlarmDuration );

	// ��������ʱ�������ƣ���λ��Сʱ����0��ʾû�н�����������
	int nRestartInterval = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_RESTARTINTERVAL, nRestartInterval );
	//nRestartInterval *= 3600;

	// ������RestartIntervalʱ�����������������0��ʾû�н�����������
	int nMaxRestartTimes = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_MAXRESTARTTIMES, nMaxRestartTimes );

	// ����ؽ���һ����Ϣ�б�
#ifdef OS_WINDOWS
	std::string strItem( OS_WINDOWS_ID );
#else
	std::string strItem( OS_LINUX_ID );
#endif
	strItem.append( ITEM_PROC_FULLPATH_EXEFILE );

	std::vector<std::string> arrExeFile;
	if ( !m_pCfgFile->GetValue( SECTION_PROC, strItem.c_str(), arrExeFile )
		|| arrExeFile.size() == 0 )
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n�޷���ȡ����ؽ�����Ϣ������\n\n" ) ) );
		return false;
	}

	m_pProcCfgInfo = new SProcCfgInfo;
	ACE_ASSERT( m_pProcCfgInfo );
	if ( NULL == m_pProcCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	m_pProcCfgInfo->nCpuUsePercentLimit = nCpuUsePercent;	
	m_pProcCfgInfo->nCpuUseAlarmDuration = nCpuUseAlarmDuration;
	m_pProcCfgInfo->nMEMUseFlag = nMEMUseFlag;
	m_pProcCfgInfo->nMemUsePercentLimit = nMemUsePercent;
	m_pProcCfgInfo->nMemUseAlarmDuration = nMemUseAlarmDuration;
	m_pProcCfgInfo->nRestartInterval = nRestartInterval * 3600; // תΪ��
	m_pProcCfgInfo->nMaxRestartTimes = nMaxRestartTimes;

	SProcComInfo sTmp;
	int nSize = arrExeFile.size(), i = 0;
	for ( i=0; i<nSize; i++ )
	{
		std::string &strFile = arrExeFile[i];
		if ( strFile.empty() )
		{
			continue;
		}
		
		//strFile.AllTrim();
		sTmp.strExeFile = strFile;
		m_pProcCfgInfo->vectComInfo.push_back( sTmp );
	}

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getDiskCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡ����������Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getDiskCfgInfo()
{
//	m_pDiskCfgInfo = new SDiskCfgInfo;
//	ACE_ASSERT( m_pDiskCfgInfo );

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getFileSysCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡ�ļ�ϵͳ������Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getFileSysCfgInfo()
{
//	m_pFileSysCfgInfo = new SFileSysCfgInfo;
//	ACE_ASSERT( m_pFileSysCfgInfo );

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getCardCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡ�忨������Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getCardCfgInfo()
{
//	m_pCardCfgInfo = new SCardCfgInfo;
//	ACE_ASSERT( m_pCardCfgInfo );
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getNetCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡ����������Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getNetCfgInfo()
{
//	m_pNetCfgInfo = new SNetCfgInfo;
//	ACE_ASSERT( m_pNetCfgInfo );

	return true;
}


