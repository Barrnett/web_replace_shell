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
	//type=0��ʾ�û�ֹͣ��type=1��ʾ����Դ����type=2��ʾ���ݻطŽ���
	BOOL OnStop(int type);

	BOOL OnPause();
	BOOL OnContinue();
	BOOL Push(PACKET_LEN_TYPE type,RECV_PACKET& packet);

private:
	CDataSendMgr *	m_pDataSendMgr;
};
