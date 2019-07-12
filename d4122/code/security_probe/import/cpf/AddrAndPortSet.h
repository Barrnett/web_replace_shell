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
	int				m_inverse;//����ȡ���������ų�.�����ǽ�ֹ��ȱʡ��0.

public:
	std::string		m_str_desc;

	CVtIPAndMac		m_addr;

	BOOL			m_bAllAPP;

	PORT_RANGE_VET	m_tcp_port_range;
	PORT_RANGE_VET	m_udp_port_range;

	std::vector<ACE_UINT32>	m_vt_trans_type;//����tcp��udp֮��Ĵ����Э��

public:
	//���ip�����ϣ�����-1�����ip���ϣ����Ҷ˿ڷ��ϣ�����1.���ip���ϣ��˿ڲ����ϣ�����0
	//proto_type = CCommonIPv4Decode::INDEX_PID_IPv4_TCP,CCommonIPv4Decode::INDEX_PID_IPv4_UDP��
	//����0����1����ʾû��IP��ַ�Ͷ˿��Ƿ����
	int is_in(ACE_UINT32 comp_ip, ACE_UINT16 comp_port, ACE_UINT32 proto_type) const;

	//���ip�����ϣ�����-1�����ip���ϣ����Ҷ˿ڷ��ϣ�����1.���ip���ϣ��˿ڲ����ϣ�����0
	//proto_type = CCommonIPv4Decode::INDEX_PID_IPv4_TCP,CCommonIPv4Decode::INDEX_PID_IPv4_UDP��
	//����-1����ʾû�е�ַ��ƥ��.����1����ʾpass������0��ʾ����
	//���inverse�������ǽ�ֹ�������溯��������
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

	//���inverse�������ǽ�ֹ�������溯��������
	//����-1����ʾû�е�ַ��ƥ��.����1����ʾpass������0��ʾ����
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