//////////////////////////////////////////////////////////////////////////
//CPF_ArpTable.h

#pragma once

#include "cpf/THashFinderEx.h"

class CPF_CLASS CPF_ArpTable
{
public:
	CPF_ArpTable(void);
	virtual ~CPF_ArpTable(void);

public:
	void init(int max_count,int timeout_interval);
	void close();

public:
	typedef struct tagArpTableItem
	{
		ACE_UINT64	mac;

		ACE_UINT32	hip;

		int			last_data_time;
		int			last_arp_time;
		int			static_type;

		ACE_UINT32	vlan_id;
		ACE_UINT16	pppoe_sesion_id;

	}ArpTableItem;

	class IPCompFun
	{
	public:
		int Hash(ACE_UINT32 key) const {
			return (int)key;
		}
		int Cmp(const ArpTableItem& value, ACE_UINT32 key) const {
			return ( value.hip == key );
		}
	};
	typedef CTHashFinderEx<ArpTableItem, ACE_UINT32, IPCompFun> CARPTableFinder;


	CARPTableFinder	m_arp_table;

public:
	inline int GetHeadBlock() const
	{
		return m_arp_table.GetHeadBlock();
	}

	inline ArpTableItem * GetNextBlock(int& index) const
	{
		return m_arp_table.GetNextBlockFromHead(index);
	}

public:
	BOOL QuaryMac(ACE_UINT32 hip,void * macaddr,ACE_UINT32& vlan_id,ACE_UINT16& pppoe_sesion_id,int the_time) const;

public:
	int GetTotalCount() const
	{
		return m_arp_table.GetTotalCount();
	}

	int GetUsedCount() const
	{
		return m_arp_table.GetUsedCount();
	}

	//清除所有ARP表，包括静态表
	void ResetAllTable();

	//清除所有动态的表，静态表不删除,返回清除了多少行
	inline int ResetDynTable()
	{
		return ResetTable(0);
	}

	//清除所有静态的表，动态表不删除,返回清除了多少行
	inline int ResetStaticTable()
	{
		return ResetTable(1);
	}

	//static_type,是否是静态的arp表
	//change_type是返回值，0表示没有变化，1表示插入，2表示替换了
	BOOL InsertArpItem(ACE_UINT32 hip,const void * macaddr,ACE_UINT32 vlan_id,ACE_UINT16 pppoe_sesion_id,int the_time,int static_type,int * change_type);

	int DeleteItem(ACE_UINT32 hip);

	//返回删除了多少条记录
	int DeleteItem(const void * macaddr);

public:
	//返回删除了多少条记录,多长时间没有数据，需要清除
	int CheckTimeout_DataTime(int cur_time);

private:
	int	m_timeout_interval;

private:
	int ResetTable(int static_type);
};
