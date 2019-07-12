//////////////////////////////////////////////////////////////////////////
//TIPHashFinderEx.h

#ifndef _TIP_HASHFINDER_EX_H_2006_07_24
#define _TIP_HASHFINDER_EX_H_2006_07_24
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "THashFinderEx.h"
#include "cpf/addr_tools.h"

template<typename ValueType>
struct IPv4_ADDR_HashAndCmp
{
	int Hash(const ACE_UINT32& addr) const
	{
		return CPF_HASH_HO_IPv4(addr);
	}

	int Cmp(const ValueType& value,const ACE_UINT32& addr) const
	{
		return ((value.GetIPAddr()==addr));
	}
};


template<typename ValueType>
struct IPv4_TRANSPORT_ADDR_HashAndCmp
{
	int Hash(const IPv4_TRANSPORT_ADDR& addr) const
	{
		return CPF_HASH_IPv4_TRANSPORT_ADDR(addr);
	}

	int Cmp(const ValueType& value,const IPv4_TRANSPORT_ADDR& addr) const
	{
		return (value.GetIPTransportAddr()==addr);
	}
};

template<typename ValueType>
struct IPv4_CONNECT_ADDR_HashAndCmp
{
	int Hash(const IPv4_CONNECT_ADDR& addr) const
	{
		return CPF_HASH_HO_ADDR(addr.server.dwIP,addr.client.dwIP,addr.server.wdPort,addr.client.wdPort);
	}

	int Cmp(const ValueType& value,const IPv4_CONNECT_ADDR& addr) const
	{
		return addr.Cmp(value.GetIPConnectAddr());
	}
};


template<typename ValueType>
class CIPv4_ADDR_HashFinderEx : public CTHashFinderEx< ValueType,ACE_UINT32,IPv4_ADDR_HashAndCmp<ValueType> >
{

};


template<typename ValueType>
class CIPv4_TRANSPORT_ADDR_HashFinderEx : public CTHashFinderEx< ValueType,IPv4_TRANSPORT_ADDR,IPv4_TRANSPORT_ADDR_HashAndCmp<ValueType> >
{

};

template<typename ValueType>
class CIPv4_CONNECT_ADDR_HashFinderEx : public CTHashFinderEx< ValueType,IPv4_CONNECT_ADDR,IPv4_CONNECT_ADDR_HashAndCmp<ValueType> >
{

};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<typename ValueType>
struct String_HashAndCmp
{
	int Hash(const LPACETSTR& datastring) const
	{
		return CPF_HASH_STRING(datastring);
	}

	int Cmp(const ValueType& value,const LPACETSTR& datastring) const
	{
		return (ACE_OS::strcmp(value.GetString(),datastring)==0);
	}
};

template<typename ValueType>
class CString_HashFinderEx : public CTHashFinderEx< ValueType,LPACETSTR,String_HashAndCmp<ValueType> >
{

};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//#define _TIP_HASHFINDER_EX_H_2006_07_24