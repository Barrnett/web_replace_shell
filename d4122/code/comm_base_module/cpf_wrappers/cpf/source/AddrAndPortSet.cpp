//////////////////////////////////////////////////////////////////////////
//AddrAndPortSet.cpp

#include "cpf/AddrAndPortSet.h"
#include "cpf/addr_tools.h"
#include "cpf/CommonIPv4Decode.h"

//////////////////////////////////////////////////////////////////////////
//CAddrAndPortSet
//////////////////////////////////////////////////////////////////////////

CAddrAndPortSet::CAddrAndPortSet(void)
{
	close();

	return;
}

CAddrAndPortSet::~CAddrAndPortSet(void)
{
}


void CAddrAndPortSet::close()
{
	m_inverse = 0;

	m_str_desc.clear();

	m_addr.close();

	m_bAllAPP = true;

	m_tcp_port_range.clear();
	m_udp_port_range.clear();

	m_vt_trans_type.clear();

	return;
}

int CAddrAndPortSet::operator==(const CAddrAndPortSet& other) const
{
	return (
		m_inverse == other.m_inverse
		&& m_str_desc == other.m_str_desc
		&& m_addr == other.m_addr
		&& m_bAllAPP == other.m_bAllAPP
		&& m_tcp_port_range == other.m_tcp_port_range
		&& m_udp_port_range == other.m_udp_port_range
		&& m_vt_trans_type == other.m_vt_trans_type);

}
	
CAddrAndPortSet& CAddrAndPortSet::operator=(const CAddrAndPortSet& other)
{
	if( this != &other )
	{
		m_inverse = other.m_inverse;

		m_str_desc = other.m_str_desc;

		m_addr = other.m_addr;
		m_bAllAPP = other.m_bAllAPP;
		
		m_tcp_port_range = other.m_tcp_port_range;
		m_udp_port_range = other.m_udp_port_range;
		
		m_vt_trans_type = other.m_vt_trans_type;
	}

	return *this;
}

int CAddrAndPortSet::is_in(ACE_UINT32 comp_ip, ACE_UINT16 comp_port, ACE_UINT32 proto_type) const
{
	bool b_find_ip = CPF::_find_ip_in_range_vector(comp_ip, m_addr.vt_ip);
	
	if( !b_find_ip )
	{
		return false;
	}

	if( !m_bAllAPP )
	{
		bool b_find_port = false;

		if (CCommonIPv4Decode::INDEX_PID_IPv4_TCP == proto_type)
		{
			b_find_port = CPF::_find_port_in_vector(comp_port, m_tcp_port_range);
		}
		else if (CCommonIPv4Decode::INDEX_PID_IPv4_UDP == proto_type)
		{
			b_find_port = CPF::_find_port_in_vector(comp_port, m_udp_port_range);
		}
		else
		{
			b_find_port = CPF::find(m_vt_trans_type,proto_type);
		}

		if( !b_find_port)
		{
			return false;
		}
	}

	return true;
}


void CAddrAndPortSet::Save(CTinyXmlEx& xml_writter, TiXmlNode* pParentNode) const
{
	TiXmlNode * addr_port_node = xml_writter.InsertEndChildElement(pParentNode, "addr_port");

	if( !addr_port_node )
	{
		return;
	}

	xml_writter.SetAttr(addr_port_node,"inverse",m_inverse);

	xml_writter.SetAttr(addr_port_node,"desc",m_str_desc.c_str());

	m_addr.Write(xml_writter,addr_port_node);

	xml_writter.SetValueLikeIni(addr_port_node, "all_app", m_bAllAPP);

	Save_port(xml_writter, addr_port_node, m_tcp_port_range, "tcp_port");

	Save_port(xml_writter, addr_port_node, m_udp_port_range, "udp_port");

	TiXmlNode * other_trans_node = xml_writter.InsertEndChildElement(addr_port_node,"other_trans");

	if( other_trans_node )
	{
		xml_writter.SetValue_Uint32Vector(other_trans_node,m_vt_trans_type,"item");
	}

	return;
}

void CAddrAndPortSet::Load_from_parent_node(CTinyXmlEx& xml_reader, TiXmlNode* pParentNode)
{
	TiXmlNode * addr_port_node = xml_reader.FirstChildElement(pParentNode, "addr_port");

	if( !addr_port_node )
	{
		return;
	}

	Load_from_this_node(xml_reader,addr_port_node);

	return;
}

void CAddrAndPortSet::Load_from_this_node(CTinyXmlEx& xml_reader, TiXmlNode* addr_port_node)
{
	close();

	xml_reader.GetAttr(addr_port_node,"inverse",m_inverse);

	xml_reader.GetAttr(addr_port_node,"desc",m_str_desc);

	m_addr.Read(xml_reader,addr_port_node);

	xml_reader.GetValueLikeIni(addr_port_node, "all_app", m_bAllAPP);

	Load_port(xml_reader, addr_port_node, m_tcp_port_range, "tcp_port");
	Load_port(xml_reader, addr_port_node, m_udp_port_range, "udp_port");

	TiXmlNode * other_trans_node = xml_reader.FirstChildElement(addr_port_node,"other_trans");

	if( other_trans_node )
	{
		xml_reader.GetValue_Uint32Vector(other_trans_node,m_vt_trans_type,"item");
	}

	return;
}


void CAddrAndPortSet::Save_port(CTinyXmlEx& xml_writter, TiXmlNode* pParentNode,const PORT_RANGE_VET& port_vet, const char* str_item) const
{
	if (0 == port_vet.size())
	{
		return;
	}

	TiXmlNode * port_node = xml_writter.InsertEndChildElement(pParentNode, str_item);
	ACE_ASSERT(port_node);

	xml_writter.SetValue_Uint16RangeVector(port_node, port_vet, "item");

	return;
}

void CAddrAndPortSet::Load_port(CTinyXmlEx& xml_reader, TiXmlNode* pThisNode, PORT_RANGE_VET& port_vet, const char* str_item)
{
	port_vet.clear();

	TiXmlNode * item_node = pThisNode->FirstChildElement(str_item);
	if (!item_node)
		return;

	xml_reader.GetValue_Uint16RangeVector(item_node, port_vet, "item");

	return;
}


//////////////////////////////////////////////////////////////////////////
//CAddrAndPortSetVector
//////////////////////////////////////////////////////////////////////////


CAddrAndPortSetVector::CAddrAndPortSetVector()
{
}

CAddrAndPortSetVector::~CAddrAndPortSetVector()
{

}

void CAddrAndPortSetVector::close()
{
	m_vt_addr_port_set.clear();

	return;
}

int CAddrAndPortSetVector::operator==(const CAddrAndPortSetVector& other) const
{
	return (m_vt_addr_port_set == other.m_vt_addr_port_set);
}

CAddrAndPortSetVector& CAddrAndPortSetVector::operator=(const CAddrAndPortSetVector& other)
{
	if( this != &other )
	{
		this->m_vt_addr_port_set = other.m_vt_addr_port_set;
	}

	return *this;
}

void CAddrAndPortSetVector::Save(CTinyXmlEx& xml_writter, TiXmlNode* pParentNode) const
{
	TiXmlNode * addr_port_list_node = xml_writter.InsertEndChildElement(pParentNode, "addr_port_list");

	if( !addr_port_list_node )
	{
		return;
	}

	for(size_t i = 0; i < m_vt_addr_port_set.size(); ++i)
	{
		m_vt_addr_port_set[i].Save(xml_writter,addr_port_list_node);
	}

	return;
}

void CAddrAndPortSetVector::Load(CTinyXmlEx& xml_reader, TiXmlNode* pParentNode)
{
	close();

	TiXmlNode * addr_port_list_node = xml_reader.FirstChildElement(pParentNode, "addr_port_list");

	if( !addr_port_list_node )
	{
		return;
	}

	for(TiXmlNode * addr_port_node = xml_reader.FirstChildElement(addr_port_list_node, "addr_port");
		addr_port_node;
		addr_port_node = xml_reader.NextSiblingElement(addr_port_node, "addr_port"))
	{
		CAddrAndPortSet one_set;

		one_set.Load_from_this_node(xml_reader,addr_port_node);

		m_vt_addr_port_set.push_back(one_set);
	}

	return;
}

