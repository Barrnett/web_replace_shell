//////////////////////////////////////////////////////////////////////////
//FluxControlFrameObserver.h

#pragma once

#include "flux_control_export.h"
#include "PacketIO/MyBaseObserver.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "PacketIO/PacketIO.h"
#include "IPChangedNotify.h"
#include "cpf/ComImp.h"
#include "tcpip_mgr_common_init.h"

class CFluxControlObserver;
class CCheckStopEventThread;
class IWriteDataStream;
class CIPFragmentComp;

class CFluxControlFrameObserver : public CMyBaseObserver
											, public CIPChangeNofityObserver
{
public:
	CFluxControlFrameObserver(void);
	virtual ~CFluxControlFrameObserver(void);

public:
	BOOL init(const char * drv_name);
	void fin();

public:
	void SetStopEventInfo(const char * stop_event_name,int stop_event_interval);

	//是否收到的结束的事件
	BOOL IsGetStopEvent();

protected:
	virtual int OnAttachOKToPlatCtrl();

public:
	BOOL Reset();
	BOOL OnStart();
	//type=0表示用户停止，type=1表示数据源出错，type=2表示数据回放结束
	BOOL OnStop(int type);
	BOOL OnPause();
	BOOL OnContinue();
	BOOL Push(PACKET_LEN_TYPE type,RECV_PACKET& packet);

private:
	CFluxControlObserver *			m_pFluxControlObserver;
	// add by xumx, 2016-12-12
	char *f_get_system_version();
	char m_version_string[64];

private:
	void HandlePacket(PACKET_LEN_TYPE type,RECV_PACKET& packet);
	void HandleTimer(PACKET_LEN_TYPE type,RECV_PACKET& packet);

private:
	ACE_Log_File_Msg	m_log_file;

	unsigned int		m_nHandlePkts;

	std::string			m_str_drv_name;

public:
	virtual int OnIPChanged();

private:
	CIPChangedNotify *			m_pIPChangedNotify;

private:
	CCheckStopEventThread *		m_pCheckStopEventThread;

private:
	std::string		m_stop_event_name;
	int				m_stop_event_interval;

};

extern CFluxControlFrameObserver *			g_pFluxControlFrameObserver;;

