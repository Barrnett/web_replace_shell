//////////////////////////////////////////////////////////////////////////
//addr_tools.cpp

#include "cpf/addr_tools.h"
#include "cpf/strtools.h"
#include "cpf/Common_Func_Macor.h"

//////////////////////////////////////////////////////////////////////////
//ip
//////////////////////////////////////////////////////////////////////////

const BYTE CPF::s_zero_mac_ptr[8] = {0};
const ACE_UINT64 CPF::s_zero_mac_int = 0;

const BYTE CPF::s_broadcast_mac_ptr[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00};
const ACE_UINT64 CPF::s_broadcast_mac_int = *((ACE_UINT64 *)CPF::s_broadcast_mac_ptr);

const char * CPF::nip_to_string(ACE_UINT32 nip)
{
	return inet_ntoa(*(in_addr *)&nip);
}

ACE_UINT32 CPF::string_to_nip(const char * string_ip)
{
	while(*string_ip==' ')
		++string_ip;

	return inet_addr(string_ip);
}

std::string CPF::get_iprange_string(ACE_UINT32 beging_hip,ACE_UINT32 end_hip,BOOL bSameShowOne)
{
	char buf[128] = {0};

	strcpy(buf,hip_to_string(beging_hip));

	if( end_hip != beging_hip || !bSameShowOne )
	{
		strcat(buf,"-");
		strcat(buf,hip_to_string(end_hip));
	}

	return (std::string(buf));
}

//解析"192.168.0.1-192.168.0.255"这样的字符串，得到两个ip网络字节序地址
//如果正确就返回true。否则返回false
BOOL CPF::ParseIPRange(const char* source, size_t numtchars,ACE_UINT32& begin_nip,ACE_UINT32& end_nip,char sep)
{
	std::vector<std::string>	vt_string;

	int nums = CPF::GetWords(source,numtchars,vt_string,sep,true);

	if( nums != 1 && nums != 2 )
	{
		return false;
	}

	{
		//去掉前后的空格
		const char * pbegin = vt_string[0].c_str();

		while( *pbegin == ' ' )
		{
			++pbegin;
		}

		char * pend = (char *)vt_string[0].c_str() + vt_string[0].size()-1;

		while( *pend == ' ' )
		{
			*pend = 0;
			--pend;
		}

		if( !CPF::is_valid_ip_string(pbegin) )
			return false;

		begin_nip = ACE_OS::inet_addr(pbegin);
	}


	if( nums == 1 )
	{
		end_nip = begin_nip;
	}
	else
	{
		//去掉前后的空格
		const char * pbegin = vt_string[1].c_str();

		while( *pbegin == ' ' )
		{
			++pbegin;
		}

		char * pend = (char *)vt_string[1].c_str() + vt_string[1].size()-1;

		while( *pend == ' ' )
		{
			*pend = 0;
			--pend;
		}

		if( !CPF::is_valid_ip_string(pbegin) )
			return false;

		end_nip = ACE_OS::inet_addr(pbegin);
	}

	return true;
}

int CPF::is_valid_ip_string(IN const char * text)
{
	size_t ntextlen = ACE_OS::strlen(text);

	if( ntextlen < 7 || ntextlen > 15 )
		return false;

	char tempsep[3] = {0};
	int intip[4] = {0};

	int iNums = ::sscanf(text,
		"%d%c%d%c%d%c%d",
		intip+0,tempsep+0,intip+1,tempsep+1,intip+2,tempsep+2,intip+3);

	if( iNums != 7 )
	{			
		return false;
	}

	for(int j = 0; j < 3; ++j)
	{
		if( tempsep[j] != '.' )
			return false;
	}

	for(int i= 0; i < 4; ++i)
	{
		if( intip[i] < 0 || intip[i] > 255 )
		{
			return false;
		}
	}


	return true;
}

CPF_CLASS
int CPF::is_valid_mask_string(IN const char * text)
{
	if( CPF::is_valid_ip_string(text) )
	{
		return is_valid_mask(CPF::string_to_hip(text));
	}

	return false;
}

CPF_CLASS
int CPF::is_valid_mask(IN ACE_UINT32 hipmask)
{
	return IS_POWER_2(~hipmask+1);
}

//////////////////////////////////////////////////////////////////////////
//mac
//////////////////////////////////////////////////////////////////////////
//MAC地址到字符串的转换,sep表示间隔字符，
//如果sep=0,那么输出的是010203040506.text的空间长度至少为13个字节。
//如果间隔字符不为0,比如为':',则输出,01:02:03:04:05:06,其他字符（'-','_'）类推。
//这时text的空间长度至少为18个字节。
void CPF::mac_addr_to_a(IN const BYTE * pMacAddr,OUT ACE_TCHAR * text,
						IN ACE_TCHAR sep,IN bool upp)
{
	if( upp )
	{
		if( sep )
		{
			ACE_OS::sprintf(text, ACE_TEXT("%02X%c%02X%c%02X%c%02X%c%02X%c%02X"),
				pMacAddr[0], sep, pMacAddr[1], sep, pMacAddr[2], sep, 
				pMacAddr[3], sep, pMacAddr[4], sep, pMacAddr[5] );
		}
		else
		{
			ACE_OS::sprintf(text, ACE_TEXT("%02X%02X%02X%02X%02X%02X"),
				pMacAddr[0], pMacAddr[1], pMacAddr[2],
				pMacAddr[3], pMacAddr[4], pMacAddr[5] );			
		}
	}
	else
	{
		if( sep )
		{			
			ACE_OS::sprintf(text, ACE_TEXT("%02x%c%02x%c%02x%c%02x%c%02x%c%02x"),
				pMacAddr[0], sep, pMacAddr[1], sep, pMacAddr[2], sep, 
				pMacAddr[3], sep, pMacAddr[4], sep, pMacAddr[5] );
		}
		else
		{
			ACE_OS::sprintf(text, ACE_TEXT("%02x%02x%02x%02x%02x%02x"),
				pMacAddr[0], pMacAddr[1], pMacAddr[2],
				pMacAddr[3], pMacAddr[4], pMacAddr[5] );
		}
	}

	return;	
}

const char * CPF::mac_addr_to_a(IN const BYTE * pMacAddr,
						   IN ACE_TCHAR sep,IN bool upp)
{
	static char str_mac[32];

	str_mac[0] = 0;

	CPF::mac_addr_to_a(pMacAddr,str_mac,sep,upp);

	return str_mac;
}


//字符串到MAC地址的转换,sep表示间隔字符，如果sep=NULL，表示不需要输出间隔符号
//这个函数能够识别010203040506，返回时:*sep=0;
//也能够识别01:02:03:04:05:06，或者其它一个可见字符的间隔字符。
//如果转换成功，返回true。否则返回false
bool  CPF::mac_a_to_addr(IN const ACE_TCHAR * text,OUT BYTE * pMacAddr,OUT ACE_TCHAR * sep)
{	
	if( !text )
	{
		memset(pMacAddr,0x00,6);
		return true;
	}

	while( *text == ' ' )
		++text;

	size_t ntextlen = ACE_OS::strlen(text);

	if( ntextlen == 17 )
	{		
		char tempsep = 0;
		int intMac[6];

		int iNums = ::sscanf(text,
			"%02X%c%02X%c%02X%c%02X%c%02X%c%02X",
			intMac+0,&tempsep,intMac+1,&tempsep,intMac+2,&tempsep,
			intMac+3,&tempsep,intMac+4,&tempsep,intMac+5 );	

		if( iNums == 11 )
		{			
			for( int i = 0; i < 6; ++i )
			{
				pMacAddr[i] = (BYTE)intMac[i];
			}

			if( sep )
				*sep = tempsep;

			return true;
		}
	}
	else if( ntextlen == 12 )
	{		
		int intMac[6];

		int iNums = ::sscanf(text,
			"%02X%02X%02X%02X%02X%02X",
			intMac+0,intMac+1,intMac+2,
			intMac+3,intMac+4,intMac+5 );		

		if( iNums == 6 )
		{
			for( int i = 0; i < 6; ++i )
			{
				pMacAddr[i] = (BYTE)intMac[i];
			}

			if( sep )
				*sep = 0;	

			return true;
		}
	}

	return false;
}

//看输入的字符串是否是符合MAC地址格式
bool CPF::is_valid_mac_string(IN const char * text,OUT ACE_TCHAR * sep)
{
	while( *text == ' ' )
		++text;

	size_t ntextlen = ACE_OS::strlen(text);

	if( ntextlen == 17 )
	{		
		char tempsep = 0;
		int intMac[6];

		int iNums = ::sscanf(text,
			"%02X%c%02X%c%02X%c%02X%c%02X%c%02X",
			intMac+0,&tempsep,intMac+1,&tempsep,intMac+2,&tempsep,
			intMac+3,&tempsep,intMac+4,&tempsep,intMac+5 );	

		if( iNums == 11 )
		{			
			if( sep )
				*sep = tempsep;

			for(int i= 0; i < 6; ++i)
			{
				if( intMac[i] < 0 || intMac[i] > 255 )
				{
					return false;
				}
			}

			return true;
		}
	}
	else if( ntextlen == 12 )
	{		
		int intMac[6];

		int iNums = ::sscanf(text,
			"%02X%02X%02X%02X%02X%02X",
			intMac+0,intMac+1,intMac+2,
			intMac+3,intMac+4,intMac+5 );		

		if( iNums == 6 )
		{
			if( sep )
				*sep = 0;

			for(int i= 0; i < 6; ++i)
			{
				if( intMac[i] < 0 || intMac[i] > 255 )
				{
					return false;
				}
			}

			return true;
		}
	}

	return false;
}


BOOL CPF::TransMACStringToVector(const char * pMacs,std::vector<ACE_UINT64>& vt_macs,char sep)
{
	std::vector<std::string> vt_macstring;

	CPF::GetWords(pMacs,vt_macstring,sep);

	//去掉后面的一些字符
	for(size_t i = 0; i < vt_macstring.size(); ++i)
	{
		char * pbegin = (char *)vt_macstring[i].c_str();

		char * pdata = pbegin + vt_macstring[i].length() - 1;

		while( pdata >= pbegin )
		{
			if (*pdata == ' ' || *pdata == '\r' || *pdata == '\n' )
			{
				*pdata = 0;
				++pdata;
			}
			else
			{
				break;;
			}
		}

		vt_macstring[i] = pbegin;
	}

	BOOL bAllOK = true;

	for(size_t i = 0; i < vt_macstring.size(); ++i)
	{
		const char * pdata = vt_macstring[i].c_str();

		while( *pdata == ' ' || *pdata == '\r' || *pdata == '\n' )
		{
			++pdata;
		}

		if( strlen(pdata) == 0 )
		{
			continue;
		}

		ACE_UINT64 mac_addr = 0;
		
		if( !CPF::mac_a_to_addr(pdata,(BYTE *)&mac_addr) )
		{
			bAllOK = false;
		}
		else
		{
			vt_macs.push_back(mac_addr);
		}
	}

	return bAllOK;

}


std::string CPF::TransMACVectorToString(const std::vector<ACE_UINT64>& vt_macs,char sep)
{
	std::string str_macs;

	for(size_t i = 0; i < vt_macs.size(); ++i)
	{
		const char * mac_string = CPF::mac_addr_to_a((const BYTE *)&vt_macs[i]);

		if( i != 0 )
		{
			str_macs += sep;
		}

		str_macs += mac_string;
	}

	return str_macs;
}


//将192.168.0.1,192.168.0.6,192.168.0.10转换成vector中的整数
//vt_ips保存的是主机字节序ip
BOOL CPF::TransIPStringToVector(const char * pIPs,std::vector<ACE_UINT32>& vt_ips,char sep)
{
	BOOL bAllOK = true;

	std::vector<std::string> vt_ipstring;

	CPF::GetWords(pIPs,vt_ipstring,sep);

	for(size_t i = 0; i < vt_ipstring.size(); ++i)
	{
		const char * pdata = vt_ipstring[i].c_str();

		while( *pdata == ' ' )
		{
			++pdata;
		}

		ACE_UINT32 hip = CPF::string_to_hip(pdata);

		if( hip == 0 )
		{
			bAllOK = false;
		}

		vt_ips.push_back(hip);
	}

	return bAllOK;
}

CPF_CLASS
//和TransIPStringToVector进行相反的动作
//vt_ips保存的是主机字节序ip
std::string CPF::TransIPVectorToString(const std::vector<ACE_UINT32>& vt_ips,char sep)
{
	std::string str_ips;

	for(size_t i = 0; i < vt_ips.size(); ++i)
	{
		const char * ipstring = CPF::hip_to_string(vt_ips[i]);

		if( i != 0 )
		{
			str_ips += sep;
		}

		str_ips += ipstring;
	}

	return str_ips;
}


//将192.168.0.1,192.168.1.1-192.168.1.254,192.168.2.10-192.168.3.100
//vt_ips保存的是主机字节序ip1-ip2
BOOL CPF::TransIPRangeStringToVector(const char * pIPs,
						   std::vector< std::pair<ACE_UINT32,ACE_UINT32> >& vt_iprange,
						   char sep)
{
	vt_iprange.clear();

	std::vector<std::string> vt_ip_range_string;

	CPF::GetWords(pIPs,vt_ip_range_string,sep);

	BOOL bAllSuccess = true;

	for(size_t i = 0; i < vt_ip_range_string.size(); ++i)
	{
		if( vt_ip_range_string[i].empty() )//去掉空行
			continue;

		ACE_UINT32 begin_ip = 0;
		ACE_UINT32 end_ip = 0;

		if( ParseIPRange(vt_ip_range_string[i].c_str(),vt_ip_range_string[i].size(),
			begin_ip,end_ip) )
		{
			begin_ip = ACE_NTOHL(begin_ip);
			end_ip = ACE_NTOHL(end_ip);

			vt_iprange.push_back( std::make_pair(begin_ip,end_ip) );
		}
		else
		{
			bAllSuccess = FALSE;
		}
	}

	return (bAllSuccess);
}

CPF_CLASS
//和TransIPStringToVector进行相反的动作
std::string CPF::TransIPRangeVectorToString(
								const std::vector< std::pair<ACE_UINT32,ACE_UINT32> >& vt_iprange,
								char sep)
{
	std::string str_ips;

	for(size_t i = 0; i < vt_iprange.size(); ++i)
	{
		if( i != 0 )
		{
			str_ips += sep;
		}

		str_ips += get_iprange_string(vt_iprange[i].first,
			vt_iprange[i].second,true);
	}

	return str_ips;
}


BOOL CPF::TransPortRangeStringToVector(const char * str_port,
								  std::vector< std::pair<ACE_UINT16,ACE_UINT16> >& vt_port_range,
								  char sep)
{
	vt_port_range.clear();

	std::vector<std::string> vt_str_port;

	CPF::GetWords(str_port,vt_str_port,sep,true);

	BOOL bAllSuccess = true;

	for(size_t i = 0; i < vt_str_port.size(); ++i)
	{
		if( vt_str_port[i].empty() )//去掉空行
			continue;

		unsigned int port_begin=0,port_end=0;

		if( !CPF::ParseUintRange(vt_str_port[i].c_str(),vt_str_port[i].size(),port_begin,port_end) )
		{
			bAllSuccess = false;
			continue;
		}

		if( port_begin != 0 && port_end != 0 )
		{
			vt_port_range.push_back( std::make_pair((ACE_UINT16)port_begin,(ACE_UINT16)port_end) );
		}
		else
		{
			bAllSuccess = false;
		}
	}

	return bAllSuccess;
}

std::string CPF::TransPortRangeVectorToString(const std::vector< std::pair<ACE_UINT16,ACE_UINT16> >& vt_port_range,
									   char sep)
{
	std::string str_port;

	for(size_t i = 0; i < vt_port_range.size(); ++i)
	{
		if( !str_port.empty() )
		{
			str_port += sep;
		}

		if( vt_port_range[i].first == vt_port_range[i].second )
		{
			char buf[32]={0};
			sprintf(buf,"%u",vt_port_range[i].first);

			str_port += buf;
		}
		else
		{
			char buf[64]={0};
			sprintf(buf,"%u-%u", vt_port_range[i].first,vt_port_range[i].second);

			str_port += buf;
		}
	}

	return str_port;
}


//////////////////////////////////////////////////////////////////////////
void CPF::TransPortStringToVector(const char * str_port,
							 std::vector< ACE_UINT16 >& vt_port,
							 char sep)
{
	vt_port.clear();

	std::vector<std::string> vt_str_port;

	CPF::GetWords(str_port,vt_str_port,sep,true);

	for(size_t i = 0; i < vt_str_port.size(); ++i)
	{
		vt_port.push_back(ACE_OS::atoi(vt_str_port[i].c_str()));
	}

	return;
}


std::string CPF::TransPortVectorToString(
									const std::vector< ACE_UINT16 >& vt_port,
									char sep)
{
	std::string str_port;
	char buf[32]={0};

	for(size_t i = 0; i < vt_port.size(); ++i)
	{
		if( 0 != i )
		{
			str_port += sep;
		}
		str_port += ACE_OS::itoa(vt_port[i], buf, 10);
	}

	return str_port;
}



void CPF::TransPort32StringToVector(const char * str_port,
							   std::vector< ACE_UINT32 >& vt_port,
							   char sep)
{
	vt_port.clear();

	std::vector<std::string> vt_str_port;

	CPF::GetWords(str_port,vt_str_port,sep,true);

	for(size_t i = 0; i < vt_str_port.size(); ++i)
	{
		vt_port.push_back(ACE_OS::atoi(vt_str_port[i].c_str()));
	}

	return;
}


std::string CPF::TransPort32VectorToString(
									  const std::vector< ACE_UINT32 >& vt_port,
									  char sep)
{
	std::string str_port;
	char buf[32]={0};

	for(size_t i = 0; i < vt_port.size(); ++i)
	{
		if( 0 != i )
		{
			str_port += sep;
		}
		str_port += ACE_OS::itoa(vt_port[i], buf, 10);
	}

	return str_port;
}



int CPF::_delete_ip_in_range_vector(ACE_UINT32 comp_ip, BOOL bSplitIPRange,VET_IPADDR_RANGE& ip_range)
{
	int count = 0;

	std::vector< std::pair<ACE_UINT32,ACE_UINT32> >::iterator it = ip_range.begin();

	while( it != ip_range.end() )
	{
		if( (*it).first == (*it).second && (*it).first == comp_ip )
		{
			it = ip_range.erase(it);

			++count;
		}
		else if( bSplitIPRange )
		{
			if ((*it).first <= comp_ip && comp_ip <= (*it).second)
			{
				ACE_UINT32 old_ip_end = (*it).second;

				(*it).second  = comp_ip-1;

				it = ip_range.insert(++it,std::make_pair(comp_ip+1,old_ip_end));

				++it;//越过新的元素

				++count;
			}
			else
			{
				++it;
			}
		}
		else
		{
			++it;
		}
	}

	return count;;
}

int CPF::_delete_mac_in_vector(const BYTE* comp_mac1, const BYTE* comp_mac2, VET_MACADDR& mac_vet)
{
	int count = 0;

	std::vector< ACE_UINT64 >::iterator it = mac_vet.begin();

	while( it != mac_vet.end() )
	{
		ACE_UINT64 mac_addr = *it;

		if ( CPF::IsMacAddrEqual(comp_mac1,(void *)&mac_addr)
			|| CPF::IsMacAddrEqual(comp_mac2,(void *)&mac_addr)
			)
		{
			it = mac_vet.erase(it);

			++count;
		}
		else
		{
			++it;
		}
	}

	return count;
}
