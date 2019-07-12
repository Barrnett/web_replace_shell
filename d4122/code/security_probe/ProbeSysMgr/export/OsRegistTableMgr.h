/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  OsRegistTableMgr.h
 *
 *  TITLE      :  操作系统注册表管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供对操作系统注册表进行操作的方法；
 *
 *****************************************************************************/

#ifndef __PROBE_OS_REGISTTABLE_MANAGER_H__
#define __PROBE_OS_REGISTTABLE_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// 整数返回值定义
#define RETURN_VAL_SUCCESS			1		// 操作成功
#define RETURN_VAL_DO_NOTHING		0		// 原状已经符合请求
#define RETURN_VAL_FAILED			-1		// 操作失败


// 操作系统注册表管理类
class PROBESYSMGR_CLASS COsRegistTableMgr
{
public:
	// 获取注册表项字符串值，若指定的注册表项不存在则创建它
	static bool GetRegValue(	IN const std::string &strKey,
								IN const std::string &strValueName,
								OUT std::string &strValue );

	// 获取注册表项整数值，若指定的注册表项不存在则创建它
	static bool GetRegValue(	IN const std::string &strKey,
								IN const std::string &strValueName,
								OUT DWORD &dwValue );

	// 设置注册表项字符串值
	static int SetRegValue(	IN const std::string &strKey,
							IN const std::string &strValueName,
							IN const std::string &strValue );

	// 设置注册表项整数值
	static int SetRegValue(	IN const std::string &strKey,
							IN const std::string &strValueName,
							IN DWORD dwValue );

	// 删除注册表项
	static int DelRegValue(	IN const std::string &strKey,
							IN const std::string &strValueName );

private:
	COsRegistTableMgr(){}
	~COsRegistTableMgr(){}

private:
	
};

#endif // __PROBE_OS_REGISTTABLE_MANAGER_H__

