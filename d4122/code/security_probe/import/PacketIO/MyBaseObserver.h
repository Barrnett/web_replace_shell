//////////////////////////////////////////////////////////////////////////
//MyBaseObserver.h

#ifndef MY_BASEOBSERVER_H
#define MY_BASEOBSERVER_H
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "PacketIO/PacketIO.h"

class CMyPlatRunCtrl;

class PACKETIO_CLASS CMyBaseObserver
{
public:
	CMyBaseObserver(void)
	{
		m_pPlatRunCtrl = NULL;
	}
	virtual ~CMyBaseObserver(void)
	{
	}

public:
	virtual BOOL Reset() = 0;
	virtual BOOL OnStart() = 0;
	//type=0��ʾ�û�ֹͣ��type=1��ʾ����Դ����type=2��ʾ���ݻطŽ���
	virtual BOOL OnStop(int type) = 0;
	virtual BOOL OnPause() = 0;
	virtual BOOL OnContinue() = 0;
	virtual BOOL Push(PACKET_LEN_TYPE type,RECV_PACKET& packet) = 0;
	
	virtual int OnAttachOKToPlatCtrl()
	{
		return 0;
	}

	virtual int OnDetachFromPlatCtrl()
	{
		return 0;
	}

public:
	//��ȡ���һ������ʱ��
	const CTimeStamp32& GetLastPktTimeStamp() const;

	CMyPlatRunCtrl * GetPlayRunCtrl()
	{
		return m_pPlatRunCtrl;
	}

protected:
	CMyPlatRunCtrl *	m_pPlatRunCtrl;

	friend class CMyPlatRunCtrl;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif // #ifndef MY_BASEOBSERVER_H

