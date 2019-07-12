#if !defined(__TS_BACKCOML3__)
#define __TS_BACKCOML3__

#include "TS_BackCom.h"

class CTS_BackComL3 : public CTS_BackCom
{
public:
	CTS_BackComL3();
	virtual ~CTS_BackComL3();

public:
	BOOL f_ForwardIp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
	BOOL f_MulticastIp(PACKET_INFO_EX & packet_info);
	BOOL f_ForwardArp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
};

#endif

