#if !defined(__TS_MIRRORL2__)
#define __TS_MIRRORL2__

#include "TS_Mirror.h"

class CTS_MirrorL2 : public CTS_Mirror
{
public:
	CTS_MirrorL2();
	virtual ~CTS_MirrorL2();

public:
	BOOL f_HandleArp(PACKET_INFO_EX & packet_info);
	BOOL f_HandleIp(PACKET_INFO_EX & packet_info);
};

#endif

