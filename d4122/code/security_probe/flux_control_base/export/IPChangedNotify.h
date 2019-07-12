//////////////////////////////////////////////////////////////////////////
//IPChangedNotify.h

#pragma once
#include "ace/Task.h"
#include "flux_control_base_export.h"

class FLUX_CONTROL_BASE_CLASS CIPChangeNofityObserver
{
public:
	CIPChangeNofityObserver(){}
	virtual ~CIPChangeNofityObserver(){}

public:
	virtual int OnIPChanged() = 0;

};

class FLUX_CONTROL_BASE_CLASS CIPChangedNotify : public ACE_Task_Base
{
public:
	CIPChangedNotify(void);
	virtual ~CIPChangedNotify(void);

public:
	int Start(CIPChangeNofityObserver * pObserver);
	int Stop();

protected:
	CIPChangeNofityObserver *	m_pObserver;

private:
	virtual int svc(void);
	bool m_bUserStop;

// 暂时没找到自动通知的方法，自己写一个监控程序来判断
#ifdef OS_LINUX
	typedef struct __ST_DEV_INFO_SAVE__
	{
		char		szDevName[16];
		char		szDevIP[16];
	}ST_DEV_INFO_SAVE;
	ST_DEV_INFO_SAVE		m_stDevInfo[16];
	int									m_iDevCount;
#endif
};
