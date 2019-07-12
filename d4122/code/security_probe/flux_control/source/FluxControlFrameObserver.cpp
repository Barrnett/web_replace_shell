//////////////////////////////////////////////////////////////////////////
//FluxControlFrameObserver.cpp

#include "stdafx.h"
#include "FluxControlFrameObserver.h"
#include "cpf/path_tools.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "cpf/CommonEtherDecode.h"
#include "PacketIO/MyPlatRunCtrl.h"
#include "PacketIO/EtherioSendPacket.h"

#include "FluxControlObserver.h"
#include "CheckStopEventThread.h"
#include "config_center.h"
#include "cpf/IfIPTool.h"
#include "cpf/proto_struct_def.h"
#include "cpf/ConstructPacket.h"
#include "ConfigFacade.h"

CFluxControlFrameObserver::CFluxControlFrameObserver(void)
{
	m_pFluxControlObserver = NULL;

	m_pIPChangedNotify = NULL;

	m_stop_event_interval = 10;
	m_stop_event_name = "";

	m_pCheckStopEventThread = NULL;

	m_nHandlePkts = 0;
}

CFluxControlFrameObserver::~CFluxControlFrameObserver(void)
{
}

BOOL CFluxControlFrameObserver::init(const char * drv_name)
{
	m_str_drv_name = drv_name;

	CPF::CreateFullDirecory(CPF::GetModuleFullFileName("log"));

	char log_filename[MAX_PATH];

	CPF::GetModuleOtherFileName(log_filename,sizeof(log_filename),"log","flux_control.log");

	CPF::DeleteTimerLogFile(CPF::GetModuleFullFileName("log"),"flux_control_","log",30*24*3600);

	BOOL brtn = m_log_file.init_oneday_log(log_filename,false,true);

	{
		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("\n\n\n###############################################################\n")
			));
		char *pt_version = f_get_system_version(); // add by xumx, 2016-12-12
		if(pt_version != NULL)
		{
			MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("[Info][%D]software version: %s\n"),pt_version
			));
		}
		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("###############################################################\n")
			));

		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("[Info][%D]CFluxControlFrameObserver::init()\n")
			));

		MY_ACE_DEBUG(&m_log_file,
			(LM_ERROR,
			ACE_TEXT("[Info][%D]str_driver_name = %s\n"),
			drv_name
			));
	}

	return true;
}

int CFluxControlFrameObserver::OnAttachOKToPlatCtrl()
{
	MY_ACE_DEBUG(&m_log_file,
		(LM_ERROR,
		ACE_TEXT("[Info][%D]OnAttachOKToPlatCtrl.\n")
		));

	//
	m_pFluxControlObserver = new CFluxControlObserver;
	if (NULL == m_pFluxControlObserver)
	{
		return false;
	}

	g_pFluxControlObserver = m_pFluxControlObserver;

	GetPlayRunCtrl()->SignalMonitorEvent();

	if (m_pFluxControlObserver->init(&m_log_file, m_str_drv_name.c_str()) == false) 
	{
		return(false);
	}

#if(1 == FUNC_IP_CHANGE_NODITY)
	//
	m_pIPChangedNotify = new CIPChangedNotify;
	
	m_pIPChangedNotify->Start(this);
#endif

	return true;
}

void CFluxControlFrameObserver::fin()
{
	if( m_pIPChangedNotify )
	{
		m_pIPChangedNotify->Stop();

		delete m_pIPChangedNotify;

		m_pIPChangedNotify = NULL;
	}

	if( m_pFluxControlObserver )
	{
		m_pFluxControlObserver->fin();
		delete m_pFluxControlObserver;
		m_pFluxControlObserver = NULL;
	}

	return;
}

void CFluxControlFrameObserver::SetStopEventInfo(const char * stop_event_name,int stop_event_interval)
{
	m_stop_event_name = stop_event_name;
	m_stop_event_interval = stop_event_interval;

	return;
}


BOOL CFluxControlFrameObserver::Reset()
{
	return true;

}
BOOL CFluxControlFrameObserver::OnStart()
{
	m_nHandlePkts = 0;

	if( m_pFluxControlObserver )
	{
		m_pFluxControlObserver->OnStart();
	}

	m_pCheckStopEventThread = new CCheckStopEventThread;

	m_pCheckStopEventThread->init(m_stop_event_name.c_str());

	m_pCheckStopEventThread->start();
	MY_ACE_DEBUG(&m_log_file,
		(LM_INFO,
		ACE_TEXT("[Info][%D]create stop_event '%s' success.\n"), m_stop_event_name.c_str()
		));

	printf("create stop_event '%s' success.\n\n", m_stop_event_name.c_str());

	return true;
}

BOOL CFluxControlFrameObserver::OnStop(int type)
{
	if( m_pIPChangedNotify )
	{
		m_pIPChangedNotify->Stop();
	}

	if( m_pCheckStopEventThread )
	{
		m_pCheckStopEventThread->stop();
		delete m_pCheckStopEventThread;
		m_pCheckStopEventThread = NULL;
	}

	if( m_pFluxControlObserver )
	{
		m_pFluxControlObserver->OnStop(type);
	}

	return true;

}

BOOL CFluxControlFrameObserver::OnPause()
{
	return true;
}

BOOL CFluxControlFrameObserver::OnContinue()
{
	return true;
}

BOOL CFluxControlFrameObserver::Push(PACKET_LEN_TYPE type,RECV_PACKET& packet)
{
	CChannleCtrlMgr_LOCKER_ATOM lock_;

	/*if( PACKET_OK == type )
	{
		//HandlePacket(type,packet);
		m_pFluxControlObserver->PushPacket(type, packet);
	}
	else if( PACKET_TIMER == type )
	{
		//HandleTimer(type,packet);
		m_pFluxControlObserver->PushPacket(type, packet);
	}*/
	m_pFluxControlObserver->PushPacket(type, packet);

	return true;
}


void CFluxControlFrameObserver::HandleTimer(PACKET_LEN_TYPE type,RECV_PACKET& packet)
{
	////if( m_pFluxControlObserver )
	{
		//m_pFluxControlObserver->HandleTimer(type,packet);
		m_pFluxControlObserver->PushPacket(type, packet); // change by xumx 2016-9-13
		
	}

	////g_pCConfigFacade->CheckIPFragmentTimetout(packet.pHeaderinfo->stamp.sec);
	

	return;
}

void CFluxControlFrameObserver::HandlePacket(PACKET_LEN_TYPE type,RECV_PACKET& packet)
{
	/*IsMirroring() and IsCapping() equal to 0, deleted by zhongjh
	int input_card_index = packet.pHeaderinfo->btCardNo;

	if (m_pFluxControlObserver->m_mirrorout_mgr.IsMirroring())
	{
		if (m_pFluxControlObserver->m_mirrorout_mgr.IsMirrorFromCardIndex(input_card_index))
		{
			m_pFluxControlObserver->m_mirrorout_mgr.write_packet(type, packet, -1);
		}
	}

	if (m_pFluxControlObserver->m_cap_data_mgr_ex.IsCapping())
	{
		if (m_pFluxControlObserver->m_cap_data_mgr_ex.IsCapFromCardIndex(input_card_index))
		{
			m_pFluxControlObserver->m_cap_data_mgr_ex.write_packet(type, packet, -1);
		}
	}*/

	m_pFluxControlObserver->PushPacket(type, packet);

	return;
}



BOOL CFluxControlFrameObserver::IsGetStopEvent()
{
	if( m_pCheckStopEventThread )
	{
		return m_pCheckStopEventThread->IsGetStopEvent();
	}

	return false;
}

int CFluxControlFrameObserver::OnIPChanged()
{
	CChannleCtrlMgr_LOCKER_ATOM lock_;

	if( m_pFluxControlObserver )
	{
		m_pFluxControlObserver->OnIPChanged();
	}

	return 0;
}

// add by xumx, 2016-12-12
char *CFluxControlFrameObserver::f_get_system_version()
{
	CTinyXmlEx xml_doc;

	// 打开文件
	BOOL result = xml_doc.open_for_readonly("/home/ts/version/version.xml");
	if(result < 0){ return(NULL); }

	// XML文件的根节点
	TiXmlNode *xml_root_node = xml_doc.GetRootNode();

	// 版本信息节点
	TiXmlNode *version_root = xml_doc.FirstChildElement(xml_root_node, "SERVICE");
	if(version_root == NULL)
	{
		xml_doc.close();
		return(NULL);
	}

	std::string version;
	xml_doc.GetValueLikeIni(version_root, "VERSION", version);
	if((version.length() == 0) || (version.length() > 3)){ xml_doc.close(); return(NULL); }

	std::string release;
	xml_doc.GetValueLikeIni(version_root, "RELEASE", release);
	if((release.length() == 0) || (release.length() > 3)){ xml_doc.close(); return(NULL); }
	
	std::string build;
	xml_doc.GetValueLikeIni(version_root, "BUILD", build);
	if((build.length() == 0) || (build.length() > 3)){ xml_doc.close(); return(NULL); }

	memset(m_version_string, 0, 64);
	sprintf(m_version_string, "%s.%s.%s", version.c_str(), release.c_str(), build.c_str());

	xml_doc.close();

	return(m_version_string);
}
//////////////////////////

