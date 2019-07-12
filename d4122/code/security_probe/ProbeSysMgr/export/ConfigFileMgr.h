/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ConfigFileMgr.h
 *
 *  TITLE      :  Probeϵͳ���ά��ϵͳ�����ļ�����������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:
 *
 *****************************************************************************/

#ifndef __CONFIG_FILE_MANAGER_H__
#define __CONFIG_FILE_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

class CIniFile;

// Probeϵͳ���ά��ϵͳ�����ļ�������
class PROBESYSMGR_CLASS CConfigFileMgr
{
public:
	CConfigFileMgr();
	~CConfigFileMgr();

	// ��ʼ��
	bool Init( IN const std::string &strCfgFileName );

	// ��ȡ�����ļ�ȫ·������
	const std::string& GetFileName() const
	{	return m_strFileName;	}


	// ��ȡSNMP������Ϣ
	const SSnmpCfgInfo* GetSnmpCfgInfo() const
	{	return m_pSnmpCfgInfo;	}

	// ��ȡ����������Ϣ
	const SCtlCfgInfo* GetCtlCfgInfo() const
	{	return m_pCtlCfgInfo;	}
	
	// ��ȡ��־������Ϣ
	const SLogCfgInfo* GetLogCfgInfo() const
	{	return m_pLogCfgInfo;	}

	
	// ��ȡCPU������Ϣ
	const SCpuCfgInfo* GetCpuCfgInfo() const
	{	return m_pCpuCfgInfo;	}

	// ��ȡ�ڴ�������Ϣ
	const SMemCfgInfo* GetMemCfgInfo() const
	{	return m_pMemCfgInfo;	}

	// ��ȡ����������Ϣ
	const SProcCfgInfo* GetProcCfgInfo() const
	{	return m_pProcCfgInfo;	}

	// ��ȡ����������Ϣ
	const SDiskCfgInfo* GetDiskCfgInfo() const
	{	return m_pDiskCfgInfo;	}

	// ��ȡ�ļ�ϵͳ������Ϣ
	const SFileSysCfgInfo* GetFileSysCfgInfo() const
	{	return m_pFileSysCfgInfo;	}

	// ��ȡ�忨������Ϣ
	const SCardCfgInfo* GetCardCfgInfo() const
	{	return m_pCardCfgInfo;	}

	// ��ȡ����������Ϣ
	const SNetCfgInfo* GetNetCfgInfo() const
	{	return m_pNetCfgInfo;	}

private:
	// ��ȡSNMP������Ϣ
	bool getSnmpCfgInfo();

	// ��ȡ����������Ϣ
	bool getCtlCfgInfo();

	// ��ȡ��־������Ϣ
	bool getLogCfgInfo();

	// ��ȡCPU������Ϣ
	bool getCpuCfgInfo();

	// ��ȡ�ڴ�������Ϣ
	bool getMemCfgInfo();

	// ��ȡ����������Ϣ
	bool getDiskCfgInfo();

	// ��ȡ�ļ�ϵͳ������Ϣ
	bool getFileSysCfgInfo();

	// ��ȡ�忨������Ϣ
	bool getCardCfgInfo();

	// ��ȡ����������Ϣ
	bool getNetCfgInfo();

	// ��ȡ����������Ϣ
	bool getProcCfgInfo();

private:
	// �����ļ�����
	std::string		m_strFileName;

	// �����ļ����ʶ���
	CIniFile		*m_pCfgFile;


	// SNMP������Ϣ
	SSnmpCfgInfo	*m_pSnmpCfgInfo;

	// ��־������Ϣ
	SLogCfgInfo		*m_pLogCfgInfo;

	// ����������Ϣ
	SCtlCfgInfo		*m_pCtlCfgInfo;

	// CPU������Ϣ
	SCpuCfgInfo		*m_pCpuCfgInfo;

	// �ڴ�������Ϣ
	SMemCfgInfo		*m_pMemCfgInfo;

	// ����������Ϣ
	SDiskCfgInfo	*m_pDiskCfgInfo;

	// �ļ�ϵͳ������Ϣ
	SFileSysCfgInfo	*m_pFileSysCfgInfo;

	// �忨������Ϣ
	SCardCfgInfo	*m_pCardCfgInfo;

	// ����������Ϣ
	SNetCfgInfo		*m_pNetCfgInfo;

	// ����������Ϣ
	SProcCfgInfo	*m_pProcCfgInfo;

};

#endif // __CONFIG_FILE_MANAGER_H__

