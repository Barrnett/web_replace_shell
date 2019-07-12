/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  FileDirInfo.h
 *
 *  TITLE      :  Probe系统文件目录信息类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一文件目录的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_FILE_DIR_INFOMATION_H__
#define __PROBE_FILE_DIR_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// 文件/目录描述结构
typedef struct _SFileDirContent
{
	std::string strPath;
	std::string strName;
		
	bool bHasSubObj;
	std::vector< _SFileDirContent > vectSubDir;
} SFileDirContent;

// 文件目录信息类
class PROBESYSMGR_CLASS CFileDirInfo
{
public:
	// 获取文件目录信息
	static bool GetDirInfo( IN const std::string &strDir, OUT SFileDirContent &sDirContent );

	// 判断目录strDir下文件strFile是否存在
	static bool IsFileExist( IN const std::string &strDir, IN const std::string &strFile );

	// 递归查找目录strDir，以获取文件strFile存在的绝对路径
	static bool GetFilePath( 	IN const std::string &strDir, 
								IN const std::string &strFile,
								OUT std::string &strFilePath );

private:
	CFileDirInfo(){}
	~CFileDirInfo(){}

	typedef int (*P_FUNC_SELECTOR) (const dirent *entry);
	typedef int (*P_FUNC_COMPARATOR) (const dirent **f1, const dirent **f2);

private:
	
};

#endif // __PROBE_FILE_DIR_INFOMATION_H__

