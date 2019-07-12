#if !defined(__TS_BACKCOMLAN__)
#define __TS_BACKCOMLAN__

#include "TS_BackCom.h"

class CTS_BackComLan : public CTS_BackCom
{
public:
	CTS_BackComLan();
	virtual ~CTS_BackComLan();

public:
	BOOL f_ForwardIp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
	BOOL f_MulticastIp(PACKET_INFO_EX & packet_info);
	BOOL f_ForwardArp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
};

#endif

