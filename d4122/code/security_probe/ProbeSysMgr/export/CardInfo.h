/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  CardInfo.h
 *
 *  TITLE      :  Probeϵͳ�忨��Ϣ������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһ�忨��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_CARD_INFOMATION_H__
#define __PROBE_CARD_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// �忨��Ϣ��
class EXPORT_CLASS CCardInfo
{
public:
	CCardInfo();
	~CCardInfo();

	// ��ʼ��
	bool Init( IN const SCardCfgInfo *pCfgInfo = NULL );


private:


public:
	// ���
	int				m_nIndex;
	// ����
	std::string		m_strDescribe;

private:
	// �忨������Ϣ
	SCardCfgInfo	*m_pCfgInfo;

};

#endif // __PROBE_CARD_INFOMATION_H__

