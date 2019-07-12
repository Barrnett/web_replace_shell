//////////////////////////////////////////////////////////////////////////
//CommonMainCtrl.h

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/MyPlatRunCtrl.h"
#include "PacketIO/DataSourceMgr.h"

class OBSERVER_MGR;

class PACKETIO_CLASS CCommonMainCtrl
{
public:
	enum ENState
	{
		STATE_UNINIT,//û�г�ʼ��
		STATE_STOPPING,//׼������
		STATE_IDLE,//��û�п�ʼ���Ի����Ѿ�ֹͣ����
		STATE_RUN,//��������
		STATE_PAUSE,//�������У�������ͣ
	};

public:
	CCommonMainCtrl(void);
	virtual ~CCommonMainCtrl(void);

public:
	virtual BOOL init(ACE_Time_Value empty_sleep_time,
		const char * datasourc_cfg_name,
		const char * observer_list_cfg_name,
		const char * observer_section_name,
		ACE_Log_Msg * pLogInstance,
		const char * monitor_event_name,
		size_t monitor_event_interval,
		int copy_drv_data);

	virtual BOOL init(ACE_Time_Value empty_sleep_time,
		ACE_Log_Msg * pLogInstance,
		const char * monitor_event_name,
		size_t monitor_event_interval,
		int copy_drv_data);

	//������StartTest֮ǰ���á�Ӧ�����������ʼ����ʱ�򣬼��롣
	void attach_observer(CMyBaseObserver * pObserver);

	virtual void close();

public:
	//pdataSource=NULL,��ʾ���ó�ʼ����ʱ��ʹ�õ������ļ���ָ��������Դ
	virtual bool StartTest(IRecvDataSource* pdataSource=NULL);
	virtual bool StartTest(IRecvDataSource* pdataSource, CSerialDataRecv* pSerialRecv);
	virtual bool StopTest();
	virtual bool PauseTest();
	virtual bool ContinueTest();

public:
	ENState GetRunState() const;
	BOOL IsStopped() const;

	BOOL GetRunInfo(CMyPlatRunCtrl::PlatRun_Info& runinfo) const;
	const CMyPlatRunCtrl::PlatRun_Info& GetRunInfo() const;

	PACKET_LEN_TYPE GetLastPacketLenType() const;

	inline CMyPlatRunCtrl * GetPlatRunCtrl() const
	{	return m_pPlatRunCtrl;	}

	int GetDataSourceConnectState();

private:
	CDataSourceMgr			m_datasource_mgr;

	CMyPlatRunCtrl *		m_pPlatRunCtrl;

	ACE_Log_Msg *			m_pLogInstance;
	BOOL					m_bLogInstanceCreate;

	std::string				m_monitor_event_name;
	size_t					m_monitor_event_interval;

	OBSERVER_MGR 	*		m_pObserverMgr;
	
private:
	BOOL Inner_init(ACE_Time_Value empty_sleep_time,
		ACE_Log_Msg * pLogInstance,
		const char * monitor_event_name,
		size_t monitor_event_interval,
		int copy_drv_data);

	BOOL LoadObserversFromCfg(
		const char * observer_list_cfg_name,
		const char * observer_section_name);

	void UnLoadObserversFromCfg();

	ACE_Log_Msg *ReadyLogMsg(unsigned int log_interval);

	bool WaitProtecoterFree(int sec);
};
