//////////////////////////////////////////////////////////////////////////
//SendPktObserver.h

#pragma once

#include "PacketIO/MyBaseObserver.h"
#include "PacketIO/WriteDataStream.h"
#include "PacketIO/BufferPktThreadObserver.h"


class PACKETIO_CLASS CSendPktObserver : public CMyBaseObserver
{
public:
	CSendPktObserver(void);
	virtual ~CSendPktObserver(void);

	BOOL init(IWriteDataStream * pWriteDataStream);
	void fini();

	ACE_UINT64 GetTotalSendPkts() const;
	ACE_UINT64 GetTotalFailedPkts() const;

public:
	virtual BOOL Reset();
	virtual BOOL OnStart();
	virtual BOOL OnStop(int type);
	virtual BOOL OnPause();
	virtual BOOL OnContinue();
	virtual BOOL Push(PACKET_LEN_TYPE type,RECV_PACKET& packet);

private:
	IWriteDataStream *	m_pWriteDataStream;
};


class PACKETIO_CLASS CSendPktThreadObserver : public CBufferPktThreadObserver
{
public:
	CSendPktThreadObserver()
	{

	}
	virtual ~CSendPktThreadObserver()
	{

	}

public:
	//需要缓存的大小，如果buffersize==0，表示不需要使用缓存和使用多线程
	BOOL init(IWriteDataStream * pWriteDataStream,unsigned int buffersize=5*1024*1024)
	{
		CSendPktObserver * pSendPktObserver = new CSendPktObserver;

		if( !pSendPktObserver->init(pWriteDataStream) )
		{
			delete pSendPktObserver;
			pSendPktObserver = NULL;
			return false;
		}

		return CBufferPktThreadObserver::init(buffersize,pSendPktObserver,true);
	}

};

