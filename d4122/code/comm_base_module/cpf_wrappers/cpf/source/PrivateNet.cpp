//////////////////////////////////////////////////////////////////////////
//PrivateNet.h

#include "cpf/PrivateNet.h"

BOOL CPrivateNetList::AddDefaultPrivateNetRange()
{
	{
		ACE_UINT32 ipbegin = inet_addr("172.224.0.0");
		ACE_UINT32 ipend = inet_addr("172.255.255.255");
		ACE_UINT32 ipmask = inet_addr("255.255.255.0");

		AddPrivateNetRange(ipbegin,ipend,ipmask);
	}

	{
		ACE_UINT32 ipbegin = inet_addr("172.16.0.0");
		ACE_UINT32 ipend = inet_addr("172.31.255.255");
		ACE_UINT32 ipmask = inet_addr("255.255.255.0");

		AddPrivateNetRange(ipbegin,ipend,ipmask);
	}

	{
		ACE_UINT32 ipbegin = inet_addr("192.168.0.0");
		ACE_UINT32 ipend = inet_addr("192.168.255.255");
		ACE_UINT32 ipmask = inet_addr("255.255.255.0");

		AddPrivateNetRange(ipbegin,ipend,ipmask);
	}

	{
		ACE_UINT32 ipbegin = inet_addr("10.0.0.0");
		ACE_UINT32 ipend = inet_addr("10.255.255.255");
		ACE_UINT32 ipmask = inet_addr("255.255.255.0");

		AddPrivateNetRange(ipbegin,ipend,ipmask);
	}

	{
		ACE_UINT32 ipbegin = inet_addr("169.254.0.0");
		ACE_UINT32 ipend = inet_addr("169.254.255.255");
		ACE_UINT32 ipmask = inet_addr("255.255.255.0");

		AddPrivateNetRange(ipbegin,ipend,ipmask);
	}

	return true;
}

BOOL CPrivateNetList::AddPrivateNetRange(ACE_UINT32 netorder_begin_ip,ACE_UINT32 netorder_end_ip,ACE_UINT32 netorder_mask)
{
	if( find(netorder_begin_ip) )
		return false;

	if( find(netorder_end_ip) )
		return false;

	m_vtPrivateNet.push_back(CPrivateNetRange(netorder_begin_ip,netorder_end_ip,netorder_mask));

	return true;
}

void CPrivateNetList::RemoveAll()
{
	m_vtPrivateNet.clear();

	return;
}

void CPrivateNetList::SetToDefaultPrivateNetRange()
{
	RemoveAll();

	AddDefaultPrivateNetRange();

	return;
}


//////////////////////////////////////////////////////////////////////////

BOOL CPF::IsPrivateIPAddr(ACE_UINT32 hostorder_ip)
{
	return (NULL != CPrivateNetList_Singleton::instance()->find(ACE_HTONL(hostorder_ip)));

}

DIR_CS CPF::JudgeDirBy_IPv4_CONNECT_ADDR(const IPv4_CONNECT_ADDR& connect_addr,int type)
{
	DIR_CS dir = DIR_CS_UNKNOWN;

	if( type == 0 )
	{
		if( connect_addr.client.wdPort <= 1024 && connect_addr.server.wdPort > 1024 )
			dir = DIR_C2S;
		else if( connect_addr.client.wdPort > 1024 && connect_addr.server.wdPort <= 1024 )
			dir = DIR_S2C;

		BOOL client_is_private = CPF::IsPrivateIPAddr(connect_addr.client.dwIP );
		BOOL server_is_private = CPF::IsPrivateIPAddr(connect_addr.server.dwIP );

		if( client_is_private && !server_is_private )
			dir = DIR_C2S;
		else if( !client_is_private && server_is_private )
			dir = DIR_S2C;
	}
	else
	{
		BOOL client_is_private = CPF::IsPrivateIPAddr(connect_addr.client.dwIP );
		BOOL server_is_private = CPF::IsPrivateIPAddr(connect_addr.server.dwIP );

		if( client_is_private && !server_is_private )
			dir = DIR_C2S;
		else if( !client_is_private && server_is_private )
			dir = DIR_S2C;

		if( connect_addr.client.wdPort <= 1024 && connect_addr.server.wdPort > 1024 )
			dir = DIR_C2S;
		else if( connect_addr.client.wdPort > 1024 && connect_addr.server.wdPort <= 1024 )
			dir = DIR_S2C;
	}

	return dir;
}

DIR_CS CPF::JudgeDirBy_IPv4_CONNECT_ADDR_Ex(const IPv4_CONNECT_ADDR& connect_addr,int type)
{
	DIR_CS dir = JudgeDirBy_IPv4_CONNECT_ADDR(connect_addr,type);

	if( dir == DIR_CS_UNKNOWN )
	{
		if(connect_addr.client.wdPort <= connect_addr.server.wdPort )
			dir = DIR_C2S;
		else
			dir = DIR_S2C;
	}

	return dir;
}

