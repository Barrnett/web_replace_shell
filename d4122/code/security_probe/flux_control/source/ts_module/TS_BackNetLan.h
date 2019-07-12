#if !defined(__TS_BACKNETLAN__)
#define __TS_BACKNETLAN__

#include "TS_BackNet.h"

class CTS_BackNetLan : virtual public CTS_BackNet
{
public:
	CTS_BackNetLan();
	virtual ~CTS_BackNetLan();

public:
	CTS_PACKET_QUEUE m_iBroadcastArpQueue;

public:
	BOOL f_ForwardIp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
	BOOL f_MulticastIp(PACKET_INFO_EX & packet_info);
	BOOL f_ForwardArp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
	BOOL f_PushPktQueue(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
	BOOL f_FlushPktQueue(ACE_UINT32 des_index);
};

class CTS_BackNetLanEoSgl1 : public CTS_BackNetLan, public CTS_BackNetEoSgl1
{
public:
	CTS_BackNetLanEoSgl1();
	virtual ~CTS_BackNetLanEoSgl1();

};

class CTS_BackNetLanEoSgl2 : public CTS_BackNetLan, public CTS_BackNetEoSgl2
{
public:
	CTS_BackNetLanEoSgl2();
	virtual ~CTS_BackNetLanEoSgl2();

};

class CTS_BackNetLanEoL2 : public CTS_BackNetLan, public CTS_BackNetEoL2
{
public:
	CTS_BackNetLanEoL2();
	virtual ~CTS_BackNetLanEoL2();

};

#endif

