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
		STATE_UNINIT,//没有初始化
		STATE_STOPPING,//准备结束
		STATE_IDLE,//还没有开始测试或者已经停止测试
		STATE_RUN,//正在运行
		STATE_PAUSE,//正在运行，但是暂停
	};

	typedef struct  
	{
		ACE_UINT64 ullTotalPackets;
		ACE_UINT64 ullTotalBytes;
		ACE_UINT64 ullTimerPackets;//定时包的数目
		ACE_UINT64 ullReadLoops;//循环次数

		ACE_UINT64 ullDroppedPkts;
		ACE_UINT64 ullDroppedBytes;

		ACE_UINT64 ullRandomDiscardPkts;//随机丢弃的包的个数
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


	//输入的IRecvDataSource已经被opendatasource()了，
	//start将在线程启动后由CMyPlatRunCtrl调用，在线程结束后有CMyPlatRunCtrl调用stop.
	bool StartTest(IRecvDataSource* pdataSource);
	bool StartTest(IRecvDataSource * pdataSource, CSerialDataRecv * pSerialRecv);
	bool StopTest();
	bool PauseTest();
	bool ContinueTest();

	void Reset();

	void GetRunInfo(CMyPlatRunCtrl::PlatRun_Info& runinfo) const;

	const CMyPlatRunCtrl::PlatRun_Info& GetRunInfo() const;

	//得到程序自启动后的运行时长
	inline ACE_Time_Value GetTestTime() const
	{
		if( m_starttime == ACE_Time_Value::zero )
		{//还没有开始测试
			return ACE_Time_Value::zero;
		}
		else
		{
			if( m_stoptime == ACE_Time_Value::zero )//还没有停止测试
				return ACE_OS::gettimeofday()-m_starttime;
			else//已经停止测试了
				return m_stoptime - m_starttime;
		}
	}


	//返回最后一个包的长度类型．主要用于读包循环结束后判断结束原因．
	//如果返回值=PACKET_DRV_ERROR,表示因为驱动出错而中断测试
	//如果返回值=PACKET_NO_MORE,表示因为数据读完毕了而结束测试
	//其他返回值表示用户主动的停止测试．
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

	//等待命令执行结束
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

	ACE_Time_Value	m_starttime;//启动测试时间
	ACE_Time_Value	m_stoptime;//停止测试的时间

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
