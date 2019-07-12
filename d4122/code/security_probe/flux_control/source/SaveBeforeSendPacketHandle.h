//////////////////////////////////////////////////////////////////////////
//SaveBeforeSendPacketHandle.h

#pragma once

#include "PacketIO/WriteDataStream.h"

class CFluxControlObserver;

class CSaveBeforeSendPacketHandle : public IBeforeSendPacketHandle
{
public:
	CSaveBeforeSendPacketHandle(CFluxControlObserver * pFluxControlObserver);

public:
	virtual const char* BeforeSendPacket(int card_index_array,const char * pdata, unsigned int datalen, unsigned int& outdatalen);

private:
	CFluxControlObserver *	m_pFluxControlObserver;
};


