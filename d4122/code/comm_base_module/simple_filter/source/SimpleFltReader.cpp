//////////////////////////////////////////////////////////////////////////
//SimpleFltReader.cpp

#include "simple_filter/SimpleFltReader.h"
#include "cpf/CommonEtherDecode.h"
#include "cpf/CommonIPv4Decode.h"
#include "cpf/CommonTCPUDPDecode.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "cpf/other_tools.h"
#include "cpf/strtools.h"

bool CSimpleFltReader::read_filter(const char * filename,CSimpleFltCondition& condition)
{
	CTinyXmlEx xml_reader ;

	if(0 != xml_reader.open_for_readonly(filename) )
	{
		return false;
	}

	if( read_filter(xml_reader,*xml_reader.GetRootNode(),condition.m_vt_filter) )
	{
		condition.cal_flt();
	}

	return true;

}

bool CSimpleFltReader::read_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
				 std::vector<ONE_FILTER>& vt_filter)
{

	TiXmlNode * pNode = xml_reader.FirstChildElement(&node,"flt");

	while( pNode )
	{
		ONE_FILTER one_filter;

		std::string strNotYes;

		xml_reader.GetAttr(pNode,"attr",strNotYes);

		if( ACE_OS::strcasecmp(strNotYes.c_str(),"not")==0||ACE_OS::strcasecmp(strNotYes.c_str(),"nak")==0 )
		{
			one_filter.bNot = 1;
		}
		
		if( read_one_flt(xml_reader,*pNode,one_filter) )
		{
			if( !one_filter.IsEmpty() )
			{
				vt_filter.push_back(one_filter);
			}
		}

		pNode = xml_reader.NextSiblingElement(pNode,"flt");
	}

	return true;
}

bool CSimpleFltReader::read_one_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
									  ONE_FILTER& one_filter)
{
	{
		TiXmlNode * pNode = xml_reader.FirstChildElement(&node,"ifflt");

		if( pNode )
		{			
			if( read_if_flt(xml_reader,*pNode,one_filter.if_filter) )
			{
			}
		}

	}

	{
		TiXmlNode * pNode = xml_reader.FirstChildElement(&node,"timeflt");

		while( pNode )
		{
			Time_FILTER time_filter;

			if( read_time_flt(xml_reader,*pNode,time_filter) )
			{
				one_filter.vt_time.push_back(time_filter);
			}

			pNode = xml_reader.NextSiblingElement(pNode,"timeflt");
		}
	}

	{
		TiXmlNode * pNode = xml_reader.FirstChildElement(&node,"macflt");

		while( pNode )
		{
			MAC_FILTER mac_filter;

			if( read_mac_flt(xml_reader,*pNode,mac_filter) )
			{
				one_filter.vt_mac.push_back(mac_filter);
			}

			pNode = xml_reader.NextSiblingElement(pNode,"macflt");
		}
	}

	{
		TiXmlNode * pNode = xml_reader.FirstChildElement(&node,"ipflt");

		while( pNode )
		{
			IP_FILTER ip_filter;

			if( read_ip_flt(xml_reader,*pNode,ip_filter) )
			{
				one_filter.vt_ip.push_back(ip_filter);
			}

			pNode = xml_reader.NextSiblingElement(pNode,"ipflt");
		}
	}

	{
		TiXmlNode * pNode = xml_reader.FirstChildElement(&node,"portflt");

		while( pNode )
		{
			PORT_FILTER port_filter;

			if( read_port_flt(xml_reader,*pNode,port_filter) )
			{
				one_filter.vt_port.push_back(port_filter);
			}

			pNode = xml_reader.NextSiblingElement(pNode,"portflt");
		}
	}

	{
		TiXmlNode * pNode = xml_reader.FirstChildElement(&node,"datapatflt");

		while( pNode )
		{
			DATAPAT_FILTER datapat_filter;

			if( read_datapat_flt(xml_reader,*pNode,datapat_filter) && datapat_filter.patlen > 0 )
			{
				one_filter.vt_data_pat.push_back(datapat_filter);
			}

			pNode = xml_reader.NextSiblingElement(pNode,"datapatflt");
		}
	}
	return true;

}

bool CSimpleFltReader::read_time_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
				   Time_FILTER& time_filter)
{
	std::string strNotYes;
	xml_reader.GetAttr(&node,"attr",strNotYes);

	if( ACE_OS::strcasecmp(strNotYes.c_str(),"not")==0||ACE_OS::strcasecmp(strNotYes.c_str(),"nak")==0 )
	{
		time_filter.bNot = 1;
	}

	std::string str_from_time;
	xml_reader.GetAttr(&node,"from",str_from_time);
	if( !str_from_time.empty() )
	{
		if( !scanftime(str_from_time.c_str(),time_filter.from.m_ts) )
		{
			return false;
		}
	}

	std::string str_to_time;
	xml_reader.GetAttr(&node,"to",str_to_time);
	if( !str_to_time.empty() )
	{
		if( !scanftime(str_to_time.c_str(),time_filter.to.m_ts) )
		{
			return false;
		}	
	}

	if( str_from_time.empty()
		&& str_to_time.empty() )
	{
		return false;
	}

	if( time_filter.from > time_filter.to )
	{
		return false;
	}

	return true;
}

bool CSimpleFltReader::read_if_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
				 If_FILTER& if_filter)
{
	std::string strNotYes;
	xml_reader.GetAttr(&node,"attr",strNotYes);

	if( ACE_OS::strcasecmp(strNotYes.c_str(),"not")==0||ACE_OS::strcasecmp(strNotYes.c_str(),"nak")==0 )
	{
		if_filter.bNot = 1;
	}

	int from = -1;
	xml_reader.GetAttr(&node,"from",from);

	int to = -1;
	xml_reader.GetAttr(&node,"to",to);

	if( from >= 0 && to < 0 )
	{
		to = from;
	}

	if( from >= 0 && to >= 0 )
	{
		if_filter.card.first = from;
		if_filter.card.second = to;

		return true;
	}
	else
	{
		if_filter.card.first = -1;
		if_filter.card.second = -1;

		return false;	

	}

	return false;	
}

bool CSimpleFltReader::read_mac_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
									  MAC_FILTER& one_filter)
{
	std::string strNotYes;
	xml_reader.GetAttr(&node,"attr",strNotYes);

	if( ACE_OS::strcasecmp(strNotYes.c_str(),"not")==0||ACE_OS::strcasecmp(strNotYes.c_str(),"nak")==0 )
	{
		one_filter.bNot = 1;
	}

	std::string strmac1;
	xml_reader.GetAttr(&node,"mac1",strmac1);
	if( !strmac1.empty() )
	{
		CPF::mac_a_to_addr(strmac1.c_str(),one_filter.macaddr1);
	}

	std::string strmac2;
	xml_reader.GetAttr(&node,"mac2",strmac2);
	if( !strmac2.empty() )
	{
		CPF::mac_a_to_addr(strmac2.c_str(),one_filter.macaddr2);
	}

	std::string strproto;
	xml_reader.GetAttr(&node,"proto",strproto);
	if( !strproto.empty() )
	{
		if( strproto.compare("ip") == 0 )
		{
			one_filter.upperproto = CCommonEtherDecode::INDEX_PID_MAC_IP;
		}
		else if( strproto.compare("arp") == 0 )
		{
			one_filter.upperproto = CCommonEtherDecode::INDEX_PID_MAC_ARP;
		}
		else
		{
			one_filter.upperproto = CCommonEtherDecode::INDEX_PID_MAC_OTHER;
		}
	}
	
	xml_reader.GetAttr(&node,"dir",(int&)one_filter.dir);

	if( strmac1.empty() 
		&& strmac2.empty() 
		&& strproto.empty() 
		&& one_filter.dir == dir_uncare )
	{
		return false;
	}

	return true;
}

bool CSimpleFltReader::read_ip_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
									 IP_FILTER& one_filter)
{
	std::string strNotYes;
	xml_reader.GetAttr(&node,"attr",strNotYes);

	if( ACE_OS::strcasecmp(strNotYes.c_str(),"not")==0||ACE_OS::strcasecmp(strNotYes.c_str(),"nak")==0 )
	{
		one_filter.bNot = 1;
	}

	std::string strip1;
	xml_reader.GetAttr(&node,"ip1",strip1);
	if( !strip1.empty() )
	{
		one_filter.ip1 = inet_addr(strip1.c_str());
	}

	std::string strip2;
	xml_reader.GetAttr(&node,"ip2",strip2);
	if( !strip2.empty() )
	{
		one_filter.ip2 = inet_addr(strip2.c_str());
	}

	xml_reader.GetAttr(&node,"dir",(int&)one_filter.dir);

	std::string strproto;
	xml_reader.GetAttr(&node,"proto",strproto);
	if( !strproto.empty() )
	{
		if( strproto.compare("tcp") == 0 )
		{
			one_filter.upperproto = CCommonIPv4Decode::INDEX_PID_IPv4_TCP;
		}
		else if( strproto.compare("udp") == 0 )
		{
			one_filter.upperproto = CCommonIPv4Decode::INDEX_PID_IPv4_UDP;
		}
		else if( strproto.compare("icmp") == 0 )
		{
			one_filter.upperproto = CCommonIPv4Decode::INDEX_PID_IPv4_ICMP;
		}
		else
		{
			one_filter.upperproto = CCommonIPv4Decode::INDEX_PID_IPv4_OTHER;
		}
	}

	if( strip1.empty() 
		&& strip2.empty() 
		&& strproto.empty() 
		&& one_filter.dir == dir_uncare )
	{
		return false;
	}

	return true;

}

bool CSimpleFltReader::read_port_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
									   PORT_FILTER& one_filter)
{
	std::string strNotYes;
	xml_reader.GetAttr(&node,"attr",strNotYes);

	if( ACE_OS::strcasecmp(strNotYes.c_str(),"not")==0||ACE_OS::strcasecmp(strNotYes.c_str(),"nak")==0 )
	{
		one_filter.bNot = 1;
	}

	xml_reader.GetAttr(&node,"port1",one_filter.port1);

	xml_reader.GetAttr(&node,"port2",one_filter.port2);

	xml_reader.GetAttr(&node,"dir",(int&)one_filter.dir);

	if( one_filter.port1 == 0
		&& one_filter.port2 == 0
		&& one_filter.dir == dir_uncare )
	{
		return false;
	}

	return true;
}


bool CSimpleFltReader::read_datapat_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
							 DATAPAT_FILTER& datapat_filter)
{
	std::string strNotYes;
	xml_reader.GetAttr(&node,"attr",strNotYes);

	if( ACE_OS::strcasecmp(strNotYes.c_str(),"not")==0||ACE_OS::strcasecmp(strNotYes.c_str(),"nak")==0 )
	{
		datapat_filter.bNot = 1;
	}

	xml_reader.GetAttr(&node,"offset",datapat_filter.offset);

	xml_reader.GetAttr(&node,"type",datapat_filter.type);

	int datatype = 0;
	xml_reader.GetAttr(&node,"datatype",datatype);

	std::string strdata;

	xml_reader.GetAttr(&node,"datapat",strdata);

	if( strdata.empty() )
	{
		return false;
	}

	if( 0 == datatype )
	{
		memcpy(datapat_filter.pat,strdata.c_str(),mymin(sizeof(datapat_filter.pat),strdata.size()));

		datapat_filter.patlen = mymin(sizeof(datapat_filter.pat),strdata.size());
	}
	else if( 1 == datatype )
	{
		CPF::hexstr2bin_littleendian(strdata.c_str(),datapat_filter.pat,
			mymin(strdata.size()/2,sizeof(datapat_filter.pat)),
			&datapat_filter.patlen );
	}
	else
	{
		ACE_ASSERT(false);
		return false;
	}

	return true;
}


//2007_03_01-14_01_59.100
bool CSimpleFltReader::scanftime(const char * str_time,Time_Stamp& ts)
{
	int year = -1;
	int month = -1;
	int date = -1;
	int hour = -1;
	int minute = -1;
	int sec = -1;
	int ns = -1;

	sscanf(str_time,"%d_%d_%d-%d_%d_%d.%d",
		&year,&month,&date,
		&hour,&minute,&sec,
		&ns );

	if( year == -1 
		|| month == -1
		|| date == -1 
		|| hour == -1 
		|| minute == -1 
		|| sec == -1 )
	{
		return false;
	}

	if( ns == -1 )
	{
		ns = 0;
	}
	
	tm retm;

	retm.tm_sec = sec;
	retm.tm_min = minute;
	retm.tm_hour = hour;
	retm.tm_mday = date;
	retm.tm_mon = month-1;
	retm.tm_year = year-1900;

	time_t rtntime = mktime(&retm);

	ts.sec = rtntime;
	ts.ns = ns;

	return true;

}