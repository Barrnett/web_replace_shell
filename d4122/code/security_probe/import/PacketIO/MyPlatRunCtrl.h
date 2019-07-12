//////////////////////////////////////////////////////////////////////////
//MyPlatRunCtrl.h

#ifndef MY_PLAT_RUN_CTRL_H_20060411
#define MY_PLAT_RUN_CTRL_H_20060411
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <list>
#include <ace/Task.h>
#include "PacketIO/RecvDataSource.h"
#include "PacketIO/SerialDataRecv.h"
#include "PacketIO/MyBaseObserver.h"
#include "PacketIO/PacketIO.h"
#include "cpf/my_event.h"

class PACKETIO_CLASS CMyPlatRunCtrl : public ACE_Task_Base
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

	typedef struct  
	{
		ACE_UINT64 ullTotalPackets;
		ACE_UINT64 ullTotalBytes;
		ACE_UINT64 ullTimerPackets;//��ʱ������Ŀ
		ACE_UINT64 ullReadLoops;//ѭ������

		ACE_UINT64 ullDroppedPkts;
		ACE_UINT64 ullDroppedBytes;

		ACE_UINT64 ullRandomDiscardPkts;//��������İ��ĸ���
		ACE_UINT64 ullRandomDiscardBytes;

		void reset()
		{
			memset(this,0x00,sizeof(*this));

			ullDroppedPkts = ullDroppedBytes = -1;
		}

	}PlatRun_Info;

public:
	CMyPlatRunCtrl();
	~CMyPlatRunCtrl();

	BOOL init(ACE_Time_Value empty_sleep_time=ACE_Time_Value(0,5*1000),
		ACE_Log_Msg * pLogInstance=NULL,
		const char * monitor_event_name=NULL,
		int monitor_event_interval=0,
		int copy_drv_data =0);

	void close();

public:
	inline CMyPlatRunCtrl::ENState GetRunState() const
	{
		return m_enState;
	}

	inline BOOL IsStopped() const
	{
		return (GetRunState() == CMyPlatRunCtrl::STATE_IDLE
			|| GetRunState() == CMyPlatRunCtrl::STATE_UNINIT);
	}

	bool AttachObserver(CMyBaseObserver* pObserver);
	bool DetachObserver(CMyBaseObserver* pObserver);
	void DetachAllObserver();


	//�����IRecvDataSource�Ѿ���opendatasource()�ˣ�
	//start�����߳���������CMyPlatRunCtrl���ã����߳̽�������CMyPlatRunCtrl����stop.
	bool StartTest(IRecvDataSource* pdataSource);
	bool StartTest(IRecvDataSource * pdataSource, CSerialDataRecv * pSerialRecv);
	bool StopTest();
	bool PauseTest();
	bool ContinueTest();

	void Reset();

	void GetRunInfo(CMyPlatRunCtrl::PlatRun_Info& runinfo) const;

	const CMyPlatRunCtrl::PlatRun_Info& GetRunInfo() const;

	//�õ������������������ʱ��
	inline ACE_Time_Value GetTestTime() const
	{
		if( m_starttime == ACE_Time_Value::zero )
		{//��û�п�ʼ����
			return ACE_Time_Value::zero;
		}
		else
		{
			if( m_stoptime == ACE_Time_Value::zero )//��û��ֹͣ����
				return ACE_OS::gettimeofday()-m_starttime;
			else//�Ѿ�ֹͣ������
				return m_stoptime - m_starttime;
		}
	}


	//�������һ�����ĳ������ͣ���Ҫ���ڶ���ѭ���������жϽ���ԭ��
	//�������ֵ=PACKET_DRV_ERROR,��ʾ��Ϊ����������жϲ���
	//�������ֵ=PACKET_NO_MORE,��ʾ��Ϊ���ݶ�����˶���������
	//��������ֵ��ʾ�û�������ֹͣ���ԣ�
	inline PACKET_LEN_TYPE GetLastPacketLenType() const
	{	return m_lastPktLenType;	}

	inline const CTimeStamp32 & GetLastPktTimeStamp() const
	{
		return m_lastPktStamp;
	}

public:
	inline my_event_t * GetMonitorEvent()
	{
		return m_pMonitorEvent;
	}

	void SignalMonitorEvent();

private:
	virtual int svc(void);

	//�ȴ�����ִ�н���
	void WaitDone();

	void DoCommand();

	void OnStart();

	void OnStop(int type);

	void OnPause();

	void OnContinue();

	inline void PushPacketToObservers(PACKET_LEN_TYPE type,RECV_PACKET&	packet)
	{
		for(int i = 0; i < m_cur_observer_nums; ++i)
		{
			if (!m_arr_observers[i]->Push(type,packet))
			{
				break;
			}
		}
	}

private:
	enum{MAX_OBSERVER_NUMS=16};

	CMyBaseObserver *	m_arr_observers[MAX_OBSERVER_NUMS];
	int					m_cur_observer_nums;

	IRecvDataSource* m_pDataSource;

	CSerialDataRecv* m_pSerialDataRecv;

	my_event_t *	m_pMonitorEvent;
	int			m_monitor_event_interval;

	int			m_copy_drv_data;

	ACE_Time_Value m_empty_sleep_time;

	enum ENCommand
	{
		CMD_NONE,
		CMD_START,
		CMD_STOP,
		CMD_PAUSE,
		CMD_CONTINUE,
	};

	CMyPlatRunCtrl::ENState m_enState;

	ENCommand m_enCmd;

	ACE_Log_Msg * m_pLogInstance;

	ACE_Time_Value	m_starttime;//��������ʱ��
	ACE_Time_Value	m_stoptime;//ֹͣ���Ե�ʱ��

	PACKET_LEN_TYPE	m_lastPktLenType;
	CTimeStamp32	m_lastPktStamp;

	PlatRun_Info	m_runinfo;

private:
	BYTE *			m_pCopyDataBuffer;
	int				m_nCopyDataBufferLen;

};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif // #ifndef MY_PLAT_RUN_CTRL_H_20060411
