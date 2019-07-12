// VPKGenerator.h: interface for the CVPKGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VPKGENERATOR_H__BF246547_B5DB_410B_AB02_E10243B2185F__INCLUDED_)
#define AFX_VPKGENERATOR_H__BF246547_B5DB_410B_AB02_E10243B2185F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cpf/TimeStamp32.h"
#include "PacketIO/PacketIO.h"

//从第一个有效包开始，每隔多少个ms中，产生一个定时时戳
class PACKETIO_CLASS CVPKGenerator  
{
public:
	CVPKGenerator(int ms=1000);
	~CVPKGenerator();	

	//如果有定时包产生，则返回定时的时间
	//如果没有，返回NULL
	const CTimeStamp32* GenratorVirtualPkt(const CTimeStamp32& lastPktTs);	

	void reset();

private:
	CTimeStamp32	m_stamp;
	int				m_ms;

};

#endif // !defined(AFX_VPKGENERATOR_H__BF246547_B5DB_410B_AB02_E10243B2185F__INCLUDED_)
