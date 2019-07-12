/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  OsRegistTableMgr.cpp
 *               
 *  TITLE      :  ����ϵͳע��������ʵ��
 *               
 *  CREATE TIME:  2005-10-27 15:38:30
 *               
 *  AUTHOR     :  
 *               
 *  DESCRIPTION:  
 * 				
 *****************************************************************************/

#include "OsRegistTableMgr.h"


/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : COsRegistTableMgr::GetRegValue(IN const std::string &strKey, 
 *               IN const std::string &strValueName, 
 *               OUT std::string &strValue)
 *       
 * DESCRIPTION : 
 *       -- ��ȡע�����ֵ
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, ע����ֵ
 *       2: strValueName -- IN, ע���������
 *       3: strValue -- OUT, ע�����ֵ
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : ��ָ����ע���������򴴽���
 *       
 *****************************************************************************/
bool COsRegistTableMgr::GetRegValue(	IN const std::string &strKey,
										IN const std::string &strValueName,
										OUT std::string &strValue )
{	
	ACE_ASSERT( !strKey.empty() && !strValueName.empty() );
	if ( strKey.empty() || strValueName.empty() )
	{
		ASSERT_INFO;
		return false;
	}
	
	bool	bSuccess = true;

#ifdef OS_WINDOWS

	DWORD dwAccessType = KEY_ALL_ACCESS;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}

	HKEY	hkResult = NULL;

	LONG dwResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKey.c_str(), 0,dwAccessType, &hkResult );
	// ��Ŀ¼
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// ���ý�ֵ������������򴴽���
		dwResult = RegQueryValueEx( hkResult, strValueName.c_str(), 0, &dwType, lpData, &dwcbData );
		if ( dwResult != ERROR_SUCCESS )
		{
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I����RegQueryValueExʧ�ܣ�������[%d]��������\n\n" ), dwResult ) );
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%IstrKey[%s]\n" ), ACE_TEXT( strKey.c_str() ) ) );
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%IstrValueName[%s]\n\n" ), ACE_TEXT( strValueName.c_str() ) ) );
			
			bSuccess = false;
		}
		else
		{
			if ( REG_SZ == dwType )
			{
				strValue = ( char * )lpData;
			}
			else
			{
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%Iע�����ֵ����[%d]�����ַ����ͣ�\n\n" ), dwType ) );
				bSuccess = false;
			}
		}

		RegCloseKey( hkResult );
	}
	else
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I�޷���%s����������\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
		bSuccess = false;
	}

#endif
	
	return bSuccess;
}

/******************************************************************************
 * FUNCTION    : COsRegistTableMgr::GetRegValue(IN const std::string &strKey, 
 *               IN const std::string &strValueName, 
 *               OUT DWORD &dwValue)
 *       
 * DESCRIPTION : 
 *       -- ��ȡע�����ֵ
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, ע����ֵ
 *       2: strValueName -- IN, ע���������
 *       3: dwValue -- OUT, ע�����ֵ
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : ��ָ����ע���������򴴽���
 *       
 *****************************************************************************/
bool COsRegistTableMgr::GetRegValue(	IN const std::string &strKey,
										IN const std::string &strValueName,
										OUT DWORD &dwValue )
{
	ACE_ASSERT( !strKey.empty() && !strValueName.empty() );
	if ( strKey.empty() || strValueName.empty() )
	{
		ASSERT_INFO;
		return false;
	}
	
	bool	bSuccess = true;

#ifdef OS_WINDOWS
	HKEY	hkResult = NULL;

	DWORD dwAccessType = KEY_ALL_ACCESS;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}

	LONG dwResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKey.c_str(), 0,dwAccessType,&hkResult );
	// ��Ŀ¼
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// ���ý�ֵ������������򴴽���
		dwResult = RegQueryValueEx( hkResult, strValueName.c_str(), 0, &dwType, lpData, &dwcbData );
		if ( dwResult != ERROR_SUCCESS )
		{
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I����RegQueryValueExʧ�ܣ�����\n\n" ) ) );
			bSuccess = false;
		}
		else
		{
			if ( REG_DWORD == dwType )
			{
				dwValue = *( ( DWORD* )lpData );
			}
			else
			{
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%Iע�����ֵ����[%d]���������ͣ�\n\n" ), dwType ) );
				bSuccess = false;
			}
		}

		RegCloseKey( hkResult );
	}
	else
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I�޷���%s����������\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
		bSuccess = false;
	}

#endif
	
	return bSuccess;
}

/******************************************************************************
 * FUNCTION    : COsRegistTableMgr::SetRegValue(IN const std::string &strKey, 
 *               IN const std::string &strValueName, 
 *               IN const std::string &strValue)
 *       
 * DESCRIPTION : 
 *       -- ����ע�����ֵ
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, ע����ֵ
 *       2: strValueName -- IN, ע���������
 *       3: strValue -- IN, ע�����ֵ
 *       
 * RETURN     : 
 *       int -- �ɹ�ɾ������RETURN_VAL_SUCCESS��ԭ״�Ѿ��������󷵻�RETURN_VAL_DO_NOTHING��
 *				����ʧ�ܷ���RETURN_VAL_FAILED��
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
int COsRegistTableMgr::SetRegValue(	IN const std::string &strKey,
										IN const std::string &strValueName,
										IN const std::string &strValue )
{
	ACE_ASSERT( !strKey.empty() && !strValueName.empty() && !strValue.empty() );
	if ( strKey.empty() || strValueName.empty() || strValue.empty() )
	{
		ASSERT_INFO;
		return false;
	}
	
	int nRet = RETURN_VAL_SUCCESS;

#ifdef OS_WINDOWS
	HKEY	hkResult = NULL;

	DWORD dwAccessType = KEY_ALL_ACCESS;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}

	LONG dwResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKey.c_str(),0, dwAccessType, &hkResult );
	// ��Ŀ¼
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// ���ý�ֵ������������򴴽���
		dwResult = RegQueryValueEx( hkResult, strValueName.c_str(), 0, &dwType, lpData, &dwcbData );
		if ( dwResult == ERROR_SUCCESS && 0 == strValue.compare( ( const char* )lpData ) )
		{
			nRet = RETURN_VAL_DO_NOTHING;
		}
		else
		{
			dwResult = RegSetValueEx(	hkResult, 
										strValueName.c_str(), 
										0, 
										REG_SZ, 
										( const unsigned char * )strValue.c_str(), 
										strValue.size() );
			if ( dwResult != ERROR_SUCCESS )
			{
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I����RegSetValueExʧ�ܣ�����\n\n" ) ) );
				nRet = RETURN_VAL_FAILED;
			}
		}
		
		RegCloseKey( hkResult );
	}
	else
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I�޷���%s����������\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
		nRet = RETURN_VAL_FAILED;
	}

#endif
	
	return nRet;
}

/******************************************************************************
 * FUNCTION    : COsRegistTableMgr::SetRegValue(IN const std::string &strKey, 
 *               IN const std::string &strValueName, 
 *               IN DWORD dwValue)
 *       
 * DESCRIPTION : 
 *       -- ����ע�����ֵ
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, ע����ֵ
 *       2: strValueName -- IN, ע���������
 *       3: dwValue -- IN, ע�����ֵ
 *       
 * RETURN     : 
 *       int -- �ɹ�ɾ������RETURN_VAL_SUCCESS��ԭ״�Ѿ��������󷵻�RETURN_VAL_DO_NOTHING��
 *				����ʧ�ܷ���RETURN_VAL_FAILED��
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
int COsRegistTableMgr::SetRegValue(	IN const std::string &strKey,
									IN const std::string &strValueName,
									IN DWORD dwValue )
{
	ACE_ASSERT( !strKey.empty() && !strValueName.empty() );
	if ( strKey.empty() || strValueName.empty() )
	{
		ASSERT_INFO;
		return false;
	}
	
	int nRet = RETURN_VAL_SUCCESS;

#ifdef OS_WINDOWS
	HKEY	hkResult = NULL;

	DWORD dwAccessType = KEY_ALL_ACCESS;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}

	LONG dwResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKey.c_str(), 0, dwAccessType, &hkResult );
	// ��Ŀ¼
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// ���ý�ֵ������������򴴽���
		dwResult = RegQueryValueEx( hkResult, strValueName.c_str(), 0, &dwType, lpData, &dwcbData );
		if ( dwResult == ERROR_SUCCESS && dwValue == *( ( DWORD* )lpData ) )
		{
			nRet = RETURN_VAL_DO_NOTHING;
		}
		else
		{
			dwResult = RegSetValueEx(	hkResult, 
										strValueName.c_str(), 
										0, 
										REG_DWORD, 
										( const unsigned char * )&dwValue, 
										sizeof( dwValue ) );
			if ( dwResult != ERROR_SUCCESS )
			{
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I����RegSetValueExʧ�ܣ�����\n\n" ) ) );
				nRet = RETURN_VAL_FAILED;
			}
		}
		
		RegCloseKey( hkResult );
	}
	else
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I�޷���%s����������\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
		nRet = RETURN_VAL_FAILED;
	}

#endif
	
	return nRet;
}

/******************************************************************************
 * FUNCTION    : COsRegistTableMgr::DelRegValue(IN const std::string &strKey, 
 *               IN const std::string &strValueName)
 *       
 * DESCRIPTION : 
 *       -- ɾ��ע�����
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, ע����ֵ
 *       2: strValueName -- IN, ע���������
 *       
 * RETURN     : 
 *       int -- �ɹ�ɾ������RETURN_VAL_SUCCESS��ԭ״�Ѿ��������󷵻�RETURN_VAL_DO_NOTHING��
 *				����ʧ�ܷ���RETURN_VAL_FAILED��
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
int COsRegistTableMgr::DelRegValue(	IN const std::string &strKey,
									IN const std::string &strValueName )
{
	ACE_ASSERT( !strKey.empty() && !strValueName.empty() );
	if ( strKey.empty() || strValueName.empty() )
	{
		ASSERT_INFO;
		return false;
	}
	
	int nRet = RETURN_VAL_SUCCESS;

#ifdef OS_WINDOWS
	HKEY	hkResult = NULL;

	DWORD dwAccessType = KEY_ALL_ACCESS;

	if( CPF::GetOSVerIs64Bit() && (sizeof(void *) ==4) )
	{
		dwAccessType |= KEY_WOW64_64KEY;
	}

	LONG dwResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, strKey.c_str(), 0, dwAccessType, &hkResult );
	// ��Ŀ¼
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// ���ý�ֵ�����������ɾ����
		dwResult = RegQueryValueEx( hkResult, strValueName.c_str(), 0, &dwType, lpData, &dwcbData );
		if ( dwResult == ERROR_SUCCESS )
		{
			dwResult = RegDeleteValue( hkResult, strValueName.c_str() );
			if ( dwResult != ERROR_SUCCESS )
			{
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I����RegDeleteKeyʧ�ܣ�����\n\n" ) ) );
				nRet = RETURN_VAL_FAILED;
			}
		}
		else
		{
			nRet = RETURN_VAL_DO_NOTHING;
		}
		
		RegCloseKey( hkResult );
	}
	else
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I�޷���%s����������\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
		nRet = RETURN_VAL_FAILED;
	}

#endif
	
	return nRet;
}



