//////////////////////////////////////////////////////////////////////////
//DataSendParamReader.cpp

#include "DataSendParamReader.h"
#include "cpf/path_tools.h"

C_Ini_DataSendParamReader::C_Ini_DataSendParamReader()
{

}

C_Ini_DataSendParamReader::~C_Ini_DataSendParamReader()
{
	Close();
}

BOOL C_Ini_DataSendParamReader::Open(const char * filename)
{
	m_strIniFileName = filename;

	return true;
}


void C_Ini_DataSendParamReader::Close()
{
}

BOOL C_Ini_DataSendParamReader::Read(CDataSend_Param_Info& param_info)
{
	CIniFile inifile;

	if( !inifile.Open(m_strIniFileName.c_str()) )
		return false;

	const ACE_TCHAR section[] = ACE_TEXT("driver_type");
	const ACE_TCHAR item[] =  ACE_TEXT("send_driver_type");

	int drvtype = -1;


	if( !inifile.GetValue(section,item,drvtype) )
	{
		param_info.m_drvType = DRIVER_TYPE_ETHERIO;
	}
	else
	{
		param_info.m_drvType = DRIVER_TYPE(drvtype);
	}

	ReadSendWinpcapInfo(param_info,inifile);
	ReadSendEtherioParamInfo(param_info,inifile);
	Read_ethereal_SendPlaybackInfo(param_info,inifile);
	Read_cpt_SendPlaybackInfo(param_info,inifile);
	Read_ShareMem_Send_ParamInfo(param_info,inifile);

	inifile.Close();

	return true;

}

BOOL C_Ini_DataSendParamReader::Write(const CDataSend_Param_Info& param_info,const char * filename)
{
	CIniFile inifile;

	if( !inifile.Open(m_strIniFileName.c_str()) )
		return false;

	const ACE_TCHAR section[] = ACE_TEXT("driver_type");
	const ACE_TCHAR item[] =  ACE_TEXT("send_driver_type");

	inifile.SetValue(section,item,(int)param_info.m_drvType);

	WriteSendWinpcapInfo(param_info,inifile);
	WriteSendEtherioParamInfo(param_info,inifile);
	Write_ethereal_SendPlaybackInfo(param_info,inifile);
	Write_cpt_SendPlaybackInfo(param_info,inifile);
	Write_ShareMem_Send_ParamInfo(param_info,inifile);

	inifile.Close();

	return true;
}



void C_Ini_DataSendParamReader::ReadSendWinpcapInfo(CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const ACE_TCHAR section[] = ACE_TEXT("send_winpcap");

	ini.GetValue(section,ACE_TEXT("cardindex"),param_info.m_winpcap_drv_param.cardIndex);

	return;
}

void C_Ini_DataSendParamReader::Read_ethereal_SendPlaybackInfo(CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const ACE_TCHAR section[] = ACE_TEXT("ethereal_send_playback");

	ini.GetValue(section,ACE_TEXT("savefilepath"),param_info.m_ethereal_write_param.strPath);

	if( param_info.m_ethereal_write_param.strPath.empty() )
	{
		param_info.m_ethereal_write_param.strPath = "capdata";
	}

	param_info.m_ethereal_write_param.strPath = CPF::TransToFullPathName(param_info.m_ethereal_write_param.strPath.c_str());
	
	//ini.GetValue(section,ACE_TEXT("savefilename"),param_info.m_ethereal_write_param.strFileName);

	if( param_info.m_ethereal_write_param.strFileName.empty() )
	{
		param_info.m_ethereal_write_param.strFileName = CPF::FormatTime(5,ACE_OS::gettimeofday().sec());

		param_info.m_ethereal_write_param.strFileName += ".cap";
	}		

	ini.GetValue(section,ACE_TEXT("filetype"),param_info.m_ethereal_write_param.filetype);

	ini.GetValue(section,ACE_TEXT("num_files"),param_info.m_ethereal_write_param.num_files);
	ini.GetValue(section,ACE_TEXT("one_filesize_mb"),param_info.m_ethereal_write_param.one_filesize_mb);
	ini.GetValue(section,ACE_TEXT("captime"),param_info.m_ethereal_write_param.captime);

	return;
}

void C_Ini_DataSendParamReader::Read_cpt_SendPlaybackInfo(CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const ACE_TCHAR section[] = ACE_TEXT("cpt_send_playback");

	ini.GetValue(section,ACE_TEXT("savefilepath"),param_info.m_cpt_write_param.strPath);

	if( param_info.m_cpt_write_param.strPath.empty() )
	{
		param_info.m_cpt_write_param.strPath = "capdata";
	}

	param_info.m_cpt_write_param.strPath = CPF::TransToFullPathName(param_info.m_cpt_write_param.strPath.c_str());

	//ini.GetValue(section,ACE_TEXT("savefilename"),param_info.m_cpt_write_param.strFileName);

	if( param_info.m_cpt_write_param.strFileName.empty() )
	{
		param_info.m_cpt_write_param.strFileName = CPF::FormatTime(5,ACE_OS::gettimeofday().sec());

		param_info.m_cpt_write_param.strFileName += ".cpt";
	}		

	ini.GetValue(section,ACE_TEXT("filetype"),param_info.m_cpt_write_param.filetype);

	ini.GetValue(section,ACE_TEXT("num_files"),param_info.m_cpt_write_param.num_files);
	ini.GetValue(section,ACE_TEXT("one_filesize_mb"),param_info.m_cpt_write_param.one_filesize_mb);
	ini.GetValue(section,ACE_TEXT("captime"),param_info.m_cpt_write_param.captime);
	ini.GetValue(section,ACE_TEXT("buffer_size"),param_info.m_cpt_write_param.buffer_size_mb);

	return;
}


void C_Ini_DataSendParamReader::ReadSendEtherioParamInfo(CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const char section[] = "send_etherio";
	ReadSendEtherioParamInfo_helper(section, param_info.m_etherioParamInfo, ini);
}

void C_Ini_DataSendParamReader::ReadSendEtherioParamInfo_helper(const ACE_TCHAR* section, Etherio_SendParam_Info& etherioParamInfo, CIniFile& ini)
{
	ini.GetValue(section,"sysfile_name",etherioParamInfo.m_strSysFileName);
	ini.GetValue(section,"driver_name",etherioParamInfo.m_strDriverName);
	ini.GetValue(section,"card_mac",etherioParamInfo.m_strMacAddr);
	ini.GetValue(section,"drv_load_type",etherioParamInfo.m_drv_load_type);

	ini.GetValue(section,"mem_size",etherioParamInfo.m_nMemSize);

	if( etherioParamInfo.m_nMemSize <= 0 )
		etherioParamInfo.m_nMemSize = 20;

	ini.GetValue(section,"send_type",etherioParamInfo.m_sendtype);

	if( etherioParamInfo.m_sendtype != 1
		&& etherioParamInfo.m_sendtype != 3 )
	{
		etherioParamInfo.m_sendtype = 1;
	}

	return;

}

void C_Ini_DataSendParamReader::WriteSendWinpcapInfo(const CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const ACE_TCHAR section[] = ACE_TEXT("send_winpcap");

	ini.SetValue(section,ACE_TEXT("cardindex"),param_info.m_winpcap_drv_param.cardIndex);

	return;
}

void C_Ini_DataSendParamReader::WriteSendEtherioParamInfo(const CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const char section[] = "send_etherio";
	WriteSendEtherioParamInfo_helper(section, param_info.m_etherioParamInfo, ini);
}

void C_Ini_DataSendParamReader::WriteSendEtherioParamInfo_helper(const ACE_TCHAR* section, const Etherio_SendParam_Info& etherioParamInfo, CIniFile& ini)
{
	ini.SetValue(section,"sysfile_name",etherioParamInfo.m_strSysFileName);

	ini.SetValue(section,"driver_name",etherioParamInfo.m_strDriverName);

	ini.SetValue(section,"card_mac",etherioParamInfo.m_strMacAddr);

	ini.SetValue(section,"mem_size",etherioParamInfo.m_nMemSize);

	ini.SetValue(section,"send_type",etherioParamInfo.m_sendtype);

	ini.SetValue(section,"drv_load_type",etherioParamInfo.m_drv_load_type);

}

void C_Ini_DataSendParamReader::Write_ethereal_SendPlaybackInfo(const CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const ACE_TCHAR section[] = ACE_TEXT("ethereal_send_playback");

	ini.SetValue(section,ACE_TEXT("savefilepath"),param_info.m_ethereal_write_param.strPath.c_str());
	ini.SetValue(section,ACE_TEXT("savefilename"),param_info.m_ethereal_write_param.strFileName.c_str());

	ini.SetValue(section,ACE_TEXT("filetype"),param_info.m_ethereal_write_param.filetype);

	ini.SetValue(section,ACE_TEXT("num_files"),param_info.m_ethereal_write_param.num_files);
	ini.SetValue(section,ACE_TEXT("one_filesize_mb"),param_info.m_ethereal_write_param.one_filesize_mb);
	ini.SetValue(section,ACE_TEXT("captime"),param_info.m_ethereal_write_param.captime);

	return;
}

void C_Ini_DataSendParamReader::Write_cpt_SendPlaybackInfo(const CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const ACE_TCHAR section[] = ACE_TEXT("cpt_send_playback");

	ini.SetValue(section,ACE_TEXT("savefilepath"),param_info.m_cpt_write_param.strPath.c_str());
	ini.SetValue(section,ACE_TEXT("savefilename"),param_info.m_cpt_write_param.strFileName.c_str());

	ini.SetValue(section,ACE_TEXT("filetype"),param_info.m_cpt_write_param.filetype);

	ini.SetValue(section,ACE_TEXT("num_files"),param_info.m_cpt_write_param.num_files);
	ini.SetValue(section,ACE_TEXT("one_filesize_mb"),param_info.m_cpt_write_param.one_filesize_mb);
	ini.SetValue(section,ACE_TEXT("captime"),param_info.m_cpt_write_param.captime);
	ini.SetValue(section,"buffer_size",param_info.m_cpt_write_param.buffer_size_mb);

	return;
}

void C_Ini_DataSendParamReader::Read_ShareMem_Send_ParamInfo(CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const char section[] = "send_share_memory";

	ini.GetValue(section,"share_name",param_info.m_sharemem_write_param.m_strShareName);
	ini.GetValue(section,"share_size",param_info.m_sharemem_write_param.m_mem_size_mb);

	return;

}

void C_Ini_DataSendParamReader::Write_ShareMem_Send_ParamInfo(const CDataSend_Param_Info& param_info,CIniFile& ini)
{
	const char section[] = "send_share_memory";

	ini.SetValue(section,"share_name",param_info.m_sharemem_write_param.m_strShareName.c_str());
	ini.SetValue(section,"share_size",param_info.m_sharemem_write_param.m_mem_size_mb);

	return;
}

