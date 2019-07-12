//////////////////////////////////////////////////////////////////////////
//CPF_SimpleFltReader.cpp

#include "cpf/simple_filter/CPF_SimpleFltReader.h"

#include "cpf/CommonEtherDecode.h"
#include "cpf/CommonIPv4Decode.h"
#include "cpf/CommonTCPUDPDecode.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "cpf/other_tools.h"

const char CPF_SimpleFltIO::root_name[] = "simple_flt";

bool CPF_SimpleFltIO::read_filter(const char * filename,
									  CPF_SimpleFltCondition& condition)
{
	CTinyXmlEx xml_reader ;

	if(0 != xml_reader.open_for_readonly(filename) )
	{
		return false;
	}

	return read_filter(xml_reader,condition);
}

bool CPF_SimpleFltIO::read_filter(const char * buf,int length,
								  CPF_SimpleFltCondition& condition)
{
	CTinyXmlEx xml_reader ;

	if(0 != xml_reader.open(buf,length) )
	{
		return false;
	}

	return read_filter(xml_reader,condition);
}


bool CPF_SimpleFltIO::read_filter(CTinyXmlEx &xml_reader,
								  CPF_SimpleFltCondition& condition)
{
	condition.close();

	if( strcmp(xml_reader.GetRootNode()->Value(),
		CPF_SimpleFltIO::root_name) != 0 )
	{
		return false;
	}

	return read_filter(xml_reader,*xml_reader.GetRootNode(),condition);
}

bool CPF_SimpleFltIO::read_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
						CPF_SimpleFltCondition& condition)
{
	if( read_filter(xml_reader,node,condition.m_vt_filter) )
	{
		condition.cal_flt();

		return true;
	}

	return false;
}

bool CPF_SimpleFltIO::read_filter(CTinyXmlEx &xml_reader,TiXmlNode& node,
				 std::vector<CPF_ONE_FILTER>& vt_filter)
{
	
	for(TiXmlNode * pNode = node.FirstChildElement("flt");
		pNode;
		pNode = pNode->NextSiblingElement("flt") )
	{
		CPF_ONE_FILTER one_filter;

		if( read_one_flt(xml_reader,*pNode,one_filter) )
		{
			if( !one_filter.IsEmpty() )
			{
				vt_filter.push_back(one_filter);
			}
		}
	}

	return true;
}

bool CPF_SimpleFltIO::read_one_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
									  CPF_ONE_FILTER& one_filter)
{
	{
		std::string strNotYes;
		xml_reader.GetAttr(&node,"not_attr",strNotYes);
		one_filter.bNot = translate_not_attr(strNotYes.c_str());

		xml_reader.GetAttr(&node,"name",one_filter.name);
	}

	{
		TiXmlNode * pNode = node.FirstChildElement("ip_flt");
		
		if( pNode)
		{			
			read_ip_flt(xml_reader,*pNode,one_filter.ip_flt);
		}

	}

	{
		TiXmlNode * pNode = node.FirstChildElement("port_flt");

		if( pNode )	
		{			
			read_port_flt(xml_reader,*pNode,one_filter.port_flt);
		}

	}

	return true;

}


bool CPF_SimpleFltIO::read_ip_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
									 SIMPLE_IP_FILTER& ip_filter)
{
	std::string strNotYes;
	xml_reader.GetAttr(&node,"not_attr",strNotYes);

	ip_filter.bNot = translate_not_attr(strNotYes.c_str());

	int dir = 0;
	xml_reader.GetAttr(&node,"dir",dir);

	ip_filter.dir = (CPF_FILTER_DIR)dir;

	{
		TiXmlNode * ipsrc_node = node.FirstChildElement("ip_src");

		if( ipsrc_node )
		{
			strNotYes.clear();
			xml_reader.GetAttr(ipsrc_node,"not_attr",strNotYes);

			ip_filter.bNotInRangeIP1 = translate_not_attr(strNotYes.c_str());

			xml_reader.GetValue_HostOrderIPRangeVector(ipsrc_node,ip_filter.vt_ip1,"ip_range");
		}
	}
	
	{

		TiXmlNode * ipdst_node = node.FirstChildElement("ip_dst");

		if( ipdst_node )
		{
			strNotYes.clear();
			xml_reader.GetAttr(ipdst_node,"not_attr",strNotYes);

			ip_filter.bNotInRangeIP2 = translate_not_attr(strNotYes.c_str());

			xml_reader.GetValue_HostOrderIPRangeVector(ipdst_node,ip_filter.vt_ip2,"ip_range");
		}
	}

	{
		TiXmlNode * upper_proto_node = node.FirstChildElement("upper_proto");

		if( upper_proto_node )
		{
			strNotYes.clear();
			xml_reader.GetAttr(upper_proto_node,"not_attr",strNotYes);

			ip_filter.bNotInProto = translate_not_attr(strNotYes.c_str());


			for( TiXmlNode * protoname_node = upper_proto_node->FirstChildElement("proto_name");
				protoname_node;
				protoname_node = protoname_node->NextSiblingElement("proto_name") )
			{
				std::string protoname;			

				if( xml_reader.GetValue(protoname_node,protoname) )
				{
					int type = translate_trans_type(protoname.c_str());

					ip_filter.vt_upperproto.push_back(type);
				}				
			}
		}
	}

	return true;

}

bool CPF_SimpleFltIO::read_port_flt(CTinyXmlEx &xml_reader,TiXmlNode& node,
									   SIMPLE_PORT_FILTER& port_filter)
{
	std::string strNotYes;
	xml_reader.GetAttr(&node,"not_attr",strNotYes);

	port_filter.bNot = translate_not_attr(strNotYes.c_str());

	int dir = 0;
	xml_reader.GetAttr(&node,"dir",dir);

	port_filter.dir = (CPF_FILTER_DIR)dir;

	{
		TiXmlNode * portsrc_node = node.FirstChildElement("port_src");

		if( portsrc_node )
		{
			strNotYes.clear();
			xml_reader.GetAttr(portsrc_node,"not_attr",strNotYes);

			port_filter.m_bNotInPort1 = translate_not_attr(strNotYes.c_str());

			xml_reader.GetValue_Uint16RangeVector(portsrc_node,port_filter.vt_port1,"port_range");
		}
	}

	{
		TiXmlNode * portdst_node = node.FirstChildElement("port_dst");

		if( portdst_node )
		{
			strNotYes.clear();
			xml_reader.GetAttr(portdst_node,"not_attr",strNotYes);

			port_filter.m_bNotInPort2 = translate_not_attr(strNotYes.c_str());

			xml_reader.GetValue_Uint16RangeVector(portdst_node,port_filter.vt_port2,"port_range");
		}
	}





	return true;
}


bool CPF_SimpleFltIO::write_filter(const char * filename,const CPF_SimpleFltCondition& condition)
{
	CTinyXmlEx xml_writter;

	if( 0 != xml_writter.open_ex(filename,CTinyXmlEx::OPEN_MODE_CREATE_TRUNC) )
		return false;

	TiXmlNode* pRootNode = xml_writter.InsertRootElement(CPF_SimpleFltIO::root_name);

	if( !pRootNode )
		return false;

	if( !write_filter(xml_writter,*pRootNode,condition) )
		return false;

	xml_writter.savefile();

	xml_writter.close();

	return true;

}

bool CPF_SimpleFltIO::write_filter(CTinyXmlEx &xml_writter,TiXmlNode& node,
						 const CPF_SimpleFltCondition& condition)
{
	return write_filter(xml_writter,node,condition.m_vt_filter);
}

bool CPF_SimpleFltIO::write_filter(CTinyXmlEx &xml_writter,TiXmlNode& node,
						 const std::vector<CPF_ONE_FILTER>& vt_filter)
{
	for(size_t i = 0; i < vt_filter.size(); ++i)
	{
		TiXmlNode * flt_node = xml_writter.InsertEndChildElement(&node,"flt");

		write_one_flt(xml_writter,*flt_node,vt_filter[i]);
	}

	return true;
}

bool CPF_SimpleFltIO::write_one_flt(CTinyXmlEx &xml_writter,TiXmlNode& node,
						 const CPF_ONE_FILTER& one_filter)
{
	{
		xml_writter.SetAttr(&node,"not_attr",
			translate_not_attr(one_filter.bNot));

		xml_writter.SetAttr(&node,"name",one_filter.name);
	}

	{
		TiXmlNode * ip_flt_node = xml_writter.InsertEndChildElement(&node,"ip_flt");

		write_ip_flt(xml_writter,*ip_flt_node,one_filter.ip_flt);
	}

	{
		TiXmlNode * port_flt_node = xml_writter.InsertEndChildElement(&node,"port_flt");

		write_port_flt(xml_writter,*port_flt_node,one_filter.port_flt);
	}

	return true;
}

bool CPF_SimpleFltIO::write_ip_flt(CTinyXmlEx &xml_writter,TiXmlNode& node,
						 const SIMPLE_IP_FILTER& ip_filter)
{
	xml_writter.SetAttr(&node,"not_attr",
		translate_not_attr(ip_filter.bNot));

	xml_writter.SetAttr(&node,"dir",(int)ip_filter.dir);

	{
		TiXmlNode * ipsrc_node = xml_writter.InsertEndChildElement(&node,"ip_src");

		if( ipsrc_node )
		{
			xml_writter.SetAttr(ipsrc_node,"not_attr",
				translate_not_attr(ip_filter.bNotInRangeIP1));

			xml_writter.SetValue_HostOrderIPRangeVector(ipsrc_node,ip_filter.vt_ip1,"ip_range");
		}
	}

	{
		TiXmlNode * ipdst_node = xml_writter.InsertEndChildElement(&node,"ip_dst");

		if( ipdst_node )
		{
			xml_writter.SetAttr(ipdst_node,"not_attr",
				translate_not_attr(ip_filter.bNotInRangeIP2));

			xml_writter.SetValue_HostOrderIPRangeVector(ipdst_node,ip_filter.vt_ip2,"ip_range");
		}
	}

	{
		TiXmlNode * upper_proto_node = xml_writter.InsertEndChildElement(&node,"upper_proto");

		if( upper_proto_node )
		{
			xml_writter.SetAttr(upper_proto_node,"not_attr",
				translate_not_attr(ip_filter.bNotInProto));

			for(size_t i = 0; i < ip_filter.vt_upperproto.size(); ++i)
			{
				TiXmlNode * protoname_node = xml_writter.InsertEndChildElement(upper_proto_node,"proto_name");

				xml_writter.SetValue(protoname_node,
					translate_trans_type(ip_filter.vt_upperproto[i]));
			}
		}

	}

	return true;
}

bool CPF_SimpleFltIO::write_port_flt(CTinyXmlEx &xml_writter,TiXmlNode& node,
						   const SIMPLE_PORT_FILTER& port_filter)
{
	xml_writter.SetAttr(&node,"not_attr",
		translate_not_attr(port_filter.bNot));

	xml_writter.SetAttr(&node,"dir",(int)port_filter.dir);

	{
		TiXmlNode * portsrc_node = xml_writter.InsertEndChildElement(&node,"port_src");

		if( portsrc_node )
		{
			xml_writter.SetAttr(portsrc_node,"not_attr",
				translate_not_attr(port_filter.m_bNotInPort1));

			xml_writter.SetValue_Uint16RangeVector(portsrc_node,port_filter.vt_port1,"port_range");
		}
	}

	{
		TiXmlNode * portdst_node = xml_writter.InsertEndChildElement(&node,"port_dst");

		if( portdst_node )
		{
			xml_writter.SetAttr(portdst_node,"not_attr",
				translate_not_attr(port_filter.m_bNotInPort2));

			xml_writter.SetValue_Uint16RangeVector(portdst_node,port_filter.vt_port2,"port_range");
		}
	}


	return true;
}

std::string CPF_SimpleFltIO::translate_not_attr(int not_attr)
{
	if( not_attr )
	{
		return std::string("not");
	}
	else
	{
		return std::string("");
	}
}

BOOL CPF_SimpleFltIO::translate_not_attr(const char * not_attr)
{
	if( ACE_OS::strcasecmp(not_attr,"not")==0
		||ACE_OS::strcasecmp(not_attr,"nak")==0
		||ACE_OS::strcasecmp(not_attr,"no")==0)
	{
		return 1;
	}

	return 0;
}

std::string CPF_SimpleFltIO::translate_trans_type(int type)
{
	if( CCommonIPv4Decode::INDEX_PID_IPv4_TCP == type )
	{
		return std::string("tcp");
	}
	else if( CCommonIPv4Decode::INDEX_PID_IPv4_UDP == type )
	{
		return std::string("udp");
	}
	else if( CCommonIPv4Decode::INDEX_PID_IPv4_ICMP == type )
	{
		return std::string("icmp");
	}
	else
	{
		ACE_ASSERT(false);
		return "tcp";
	}	
}

int CPF_SimpleFltIO::translate_trans_type(const char * trans_type)
{
	if( 0 == stricmp(trans_type,"tcp") )
	{
		return CCommonIPv4Decode::INDEX_PID_IPv4_TCP;
	}
	else if( 0 == stricmp(trans_type,"udp") )
	{
		return CCommonIPv4Decode::INDEX_PID_IPv4_UDP;
	}
	else if( 0 == stricmp(trans_type,"icmp") )
	{
		return CCommonIPv4Decode::INDEX_PID_IPv4_ICMP;
	}
	else
	{
		ACE_ASSERT(FALSE);
		return 1;	
	}
}


//2007_03_01-14_01_59.100
bool CPF_SimpleFltIO::scanftime(const char * str_time,Time_Stamp& ts)
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

	ts.sec = (ACE_INT32)rtntime;
	ts.ns = ns;

	return true;

}