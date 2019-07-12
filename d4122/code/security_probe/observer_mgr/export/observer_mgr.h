#ifndef _OBSERVER_MGR_H_2006_05_11
#define _OBSERVER_MGR_H_2006_05_11
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"
#include "PacketIO/MyPlatRunCtrl.h"



#if defined(OBSERVER_MGR_EXPORTS)
#define OBSERVERMGR_API		MPLAT_API_Export_Flag
#define OBSERVERMGR_CLASS	MPLAT_CLASS_Export_Flag
#else
#define OBSERVERMGR_API		MPLAT_API_Import_Flag
#define OBSERVERMGR_CLASS	MPLAT_CLASS_Import_Flag
#endif

class CMyBaseObserver;
class OBSERVER_MGR;
class ACE_Log_File_Msg;


class OBSERVERMGR_CLASS CObserverMgrFunc
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
	CObserverMgrFunc();
	~CObserverMgrFunc();

	//�������Ϊ�գ���ʾʹ��ACE_Log_Msg::instance();
	BOOL init(ACE_Log_Msg * pLogInstance=NULL,const char * monitor_event_name=NULL,size_t monitor_event_interval=0);
	void close();

	bool StartTest(IRecvDataSource* pdataSource);
	bool StopTest();
	bool PauseTest();
	bool ContinueTest();

	//������ֹͣ���Ե�ʱ����ܵ���
	void Reset();

	ENState GetRunState() const;
	BOOL IsStopped() const;

	BOOL Reinit();

	inline BOOL GetRunInfo(CMyPlatRunCtrl::PlatRun_Info& runinfo) const
	{
		if( m_pPlatRunCtrl )
		{
			m_pPlatRunCtrl->GetRunInfo(runinfo);

			return TRUE;
		}

		return FALSE;	
	}

	inline PACKET_LEN_TYPE GetLastPacketLenType() const
	{
		if( m_pPlatRunCtrl )
		{
			return m_pPlatRunCtrl->GetLastPacketLenType();
		}

		return PACKET_NO_MORE;
	}

	CMyPlatRunCtrl * GetPlatRunCtrl() const
	{
		return m_pPlatRunCtrl;
	}

private:
	BOOL LoadObservers();
	void UnLoadObservers();

private:
	ACE_Log_Msg *			m_pLogInstance;
	CMyPlatRunCtrl *		m_pPlatRunCtrl;

	BOOL					m_bLoadNetScan;

	std::string				m_monitor_event_name;
	size_t					m_monitor_event_interval;

	OBSERVER_MGR 	*		m_pObserverMgr;
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_OBSERVER_MGR_H_2006_05_11