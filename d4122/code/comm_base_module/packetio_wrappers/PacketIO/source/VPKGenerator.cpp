// VPKGenerator.cpp: implementation of the CVPKGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "PacketIO/VPKGenerator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVPKGenerator::CVPKGenerator(int ms)
:m_ms(ms)
{
	reset();
}

CVPKGenerator::~CVPKGenerator()
{

}

void CVPKGenerator::reset()
{
	m_stamp = CTimeStamp32::zero;
}


//产生定时按照整秒来产生
const CTimeStamp32* CVPKGenerator::GenratorVirtualPkt(const CTimeStamp32& lastPktTs)
{
	if( m_stamp.m_ts.sec < lastPktTs.m_ts.sec )
	{
		if( m_stamp.GetSEC() == 0 )
		{
			m_stamp.m_ts.sec = lastPktTs.m_ts.sec;
			return NULL;
		}

		++m_stamp.m_ts.sec;
		return &m_stamp;
	}

	return NULL;
}

