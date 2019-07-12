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

	//�������ARP��������̬��
	void ResetAllTable();

	//������ж�̬�ı���̬��ɾ��,��������˶�����
	inline int ResetDynTable()
	{
		return ResetTable(0);
	}

	//������о�̬�ı���̬��ɾ��,��������˶�����
	inline int ResetStaticTable()
	{
		return ResetTable(1);
	}

	//static_type,�Ƿ��Ǿ�̬��arp��
	//change_type�Ƿ���ֵ��0��ʾû�б仯��1��ʾ���룬2��ʾ�滻��
	BOOL InsertArpItem(ACE_UINT32 hip,const void * macaddr,ACE_UINT32 vlan_id,ACE_UINT16 pppoe_sesion_id,int the_time,int static_type,int * change_type);

	int DeleteItem(ACE_UINT32 hip);

	//����ɾ���˶�������¼
	int DeleteItem(const void * macaddr);

public:
	//����ɾ���˶�������¼,�೤ʱ��û�����ݣ���Ҫ���
	int CheckTimeout_DataTime(int cur_time);

private:
	int	m_timeout_interval;

private:
	int ResetTable(int static_type);
};
