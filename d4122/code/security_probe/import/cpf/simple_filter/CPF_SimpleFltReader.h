//////////////////////////////////////////////////////////////////////////
//CPF_SimpleFltReader.h

#pragma once

#include "cpf/simple_filter/CPF_SimpleFltCondition.h"
#include "cpf/TinyXmlEx.h"

class CPF_CLASS CPF_SimpleFltIO
{
public:
	static bool read_filter(const char * filename,CPF_SimpleFltCondition& condition);

	static bool read_filter(const char * buf,int length,CPF_SimpleFltCondition& condition);

	static bool read_filter(CTinyXmlEx &xml_reader,CPF_SimpleFltCondition& condition);

	static bool read_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
		CPF_SimpleFltCondition& condition);

	static bool read_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
		std::vector<CPF_ONE_FILTER>& vt_filter);

	static bool read_one_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		CPF_ONE_FILTER& one_filter);

	static bool read_ip_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		SIMPLE_IP_FILTER& ip_filter);

	static bool read_port_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		SIMPLE_PORT_FILTER& port_filter);

public:
	static bool write_filter(const char * filename,const CPF_SimpleFltCondition& condition);

	static bool write_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
		const CPF_SimpleFltCondition& condition);

	static bool write_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
		const std::vector<CPF_ONE_FILTER>& vt_filter);

	static bool write_one_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		const CPF_ONE_FILTER& one_filter);

	static bool write_ip_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		const SIMPLE_IP_FILTER& ip_filter);

	static bool write_port_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		const SIMPLE_PORT_FILTER& port_filter);
public:
	static const char root_name[];

public:
	static BOOL translate_not_attr(const char * attr);
	static std::string translate_not_attr(int not_attr);

	static std::string translate_trans_type(int type);
	static int	translate_trans_type(const char * trans_type);

private:
	//2007_03_01-14_01_59.100
	static bool scanftime(const char * str_time,Time_Stamp& ts);

};
