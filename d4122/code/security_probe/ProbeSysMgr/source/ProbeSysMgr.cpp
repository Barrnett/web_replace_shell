/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  ProbeSysMgr.cpp
 *               
 *  TITLE      :  Probe系统资源管理类实现
 *               
 *  CREATE TIME:  2005-10-27 15:38:30
 *               
 *  AUTHOR     :  
 *               
 *  DESCRIPTION:  
 * 				
 *****************************************************************************/

#include "ProbeSysMgr.h"
	
#ifdef OS_WINDOWS
	PROCNTQSI NtQuerySystemInformation = NULL;
	
	#define SE_SHUTDOWN_PRIVILEGE	0x13

	typedef LONG (WINAPI *RTLADJUSTPRIVILEGE)(int, BOOL, BOOL, int*);
	typedef LONG (WINAPI *ZWSHUTDOWNSYSTEM)(DWORD);

#endif

/******************************************************************************
* FUNCTION    : DllMain(...)
*               
* DESCRIPTION : 动态库主函数
*               
*               
* PARAMETER   : 3
*            1: HANDLE hModule
*            2: DWORD ul_reason_for_call
*            3: LPVOID lpReserved
*            
*            
* RETURN     : BOOL APIENTRY 
*              
* SPECIAL    : 
*              None
******************************************************************************/
#ifdef OS_WINDOWS
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
#ifdef OS_WINDOWS // WINDOWS---------------------------------------------------
				// 获取WINDOWS系统信息获取函数指针
				NtQuerySystemInformation = ( PROCNTQSI )GetProcAddress( GetModuleHandle("ntdll"), 
					"NtQuerySystemInformation" );
				if ( NULL == NtQuerySystemInformation )
				{
					ACE_ASSERT( 0 );
					ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取“NtQuerySystemInformation”函数指针失败！！！\n" ) ) );
					return false;
				}

#endif
			}
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif

/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CProbeSysMgr::CProbeSysMgr()
 *       
 * DESCRIPTION : 
 *       -- 
 *       
 * PARAMETER   :
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CProbeSysMgr::CProbeSysMgr()
 : 	m_pCpuMgr( NULL ), m_pMemMgr( NULL ), m_pProcMgr( NULL )/*, m_pOSMgr( NULL ), 
	m_pDiskMgr( NULL ), m_pFileSysMgr( NULL ), m_pCardMgr( NULL ), m_pNetMgr( NULL )*/
{
	
}

/******************************************************************************
 * FUNCTION    : ProbeSysMgr::~CProbeSysMgr(
 *       
 * DESCRIPTION : 析构函数
 *       
 *       
 * PARAMETER   : 
 *       
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CProbeSysMgr::~CProbeSysMgr()
{
	if ( NULL != m_pCpuMgr )
	{
		delete m_pCpuMgr;
		m_pCpuMgr = NULL;
	}
	
	if ( NULL != m_pProcMgr )
	{
		delete m_pProcMgr;
		m_pProcMgr = NULL;
	}
	
	if ( NULL != m_pMemMgr )
	{
		delete m_pMemMgr;
		m_pMemMgr = NULL;
	}
}


/******************************************************************************
 * FUNCTION    : ProbeSysMgr::Init(
 *       
 * DESCRIPTION : 初始化
 *       
 *       
 * PARAMETER   : 
 *       
 *       
 * RETURN     : 
 *       BOOL -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProbeSysMgr::Init( IN const CConfigFileMgr *pCfgFile )
{
	ACE_ASSERT( pCfgFile );
	if ( NULL == pCfgFile )
	{
		ASSERT_INFO;
		return false;
	}

	m_pCfgFile = pCfgFile;


	m_pCpuMgr = new CProbeCpuMgr(); 
	ACE_ASSERT( m_pCpuMgr );
	if ( NULL == m_pCpuMgr )
	{
		ASSERT_INFO;
		return false;
	}

	if ( !m_pCpuMgr->Init( m_pCfgFile->GetCpuCfgInfo() ) )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I初始化CUP信息管理对象失败！！！\n" ) ) );
        return false;
	}
	
	m_pMemMgr = new CProbeMemMgr(); 
	ACE_ASSERT( m_pMemMgr );
	if ( NULL == m_pMemMgr )
	{
		ASSERT_INFO;
		return false;
	}

	if ( !m_pMemMgr->Init( m_pCfgFile->GetMemCfgInfo() ) )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I初始化内存信息管理对象失败！！！\n" ) ) );
        return false;
	}
	
	// 进程
	m_pProcMgr = new CProbeProcMgr();
	ACE_ASSERT( m_pProcMgr );
	if ( NULL == m_pProcMgr )
	{
		ASSERT_INFO;
		return false;
	}

	const SProcCfgInfo *pProcCfgInfo = m_pCfgFile->GetProcCfgInfo();
	if ( NULL == pProcCfgInfo )
	{
		delete m_pProcMgr;
		m_pProcMgr = NULL;
		ACE_DEBUG( ( LM_INFO, ACE_TEXT( "%I无法获取进程配置信息，系统不对任何进程进行监控。\n" ) ) );
		return true;
	}

	const CMemInfo &rMemInfo = m_pMemMgr->GetMemInfo();
	
	if ( !m_pProcMgr->Init( &rMemInfo, pProcCfgInfo ) )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I初始化进程信息管理对象失败！！！\n" ) ) );
        return false;
	}
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CProbeSysMgr::Refresh()
 *       
 * DESCRIPTION : 
 *       -- 更新所有被监控Probe系统资源状态
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       None
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
void CProbeSysMgr::Refresh()
{
	LOCK_THREAD_GUARD;

	// 更新CPU状态
	ACE_ASSERT( m_pCpuMgr );
	if ( NULL == m_pCpuMgr )
	{
		ASSERT_INFO;
		return;
	}
	m_pCpuMgr->Refresh();

	// 更新内存状态
	ACE_ASSERT( m_pMemMgr );
	if ( NULL == m_pMemMgr )
	{
		ASSERT_INFO;
		return;
	}
	m_pMemMgr->Refresh();

	// 更新进程状态
	if ( NULL == m_pProcMgr )
	{
		return;
	}

	VectorSingleProcMgr vectFailedProc;
	m_pProcMgr->Refresh( vectFailedProc );
}

/******************************************************************************
 * FUNCTION    : CProbeSysMgr::CloseOS()
 *       
 * DESCRIPTION : 
 *       -- 关闭操作系统
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       NONE
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
void CProbeSysMgr::CloseOS()
{
	LOCK_THREAD_GUARD;

	setOs( E_OS_CLOSE );
}

/******************************************************************************
 * FUNCTION    : CProbeSysMgr::RebootOS()
 *       
 * DESCRIPTION : 
 *       -- 重启操作系统
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       NONE
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
void CProbeSysMgr::RebootOS()
{
	LOCK_THREAD_GUARD;

	setOs( E_OS_REBOOT );
}

/********************************************************************
Private Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CProbeSysMgr::setOs(IN ESetOsCmd eCmd)
 *       
 * DESCRIPTION : 
 *       -- 关闭或重启操作系统
 *       
 * PARAMETER   : 
 *       1: eCmd -- IN, 关机或重启
 *       
 * RETURN     : 
 *       NONE
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
void CProbeSysMgr::setOs( IN ESetOsCmd eCmd )
{
#ifdef OS_WINDOWS

    HANDLE hToken = NULL; 
    TOKEN_PRIVILEGES NewState; 
    DWORD  ReturnLength = 0; 
    LUID luidPrivilegeLUID; 

    OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken ); 
    LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &luidPrivilegeLUID );
	
    NewState.PrivilegeCount = 1; 
    NewState.Privileges[0].Luid = luidPrivilegeLUID; 
    NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
    AdjustTokenPrivileges( hToken, FALSE, &NewState, NULL, NULL, NULL ); 
	
    HINSTANCE hinstLib = NULL; 
    RTLADJUSTPRIVILEGE RtlAdjustPrivilege = NULL; 
	ZWSHUTDOWNSYSTEM ZwShutdownSystem = NULL; 
	BOOL fFreeResult = FALSE, fRunTimeLinkSuccess = FALSE; 
 
    // Get a handle to the DLL module.
 
    hinstLib = LoadLibrary( "NTDLL.dll" ); 
 
    // If the handle is valid, try to get the function address.
	int en = 0;
    if ( hinstLib != NULL ) 
    { 
        RtlAdjustPrivilege = ( RTLADJUSTPRIVILEGE ) GetProcAddress( hinstLib, "RtlAdjustPrivilege" ); 
		ZwShutdownSystem = ( ZWSHUTDOWNSYSTEM ) GetProcAddress( hinstLib, "ZwShutdownSystem" ); 
 
		if ( RtlAdjustPrivilege == NULL || ZwShutdownSystem == NULL )
		{
			ACE_ASSERT( 0 );
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I在NTDLL.dll中获取接口失败！！！\n" ) ) );
			FreeLibrary(hinstLib); 
			return;
		}
 
		int nRet = -1;
		nRet = RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, TRUE, &en);
		if(nRet == 0x0C000007C)
		{
			nRet = RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &en);    
		}    

		fFreeResult = FreeLibrary( hinstLib ); 
		
		nRet= ZwShutdownSystem( eCmd );
		ACE_DEBUG( ( LM_INFO, ACE_TEXT( "%IZwShutdownSystem返回值为[%d]\n" ), nRet ) );
    }

#endif
	
}


