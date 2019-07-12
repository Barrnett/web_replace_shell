//////////////////////////////////////////////////////////////////////////
//InetAddress.cpp


#include "cpf/InetAddress.h"

InetAddress::InetAddress (void)
{ 
	memset(address_buffer, 0, sizeof(unsigned char)*IPLEN);
	format_output();	
}

InetAddress::InetAddress(DWORD dwNetOrderIP)
{
	*(DWORD *)address_buffer = dwNetOrderIP;

	format_output();
}

InetAddress::InetAddress(const char *address)
{
	memset(address_buffer, 0, sizeof(unsigned char)*IPLEN);
	// parse_address initializes valid, address_buffer & iv_friendly_name
	parse_address(address);
	format_output();
}

InetAddress::InetAddress(const InetAddress &inetaddr)
{
	memcpy(address_buffer, inetaddr.address_buffer, IPLEN);    // copy the address data
	format_output();
}

InetAddress::~InetAddress()
{}

InetAddress &InetAddress::operator=(const InetAddress &rhs)
{
	if( &rhs==this ) { return *this; };

	memcpy(address_buffer, rhs.address_buffer, IPLEN);
	format_output();
	return *this;
}

InetAddress& InetAddress::operator++()
{
	ACE_UINT32 ip = ACE_NTOHL(*(ACE_UINT32 *)address_buffer);

	if( ip == ACE_UINT32(-1) )
		return *this;

	ip++;

	*(ACE_UINT32 *)address_buffer = ACE_HTONL(ip);

	format_output();
	return (*this);
}

int operator< (const InetAddress &lhs, const InetAddress &rhs)
{
	ACE_UINT32 ip1 = ACE_NTOHL(*(ACE_UINT32 *)lhs.address_buffer);
	ACE_UINT32 ip2 = ACE_NTOHL(*(ACE_UINT32 *)rhs.address_buffer);

	return ip1<ip2;
	
}

int operator<= (const InetAddress &lhs, const InetAddress &rhs)
{
	ACE_UINT32 ip1 = ACE_NTOHL(*(ACE_UINT32 *)lhs.address_buffer);
	ACE_UINT32 ip2 = ACE_NTOHL(*(ACE_UINT32 *)rhs.address_buffer);

	return ip1<=ip2;

}

int operator> (const InetAddress &lhs, const InetAddress &rhs)
{
	ACE_UINT32 ip1 = ACE_NTOHL(*(ACE_UINT32 *)lhs.address_buffer);
	ACE_UINT32 ip2 = ACE_NTOHL(*(ACE_UINT32 *)rhs.address_buffer);

	return ip1>ip2;

}

int operator>= (const InetAddress &lhs, const InetAddress &rhs)
{
	ACE_UINT32 ip1 = ACE_NTOHL(*(ACE_UINT32 *)lhs.address_buffer);
	ACE_UINT32 ip2 = ACE_NTOHL(*(ACE_UINT32 *)rhs.address_buffer);

	return ip1>=ip2;

}


int operator== (const InetAddress &lhs, const InetAddress &rhs)
{
	return (*(ACE_UINT32 *)lhs.address_buffer == *(ACE_UINT32 *)rhs.address_buffer);
}

int operator!= (const InetAddress &lhs, const InetAddress &rhs)
{
	return (*(ACE_UINT32 *)lhs.address_buffer != *(ACE_UINT32 *)rhs.address_buffer);
}

UINT operator-(const InetAddress &lhs, const InetAddress &rhs)
{
	ACE_UINT32 ip1 = ACE_NTOHL(*(ACE_UINT32 *)lhs.address_buffer);
	ACE_UINT32 ip2 = ACE_NTOHL(*(ACE_UINT32 *)rhs.address_buffer);

	return ip1-ip2;
}

InetAddress operator & (const InetAddress &lhs, const InetAddress &rhs)
{
	InetAddress ret;

	ret.address_buffer[0] = lhs.address_buffer[0] & rhs.address_buffer[0];
	ret.address_buffer[1] = lhs.address_buffer[1] & rhs.address_buffer[1];
	ret.address_buffer[2] = lhs.address_buffer[2] & rhs.address_buffer[2];
	ret.address_buffer[3] = lhs.address_buffer[3] & rhs.address_buffer[3];
	ret.format_output();
	return ret;
}

InetAddress GetBroadcast (const InetAddress &net, const InetAddress &mask)
{
	InetAddress ret;

	ret.address_buffer[0] = net.address_buffer[0] | ~mask.address_buffer[0];
	ret.address_buffer[1] = net.address_buffer[1] | ~mask.address_buffer[1];
	ret.address_buffer[2] = net.address_buffer[2] | ~mask.address_buffer[2];
	ret.address_buffer[3] = net.address_buffer[3] | ~mask.address_buffer[3];
	ret.format_output();
	return ret;
}

//-----[ IP CAddress parse CAddress ]---------------------------------
bool InetAddress::parse_address( const char *inaddr)
{
	if ( !inaddr || (strlen( inaddr) > 18)) return false;

	unsigned int addr0, addr1, addr2, addr3;
	if ( 4!=sscanf(inaddr, "%u.%u.%u.%u", &addr0, &addr1, &addr2, &addr3) )	
		return false;

	address_buffer[0] = addr0;	
	address_buffer[1] = addr1;	
	address_buffer[2] = addr2;	
	address_buffer[3] = addr3;	

	return true;
}

//----[ IP address format output ]------------------------------------
void InetAddress::format_output()	{
	sprintf( (char *) output_buffer,"%d.%d.%d.%d",address_buffer[0],
		address_buffer[1], address_buffer[2], address_buffer[3]);
}

const char * InetAddress::get_printable() const
{  return (const char *)output_buffer;	};
