/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  OsRegistTableMgr.h
 *
 *  TITLE      :  ����ϵͳע������������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩ�Բ���ϵͳע�����в����ķ�����
 *
 *****************************************************************************/

#ifndef __PROBE_OS_REGISTTABLE_MANAGER_H__
#define __PROBE_OS_REGISTTABLE_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// ��������ֵ����
#define RETURN_VAL_SUCCESS			1		// �����ɹ�
#define RETURN_VAL_DO_NOTHING		0		// ԭ״�Ѿ���������
#define RETURN_VAL_FAILED			-1		// ����ʧ��


// ����ϵͳע��������
class PROBESYSMGR_CLASS COsRegistTableMgr
{
public:
	// ��ȡע������ַ���ֵ����ָ����ע���������򴴽���
	static bool GetRegValue(	IN const std::string &strKey,
								IN const std::string &strValueName,
								OUT std::string &strValue );

	// ��ȡע���������ֵ����ָ����ע���������򴴽���
	static bool GetRegValue(	IN const std::string &strKey,
								IN const std::string &strValueName,
								OUT DWORD &dwValue );

	// ����ע������ַ���ֵ
	static int SetRegValue(	IN const std::string &strKey,
							IN const std::string &strValueName,
							IN const std::string &strValue );

	// ����ע���������ֵ
	static int SetRegValue(	IN const std::string &strKey,
							IN const std::string &strValueName,
							IN DWORD dwValue );

	// ɾ��ע�����
	static int DelRegValue(	IN const std::string &strKey,
							IN const std::string &strValueName );

private:
	COsRegistTableMgr(){}
	~COsRegistTableMgr(){}

private:
	
};

#endif // __PROBE_OS_REGISTTABLE_MANAGER_H__

