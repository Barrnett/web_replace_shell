//////////////////////////////////////////////////////////////////////////
//DataSourceParamReader.h

#pragma once

#include "PacketIO/DataSource_Param_Info.h"

class CDataSourceParamReader
{
public:
	CDataSourceParamReader(void){}
	virtual ~CDataSourceParamReader(void){}

public:
	virtual BOOL Open(const char * filename) = 0;
	virtual void Close() = 0;

	virtual BOOL Read(CDataSource_Param_Info& param_info) = 0;
	virtual BOOL Write(const CDataSource_Param_Info& param_info,const char * filename = NULL) = 0;

};

#include "cpf/IniFile.h"

class C_Ini_DataSourceParamReader : public CDataSourceParamReader
{
public:
	C_Ini_DataSourceParamReader(void);
	virtual ~C_Ini_DataSourceParamReader(void);

public:
	virtual BOOL Open(const char * filename);
	virtual void Close();

	virtual BOOL Read(CDataSource_Param_Info& param_info);
	virtual BOOL Write(const CDataSource_Param_Info& param_info,const char * filename = NULL);

private:
#ifdef OS_WINDOWS
	void ReadWinpcapInfo(CDataSource_Param_Info& param_info,CIniFile& inifile);
#endif//OS_WINDOWS

	void ReadEtherioParamInfo(CDataSource_Param_Info& param_info,CIniFile& inifile);
	void ReadEtherioParamInfo_helper(const char* section, Etherio_RecvParam_Info& etherioParamInfo,CIniFile& ini);

	void ReadEthreadPlaybackParamInfo(CDataSource_Param_Info & param_info,CIniFile& inifile);

	void ReadCptPlaybackParamInfo(CDataSource_Param_Info & param_info,CIniFile& inifile);
	void ReadCommonPlaybackParamInfo(Playback_Param_Info& common_playback_param_info,CIniFile& inifile);

	void ReadMemDatasourceParamInfo(CDataSource_Param_Info& param_info,CIniFile& inifile);
	void ReadMemEditDataParamInfo(CDataSource_Param_Info& param_info,CIniFile& inifile);

	void ReadShareMemDatasouceParamInfo(CDataSource_Param_Info& param_info,CIniFile& inifile);

#ifdef OS_WINDOWS
	void WriteWinpcapInfo(const CDataSource_Param_Info& param_info,CIniFile& inifile);
#endif//OS_WINDOWS

	void WriteEtherioParamInfo(const CDataSource_Param_Info& param_info,CIniFile& inifile);
	void WriteEtherioParamInfo_helper(const char* section, const Etherio_RecvParam_Info& etherioParamInfo,CIniFile& inifile);

	void WriteEthreadPlaybackParamInfo(const CDataSource_Param_Info & param_info,CIniFile& inifile);

	void WriteCptPlaybackParamInfo(const CDataSource_Param_Info & param_info,CIniFile& inifile);
	void WriteCommonPlaybackParamInfo(const Playback_Param_Info& common_playback_param_info,CIniFile& inifile);

	void WriteMemDatasourceParamInfo(const CDataSource_Param_Info& param_info,CIniFile& inifile);
	void WriteMemEditDataParamInfo(const CDataSource_Param_Info& param_info,CIniFile& inifile);

	void WriteShareMemDatasouceParamInfo(const CDataSource_Param_Info& param_info,CIniFile& inifile);

private:
	std::string				m_strIniFileName;
	//CIniFile				m_inifile;
};
