//////////////////////////////////////////////////////////////////////////
//DataSourceParamReader.cpp

#include "DataSourceParamReader.h"
#include "algorithm"
#include "cpf/addr_tools.h"

C_Ini_DataSourceParamReader::C_Ini_DataSourceParamReader()
{

}

C_Ini_DataSourceParamReader::~C_Ini_DataSourceParamReader()
{
	Close();
}

BOOL C_Ini_DataSourceParamReader::Open(const char * filename)
{
	m_strIniFileName = filename;

	return true;
}


void C_Ini_DataSourceParamReader::Close()
{
	
}

BOOL C_Ini_DataSourceParamReader::Read(CDataSource_Param_Info& param_info)
{
	CIniFile inifile;

	if( !inifile.Open(m_strIniFileName.c_str()) )
		return false;

	{
		const ACE_TCHAR section[] = ACE_TEXT("driver_type");

		const ACE_TCHAR item_drv_type[] =  ACE_TEXT("recv_driver_type");

		int drvtype = -1;

		if( !inifile.GetValue(section,item_drv_type,drvtype) )
		{
			param_info.m_drvType = DRIVER_TYPE_ETHERIO;
		}
		else
		{
			param_info.m_drvType = DRIVER_TYPE(drvtype);
		}
	}

	{
		const ACE_TCHAR other_ctrl_section[] = ACE_TEXT("other_ctrl");

		const ACE_TCHAR item_thread_buffer_size[] =  ACE_TEXT("thread_buffer_size");
		inifile.GetValue(other_ctrl_section,item_thread_buffer_size,param_info.m_nThreadBufferSize);

		const ACE_TCHAR enable_timer_packet[] =  ACE_TEXT("enable_timer_packet");
		inifile.GetValue(other_ctrl_section,enable_timer_packet,param_info.m_bEnableTimerPacket);

		const ACE_TCHAR discard_rate_item[] =  ACE_TEXT("discard_rate");
		inifile.GetValue(other_ctrl_section,discard_rate_item,param_info.m_nDiscartRate);

		const ACE_TCHAR copy_drv_packet_item[] =  ACE_TEXT("copy_drv_packet");
		inifile.GetValue(other_ctrl_section,copy_drv_packet_item,param_info.m_bCopyPkt);
	}

	{
		const ACE_TCHAR speed_ctrl_section[] = ACE_TEXT("speed_ctrl");

		param_info.m_nbps = (ACE_UINT64)-1;
		param_info.m_unit_nums = 100;

		inifile.GetValue(speed_ctrl_section,"bps",param_info.m_nbps);
		inifile.GetValue(speed_ctrl_section,"uint",param_info.m_unit_nums);

		if( param_info.m_unit_nums == 0 )
		{
			param_info.m_unit_nums = 100;
		}
	}


#ifdef OS_WINDOWS
	ReadWinpcapInfo(param_info,inifile);
#endif//OS_WINDOWS

	ReadEtherioParamInfo(param_info,inifile);

	ReadEthreadPlaybackParamInfo(param_info,inifile);

	ReadCptPlaybackParamInfo(param_info,inifile);
	ReadMemEditDataParamInfo(param_info,inifile);
	ReadMemDatasourceParamInfo(param_info,inifile);

	CDataSource_Param_Info::ConstrouctPacket(param_info.m_memParamInfo,
		param_info.m_memEditDataInfo);

	ReadShareMemDatasouceParamInfo(param_info,inifile);

	inifile.Close();

	return true;
}
	
BOOL C_Ini_DataSourceParamReader::Write(const CDataSource_Param_Info& param_info,const char * filename)
{
	CIniFile inifile;

	if( !inifile.Open(m_strIniFileName.c_str()) )
		return false;


	{
		const ACE_TCHAR section[] = ACE_TEXT("driver_type");

		const ACE_TCHAR item_drv_type[] =  ACE_TEXT("recv_driver_type");
		inifile.SetValue(section,item_drv_type,(int)param_info.m_drvType);
	}

	{
		const ACE_TCHAR other_ctrl_section[] = ACE_TEXT("other_ctrl");

		const ACE_TCHAR item_thread_buffer_size[] =  ACE_TEXT("thread_buffer_size");
		inifile.SetValue(other_ctrl_section,item_thread_buffer_size,param_info.m_nThreadBufferSize);

		const ACE_TCHAR enable_timer_packet[] =  ACE_TEXT("enable_timer_packet");
		inifile.SetValue(other_ctrl_section,enable_timer_packet,param_info.m_bEnableTimerPacket);

		const ACE_TCHAR discard_rate_item[] =  ACE_TEXT("discard_rate");
		inifile.SetValue(other_ctrl_section,discard_rate_item,param_info.m_nDiscartRate);

		const ACE_TCHAR copy_drv_packet_item[] =  ACE_TEXT("copy_drv_packet");
		inifile.SetValue(other_ctrl_section,copy_drv_packet_item,param_info.m_bCopyPkt);

	}

	{
		const ACE_TCHAR speed_ctrl_section[] = ACE_TEXT("speed_ctrl");

		inifile.SetValue(speed_ctrl_section,"bps",param_info.m_nbps);
		inifile.SetValue(speed_ctrl_section,"uint",param_info.m_unit_nums);
	}

#ifdef OS_WINDOWS
	WriteWinpcapInfo(param_info,inifile);
#endif//OS_WINDOWS

	WriteEtherioParamInfo(param_info,inifile);

	if( param_info.GetDrvType() == DRIVER_TYPE_FILE_ETHEREAL )
	{
		WriteEthreadPlaybackParamInfo(param_info,inifile);
	}
	else if( param_info.GetDrvType() == DRIVER_TYPE_FILE_ZCCPT )
	{
		WriteCptPlaybackParamInfo(param_info,inifile);
	}
	
	WriteMemDatasourceParamInfo(param_info,inifile);
	WriteMemEditDataParamInfo(param_info,inifile);
	WriteShareMemDatasouceParamInfo(param_info,inifile);

	inifile.Close();

	return true;
}

void C_Ini_DataSourceParamReader::ReadMemDatasourceParamInfo(CDataSource_Param_Info& param_info,CIniFile& inifile)
{
	const char section[] = "recv_memory";

	inifile.GetValue(section,"minlen",param_info.m_memParamInfo.m_minlen);
	inifile.GetValue(section,"maxlen",param_info.m_memParamInfo.m_maxlen);
	inifile.GetValue(section,"proto",param_info.m_memParamInfo.m_wProto);
	inifile.GetValue(section,"memdata_type",param_info.m_memParamInfo.m_datatype);

	return;
}

void C_Ini_DataSourceParamReader::ReadMemEditDataParamInfo(CDataSource_Param_Info& param_info,CIniFile& inifile)
{
	const char section[] = "mem_edit_data";

	inifile.GetValue_Macaddr(section,"src_mac",param_info.m_memEditDataInfo.src_mac);
	inifile.GetValue_Macaddr(section,"dst_mac",param_info.m_memEditDataInfo.dst_mac);

	inifile.GetValue_HostOrderIP(section,"src_ip",param_info.m_memEditDataInfo.hostorder_src_ip);
	inifile.GetValue_HostOrderIP(section,"dst_ip",param_info.m_memEditDataInfo.hostorder_dst_ip);

	inifile.GetValue(section,"tos",param_info.m_memEditDataInfo.tos);
	inifile.GetValue(section,"ttl",param_info.m_memEditDataInfo.ttl);

	inifile.GetValue(section,"trans_type",param_info.m_memEditDataInfo.trans_type);

	inifile.GetValue(section,"src_port",param_info.m_memEditDataInfo.hostorder_src_port);
	inifile.GetValue(section,"dst_port",param_info.m_memEditDataInfo.hostorder_dst_port);

	inifile.GetValue(section,"vlan",param_info.m_memEditDataInfo.vlan_id);

	return;
}


void C_Ini_DataSourceParamReader::ReadShareMemDatasouceParamInfo(CDataSource_Param_Info& param_info,CIniFile& inifile)
{
	const char section[] = "recv_share_memory";

	inifile.GetValue(section,"share_name",param_info.m_ShareMemParamInfo.m_strShareName);
	inifile.GetValue(section,"share_size",param_info.m_ShareMemParamInfo.m_mem_size_mb);

	return;

}


void C_Ini_DataSourceParamReader::WriteMemDatasourceParamInfo(const CDataSource_Param_Info& param_info,CIniFile& inifile)
{
	const char section[] = "recv_memory";

	inifile.SetValue(section,"minlen",param_info.m_memParamInfo.m_minlen);
	inifile.SetValue(section,"maxlen",param_info.m_memParamInfo.m_maxlen);
	inifile.SetValue(section,"proto",(UINT)param_info.m_memParamInfo.m_wProto);
	inifile.SetValue(section,"memdata_type",param_info.m_memParamInfo.m_datatype);

	return;
}

void C_Ini_DataSourceParamReader::WriteMemEditDataParamInfo(const CDataSource_Param_Info& param_info,CIniFile& inifile)
{
	const char section[] = "mem_edit_data";

	inifile.SetValue(section,"src_mac",
		(LPACECTSTR)CPF::mac_addr_to_a(param_info.m_memEditDataInfo.src_mac,'-'));

	inifile.SetValue(section,"dst_mac",
		(LPACECTSTR)CPF::mac_addr_to_a(param_info.m_memEditDataInfo.dst_mac,'-'));

	inifile.SetValue(section,"src_ip",
		(LPACECTSTR)CPF::hip_to_string(param_info.m_memEditDataInfo.hostorder_src_ip));

	inifile.SetValue(section,"dst_ip",
		(LPACECTSTR)CPF::hip_to_string(param_info.m_memEditDataInfo.hostorder_dst_ip));

	inifile.SetValue(section,"tos",param_info.m_memEditDataInfo.tos);
	inifile.SetValue(section,"ttl",param_info.m_memEditDataInfo.ttl);

	inifile.SetValue(section,"trans_type",param_info.m_memEditDataInfo.trans_type);

	inifile.SetValue(section,"src_port",(int)param_info.m_memEditDataInfo.hostorder_src_port);
	inifile.SetValue(section,"dst_port",(int)param_info.m_memEditDataInfo.hostorder_dst_port);

	inifile.SetValue(section,"vlan",param_info.m_memEditDataInfo.vlan_id);

	return;
}


void C_Ini_DataSourceParamReader::WriteShareMemDatasouceParamInfo(const CDataSource_Param_Info& param_info,CIniFile& inifile)
{
	const char section[] = "recv_share_memory";

	inifile.SetValue(section,"share_name",param_info.m_ShareMemParamInfo.m_strShareName.c_str());
	inifile.SetValue(section,"share_size",param_info.m_ShareMemParamInfo.m_mem_size_mb);

	return;

}


void C_Ini_DataSourceParamReader::ReadCptPlaybackParamInfo(CDataSource_Param_Info & param_info,CIniFile& inifile)
{
	const char section[] = "cpt_playback";

	inifile.GetValue(section,"read_nums",param_info.m_cpt_playback_param.m_nReadNums);
	inifile.GetValue(section,"buffer_size",param_info.m_cpt_playback_param.m_nBufferSizeMB);

	ReadCommonPlaybackParamInfo(param_info.m_cpt_playback_param.m_playback_info,inifile);

	return;
}



void C_Ini_DataSourceParamReader::ReadEthreadPlaybackParamInfo(CDataSource_Param_Info & param_info,CIniFile& inifile)
{
	const char section[] = "ethreal_playback";

	ReadCommonPlaybackParamInfo(param_info.m_ethreal_playback_param.m_playback_info,inifile);

	return;
}

void C_Ini_DataSourceParamReader::ReadCommonPlaybackParamInfo(Playback_Param_Info& common_playback_param_info,CIniFile& inifile)
{
	common_playback_param_info.m_vtFileName.clear();

	const char section[] = "common_playback";

	inifile.GetGroupValue(section,"filename",common_playback_param_info.m_vtFileName);

	common_playback_param_info.m_nloopnums = 1;
	inifile.GetValue(section,ACE_TEXT("loopnums"),common_playback_param_info.m_nloopnums);

	common_playback_param_info.m_skip_error_file = 0;
	inifile.GetValue(section,ACE_TEXT("skip_error_file"),common_playback_param_info.m_skip_error_file);

	common_playback_param_info.m_re_calculate_time = 1;
	inifile.GetValue(section,ACE_TEXT("re_calculate_time"),common_playback_param_info.m_re_calculate_time);

	common_playback_param_info.m_nAdjustType = 0;
	inifile.GetValue(section,ACE_TEXT("adjust_time_type"),common_playback_param_info.m_nAdjustType);

	common_playback_param_info.m_strAdjustBeginTime.clear();
	inifile.GetValue(section,ACE_TEXT("adjust_begin_time"),common_playback_param_info.m_strAdjustBeginTime);

	return;	
}

void C_Ini_DataSourceParamReader::WriteCptPlaybackParamInfo(const CDataSource_Param_Info & param_info,CIniFile& inifile)
{
	const char section[] = "cpt_playback";

	inifile.SetValue(section,"read_nums",param_info.m_cpt_playback_param.m_nReadNums);
	inifile.SetValue(section,"buffer_size",param_info.m_cpt_playback_param.m_nBufferSizeMB);

	WriteCommonPlaybackParamInfo(param_info.m_cpt_playback_param.m_playback_info,inifile);

	return;
}

void C_Ini_DataSourceParamReader::WriteEthreadPlaybackParamInfo(const CDataSource_Param_Info & param_info,CIniFile& inifile)
{
	const char section[] = "ethreal_playback";

	WriteCommonPlaybackParamInfo(param_info.m_ethreal_playback_param.m_playback_info,inifile);

	return;
}


void C_Ini_DataSourceParamReader::WriteCommonPlaybackParamInfo(const Playback_Param_Info& common_playback_param_info,CIniFile& inifile)
{
	const char section[] = "common_playback";

	inifile.SetGroupValue(section,"filename",common_playback_param_info.m_vtFileName);

	inifile.SetValue(section,ACE_TEXT("loopnums"),(int)common_playback_param_info.m_nloopnums);
	inifile.SetValue(section,ACE_TEXT("skip_error_file"),common_playback_param_info.m_skip_error_file);

	inifile.SetValue(section,ACE_TEXT("re_calculate_time"),common_playback_param_info.m_re_calculate_time);

	inifile.SetValue(section,ACE_TEXT("adjust_time_type"),common_playback_param_info.m_nAdjustType);
	inifile.SetValue(section,ACE_TEXT("adjust_begin_time"),common_playback_param_info.m_strAdjustBeginTime);

	return;
}

#ifdef OS_WINDOWS
void C_Ini_DataSourceParamReader::ReadWinpcapInfo(CDataSource_Param_Info& param_info,CIniFile& ini)
{
	memset(&param_info.m_winpcap_drv_param,0x00,sizeof(param_info.m_winpcap_drv_param));

	param_info.m_winpcap_drv_param.driveBuffer = 1;
	param_info.m_winpcap_drv_param.readBuffer = 1;

	const ACE_TCHAR section[] = ACE_TEXT("recv_winpcap");

	ini.GetValue(section,ACE_TEXT("cardindex"),param_info.m_winpcap_drv_param.cardIndex);

	ini.GetValue(section,ACE_TEXT("filterMode"),param_info.m_winpcap_drv_param.filterMode);
	ini.GetValue(section,ACE_TEXT("driveBuffer"),param_info.m_winpcap_drv_param.driveBuffer);
	ini.GetValue(section,ACE_TEXT("readBuffer"),param_info.m_winpcap_drv_param.readBuffer);
	ini.GetValue(section,ACE_TEXT("minToCopy"),param_info.m_winpcap_drv_param.minToCopy);
	ini.GetValue(section,ACE_TEXT("readTimeout"),param_info.m_winpcap_drv_param.readTimeout);

	return;
}

void C_Ini_DataSourceParamReader::WriteWinpcapInfo(const CDataSource_Param_Info& param_info,CIniFile& ini)
{
	const ACE_TCHAR section[] = ACE_TEXT("recv_winpcap");

	ini.SetValue(section,ACE_TEXT("cardindex"),param_info.m_winpcap_drv_param.cardIndex);

	ini.SetValue(section,ACE_TEXT("filterMode"),param_info.m_winpcap_drv_param.filterMode);
	ini.SetValue(section,ACE_TEXT("driveBuffer"),param_info.m_winpcap_drv_param.driveBuffer);
	ini.SetValue(section,ACE_TEXT("readBuffer"),param_info.m_winpcap_drv_param.readBuffer);
	ini.SetValue(section,ACE_TEXT("minToCopy"),param_info.m_winpcap_drv_param.minToCopy);
	ini.SetValue(section,ACE_TEXT("readTimeout"),param_info.m_winpcap_drv_param.readTimeout);

	return;
}
#endif//OS_WINDOWS



void C_Ini_DataSourceParamReader::ReadEtherioParamInfo(CDataSource_Param_Info& param_info,CIniFile& ini)
{
	const char section[] = "recv_etherio";
	ReadEtherioParamInfo_helper(section, param_info.m_etherioParamInfo, ini);
}

void C_Ini_DataSourceParamReader::ReadEtherioParamInfo_helper(const char* section, Etherio_RecvParam_Info& etherioParamInfo,CIniFile& ini)
{

	ini.GetValue(section,"sysfile_name",etherioParamInfo.m_strSysFileName);
	ini.GetValue(section,"driver_name",etherioParamInfo.m_strDriverName);
	ini.GetValue(section,"card_mac",etherioParamInfo.m_strMacAddr);

	ini.GetValue(section,"drv_load_type",etherioParamInfo.m_drv_load_type);

	ini.GetValue(section,"enable_timer_packet",etherioParamInfo.m_bEtherioEnableTimerPacket);

	ini.GetValue(section,"mem_size",etherioParamInfo.m_nMemSize);
	if( etherioParamInfo.m_nMemSize <= 0 )
		etherioParamInfo.m_nMemSize = 20;

	ini.GetValue(section,"timestamp_type",etherioParamInfo.m_nTimeStampType);

	if( etherioParamInfo.m_nTimeStampType < 0 
		|| etherioParamInfo.m_nTimeStampType > 6 )
	{
		etherioParamInfo.m_nTimeStampType = 6;
	}

	ini.GetValue(section,"error_recover",etherioParamInfo.m_bErrorRecover);

	return;
}


void C_Ini_DataSourceParamReader::WriteEtherioParamInfo(const CDataSource_Param_Info& param_info,CIniFile& ini)
{
	const char section[] = "recv_etherio";
	WriteEtherioParamInfo_helper(section, param_info.m_etherioParamInfo, ini);
}

void C_Ini_DataSourceParamReader::WriteEtherioParamInfo_helper(const char* section, const Etherio_RecvParam_Info& etherioParamInfo,CIniFile& ini)
{
	ini.SetValue(section,"sysfile_name",etherioParamInfo.m_strSysFileName);
	ini.SetValue(section,"driver_name",etherioParamInfo.m_strDriverName);
	ini.SetValue(section,"card_mac",etherioParamInfo.m_strMacAddr);

	ini.SetValue(section,"drv_load_type",etherioParamInfo.m_drv_load_type);

	ini.SetValue(section,"enable_timer_packet",etherioParamInfo.m_bEtherioEnableTimerPacket);
	ini.SetValue(section,"mem_size",etherioParamInfo.m_nMemSize);
	ini.SetValue(section,"timestamp_type",etherioParamInfo.m_nTimeStampType);

	ini.SetValue(section,"error_recover",etherioParamInfo.m_bErrorRecover);

	return;
}


