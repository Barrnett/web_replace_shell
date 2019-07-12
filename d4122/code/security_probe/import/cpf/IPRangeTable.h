//////////////////////////////////////////////////////////////////////////
//IPRangeTable.h
//利用二分法将IP地址段进行排序和查找。

#ifndef _IP_RANGE_TABLE_H_2006_05_18
#define _IP_RANGE_TABLE_H_2006_05_18
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4786)

#include "cpf/ostypedef.h"
#include <algorithm>
#include <functional>
#include <vector>


//first表示小地址，second表示大地址
//地址是主机字节序
struct IP_RANGE_PAIR
{
	std::pair<ACE_UINT32,ACE_UINT32>	ip_range;

	IP_RANGE_PAIR()
	{

	}

	IP_RANGE_PAIR(const IP_RANGE_PAIR& other)
	{
		*this = other;
	}

	IP_RANGE_PAIR(const std::pair<ACE_UINT32,ACE_UINT32>& other_ip_range)
	{
		this->ip_range = other_ip_range;
	}

	IP_RANGE_PAIR& operator = (const IP_RANGE_PAIR& range_pair)
	{
		if( this != &range_pair )
		{
			ip_range = range_pair.ip_range;
		}

		return *this;
	}

	inline int operator ==(const IP_RANGE_PAIR& range_pair) const
	{
		return ip_range == range_pair.ip_range;
	}

	//判断ip是否全部落在this范围地址内
	inline int compare(const ACE_UINT32& ip) const
	{
		return ip_range.first <= ip && ip_range.second >= ip;
	}

	//判断other是否全部落在this范围地址内
	inline int IsInThis(const IP_RANGE_PAIR& other) const
	{
		return IsInThis(other.ip_range);
	}

	inline int IsInThis(const std::pair<ACE_UINT32,ACE_UINT32>& ip_range) const
	{
		return (compare(ip_range.first)&&compare(ip_range.second));
	}

	inline int IsInThis(const std::vector< std::pair<ACE_UINT32,ACE_UINT32> >& vt_ip_range) const
	{
		for(size_t i = 0; i < vt_ip_range.size(); ++i)
		{
			if( !IsInThis(vt_ip_range[i]) )
				return 0;
		}

		return 1;
	}

	inline int IsInThis(const std::vector< IP_RANGE_PAIR >& vt_ip_range) const
	{
		for(size_t i = 0; i < vt_ip_range.size(); ++i)
		{
			if( !IsInThis(vt_ip_range[i].ip_range) )
				return 0;
		}

		return 1;
	}

	//注意比较大小，是比较两个互相不交叉的IP地址范围。如果地址交叉，则两个函数不能使用
	inline int operator < (const IP_RANGE_PAIR& other) const
	{
		return this->ip_range.second < other.ip_range.first;
	}

	inline int operator > (const IP_RANGE_PAIR& other) const
	{
		return this->ip_range.first > other.ip_range.second;
	}

};


template<typename T>
class CIPRangeTable
{
public:
	struct IP_RANGE_TYPE
	{
		IP_RANGE_TYPE()
		{	iplower = ipupper = 0;	}

		IP_RANGE_TYPE(ACE_UINT32 ip)
		{	iplower = ipupper = ip;	}

		IP_RANGE_TYPE(ACE_UINT32 ip1,ACE_UINT32 ip2)
		{	iplower = ip1;		ipupper = ip2;	}

		bool operator < (const IP_RANGE_TYPE& other) const
		{
			return ipupper < other.iplower;
		}

		ACE_UINT32 iplower;//host_order
		ACE_UINT32 ipupper;//host_order

		T			data;
	};

public:
	//预留空间
	void reserve(size_t num)
	{
		m_ipTable.reserve(num);
	}

	void clear()
	{
		m_ipTable.clear();
	}

	//如果用户能够保证输入的顺序按照大小排序输入，可以使用这个函数来加快输入
	BOOL Push_Back(ACE_UINT32 hostorder_ip1,ACE_UINT32 hostorder_ip2,const T& t)
	{
		IP_RANGE_TYPE range(hostorder_ip1,hostorder_ip2);
		range.data = t;

		m_ipTable.push_back(range);

		return true;
	}

	BOOL Push_Back(const IP_RANGE_TYPE& ipRangeType)
	{
#ifdef _DEBUG
		ACE_ASSERT( m_ipTable.begin() == m_ipTable.end() || *(m_ipTable.end()-1) < ipRangeType );
#endif

		m_ipTable.push_back(ipRangeType);
		return true;
	}

	//如果成功返回true,如果不成功一般表示有重复或者交叉
	BOOL Insert(ACE_UINT32 hostorder_ip1,ACE_UINT32 hostorder_ip2,const T& t)
	{
		IP_RANGE_TYPE range(hostorder_ip1,hostorder_ip2);
		range.data = t;

		return Insert(range);
	}

	//如果成功返回true,如果不成功一般表示有重复或者交叉
	BOOL Insert(const IP_RANGE_TYPE& ipRangeType)
	{

		typename std::vector<IP_RANGE_TYPE>::iterator it;

		it = std::lower_bound(m_ipTable.begin(),m_ipTable.end(),ipRangeType,std::less<IP_RANGE_TYPE>());

		if( it == m_ipTable.end() )
		{
			m_ipTable.push_back(ipRangeType);
			return true;
		}

		if( ipRangeType.ipupper >= it->iplower )
			return false;

		m_ipTable.insert(it,ipRangeType);

		return true;
	}

	//如果ipaddr找到，返回true,并且type是指定的类型
	const T * Find_Binary(ACE_UINT32 hostorder_ipaddr) const
	{
		typename std::vector<IP_RANGE_TYPE>::const_iterator it;

		IP_RANGE_TYPE range(hostorder_ipaddr);

		it = std::lower_bound(m_ipTable.begin(),m_ipTable.end(),range,std::less<IP_RANGE_TYPE>());

		if ( it != m_ipTable.end() && hostorder_ipaddr >= it->iplower && hostorder_ipaddr <= it->ipupper )
		{
			return &it->data;
		}
		return NULL;
	}

	const T * Find_Order(ACE_UINT32 hostorder_ipaddr) const
	{
		for(size_t i = 0; i < m_ipTable.size(); ++i)
		{
			if( hostorder_ipaddr < m_ipTable[i].iplower )
			{//因为m_ipTable是已经排了顺序，而且没有交叉
				return NULL;
			}

			if( m_ipTable[i].iplower <= hostorder_ipaddr && m_ipTable[i].ipupper >= hostorder_ipaddr )
			{
				return &m_ipTable[i].data;
			}
		}

		return NULL;
	}

public:
	BOOL Push_Back_Or_Insert(ACE_UINT32 hostorder_ip1,ACE_UINT32 hostorder_ip2,const T& t)
	{
		IP_RANGE_TYPE range(hostorder_ip1,hostorder_ip2);
		range.data = t;

		Push_Back_Or_Insert(range);

		return true;
	}

	BOOL Push_Back_Or_Insert(const IP_RANGE_TYPE& ipRangeType)
	{
		if( m_ipTable.begin() == m_ipTable.end() )
		{
			return Push_Back(ipRangeType);
		}

		if( *(m_ipTable.end()-1) < ipRangeType )
		{
			return Push_Back(ipRangeType);
		}

		return Insert(ipRangeType);

	}

	const std::vector<IP_RANGE_TYPE>& GetIPTable() const
	{
		return m_ipTable;
	}

private:
	std::vector<IP_RANGE_TYPE>	m_ipTable;

};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_IP_RANGE_TABLE_H_2006_05_18
