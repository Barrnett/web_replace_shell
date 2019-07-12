#include "StdAfx.h"
#include "TestControlParam.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/Common_Func_Macor.h"

CTestControlParam::CTestControlParam(void)
{
}

void CTestControlParam::close()
{
	m_bypass_time_out = 8;

	m_bResetWDTInLoop = 0;
	m_bResetWDTInUI = 1;

	m_bSyncOnAddStat = true;
	m_bSyncOnDelStat = true;

	m_live_log_size = 0;
	m_live_log_level = -1;

	m_b_automap = 1;
	m_set_cfg_in_protect_mode = 0;
	m_bCopyDrvData = 0;
	m_buffer_pkt_thread = 0;

	m_bTraceCode = 0;
	m_bCreateDump = FALSE;
	m_log_flux = 0;

	m_update_stat_interval = 300;

	m_file_name[0] = 0;

	m_uk_server_stat_nums = 500;
	m_uk_port_stat_nums = 500;

	m_peer_server_stat_nums = 500;
	m_peer_port_stat_nums = 500;

	m_webvideo_time = 10;
	m_webvideo_speed = 500;

	m_default_tcp_timeout = 1800;
	m_default_udp_timeout = 600;

	m_tcp_syn_timeout = 30;
	m_udp_syn_timeout = 30;

	m_empty_sleep_time = 5;

	m_flux_control_handle_data = 1;

	m_nProtoDepth = 0;
	m_bDFI = 1;

	m_live_app_stat = 1;

	m_http_line_ingore = 0;
	m_http_data_ingore = 0;
	m_http_header_find_optimize = 0;
	m_http_line_find_optimize = 0;

	m_bypass_udp_port_nums = 0;
	m_bypass_tcp_port_nums = 0;

	m_cryp2p_param = 150;

	m_vt_bypass_tcp_port.clear();
	m_vt_bypass_udp_port.clear();

	m_data_to_audit_type = 1;
	m_data_to_audit_addr_port_cond.close();

	return;
}


CTestControlParam::~CTestControlParam(void)
{

}

void CTestControlParam::add_defalut_bypaas_port()
{
	//添加缺省的bypass端口
	m_vt_bypass_tcp_port.push_back(std::make_pair(179,"BPG"));

	m_vt_bypass_udp_port.push_back(std::make_pair(53,"DNS"));
	m_vt_bypass_udp_port.push_back(std::make_pair(123,"NTP"));
	m_vt_bypass_udp_port.push_back(std::make_pair(67,"DHCP1"));
	m_vt_bypass_udp_port.push_back(std::make_pair(68,"DHCP2"));
	m_vt_bypass_udp_port.push_back(std::make_pair(520,"RIP"));
	m_vt_bypass_udp_port.push_back(std::make_pair(179,"BPG"));

	return;
}

void CTestControlParam::init_default(BOOL bXiaocao)
{	
	close();

	if( bXiaocao )
	{
		m_bCreateDump = true;
	}

	add_defalut_bypaas_port();

	return;
}


int CTestControlParam::operator == (const CTestControlParam& other) const
{
	return (m_bypass_time_out == other.m_bypass_time_out&&

	m_bResetWDTInLoop == other.m_bResetWDTInLoop&&
	m_bResetWDTInUI == other.m_bResetWDTInUI&&

	m_bSyncOnAddStat == other.m_bSyncOnAddStat&&
	m_bSyncOnDelStat == other.m_bSyncOnDelStat&&

	m_live_log_size == other.m_live_log_size&&
	m_live_log_level == other.m_live_log_level&&

	m_bTraceCode == other.m_bTraceCode&&
	m_bCreateDump == other.m_bCreateDump&&
	m_log_flux == other.m_log_flux&&

	m_update_stat_interval == other.m_update_stat_interval && 
	
	m_uk_server_stat_nums == other.m_uk_server_stat_nums&&
	m_uk_port_stat_nums == other.m_uk_port_stat_nums&&

	m_webvideo_time == other.m_webvideo_time&&
	m_webvideo_speed == other.m_webvideo_speed&&

	m_peer_server_stat_nums == other.m_peer_server_stat_nums&&
	m_peer_port_stat_nums == other.m_peer_port_stat_nums&&

	m_vt_bypass_tcp_port == other.m_vt_bypass_tcp_port&&
	m_vt_bypass_udp_port == other.m_vt_bypass_udp_port&&

	m_b_automap == other.m_b_automap &&
	m_set_cfg_in_protect_mode == other.m_set_cfg_in_protect_mode &&
	m_bCopyDrvData == other.m_bCopyDrvData &&
	m_buffer_pkt_thread == other.m_buffer_pkt_thread &&

	m_vt_tcp_timeout_param == other.m_vt_tcp_timeout_param &&
	m_vt_udp_timeout_param == other.m_vt_udp_timeout_param &&
	m_default_tcp_timeout == other.m_default_tcp_timeout &&
	m_default_udp_timeout == other.m_default_udp_timeout &&

	m_tcp_syn_timeout == other.m_tcp_syn_timeout &&
	m_udp_syn_timeout == other.m_udp_syn_timeout &&

	m_empty_sleep_time == other.m_empty_sleep_time &&

	m_flux_control_handle_data == other.m_flux_control_handle_data &&
	m_nProtoDepth == other.m_nProtoDepth &&
	m_bDFI == other.m_bDFI &&
	m_live_app_stat == other.m_live_app_stat&&

	m_http_line_ingore == other.m_http_line_ingore&&
	m_http_data_ingore == other.m_http_data_ingore &&
	m_http_header_find_optimize == other.m_http_header_find_optimize&&
	m_http_line_find_optimize == other.m_http_line_find_optimize&&
	m_cryp2p_param == other.m_cryp2p_param&&

	m_data_to_audit_type == other.m_data_to_audit_type&&
	m_data_to_audit_addr_port_cond == other.m_data_to_audit_addr_port_cond
	);

}

CTestControlParam& CTestControlParam::operator=(const CTestControlParam& other)
{
	if( this != &other )
	{
		m_bypass_time_out = other.m_bypass_time_out;

		m_bResetWDTInLoop = other.m_bResetWDTInLoop;
		m_bResetWDTInUI = other.m_bResetWDTInUI;

		m_bSyncOnAddStat = other.m_bSyncOnAddStat;
		m_bSyncOnDelStat = other.m_bSyncOnDelStat;

		m_live_log_size = other.m_live_log_size;
		m_live_log_level = other.m_live_log_level;

		m_bTraceCode = other.m_bTraceCode;
		m_bCreateDump = other.m_bCreateDump;
		m_log_flux = other.m_log_flux;

		m_update_stat_interval = other.m_update_stat_interval;

		m_uk_server_stat_nums = other.m_uk_server_stat_nums;
		m_uk_port_stat_nums = other.m_uk_port_stat_nums;
		
		m_webvideo_time = other.m_webvideo_time;
		m_webvideo_speed = other.m_webvideo_speed;

		m_peer_server_stat_nums = other.m_peer_server_stat_nums;
		m_peer_port_stat_nums = other.m_peer_port_stat_nums;

		m_vt_bypass_tcp_port = other.m_vt_bypass_tcp_port;
		m_vt_bypass_udp_port = other.m_vt_bypass_udp_port;

		m_b_automap = other.m_b_automap;
		m_set_cfg_in_protect_mode = other.m_set_cfg_in_protect_mode;
		m_bCopyDrvData = other.m_bCopyDrvData;
		m_buffer_pkt_thread = other.m_buffer_pkt_thread;

		m_vt_tcp_timeout_param = other.m_vt_tcp_timeout_param;
		m_vt_udp_timeout_param = other.m_vt_udp_timeout_param;

		m_default_tcp_timeout = other.m_default_tcp_timeout;
		m_default_udp_timeout = other.m_default_udp_timeout;
		m_tcp_syn_timeout = other.m_tcp_syn_timeout;
		m_udp_syn_timeout = other.m_udp_syn_timeout;

		m_empty_sleep_time = other.m_empty_sleep_time;

		m_flux_control_handle_data = other.m_flux_control_handle_data;

		m_nProtoDepth = other.m_nProtoDepth;
		m_bDFI = other.m_bDFI;

		m_live_app_stat = other.m_live_app_stat;

		m_http_line_ingore = other.m_http_line_ingore;
		m_http_data_ingore = other.m_http_data_ingore;
		m_http_header_find_optimize = other.m_http_header_find_optimize;
		m_http_line_find_optimize = other.m_http_line_find_optimize;

		m_cryp2p_param = other.m_cryp2p_param;

		m_data_to_audit_type = other.m_data_to_audit_type;
		m_data_to_audit_addr_port_cond = other.m_data_to_audit_addr_port_cond;

		Format_bypass_udp_port();
		Format_bypass_tcp_port();
	}

	return *this;
}

BOOL CTestControlParam::IsBigChanged(const CTestControlParam& other) const
{
	BOOL bEqualImportParam = (

		m_bypass_time_out == other.m_bypass_time_out&&

		m_bResetWDTInLoop == other.m_bResetWDTInLoop&&
		m_bResetWDTInUI == other.m_bResetWDTInUI&&

		m_bSyncOnAddStat == other.m_bSyncOnAddStat&&
		m_bSyncOnDelStat == other.m_bSyncOnDelStat&&

		m_live_log_size == other.m_live_log_size&&
		m_live_log_level == other.m_live_log_level&&

		m_bTraceCode == other.m_bTraceCode&&
		m_bCreateDump == other.m_bCreateDump&&
		m_log_flux == other.m_log_flux&&

		m_update_stat_interval == other.m_update_stat_interval && 

		m_uk_server_stat_nums == other.m_uk_server_stat_nums&&
		m_uk_port_stat_nums == other.m_uk_port_stat_nums&&

		m_peer_server_stat_nums == other.m_peer_server_stat_nums&&
		m_peer_port_stat_nums == other.m_peer_port_stat_nums&&

		m_b_automap == other.m_b_automap &&
		m_set_cfg_in_protect_mode == other.m_set_cfg_in_protect_mode &&
		m_bCopyDrvData == other.m_bCopyDrvData &&
		m_buffer_pkt_thread == other.m_buffer_pkt_thread &&

		m_empty_sleep_time == other.m_empty_sleep_time 

		);

	return (!bEqualImportParam);
}

BOOL CTestControlParam::Write()
{
	if( m_file_name[0] )
	{
		return Write(m_file_name);
	}

	ACE_ASSERT(FALSE);

	return true;
}

BOOL CTestControlParam::Write(const char * xml_file_name)
{
	CTinyXmlEx xml_writter;

	if( 0 != xml_writter.open_ex(xml_file_name,CTinyXmlEx::OPEN_MODE_CREATE_TRUNC) )
	{
		return false;
	}

	if( !xml_writter.InsertRootElement("test_control") )
	{
		return false;
	}

	{
		TiXmlNode * pcommon_node = 
			xml_writter.InsertEndChildElement(xml_writter.GetRootNode(),"common");

		if( !pcommon_node )
		{
			return false;
		}

		xml_writter.SetValueLikeIni(pcommon_node,"bypass_time_out",m_bypass_time_out);

		xml_writter.SetValueLikeIni(pcommon_node,"reset_wdt_in_ui",m_bResetWDTInUI);
		xml_writter.SetValueLikeIni(pcommon_node,"reset_wdt_in_loop",m_bResetWDTInLoop);

		xml_writter.SetValueLikeIni(pcommon_node,"live_log_size",m_live_log_size);

		xml_writter.SetValueLikeIni(pcommon_node,"live_log_level",(int)m_live_log_level);

		xml_writter.SetValueLikeIni(pcommon_node,"live_stat_sync_on_add_stat",m_bSyncOnAddStat);
		xml_writter.SetValueLikeIni(pcommon_node,"live_stat_sync_on_del_stat",m_bSyncOnDelStat);

		xml_writter.SetValueLikeIni(pcommon_node,"log_flux",m_log_flux);
		xml_writter.SetValueLikeIni(pcommon_node,"trace_code",m_bTraceCode);
		xml_writter.SetValueLikeIni(pcommon_node,"create_dump",m_bCreateDump);

		xml_writter.SetValueLikeIni(pcommon_node,"update_stat_interval",m_update_stat_interval);

		xml_writter.SetValueLikeIni(pcommon_node,"automap",m_b_automap);
		xml_writter.SetValueLikeIni(pcommon_node,"set_cfg_in_protect_mode",m_set_cfg_in_protect_mode);
		xml_writter.SetValueLikeIni(pcommon_node,"copy_drv_data",m_bCopyDrvData);
		xml_writter.SetValueLikeIni(pcommon_node,"buffer_pkt_thread",m_buffer_pkt_thread);

		WriteLinkTimeOutParam(xml_writter,pcommon_node);

		xml_writter.SetValueLikeIni(pcommon_node,"empty_sleep_time",m_empty_sleep_time);

		xml_writter.SetValueLikeIni(pcommon_node,"flux_control_handle_data",m_flux_control_handle_data);
		
		xml_writter.SetValueLikeIni(pcommon_node,"proto_depth",m_nProtoDepth);
		xml_writter.SetValueLikeIni(pcommon_node,"proto_dfi",m_bDFI);		

		xml_writter.SetValueLikeIni(pcommon_node,"live_app_stat",m_live_app_stat);
		
		xml_writter.SetValueLikeIni(pcommon_node,"http_line_ingore",m_http_line_ingore);
		xml_writter.SetValueLikeIni(pcommon_node,"http_data_ingore",m_http_data_ingore);
		
		xml_writter.SetValueLikeIni(pcommon_node,"http_header_find_optimize",m_http_header_find_optimize);
		xml_writter.SetValueLikeIni(pcommon_node,"http_line_find_optimize",m_http_line_find_optimize);
		
		xml_writter.SetValueLikeIni(pcommon_node,"cryp2p_param",m_cryp2p_param);

	}

	{
		const ACE_TCHAR section[] = ACE_TEXT("uk_stat");

		TiXmlNode * uk_stat_node = 
			xml_writter.InsertEndChildElement(xml_writter.GetRootNode(),section);

		if( uk_stat_node )
		{
			xml_writter.SetValueLikeIni(uk_stat_node,"server_stat_nums",m_uk_server_stat_nums);
			xml_writter.SetValueLikeIni(uk_stat_node,"port_stat_nums",m_uk_port_stat_nums);
		}
	}

	{
		const ACE_TCHAR section[] = ACE_TEXT("web_video");

		TiXmlNode * web_video_node = 
			xml_writter.InsertEndChildElement(xml_writter.GetRootNode(),section);

		if( web_video_node )
		{
			xml_writter.SetValueLikeIni(web_video_node,"webvideo_time",m_webvideo_time);
			xml_writter.SetValueLikeIni(web_video_node,"webvideo_speed",m_webvideo_speed);
		}
	}

	{
		const ACE_TCHAR section[] = ACE_TEXT("peer_stat");

		TiXmlNode * peer_stat_node = 
			xml_writter.InsertEndChildElement(xml_writter.GetRootNode(),section);

		if( peer_stat_node )
		{
			xml_writter.SetValueLikeIni(peer_stat_node,"server_stat_nums",m_peer_server_stat_nums);
			xml_writter.SetValueLikeIni(peer_stat_node,"port_stat_nums",m_peer_port_stat_nums);
		}
	}

	WriteByPassPort(xml_writter,xml_writter.GetRootNode());

	{
		TiXmlNode * data_to_audit_node = 
			xml_writter.InsertEndChildElement(xml_writter.GetRootNode(),"data_to_audit");

		if( data_to_audit_node )
		{
			xml_writter.SetAttr(data_to_audit_node,"data_to_audit_type",m_data_to_audit_type);

			m_data_to_audit_addr_port_cond.Save(xml_writter,data_to_audit_node);
		}
	}


	xml_writter.savefile();

	xml_writter.close();

	return true;
}

BOOL CTestControlParam::Read(const char * xml_file_name)
{
	close();

	CTinyXmlEx xml_reader;

	if( 0 != xml_reader.open_for_readonly(xml_file_name) )
	{
		//添加缺省的bypass端口
		add_defalut_bypaas_port();

		return false;
	}

	{
		TiXmlNode * pcommon_node = xml_reader.GetRootNode()->FirstChildElement("common");

		if( !pcommon_node )
		{
			return false;
		}

		strcpy(m_file_name,xml_file_name);

		xml_reader.GetValueLikeIni(pcommon_node,"bypass_time_out",m_bypass_time_out);

		xml_reader.GetValueLikeIni(pcommon_node,"reset_wdt_in_ui",m_bResetWDTInUI);
		xml_reader.GetValueLikeIni(pcommon_node,"reset_wdt_in_loop",m_bResetWDTInLoop);

		xml_reader.GetValueLikeIni(pcommon_node,"live_log_size",m_live_log_size);

		int live_log_level = -1;
		xml_reader.GetValueLikeIni(pcommon_node,ACE_TEXT("live_log_level"),live_log_level);
		m_live_log_level = (unsigned int)live_log_level;

		xml_reader.GetValueLikeIni(pcommon_node,"live_stat_sync_on_add_stat",m_bSyncOnAddStat);
		xml_reader.GetValueLikeIni(pcommon_node,"live_stat_sync_on_del_stat",m_bSyncOnDelStat);

		xml_reader.GetValueLikeIni(pcommon_node,"log_flux",m_log_flux);
		xml_reader.GetValueLikeIni(pcommon_node,"trace_code",m_bTraceCode);
		xml_reader.GetValueLikeIni(pcommon_node,"create_dump",m_bCreateDump);

		xml_reader.GetValueLikeIni(pcommon_node,"update_stat_interval",m_update_stat_interval);

		xml_reader.GetValueLikeIni(pcommon_node,"automap",m_b_automap);	
		xml_reader.GetValueLikeIni(pcommon_node,"set_cfg_in_protect_mode",m_set_cfg_in_protect_mode);
		xml_reader.GetValueLikeIni(pcommon_node,"copy_drv_data",m_bCopyDrvData);
		xml_reader.GetValueLikeIni(pcommon_node,"buffer_pkt_thread",m_buffer_pkt_thread);

		ReadLinkTimeOutParam(xml_reader,pcommon_node);

		xml_reader.GetValueLikeIni(pcommon_node,"empty_sleep_time",m_empty_sleep_time);

		xml_reader.GetValueLikeIni(pcommon_node,"flux_control_handle_data",m_flux_control_handle_data);

		xml_reader.GetValueLikeIni(pcommon_node,"proto_depth",m_nProtoDepth);
		xml_reader.GetValueLikeIni(pcommon_node,"proto_dfi",m_bDFI);

		xml_reader.GetValueLikeIni(pcommon_node,"live_app_stat",m_live_app_stat);

		xml_reader.GetValueLikeIni(pcommon_node,"http_line_ingore",m_http_line_ingore);
		xml_reader.GetValueLikeIni(pcommon_node,"http_data_ingore",m_http_data_ingore);

		xml_reader.GetValueLikeIni(pcommon_node,"http_header_find_optimize",m_http_header_find_optimize);
		xml_reader.GetValueLikeIni(pcommon_node,"http_line_find_optimize",m_http_line_find_optimize);

		xml_reader.GetValueLikeIni(pcommon_node,"cryp2p_param",m_cryp2p_param);
	}

	enum{MAX_PEER_STAT=1000*10,MAX_UK_STAT=1000*10};

	{
		const ACE_TCHAR section[] = ACE_TEXT("uk_stat");

		TiXmlNode * uk_stat_node = xml_reader.GetRootNode()->FirstChildElement(section);

		if( uk_stat_node )
		{
			xml_reader.GetValueLikeIni(uk_stat_node,"server_stat_nums",m_uk_server_stat_nums);
			xml_reader.GetValueLikeIni(uk_stat_node,"port_stat_nums",m_uk_port_stat_nums);
		}

		if( m_uk_server_stat_nums > MAX_UK_STAT )
			m_uk_server_stat_nums = MAX_UK_STAT;

		if( m_uk_port_stat_nums > MAX_UK_STAT )
			m_uk_port_stat_nums = MAX_UK_STAT;
	}

	{
		const ACE_TCHAR section[] = ACE_TEXT("web_video");

		TiXmlNode * web_video_node = xml_reader.GetRootNode()->FirstChildElement(section);

		if( web_video_node )
		{
			xml_reader.GetValueLikeIni(web_video_node,"webvideo_time",m_webvideo_time);
			xml_reader.GetValueLikeIni(web_video_node,"webvideo_speed",m_webvideo_speed);
		}
	}

	{
		const ACE_TCHAR section[] = ACE_TEXT("peer_stat");

		TiXmlNode * peer_stat_node = xml_reader.GetRootNode()->FirstChildElement(section);

		if( peer_stat_node )
		{
			xml_reader.GetValueLikeIni(peer_stat_node,"server_stat_nums",m_peer_server_stat_nums);
			xml_reader.GetValueLikeIni(peer_stat_node,"port_stat_nums",m_peer_port_stat_nums);
		}

		if( m_peer_server_stat_nums > MAX_PEER_STAT )
			m_peer_server_stat_nums = MAX_PEER_STAT;

		if( m_peer_port_stat_nums > MAX_PEER_STAT )
			m_peer_port_stat_nums = MAX_PEER_STAT;
	}

	ReadByPassPort(xml_reader,xml_reader.GetRootNode());

	Format_bypass_udp_port();
	Format_bypass_tcp_port();

	{
		TiXmlNode * data_to_audit_node = 
			xml_reader.FirstChildElement(xml_reader.GetRootNode(),"data_to_audit");

		if( data_to_audit_node )
		{
			xml_reader.GetAttr(data_to_audit_node,"data_to_audit_type",m_data_to_audit_type);

			m_data_to_audit_addr_port_cond.Load(xml_reader,data_to_audit_node);
		}
	}

	xml_reader.close();

	return true;
}

BOOL CTestControlParam::WriteByPassPort(CTinyXmlEx& xml_file,TiXmlNode * parent)
{
	TiXmlNode * bypass_port_node = xml_file.InsertEndChildElement(parent,"bypass_port");

	if( !bypass_port_node )
		return false;

	{
		TiXmlNode * tcp_node = xml_file.InsertEndChildElement(bypass_port_node,"tcp");

		for(size_t i = 0; i < m_vt_bypass_tcp_port.size(); ++i)
		{
			TiXmlNode * port_node = xml_file.InsertEndChildElement(tcp_node,"port");

			xml_file.SetAttr(port_node,"port",m_vt_bypass_tcp_port[i].first);
			xml_file.SetAttr(port_node,"desc",m_vt_bypass_tcp_port[i].second.c_str());
		}
	}

	{
		TiXmlNode * udp_node = xml_file.InsertEndChildElement(bypass_port_node,"udp");

		for(size_t i = 0; i < m_vt_bypass_udp_port.size(); ++i)
		{
			TiXmlNode * port_node = xml_file.InsertEndChildElement(udp_node,"port");

			xml_file.SetAttr(port_node,"port",m_vt_bypass_udp_port[i].first);
			xml_file.SetAttr(port_node,"desc",m_vt_bypass_udp_port[i].second.c_str());
		}
	}

	return true;
}

void CTestControlParam::ReadByPassPort(CTinyXmlEx& xml_file,TiXmlNode * parent)
{
	m_vt_bypass_tcp_port.clear();
	m_vt_bypass_udp_port.clear();

	TiXmlNode * bypass_port_node = 
		parent->FirstChildElement("bypass_port");

	if( !bypass_port_node )
	{
		//添加缺省的bypass端口
		
		add_defalut_bypaas_port();

		return;
	}

	{
		TiXmlNode * tcp_node = bypass_port_node->FirstChildElement("tcp");

		if( tcp_node )
		{
			for(TiXmlNode * sub_node = tcp_node->FirstChildElement();
				sub_node;
				sub_node = sub_node->NextSiblingElement())
			{
				unsigned int port = 0;
				std::string str_desc;

				xml_file.GetAttr(sub_node,"port",port);
				xml_file.GetAttr(sub_node,"desc",str_desc);

				if( port != 0 )
				{
					m_vt_bypass_tcp_port.push_back(std::make_pair(port,str_desc));
				}
			}
		}
	}

	{
		TiXmlNode * udp_node = bypass_port_node->FirstChildElement("udp");

		if( udp_node )
		{
			for(TiXmlNode * sub_node = udp_node->FirstChildElement();
				sub_node;
				sub_node = sub_node->NextSiblingElement())
			{
				unsigned int port = 0;
				std::string str_desc;

				xml_file.GetAttr(sub_node,"port",port);
				xml_file.GetAttr(sub_node,"desc",str_desc);

				if( port != 0 )
				{
					m_vt_bypass_udp_port.push_back(std::make_pair(port,str_desc));
				}
			}
		}
	}

	return;
}

BOOL CTestControlParam::IsByPassport(int type,unsigned int port1,unsigned int port2) const
{
	if( 0x01 == type )
	{
		return IsByPassport(m_array_bypass_tcp_port,m_bypass_tcp_port_nums,port1,port2);
	}
	else if( 0x02 == type )
	{
		return IsByPassport(m_array_bypass_udp_port,m_bypass_udp_port_nums,port1,port2);
	}

	ACE_ASSERT(FALSE);

	return false;
}

void CTestControlParam::SetBypassUdpPort(std::vector< std::pair<unsigned int,std::string> >& vt_bypass_udp_port)
{
	m_vt_bypass_udp_port = vt_bypass_udp_port;

	Format_bypass_udp_port();

	return;
}

void CTestControlParam::SetBypassTcpPort(std::vector< std::pair<unsigned int,std::string> >& vt_bypass_tcp_port)
{
	m_vt_bypass_tcp_port = vt_bypass_tcp_port;

	Format_bypass_tcp_port();

	return;
}

void CTestControlParam::Format_bypass_udp_port()
{
	m_bypass_udp_port_nums = 0;

	for(size_t i = 0; i < m_vt_bypass_udp_port.size();++i)
	{
		if( m_bypass_udp_port_nums >= GET_NUMS_OF_ARRAY(m_array_bypass_udp_port) )
		{
			return;
		}

		m_array_bypass_udp_port[m_bypass_udp_port_nums++] = m_vt_bypass_udp_port[i].first;
	}

	return;
}

void CTestControlParam::Format_bypass_tcp_port()
{
	m_bypass_tcp_port_nums = 0;

	for(size_t i = 0; i < m_vt_bypass_tcp_port.size();++i)
	{
		if( m_bypass_tcp_port_nums >= GET_NUMS_OF_ARRAY(m_array_bypass_tcp_port) )
		{
			return;
		}

		m_array_bypass_tcp_port[m_bypass_tcp_port_nums++] = m_vt_bypass_tcp_port[i].first;
	}

	return;
}

BOOL CTestControlParam::WriteLinkTimeOutParam(CTinyXmlEx& xml_file,TiXmlNode * parent)
{
	TiXmlNode * link_timeout_node = xml_file.InsertEndChildElement(parent,"link_timeout");

	if( !link_timeout_node )
		return false;

	{
		TiXmlNode * tcp_node = xml_file.InsertEndChildElement(link_timeout_node,"tcp");

		xml_file.SetValueLikeIni(tcp_node,"default_timeout",m_default_tcp_timeout);

		for(size_t i = 0; i < m_vt_tcp_timeout_param.size(); ++i)
		{
			TiXmlNode * param_node = xml_file.InsertEndChildElement(tcp_node,"param");

			if( param_node )
			{
				const CTestControlParam::LINK_TIME_PARAM& link_param = m_vt_tcp_timeout_param[i];

				xml_file.SetAttr_HostOrderIPRange(param_node,"ip_range",link_param.begin_ip,link_param.end_ip);
				xml_file.SetAttr_UintRange(param_node,"port_range",link_param.begin_port,link_param.end_port);
				xml_file.SetAttr(param_node,"timeout",link_param.timeout_sec);
			}
		}
	}

	{
		TiXmlNode * udp_node = xml_file.InsertEndChildElement(link_timeout_node,"udp");

		xml_file.SetValueLikeIni(udp_node,"default_timeout",m_default_udp_timeout);

		for(size_t i = 0; i < m_vt_udp_timeout_param.size(); ++i)
		{
			TiXmlNode * param_node = xml_file.InsertEndChildElement(udp_node,"param");

			if( param_node )
			{
				const CTestControlParam::LINK_TIME_PARAM& link_param = m_vt_udp_timeout_param[i];

				xml_file.SetAttr_HostOrderIPRange(param_node,"ip_range",link_param.begin_ip,link_param.end_ip);
				xml_file.SetAttr_UintRange(param_node,"port_range",link_param.begin_port,link_param.end_port);
				xml_file.SetAttr(param_node,"timeout",link_param.timeout_sec);
			}
		}
	}

	return true;
}

void CTestControlParam::ReadLinkTimeOutParam(CTinyXmlEx& xml_file,TiXmlNode * parent)
{
	TiXmlNode * link_timeout_node = xml_file.FirstChildElement(parent,"link_timeout");

	if( !link_timeout_node )
		return;

	m_vt_tcp_timeout_param.clear();
	m_vt_udp_timeout_param.clear();

	{
		TiXmlNode * tcp_node = link_timeout_node->FirstChildElement("tcp");

		if( tcp_node )
		{
			xml_file.GetValueLikeIni(tcp_node,"default_timeout",m_default_tcp_timeout);

			for(TiXmlNode * sub_node = tcp_node->FirstChildElement("param");
				sub_node;
				sub_node = sub_node->NextSiblingElement("param"))
			{
				CTestControlParam::LINK_TIME_PARAM link_param;

				xml_file.GetAttr_HostOrderIPRange(sub_node,"ip_range",link_param.begin_ip,link_param.end_ip);
				xml_file.GetAttr_UintRange(sub_node,"port_range",link_param.begin_port,link_param.end_port);
				xml_file.GetAttr(sub_node,"timeout",link_param.timeout_sec);

				m_vt_tcp_timeout_param.push_back(link_param);
			}
		}
	}

	{
		TiXmlNode * udp_node = link_timeout_node->FirstChildElement("udp");

		if( udp_node )
		{
			xml_file.GetValueLikeIni(udp_node,"default_timeout",m_default_udp_timeout);

			for(TiXmlNode * sub_node = udp_node->FirstChildElement("param");
				sub_node;
				sub_node = sub_node->NextSiblingElement("param"))
			{
				CTestControlParam::LINK_TIME_PARAM link_param;

				xml_file.GetAttr_HostOrderIPRange(sub_node,"ip_range",link_param.begin_ip,link_param.end_ip);
				xml_file.GetAttr_UintRange(sub_node,"port_range",link_param.begin_port,link_param.end_port);
				xml_file.GetAttr(sub_node,"timeout",link_param.timeout_sec);

				m_vt_udp_timeout_param.push_back(link_param);
			}
		}
	}
}

ACE_UINT32 CTestControlParam::GetLinkTimeout_Tcp(const IPv4_CONNECT_ADDR& addr) const
{
	for(size_t i = 0; i < m_vt_tcp_timeout_param.size(); ++i)
	{
		const CTestControlParam::LINK_TIME_PARAM& link_timeout_param = m_vt_tcp_timeout_param[i];

		if( IsInLinkTimeoutParam(link_timeout_param,addr) )
		{
			return link_timeout_param.timeout_sec;
		}
	}

	return 0;
}

ACE_UINT32 CTestControlParam::GetLinkTimeout_Udp(const IPv4_CONNECT_ADDR& addr) const
{
	for(size_t i = 0; i < m_vt_udp_timeout_param.size(); ++i)
	{
		const CTestControlParam::LINK_TIME_PARAM& link_timeout_param = m_vt_udp_timeout_param[i];

		if( IsInLinkTimeoutParam(link_timeout_param,addr) )
		{
			return link_timeout_param.timeout_sec;
		}
	}

	return 0;
}

ACE_UINT32 CTestControlParam::GetLinkTimeout_Tcp(const IPv4_TRANSPORT_ADDR& addr) const
{
	for(size_t i = 0; i < m_vt_tcp_timeout_param.size(); ++i)
	{
		const CTestControlParam::LINK_TIME_PARAM& link_timeout_param = m_vt_tcp_timeout_param[i];

		if( IsInLinkTimeoutParam(link_timeout_param,addr) )
		{
			return link_timeout_param.timeout_sec;
		}
	}

	return 0;
}

ACE_UINT32 CTestControlParam::GetLinkTimeout_Udp(const IPv4_TRANSPORT_ADDR& addr) const
{
	for(size_t i = 0; i < m_vt_udp_timeout_param.size(); ++i)
	{
		const CTestControlParam::LINK_TIME_PARAM& link_timeout_param = m_vt_udp_timeout_param[i];

		if( IsInLinkTimeoutParam(link_timeout_param,addr) )
		{
			return link_timeout_param.timeout_sec;
		}
	}

	return 0;
}
