//////////////////////////////////////////////////////////////////////////
//CPF_SimpleFltCondition.h

#pragma once


#include "cpf/cpf.h"
#include "cpf/TimeStamp32.h"
#include "cpf/other_tools.h"
#include "cpf/cpf_std.h"
#include <vector>
#include <algorithm>


typedef enum
{
	flt_dir_uncare = 0,
	flt_addr1_to_addr2 = 1,
	flt_addr2_to_addr1 = -1,

}CPF_FILTER_DIR;


typedef struct tagSIMPLE_IP_FILTER
{
	tagSIMPLE_IP_FILTER()
	{
		bNot = false;

		dir = flt_dir_uncare;

		bNotInRangeIP1 = false;
		bNotInRangeIP2 = false;
		bNotInProto = false;
	}

	void Close()
	{
		bNot = false;

		dir = flt_dir_uncare;

		bNotInRangeIP1 = false;
		bNotInRangeIP2 = false;
		bNotInProto = false;

		vt_ip1.clear();
		vt_ip2.clear();

		vt_upperproto.clear();
	}

	tagSIMPLE_IP_FILTER& operator =(const tagSIMPLE_IP_FILTER& other)
	{
		if( this != &other )
		{
			bNot = other.bNot;

			dir = other.dir;

			vt_ip1 = other.vt_ip1;
			bNotInRangeIP1 = other.bNotInRangeIP1;

			vt_ip2 = other.vt_ip2;
			bNotInRangeIP2 = other.bNotInRangeIP2;

			vt_upperproto = other.vt_upperproto;
			bNotInProto = other.bNotInProto;
		}

		return *this;
	}

	int operator == (const tagSIMPLE_IP_FILTER& other) const
	{
		return( bNot == other.bNot
			&& dir == other.dir
			&& vt_ip1 == other.vt_ip1
			&& bNotInRangeIP1 == other.bNotInRangeIP1
			&& vt_ip2 == other.vt_ip2
			&& bNotInRangeIP2 == other.bNotInRangeIP2
			&& vt_upperproto == other.vt_upperproto
			&& bNotInProto == other.bNotInProto );
	}

	int operator != (const tagSIMPLE_IP_FILTER& other) const
	{
		return (!(*this == other));
	}

	inline bool IsEmpty() const
	{
		return (vt_ip1.size()==0&&vt_ip2.size()==0&&vt_upperproto.size()==0);
	}

	//输入主机字节序的IP地址
	inline bool Compare(ACE_UINT32 ipSrc,ACE_UINT32 ipDst, int nProtoIndex ) const
	{
		BOOL bIn = ( InnerIPCompare(ipSrc,ipDst)
			&&CPF::flt_find<int>(vt_upperproto,(const int&)nProtoIndex,bNotInProto) );

		return (bNot?(!bIn):bIn);
	}

	//输入主机字节序的IP地址
	inline bool InnerIPCompare(ACE_UINT32 ipSrc,ACE_UINT32 ipDst) const
	{
		return CPF::flt_range_find_two_side(
			(int)dir,
			vt_ip1,ipSrc,bNotInRangeIP1,
			vt_ip2,ipDst,bNotInRangeIP2);
	}

public:
	BOOL		bNot;

	CPF_FILTER_DIR	dir;

	//first,second分别表示起始和结束的IP地址(主机字节序)
	std::vector< std::pair<ACE_UINT32,ACE_UINT32> >	vt_ip1;
	BOOL	bNotInRangeIP1;

	//first,second分别表示起始和结束的IP地址(主机字节序)
	std::vector< std::pair<ACE_UINT32,ACE_UINT32> >	vt_ip2;
	BOOL	bNotInRangeIP2;

	std::vector<int>		vt_upperproto;
	BOOL	bNotInProto;

}SIMPLE_IP_FILTER;

typedef struct tagSIMPLE_PORT_FILTER
{
	tagSIMPLE_PORT_FILTER()
	{
		bNot = false;
		dir = flt_dir_uncare;

		m_bNotInPort1 = false;
		m_bNotInPort2 = false;
	}  

	tagSIMPLE_PORT_FILTER& operator =(const tagSIMPLE_PORT_FILTER& other)
	{
		if( this != &other )
		{
			bNot = other.bNot;

			dir = other.dir;

			vt_port1 = other.vt_port1;
			m_bNotInPort1 = other.m_bNotInPort1;

			vt_port2 = other.vt_port2;
			m_bNotInPort2 = other.m_bNotInPort2;
		}

		return *this;
	}

	int operator == (const tagSIMPLE_PORT_FILTER& other) const
	{
		return( bNot == other.bNot
			&& dir == other.dir
			&& vt_port1 == other.vt_port1
			&& m_bNotInPort1 == other.m_bNotInPort1
			&& vt_port2 == other.vt_port2
			&& m_bNotInPort2 == other.m_bNotInPort2);
	}

	inline bool IsEmpty() const
	{
		return (vt_port1.size()==0&&vt_port2.size()==0);
	}

	inline bool Compare(ACE_UINT16 portsrc,ACE_UINT16 portdst) const
	{
		BOOL bIn = InnerPortCompare(portsrc,portdst);

		return (bNot?(!bIn):bIn);
	}


	inline bool InnerPortCompare(ACE_UINT16 portsrc,ACE_UINT16 portdst) const
	{
		return CPF::flt_range_find_two_side(
			(int)dir,
			vt_port1,portsrc,m_bNotInPort1,
			vt_port2,portdst,m_bNotInPort2);
	}

	BOOL		bNot;

	CPF_FILTER_DIR	dir;

	//first,second分别表示起始和结束的IP端口(主机字节序)
	std::vector< std::pair<ACE_UINT16,ACE_UINT16> >	vt_port1;
	BOOL	m_bNotInPort1;

	//first,second分别表示起始和结束的IP端口(主机字节序)
	std::vector< std::pair<ACE_UINT16,ACE_UINT16> >	vt_port2;
	BOOL	m_bNotInPort2;

}SIMPLE_PORT_FILTER;


typedef struct  tagCPF_ONE_FILTER
{
	tagCPF_ONE_FILTER()
	{
		bNot = false;
	}

	tagCPF_ONE_FILTER& operator =(const tagCPF_ONE_FILTER& other)
	{
		if( this != &other )
		{
			name = other.name;

			bNot = other.bNot;

			ip_flt = other.ip_flt;
			port_flt = other.port_flt;
		}

		return *this;
	}

	int operator == (const tagCPF_ONE_FILTER& other) const
	{
		return (name == other.name
			&& bNot == other.bNot
			&& ip_flt == other.ip_flt
			&& port_flt == other.port_flt );

	}

	BOOL IsEmpty() const
	{
		return ( ip_flt.IsEmpty()
			&& port_flt.IsEmpty());
	}

	std::string				name;
	BOOL					bNot;
	
	//ip条件和port条件是与关系

	SIMPLE_IP_FILTER		ip_flt;

	SIMPLE_PORT_FILTER		port_flt;

}CPF_ONE_FILTER;	

class CPF_CLASS CPF_SimpleFltCondition
{
public:
	CPF_SimpleFltCondition()
	{
		m_bIPFilter = false;
		m_bPortFilter = false;
	}

	~CPF_SimpleFltCondition()
	{

	}

	CPF_SimpleFltCondition& operator =(const CPF_SimpleFltCondition& other)
	{
		if( this != &other )
		{
			m_vt_filter = other.m_vt_filter;

			m_bIPFilter = other.m_bIPFilter;
			m_bPortFilter = other.m_bPortFilter;
		}

		return *this;
	}

	int operator == (const CPF_SimpleFltCondition& other) const
	{
		if( m_vt_filter.size() != other.m_vt_filter.size() )
			return  0;

		for(size_t i = 0; i < m_vt_filter.size(); ++i)
		{
			if( !(m_vt_filter[i] == other.m_vt_filter[i]) )
			{
				return false;
			}				
		}

		return true;
	}

public:
	void close();

public:
	void cal_flt();

	BOOL IsEmpty() const
	{
		return m_vt_filter.size() == 0;
	}

	const CPF_ONE_FILTER * find(const char * name) const;

	CPF_ONE_FILTER * find(const char * name);

public:
	inline BOOL IsHaveIPFilter(int index) const
	{
		return (!m_vt_filter[index].ip_flt.IsEmpty());
	}

	inline BOOL IsHavePortFilter(int index) const
	{	
		return (!m_vt_filter[index].port_flt.IsEmpty());
	}

	inline BOOL IsHaveIPFilter() const
	{
		return m_bIPFilter;
	}

	inline BOOL IsHavePortFilter() const
	{	
		return m_bPortFilter;
	}

public:
	//多个过滤条件是或关系
	std::vector<CPF_ONE_FILTER>	m_vt_filter;

private:
	BOOL	m_bIPFilter;//ip是否有过滤器
	BOOL	m_bPortFilter;//端口是否有过滤器


};


