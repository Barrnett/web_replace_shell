/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  FileDirInfo.h
 *
 *  TITLE      :  Probeϵͳ�ļ�Ŀ¼��Ϣ������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһ�ļ�Ŀ¼��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_FILE_DIR_INFOMATION_H__
#define __PROBE_FILE_DIR_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// �ļ�/Ŀ¼�����ṹ
typedef struct _SFileDirContent
{
	std::string strPath;
	std::string strName;
		
	bool bHasSubObj;
	std::vector< _SFileDirContent > vectSubDir;
} SFileDirContent;

// �ļ�Ŀ¼��Ϣ��
class PROBESYSMGR_CLASS CFileDirInfo
{
public:
	// ��ȡ�ļ�Ŀ¼��Ϣ
	static bool GetDirInfo( IN const std::string &strDir, OUT SFileDirContent &sDirContent );

	// �ж�Ŀ¼strDir���ļ�strFile�Ƿ����
	static bool IsFileExist( IN const std::string &strDir, IN const std::string &strFile );

	// �ݹ����Ŀ¼strDir���Ի�ȡ�ļ�strFile���ڵľ���·��
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

