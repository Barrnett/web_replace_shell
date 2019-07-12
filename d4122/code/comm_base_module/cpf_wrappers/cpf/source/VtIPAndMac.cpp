//////////////////////////////////////////////////////////////////////////
//VtIPAndMac.cpp

#include "cpf/VtIPAndMac.h"
#include "cpf/addr_tools.h"
#include "cpf/Data_Stream.h"
#include "cpf/strtools.h"

void CVtIPAndMac::Read(CTinyXmlEx& xml_reader,TiXmlNode * parent_node)
{
	xml_reader.GetValue_HostOrderIPRangeVector(parent_node,vt_ip,"ip_range");

	xml_reader.GetValue_MacVector(parent_node,vt_mac,"mac_addr");

	return;
}

void CVtIPAndMac::Write(CTinyXmlEx& xml_writter,TiXmlNode * parent_node) const
{
	xml_writter.SetValue_HostOrderIPRangeVector(parent_node,vt_ip,"ip_range");

	xml_writter.SetValue_MacVector(parent_node,vt_mac,"mac_addr");

	return;
}


int CVtIPAndMac::WriteStream(char *& pdata) const
{
	char * begin = pdata;

	CData_Stream_LE::PutUint32(pdata,(int)vt_ip.size());
	for(size_t i = 0; i < vt_ip.size(); ++i)
	{
		CData_Stream_LE::PutUint32(pdata,vt_ip[i].first);
		CData_Stream_LE::PutUint32(pdata,vt_ip[i].second);
	}

	CData_Stream_LE::PutUint32(pdata,(int)vt_mac.size());
	for(size_t i = 0; i < vt_mac.size(); ++i)
	{
		ACE_UINT64 mac_addr = vt_mac[i];

		CData_Stream_LE::PutFixString(pdata,6,(const char *)&mac_addr);
	}

	return (int)(pdata-begin);

}

int CVtIPAndMac::ReadStream(const char *pdata)
{
	const char * begin = pdata;

	int ip_nums = (int)CData_Stream_LE::GetUint32(pdata);
	vt_ip.reserve(ip_nums);

	for(int i = 0; i < ip_nums; ++i)
	{
		ACE_UINT32 ip1 = CData_Stream_LE::GetUint32(pdata);
		ACE_UINT32 ip2 = CData_Stream_LE::GetUint32(pdata);

		vt_ip.push_back(std::make_pair(ip1,ip2));
	}

	int mac_nums = (int)CData_Stream_LE::GetUint32(pdata);
	vt_mac.reserve(mac_nums);

	for(int i = 0; i < mac_nums; ++i)
	{
		ACE_UINT64 mac_addr = 0;

		memcpy(&mac_addr,CData_Stream_LE::GetFixString(pdata,6),6);

		vt_mac.push_back(mac_addr);
	}

	return (int)(pdata-begin);
}


std::string CVtIPAndMac::GetAddrDesc(BOOL bEmptyIsAll) const
{
	if( IsEmpty() )
	{
		if( bEmptyIsAll )
			return "所有";
		else
			return "";
	}

	std::string strClientInfo;

	{
		std::string str_ip;

		for(size_t i = 0; i < vt_ip.size(); ++i)
		{
			if( !str_ip.empty() )
				str_ip += ",";

			str_ip += CPF::get_iprange_string(vt_ip[i].first,vt_ip[i].second,true).c_str();
		}

		if( !strClientInfo.empty() && !str_ip.empty() )
		{
			strClientInfo += ",";
		}

		strClientInfo += str_ip;
	}

	{
		std::string strClient_mac = CPF::TransMACVectorToString(vt_mac).c_str();

		if( !strClientInfo.empty() && !strClient_mac.empty() )
		{
			strClientInfo += ",";
		}

		strClientInfo += strClient_mac;
	}

	return strClientInfo;
}

void CVtIPAndMac::GetFromString(const char * str_desc)
{
	/*if( strcmp(str_desc,"所有") == 0 )
	{
		vt_ip.clear();
		vt_mac.clear();

		return;
	}*/

	std::vector< std::string > vt_sting_ip_mac;

	CPF::GetWords(str_desc,strlen(str_desc),vt_sting_ip_mac,',',true);

	for(size_t i = 0; i < vt_sting_ip_mac.size(); ++i)
	{
		const char * the_string = vt_sting_ip_mac[i].c_str();

		if( CPF::is_valid_mac_string(the_string))
		{
			ACE_UINT64 mac_addr = 0;
			CPF::mac_a_to_addr2(the_string,mac_addr);

			vt_mac.push_back(mac_addr);
		}
		else
		{
			ACE_UINT32 begin_ip = 0,end_ip = 0;

			if( CPF::ParseIPRange(the_string,begin_ip,end_ip) )
			{
				begin_ip = ACE_NTOHL(begin_ip);
				end_ip = ACE_NTOHL(end_ip);

				vt_ip.push_back(std::make_pair(begin_ip,end_ip));
			}
		}
	}

	return;
}

