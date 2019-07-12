//////////////////////////////////////////////////////////////////////////
//MyBaseObserver.cpp

#include "PacketIO/MyBaseObserver.h"
#include "PacketIO/MyPlatRunCtrl.h"

//获取最后一个包的时戳
const CTimeStamp32& CMyBaseObserver::GetLastPktTimeStamp() const
{
	if( m_pPlatRunCtrl )
	{
		return m_pPlatRunCtrl->GetLastPktTimeStamp();
	}
	else
	{
		static CTimeStamp32 cur_time_stamp;

		cur_time_stamp = ACE_OS::gettimeofday();

		return cur_time_stamp;
	}
}


