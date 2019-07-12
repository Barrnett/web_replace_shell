//////////////////////////////////////////////////////////////////////////
//CardIP_FltReader.h

#pragma once

#include "simple_filter/simple_filter.h"
#include "simple_filter/CardIP_FltCondition.h"
#include "cpf/TinyXmlEx.h"

class SIMPLE_FILTER_CLASS CCardIP_FltReader
{

public:
	static bool read_filter(const char * filename,CCardIP_FltCondition& condition);

	static bool read_onecard_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
		CCardIP_FltCondition::MY_IP_Pair& ip_pair);

};
