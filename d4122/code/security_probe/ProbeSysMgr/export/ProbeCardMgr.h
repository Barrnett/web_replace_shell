/******************************************************************************
 *  COPYRIGHTS :  Beijing ZhongChuang Telecom Test Co.,Ltd.
 *
 *  FILE NAME  :  CardMgr.h
 *
 *  TITLE      :  Probe系统板卡管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  安乐
 *
 *  DESCRIPTION:  负责管理并对外提供所有板卡的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_CARD_MANAGER_H__
#define __PROBE_CARD_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CardInfo.h"

// Probe系统板卡管理类
class EXPORT_CLASS CProbeCardMgr
{
public:
	CProbeCardMgr();
	~CProbeCardMgr();

	// 初始化
	bool Init( IN const SCardCfgInfo *pCfgInfo = NULL );

	// 获取被监控板卡个数
	int GetCardCount() const
	{	return m_vectInfo.size();	}

	// 按序号获取板卡信息
	const CCardInfo* GetCardInfoByNth( IN int nNth );
	
private:

private:
	// 板卡配置信息
	SCardCfgInfo				*m_pCfgInfo;

	// 板卡信息类对象链表
	std::vector< CCardInfo* >	m_vectInfo;

};

#endif // __PROBE_CARD_MANAGER_H__

