#if !defined(__TS_MIRRORL3__)
#define __TS_MIRRORL3__

#include "TS_Mirror.h"

class CTS_MirrorL3 : public CTS_Mirror
{
public:
	CTS_MirrorL3();
	virtual ~CTS_MirrorL3();

public:
	BOOL f_HandleArp(PACKET_INFO_EX & packet_info);
	BOOL f_HandleIp(PACKET_INFO_EX & packet_info);
};

#endif

