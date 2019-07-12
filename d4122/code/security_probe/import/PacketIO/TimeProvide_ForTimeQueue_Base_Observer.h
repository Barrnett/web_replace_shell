//////////////////////////////////////////////////////////////////////////
//TimeProvide_ForTimeQueue_Base_Observer.h
//基于BaseObserver为ace定时器提供时间的类

#pragma once

#include "ace/Timer_Queue_T.h"
#include "PacketIO/MyBaseObserver.h"


class PACKETIO_CLASS CTimeProvide_ForTimeQueue_Base_Observer
{
public:
	static void MyBaseObserver(CMyBaseObserver * pMyObserver)
	{
		ms_pMyObserver = pMyObserver;
	}

public:
	static ACE_Time_Value gettimeofday()
	{
		ACE_ASSERT(ms_pMyObserver);

		return ms_pMyObserver->GetLastPktTimeStamp().m_ts.to_ace_timevalue();
	}	

private:
	static CMyBaseObserver *	ms_pMyObserver;

};


