/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  ProbeSysMgr.cpp
 *               
 *  TITLE      :  Probeϵͳ��Դ������ʵ��
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
* DESCRIPTION : ��̬��������
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
				// ��ȡWINDOWSϵͳ��Ϣ��ȡ����ָ��
				NtQuerySystemInformation = ( PROCNTQSI )GetProcAddress( GetModuleHandle("ntdll"), 
					"NtQuerySystemInformation" );
				if ( NULL == NtQuerySystemInformation )
				{
					ACE_ASSERT( 0 );
					ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ��NtQuerySystemInformation������ָ��ʧ�ܣ�����\n" ) ) );
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
 * DESCRIPTION : ��������
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
 * DESCRIPTION : ��ʼ��
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
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ʼ��CUP��Ϣ�������ʧ�ܣ�����\n" ) ) );
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
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ʼ���ڴ���Ϣ�������ʧ�ܣ�����\n" ) ) );
        return false;
	}
	
	// ����
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
		ACE_DEBUG( ( LM_INFO, ACE_TEXT( "%I�޷���ȡ����������Ϣ��ϵͳ�����κν��̽��м�ء�\n" ) ) );
		return true;
	}

	const CMemInfo &rMemInfo = m_pMemMgr->GetMemInfo();
	
	if ( !m_pProcMgr->Init( &rMemInfo, pProcCfgInfo ) )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ʼ��������Ϣ�������ʧ�ܣ�����\n" ) ) );
        return false;
	}
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CProbeSysMgr::Refresh()
 *       
 * DESCRIPTION : 
 *       -- �������б����Probeϵͳ��Դ״̬
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

	// ����CPU״̬
	ACE_ASSERT( m_pCpuMgr );
	if ( NULL == m_pCpuMgr )
	{
		ASSERT_INFO;
		return;
	}
	m_pCpuMgr->Refresh();

	// �����ڴ�״̬
	ACE_ASSERT( m_pMemMgr );
	if ( NULL == m_pMemMgr )
	{
		ASSERT_INFO;
		return;
	}
	m_pMemMgr->Refresh();

	// ���½���״̬
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
 *       -- �رղ���ϵͳ
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
 *       -- ��������ϵͳ
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
 *       -- �رջ���������ϵͳ
 *       
 * PARAMETER   : 
 *       1: eCmd -- IN, �ػ�������
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
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��NTDLL.dll�л�ȡ�ӿ�ʧ�ܣ�����\n" ) ) );
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
		ACE_DEBUG( ( LM_INFO, ACE_TEXT( "%IZwShutdownSystem����ֵΪ[%d]\n" ), nRet ) );
    }

#endif
	
}


