//////////////////////////////////////////////////////////////////////////
//DataSendParamReader.h

#pragma once

#include "PacketIO/DataSend_Param_Info.h"

class CDataSendParamReader
{
public:
	CDataSendParamReader(void){}
	virtual ~CDataSendParamReader(void){}

public:
	virtual BOOL Open(const char * filename) = 0;
	virtual void Close() = 0;

	virtual BOOL Read(CDataSend_Param_Info& param_info) = 0;
	virtual BOOL Write(const CDataSend_Param_Info& param_info,const char * filename = NULL) = 0;
};



#include "cpf/IniFile.h"

class C_Ini_DataSendParamReader : public CDataSendParamReader
{
public:
	C_Ini_DataSendParamReader(void);
	virtual ~C_Ini_DataSendParamReader(void);

public:
	virtual BOOL Open(const char * filename);
	virtual void Close();

	virtual BOOL Read(CDataSend_Param_Info& param_info);
	virtual BOOL Write(const CDataSend_Param_Info& param_info,const char * filename = NULL);

private:
	void ReadSendWinpcapInfo(CDataSend_Param_Info& param_info,CIniFile& ini);
	void WriteSendWinpcapInfo(const CDataSend_Param_Info& param_info,CIniFile& ini);

	void ReadSendEtherioParamInfo(CDataSend_Param_Info& param_info,CIniFile& ini);
	void ReadSendEtherioParamInfo_helper(const ACE_TCHAR* section, Etherio_SendParam_Info& etherioParamInfo,CIniFile& ini);
	void WriteSendEtherioParamInfo(const CDataSend_Param_Info& param_info,CIniFile& ini);
	void WriteSendEtherioParamInfo_helper(const ACE_TCHAR* section, const Etherio_SendParam_Info& etherioParamInfo,CIniFile& ini);

	void Read_ethereal_SendPlaybackInfo(CDataSend_Param_Info& param_info,CIniFile& ini);
	void Write_ethereal_SendPlaybackInfo(const CDataSend_Param_Info& param_info,CIniFile& ini);

	void Read_cpt_SendPlaybackInfo(CDataSend_Param_Info& param_info,CIniFile& ini);
	void Write_cpt_SendPlaybackInfo(const CDataSend_Param_Info& param_info,CIniFile& ini);

	void Read_ShareMem_Send_ParamInfo(CDataSend_Param_Info& param_info,CIniFile& ini);
	void Write_ShareMem_Send_ParamInfo(const CDataSend_Param_Info& param_info,CIniFile& ini);

private:
	std::string	m_strIniFileName;
	//CIniFile	m_inifile;
};

