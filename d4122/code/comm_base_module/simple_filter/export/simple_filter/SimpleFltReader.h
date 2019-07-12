//////////////////////////////////////////////////////////////////////////
//SimpleFltReader.h

#pragma once

#include "simple_filter/simple_filter.h"
#include "simple_filter/SimpleFltCondition.h"
#include "cpf/TinyXmlEx.h"

class SIMPLE_FILTER_CLASS CSimpleFltReader
{
public:
	static bool read_filter(const char * filename,CSimpleFltCondition& condition);

	static bool read_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
		std::vector<ONE_FILTER>& vt_filter);

	static bool read_one_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		ONE_FILTER& one_filter);

	static bool read_if_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		If_FILTER& if_filter);

	static bool read_time_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		Time_FILTER& time_filter);

	static bool read_mac_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		MAC_FILTER& one_filter);

	static bool read_ip_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		IP_FILTER& one_filter);

	static bool read_port_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		PORT_FILTER& one_filter);

	static bool read_datapat_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
		DATAPAT_FILTER& datapat_filter);

private:
	//2007_03_01-14_01_59.100
	static bool scanftime(const char * str_time,Time_Stamp& ts);

};
