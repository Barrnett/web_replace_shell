#if !defined(__TS_MIRRORLAN__)
#define __TS_MIRRORLAN__

#include "TS_Mirror.h"

class CTS_MirrorLan : public CTS_Mirror
{
public:
	CTS_MirrorLan();
	virtual ~CTS_MirrorLan();

public:
	BOOL f_HandleArp(PACKET_INFO_EX & packet_info);
	BOOL f_HandleIp(PACKET_INFO_EX & packet_info);
};

#endif

