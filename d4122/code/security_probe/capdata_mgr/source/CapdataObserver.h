//////////////////////////////////////////////////////////////////////////
//CapdataObserver.h

#pragma once

#include "capdata_mgr.h"
#include "PacketIO/MyBaseObserver.h"

class CDataSendMgr;

class CAPDATA_MGR_CLASS CCapdataObserver : public CMyBaseObserver
{
public:
	CCapdataObserver(void);
	virtual ~CCapdataObserver(void);

	BOOL init();
	void fini();

public:
	BOOL Reset();
	BOOL OnStart();
	//type=0表示用户停止，type=1表示数据源出错，type=2表示数据回放结束
	BOOL OnStop(int type);

	BOOL OnPause();
	BOOL OnContinue();
	BOOL Push(PACKET_LEN_TYPE type,RECV_PACKET& packet);

private:
	CDataSendMgr *	m_pDataSendMgr;
};
