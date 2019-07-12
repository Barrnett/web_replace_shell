//////////////////////////////////////////////////////////////////////////
//AddrAndPortSet.h

#pragma once

#include "cpf/VtIPAndMac.h"
#include "cpf/addr_tools.h"
#include "cpf/TinyXmlEx.h"

class CPF_CLASS CAddrAndPortSet
{
public:
	CAddrAndPortSet(void);
	~CAddrAndPortSet(void);

public:
	void close();

public:
	int operator==(const CAddrAndPortSet& other) const;
	CAddrAndPortSet& operator=(const CAddrAndPortSet& other);

	void Save(CTinyXmlEx& xml_writter, TiXmlNode* pParentNode) const;

	void Load_from_parent_node(CTinyXmlEx& xml_reader, TiXmlNode* pParentNode);

	void Load_from_this_node(CTinyXmlEx& xml_reader, TiXmlNode* pParentNode);

public:
	int				m_inverse;//条件取反或者是排除.或者是禁止。缺省是0.

public:
	std::string		m_str_desc;

	CVtIPAndMac		m_addr;

	BOOL			m_bAllAPP;

	PORT_RANGE_VET	m_tcp_port_range;
	PORT_RANGE_VET	m_udp_port_range;

	std::vector<ACE_UINT32>	m_vt_trans_type;//除了tcp和udp之外的传输层协议

public:
	//如果ip不符合，返回-1。如果ip符合，并且端口符合，返回1.如果ip符合，端口不符合，返回0
	//proto_type = CCommonIPv4Decode::INDEX_PID_IPv4_TCP,CCommonIPv4Decode::INDEX_PID_IPv4_UDP等
	//返回0或者1，表示没有IP地址和端口是否符合
	int is_in(ACE_UINT32 comp_ip, ACE_UINT16 comp_port, ACE_UINT32 proto_type) const;

	//如果ip不符合，返回-1。如果ip符合，并且端口符合，返回1.如果ip符合，端口不符合，返回0
	//proto_type = CCommonIPv4Decode::INDEX_PID_IPv4_TCP,CCommonIPv4Decode::INDEX_PID_IPv4_UDP等
	//返回-1，表示没有地址是匹配.返回1，表示pass，返回0表示拦截
	//如果inverse的意义是禁止，则下面函数有意义
	inline int is_passed(ACE_UINT32 comp_ip, ACE_UINT16 comp_port, ACE_UINT32 proto_type) const
	{
		int b_is_in = is_in(comp_ip,comp_port,proto_type);

		if( b_is_in )
		{
			return !m_inverse;
		}

		return -1;
	}

private:
	void Save_port(CTinyXmlEx& xml_writter, TiXmlNode* pParentNode,const PORT_RANGE_VET& port_vet, const char* str_item) const;
	void Load_port(CTinyXmlEx& xml_reader, TiXmlNode* pThisNode, PORT_RANGE_VET& port_vet, const char* str_item);

};

class CPF_CLASS CAddrAndPortSetVector
{
public:
	CAddrAndPortSetVector();
	~CAddrAndPortSetVector();

public:
	std::vector<CAddrAndPortSet>	m_vt_addr_port_set;

public:
	void close();

	//如果inverse的意义是禁止，则下面函数有意义
	//返回-1，表示没有地址是匹配.返回1，表示pass，返回0表示拦截
	inline int is_passed(ACE_UINT32 comp_ip, ACE_UINT16 comp_port, ACE_UINT32 proto_type) const
	{
		for(size_t i = 0; i < m_vt_addr_port_set.size(); ++i)
		{
			int fit_value = m_vt_addr_port_set[i].is_passed(comp_ip,comp_port,proto_type);

			if( fit_value == -1 )
			{
				continue;
			}

			return fit_value;
		}

		return -1;
	}

public:
	int operator==(const CAddrAndPortSetVector& other) const;
	CAddrAndPortSetVector& operator=(const CAddrAndPortSetVector& other);

	void Save(CTinyXmlEx& xml_writter, TiXmlNode* pParentNode) const;
	void Load(CTinyXmlEx& xml_reader, TiXmlNode* pParentNode);

};