/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  FileDirInfo.cpp
 *               
 *  TITLE      :  Probe系统CPU信息类实现
 *               
 *  CREATE TIME:  2005-10-27 15:38:30
 *               
 *  AUTHOR     :  
 *               
 *  DESCRIPTION:  
 * 				
 *****************************************************************************/

#include "FileDirInfo.h"

/********************************************************************
Public Function
*********************************************************************/
/*
int Selector( const dirent *entry )
{
	printf( "selector : entry->d_name = [%s]\n", entry->d_name );

	std::string strDir( entry->d_name );
	if ( strDir.find( "file" ) == -1 )
	{
		return 0;
	}
	return 1;
}

int Comparator( const dirent **f1, const dirent **f2 )
{
	printf( "comparator : \n*f1->d_name = [%s]\n", (*f1)->d_name );
	printf( "*f2->d_name = [%s]\n\n", (*f2)->d_name );
	return 1;
}*/


/******************************************************************************
 * FUNCTION    : CFileDirInfo::GetDirInfo(IN const std::string &strDir, 
 *               OUT SFileDirContent &sDirContent)
 *       
 * DESCRIPTION : 
 *       -- 获取文件目录信息
 *       
 * PARAMETER   : 
 *       1: strDir -- IN, 目录全路径名称
 *       2: sDirContent -- OUT, 目录结构描述对象
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CFileDirInfo::GetDirInfo( IN const std::string &strDir, OUT SFileDirContent &sDirContent )
{
	ACE_ASSERT( !strDir.empty() );
	if ( strDir.empty() )
	{
		return false;
	}

	struct dirent **pList = NULL;
	P_FUNC_SELECTOR pFuncSel = NULL;
	P_FUNC_COMPARATOR pFuncComp = NULL;

	sDirContent.strName = strDir;
	sDirContent.strPath = strDir;
	
//	int nSubNum = ACE_OS::scandir_emulation( strDir.c_str(), &pList, pFuncSel, pFuncComp );
	int nSubNum = ACE_OS::scandir(strDir.c_str(), &pList, NULL, 0);

	if ( nSubNum <= 0 )
	{
		sDirContent.bHasSubObj = false;
		return false;
	}
	sDirContent.bHasSubObj = true;

	for( int i=0; i<nSubNum; i++ )
	{
		struct dirent *pDir = pList[i];
		ACE_ASSERT( pDir );
		if ( NULL == pDir || NULL == pDir->d_name )
		{
			continue;
		}
		
		if (	0 == ACE_OS::strcmp( pDir->d_name, "." )
			||	0 == ACE_OS::strcmp( pDir->d_name, ".." ) )
		{
			continue;
		}
		
		SFileDirContent sTmpContent;
		std::string strSubDir( strDir );
		strSubDir += FILENAME_SEPERATOR_STR;
		strSubDir += pDir->d_name;
		
		if ( !CFileDirInfo::GetDirInfo( strSubDir, sTmpContent ) )
		{
			sTmpContent.strName = pDir->d_name;
			sTmpContent.strPath = strDir;
		}

		sDirContent.vectSubDir.push_back( sTmpContent );
	}
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CFileDirInfo::IsFileExist(IN const std::string &strDir, 
 *               IN const std::string &strFile)
 *       
 * DESCRIPTION : 
 *       -- 判断目录strDir下文件strFile是否存在
 *       
 * PARAMETER   : 
 *       1: strDir -- IN, 目录全路径名称
 *       2: strFile -- IN, 文件名
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CFileDirInfo::IsFileExist( IN const std::string &strDir, IN const std::string &strFile )
{
	ACE_ASSERT( !strDir.empty() && !strFile.empty() );
	if ( strDir.empty() || strFile.empty() )
	{
		return false;
	}

	struct dirent **pList = NULL;
	P_FUNC_SELECTOR pFuncSel = NULL;
	P_FUNC_COMPARATOR pFuncComp = NULL;

//	int nSubNum = ACE_OS::scandir_emulation( strDir.c_str(), &pList, pFuncSel, pFuncComp );
	int nSubNum = ACE_OS::scandir(strDir.c_str(), &pList, NULL, 0);
	if ( nSubNum <= 0 )
	{
		return false;
	}

	for( int i=0; i<nSubNum; i++ )
	{
		struct dirent *pDir = pList[i];
		ACE_ASSERT( pDir );
		if ( NULL == pDir || NULL == pDir->d_name )
		{
			continue;
		}

		if (	0 == ACE_OS::strcmp( pDir->d_name, "." )
			||	0 == ACE_OS::strcmp( pDir->d_name, ".." ) )
		{
			continue;
		}
		
		if ( 0 == ACE_OS::strcmp( pDir->d_name, strFile.c_str() ) )
		{
			return true;
		}
	}
	
	return false;
}

/******************************************************************************
 * FUNCTION    : CFileDirInfo::GetFilePath( IN const std::string &strDir, 
 *               IN const std::string &strFile, 
 *               OUT std::string &strFilePath)
 *       
 * DESCRIPTION : 
 *       -- 递归查找目录strDir，以获取文件strFile存在的绝对路径
 *       
 * PARAMETER   : 
 *       1: strDir -- IN, 目录全路径名称
 *       2: strFile -- IN, 文件名
 *       3: strFilePath -- OUT, 文件strFile存在的绝对路径名称
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CFileDirInfo::GetFilePath(	IN const std::string &strDir, 
								IN const std::string &strFile,
								OUT std::string &strFilePath )
{
	ACE_ASSERT( !strDir.empty() && !strFile.empty() );
	if ( strDir.empty() || strFile.empty() )
	{
		return false;
	}

	struct dirent **pList = NULL;
	P_FUNC_SELECTOR pFuncSel = NULL;
	P_FUNC_COMPARATOR pFuncComp = NULL;

//	int nSubNum = ACE_OS::scandir_emulation( strDir.c_str(), &pList, pFuncSel, pFuncComp );
	int nSubNum = ACE_OS::scandir(strDir.c_str(), &pList, NULL, 0);
	if ( nSubNum <= 0 )
	{
		return false;
	}

	for( int i=0; i<nSubNum; i++ )
	{
		struct dirent *pDir = pList[i];
		ACE_ASSERT( pDir );
		if ( NULL == pDir || NULL == pDir->d_name )
		{
			continue;
		}
		
		if (	0 == ACE_OS::strcmp( pDir->d_name, "." )
			||	0 == ACE_OS::strcmp( pDir->d_name, ".." ) )
		{
			continue;
		}
		
		if ( 0 == ACE_OS::strcmp( pDir->d_name, strFile.c_str() ) )
		{
			strFilePath = strDir;
			return true;
		}
		
		SFileDirContent sTmpContent;
		std::string strSubDir( strDir );
		strSubDir += FILENAME_SEPERATOR_STR;
		strSubDir += pDir->d_name;
		
		if ( CFileDirInfo::GetFilePath( strSubDir, strFile, strFilePath ) )
		{
			return true;
		}
	}
	
	return false;
}




