/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  CardInfo.h
 *
 *  TITLE      :  Probe系统板卡信息类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一板卡的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_CARD_INFOMATION_H__
#define __PROBE_CARD_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// 板卡信息类
class EXPORT_CLASS CCardInfo
{
public:
	CCardInfo();
	~CCardInfo();

	// 初始化
	bool Init( IN const SCardCfgInfo *pCfgInfo = NULL );


private:


public:
	// 序号
	int				m_nIndex;
	// 描述
	std::string		m_strDescribe;

private:
	// 板卡配置信息
	SCardCfgInfo	*m_pCfgInfo;

};

#endif // __PROBE_CARD_INFOMATION_H__

