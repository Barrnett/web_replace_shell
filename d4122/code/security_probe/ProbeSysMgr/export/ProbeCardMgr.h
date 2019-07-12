/******************************************************************************
 *  COPYRIGHTS :  Beijing ZhongChuang Telecom Test Co.,Ltd.
 *
 *  FILE NAME  :  CardMgr.h
 *
 *  TITLE      :  Probeϵͳ�忨����������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  ����
 *
 *  DESCRIPTION:  ������������ṩ���а忨��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_CARD_MANAGER_H__
#define __PROBE_CARD_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CardInfo.h"

// Probeϵͳ�忨������
class EXPORT_CLASS CProbeCardMgr
{
public:
	CProbeCardMgr();
	~CProbeCardMgr();

	// ��ʼ��
	bool Init( IN const SCardCfgInfo *pCfgInfo = NULL );

	// ��ȡ����ذ忨����
	int GetCardCount() const
	{	return m_vectInfo.size();	}

	// ����Ż�ȡ�忨��Ϣ
	const CCardInfo* GetCardInfoByNth( IN int nNth );
	
private:

private:
	// �忨������Ϣ
	SCardCfgInfo				*m_pCfgInfo;

	// �忨��Ϣ���������
	std::vector< CCardInfo* >	m_vectInfo;

};

#endif // __PROBE_CARD_MANAGER_H__

