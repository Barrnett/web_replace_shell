//////////////////////////////////////////////////////////////////////////
//BaseXMLConfig.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/Octets.h"

class CPF_CLASS CBaseXMLConfig
{
public:
	CBaseXMLConfig();
	virtual ~CBaseXMLConfig();

public:
	virtual void close() = 0;

public:
	BOOL LoadFromFile(const char * xml_file_name);
	BOOL LoadFromMem(const char * buf,int length);

	BOOL SaveToFile(const char * xml_file_name);
	BOOL SaveToFile();

public:
	BOOL LoadFromCryDataFile(const char * data_file_name);

private:
	virtual BOOL LoadConfig(CTinyXmlEx& xml_file) = 0;
	virtual BOOL SaveConfig(CTinyXmlEx& xml_file) = 0;

public:
	static BOOL LoadCryDataFile(const char * data_file_name,CCPFOctets& outOctets);

protected:
	std::string					m_strCurOpenFilename;

};

