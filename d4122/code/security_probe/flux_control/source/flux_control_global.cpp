//////////////////////////////////////////////////////////////////////////
//flux_control_global.cpp

#include "stdafx.h"
#include "flux_control_global.h"
#include "FluxControlFrameObserver.h"
#include "FluxControlObserver.h"

const CTimeStamp32 & G_GetLastPktTimeStamp()
{
	if( g_pFluxControlFrameObserver )
	{
		return g_pFluxControlFrameObserver->GetLastPktTimeStamp();
	}
	else
	{
		static CTimeStamp32 cur_time_stamp;

		cur_time_stamp = ACE_OS::gettimeofday();

		return cur_time_stamp;
	}
}
