//////////////////////////////////////////////////////////////////////////
//CardIP_FltReader.cpp

#include "simple_filter/CardIP_FltReader.h"

bool CCardIP_FltReader::read_filter(const char * filename,CCardIP_FltCondition& condition)
{
	CTinyXmlEx xml_reader;

	if(0 != xml_reader.open_for_readonly(filename) )
	{
		return false;
	}

	TiXmlNode * pFltNode  = xml_reader.GetRootNode()->FirstChildElement("flt");

	if( !pFltNode )
	{
		return false;
	}


	TiXmlNode * pcardFltNode  = pFltNode->FirstChildElement("card");

	while( pcardFltNode )
	{
		CCardIP_FltCondition::MY_IP_Pair ip_pair;

		if( read_onecard_filter(xml_reader,*pcardFltNode,ip_pair) )
		{
			if( ip_pair.card_no >= 0 && ip_pair.card_no <= 31 )
			{
				condition.m_pIPFilter[ip_pair.card_no] = ip_pair;
			}

		}

		pcardFltNode = pcardFltNode->NextSiblingElement("card");
	}

	for(int i = 0; i < 32; ++i)
	{
		if( condition.m_pIPFilter[i].card_no == -1 )
		{
			condition.m_pIPFilter[i].vt_ip_pair.push_back(std::make_pair(0x00000000,0xffffffff));
			condition.m_pIPFilter[i].cap_notip = true;
		}
	}

	return true;
}


bool CCardIP_FltReader::read_onecard_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
						 CCardIP_FltCondition::MY_IP_Pair& ip_pair)
{
	xml_reader.GetAttr(&node,"cardno",ip_pair.card_no);
	xml_reader.GetAttr(&node,"cap_notip",ip_pair.cap_notip);

	TiXmlNode * pipFltNode  = node.FirstChildElement("ip");

	while( pipFltNode )
	{
		std::pair<ACE_UINT32,ACE_UINT32> from_to;

		std::string strip1;
		xml_reader.GetAttr(pipFltNode,"from",strip1);
		if( !strip1.empty() )
		{
			from_to.first = ACE_NTOHL(inet_addr(strip1.c_str()));
		}

		std::string strip2;
		xml_reader.GetAttr(pipFltNode,"to",strip2);
		if( !strip2.empty() )
		{
			from_to.second = ACE_NTOHL(inet_addr(strip2.c_str()));
		}

		ip_pair.vt_ip_pair.push_back(from_to);

		pipFltNode = pipFltNode->NextSiblingElement("ip");
	}

	return true;
}
