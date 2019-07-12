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

//�ӵ�һ����Ч����ʼ��ÿ�����ٸ�ms�У�����һ����ʱʱ��
class PACKETIO_CLASS CVPKGenerator  
{
public:
	CVPKGenerator(int ms=1000);
	~CVPKGenerator();	

	//����ж�ʱ���������򷵻ض�ʱ��ʱ��
	//���û�У�����NULL
	const CTimeStamp32* GenratorVirtualPkt(const CTimeStamp32& lastPktTs);	

	void reset();

private:
	CTimeStamp32	m_stamp;
	int				m_ms;

};

#endif // !defined(AFX_VPKGENERATOR_H__BF246547_B5DB_410B_AB02_E10243B2185F__INCLUDED_)
