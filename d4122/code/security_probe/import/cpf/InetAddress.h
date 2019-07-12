//////////////////////////////////////////////////////////////////////////
//InetAddress.h

#pragma once

#include "cpf/cpf.h"

class InetAddress;

CPF_CLASS int operator==(const InetAddress &rhs, const InetAddress &dhs);
CPF_CLASS int operator!=(const InetAddress &rhs, const InetAddress &dhs);
CPF_CLASS int operator<(const InetAddress &lhs, const InetAddress &rhs);
CPF_CLASS int operator<=(const InetAddress &lhs, const InetAddress &rhs);
CPF_CLASS int operator>(const InetAddress &lhs, const InetAddress &rhs);
CPF_CLASS int operator>=(const InetAddress &lhs, const InetAddress &rhs);
CPF_CLASS UINT operator-(const InetAddress &lhs, const InetAddress &rhs);

class CPF_CLASS InetAddress 
{
public:
	enum{OUTBUFF=16,IPLEN=4};

public:
	InetAddress();
	InetAddress(DWORD dwNetOrderIP);
	InetAddress(const char *address);
	InetAddress(const InetAddress &rhs);
	~InetAddress();

	ACE_UINT32 GetNetOrderIP() const{ return *(ACE_UINT32 *)address_buffer; }
	ACE_UINT32 GetHostOrderIP() const{ return ACE_NTOHL(GetNetOrderIP()); }

	InetAddress &operator=(const InetAddress &rhs);
	//InetAddress &operator=(DWORD dwNetOrderIP);
	InetAddress& operator++ ();    // prefix ++
	friend CPF_CLASS int operator==(const InetAddress &rhs, const InetAddress &dhs);
	friend CPF_CLASS int operator!=(const InetAddress &rhs, const InetAddress &dhs);
	friend CPF_CLASS int operator<(const InetAddress &lhs, const InetAddress &rhs);
	friend CPF_CLASS int operator<=(const InetAddress &lhs, const InetAddress &rhs);
	friend CPF_CLASS int operator>(const InetAddress &lhs, const InetAddress &rhs);
	friend CPF_CLASS int operator>=(const InetAddress &lhs, const InetAddress &rhs);
	friend CPF_CLASS UINT operator-(const InetAddress &lhs, const InetAddress &rhs);
	friend CPF_CLASS InetAddress operator & ( const InetAddress &lhs,const InetAddress &rhs);
	friend CPF_CLASS InetAddress GetBroadcast( const InetAddress &net,const InetAddress &mask);

	const char *get_printable() const;  // get a printable ASCII value

private:
	void format_output();
	bool parse_address( const char *inaddr);

	char output_buffer[OUTBUFF];           // output buffer
	unsigned char address_buffer[IPLEN]; // internal representation
};



