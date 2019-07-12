#if !defined(__TS_BACKCOML2__)
#define __TS_BACKCOML2__

#include "TS_BackCom.h"

class CTS_BackComL2 : public CTS_BackCom
{
public:
	CTS_BackComL2();
	virtual ~CTS_BackComL2();

public:
	BOOL f_ForwardIp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
	BOOL f_MulticastIp(PACKET_INFO_EX & packet_info);
	BOOL f_ForwardArp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
};

#endif

