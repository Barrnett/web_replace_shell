//////////////////////////////////////////////////////////////////////////
//VtIPAndMac.h

#pragma once

#include "cpf/ostypedef.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/cpf_std.h"

class CPF_CLASS CVtIPAndMac
{
public:
	CVtIPAndMac(void){}
	~CVtIPAndMac(void){}

public:
	inline void close()
	{
		vt_mac.clear();
		vt_ip.clear();

		return;
	}

	inline BOOL IsEmpty() const
	{
		return (vt_ip.size()==0&&vt_mac.size()==0);
	}

public:
	std::vector<ACE_UINT64>								vt_mac;
	std::vector< std::pair<ACE_UINT32,ACE_UINT32> >		vt_ip;


public:
	CVtIPAndMac& operator = (const CVtIPAndMac& other)
	{
		vt_mac = other.vt_mac;
		vt_ip = other.vt_ip;

		return *this;
	}

	int operator == (const CVtIPAndMac& other) const
	{
		return (
			vt_mac == other.vt_mac
			&& vt_ip == other.vt_ip);
	}

	int operator != (const CVtIPAndMac& other) const
	{
		return !(*this == other);
	}

	inline BOOL IsIn(ACE_UINT32 ipaddr) const
	{
		return ( CPF::range_find(vt_ip,ipaddr) );
	}

	inline BOOL IsIn(const BYTE * mac_addr) const
	{
		ACE_UINT64 int_mac = 0;
		memcpy(&int_mac,mac_addr,6);

		return( CPF::find(vt_mac,int_mac) );
	}

	inline BOOL IsIn(ACE_UINT32 ipaddr,const BYTE * mac_addr) const
	{
		return (IsIn(ipaddr)||IsIn(mac_addr));
	}

public:
	void Read(CTinyXmlEx& xml_reader,TiXmlNode * parent_node);

	void Write(CTinyXmlEx& xml_writter,TiXmlNode * parent_node) const;

	int WriteStream(char *& pdate) const;

	int ReadStream(const char *pdata);

public:
	std::string CVtIPAndMac::GetAddrDesc(BOOL bEmptyIsAll) const;
	void GetFromString(const char * str_desc);
};
