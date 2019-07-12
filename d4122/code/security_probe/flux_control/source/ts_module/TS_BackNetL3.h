#if !defined(__TS_BACKNETL3__)
#define __TS_BACKNETL3__

#include "TS_BackNet.h"

class CTS_BackNetL3 : virtual public CTS_BackNet
{
public:
	CTS_BackNetL3();
	virtual ~CTS_BackNetL3();

public:
	BOOL f_ForwardIp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
	BOOL f_MulticastIp(PACKET_INFO_EX & packet_info);
	BOOL f_ForwardArp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
	BOOL f_PushPktQueue(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index);
	BOOL f_FlushPktQueue(ACE_UINT32 des_index);
};

class CTS_BackNetL3EoSgl1 : public CTS_BackNetL3, public CTS_BackNetEoSgl1
{
public:
	CTS_BackNetL3EoSgl1();
	virtual ~CTS_BackNetL3EoSgl1();

};

class CTS_BackNetL3EoSgl2 : public CTS_BackNetL3, public CTS_BackNetEoSgl2
{
public:
	CTS_BackNetL3EoSgl2();
	virtual ~CTS_BackNetL3EoSgl2();

};

class CTS_BackNetL3EoL2 : public CTS_BackNetL3, public CTS_BackNetEoL2
{
public:
	CTS_BackNetL3EoL2();
	virtual ~CTS_BackNetL3EoL2();

};

class CTS_BackNetL3EoL3 : public CTS_BackNetL3, public CTS_BackNetEoL3
{
public:
	CTS_BackNetL3EoL3();
	virtual ~CTS_BackNetL3EoL3();

};

#endif

