/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  OsRegistTableMgr.cpp
 *               
 *  TITLE      :  操作系统注册表管理类实现
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
 *       -- 获取注册表项值
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, 注册表键值
 *       2: strValueName -- IN, 注册表项名称
 *       3: strValue -- OUT, 注册表项值
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 若指定的注册表项不存在则创建它
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
	// 打开目录
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// 设置健值，如果不存在则创建它
		dwResult = RegQueryValueEx( hkResult, strValueName.c_str(), 0, &dwType, lpData, &dwcbData );
		if ( dwResult != ERROR_SUCCESS )
		{
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I调用RegQueryValueEx失败（错误码[%d]）！！！\n\n" ), dwResult ) );
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
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I注册表项值类型[%d]不是字符串型！\n\n" ), dwType ) );
				bSuccess = false;
			}
		}

		RegCloseKey( hkResult );
	}
	else
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I无法打开%s主键！！！\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
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
 *       -- 获取注册表项值
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, 注册表键值
 *       2: strValueName -- IN, 注册表项名称
 *       3: dwValue -- OUT, 注册表项值
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 若指定的注册表项不存在则创建它
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
	// 打开目录
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// 设置健值，如果不存在则创建它
		dwResult = RegQueryValueEx( hkResult, strValueName.c_str(), 0, &dwType, lpData, &dwcbData );
		if ( dwResult != ERROR_SUCCESS )
		{
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I调用RegQueryValueEx失败！！！\n\n" ) ) );
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
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I注册表项值类型[%d]不是整数型！\n\n" ), dwType ) );
				bSuccess = false;
			}
		}

		RegCloseKey( hkResult );
	}
	else
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I无法打开%s主键！！！\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
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
 *       -- 设置注册表项值
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, 注册表键值
 *       2: strValueName -- IN, 注册表项名称
 *       3: strValue -- IN, 注册表项值
 *       
 * RETURN     : 
 *       int -- 成功删除返回RETURN_VAL_SUCCESS，原状已经符合请求返回RETURN_VAL_DO_NOTHING，
 *				操作失败返回RETURN_VAL_FAILED。
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
	// 打开目录
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// 设置健值，如果不存在则创建它
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
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I调用RegSetValueEx失败！！！\n\n" ) ) );
				nRet = RETURN_VAL_FAILED;
			}
		}
		
		RegCloseKey( hkResult );
	}
	else
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I无法打开%s主键！！！\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
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
 *       -- 设置注册表项值
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, 注册表键值
 *       2: strValueName -- IN, 注册表项名称
 *       3: dwValue -- IN, 注册表项值
 *       
 * RETURN     : 
 *       int -- 成功删除返回RETURN_VAL_SUCCESS，原状已经符合请求返回RETURN_VAL_DO_NOTHING，
 *				操作失败返回RETURN_VAL_FAILED。
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
	// 打开目录
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// 设置健值，如果不存在则创建它
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
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I调用RegSetValueEx失败！！！\n\n" ) ) );
				nRet = RETURN_VAL_FAILED;
			}
		}
		
		RegCloseKey( hkResult );
	}
	else
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I无法打开%s主键！！！\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
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
 *       -- 删除注册表项
 *       
 * PARAMETER   : 
 *       1: strKey -- IN, 注册表键值
 *       2: strValueName -- IN, 注册表项名称
 *       
 * RETURN     : 
 *       int -- 成功删除返回RETURN_VAL_SUCCESS，原状已经符合请求返回RETURN_VAL_DO_NOTHING，
 *				操作失败返回RETURN_VAL_FAILED。
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
	// 打开目录
	if ( dwResult == ERROR_SUCCESS )
	{
		DWORD	dwType = REG_SZ;
		BYTE	lpData[1024] = { 0 };
		DWORD	dwcbData = sizeof( lpData );

		// 设置健值，如果存在则删除它
		dwResult = RegQueryValueEx( hkResult, strValueName.c_str(), 0, &dwType, lpData, &dwcbData );
		if ( dwResult == ERROR_SUCCESS )
		{
			dwResult = RegDeleteValue( hkResult, strValueName.c_str() );
			if ( dwResult != ERROR_SUCCESS )
			{
				ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I调用RegDeleteKey失败！！！\n\n" ) ) );
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
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n%I无法打开%s主键！！！\n\n" ), ACE_TEXT( strKey.c_str() ) ) );
		nRet = RETURN_VAL_FAILED;
	}

#endif
	
	return nRet;
}



