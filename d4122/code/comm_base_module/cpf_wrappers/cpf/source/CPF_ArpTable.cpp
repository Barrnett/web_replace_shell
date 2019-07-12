//////////////////////////////////////////////////////////////////////////
//CPF_ArpTable.cpp

#include "stdafx.h"
#include "cpf/CPF_ArpTable.h"
#include "cpf/addr_tools.h"

CPF_ArpTable::CPF_ArpTable(void)
{
	m_timeout_interval = 5*60;
}

CPF_ArpTable::~CPF_ArpTable(void)
{
	close();
}

void CPF_ArpTable::init(int max_count,int timeout_interval)
{
	m_arp_table.Create(max_count);

	m_timeout_interval = timeout_interval;

	return;
}

void CPF_ArpTable::close()
{
	m_arp_table.Destroy();

}

int CPF_ArpTable::CheckTimeout_DataTime(int cur_time)
{
	int count = 0;

	int index = m_arp_table.GetHeadBlock();

	while( index != -1 )
	{
		int prev_index = index;

		ArpTableItem * pItem = m_arp_table.GetNextBlockFromHead(index);

		if( !pItem->static_type 
			&& (cur_time - pItem->last_data_time > m_timeout_interval) )
		{
			m_arp_table.FreeBlock(prev_index);

			++count;
		}
	}

	return count;
}

int CPF_ArpTable::DeleteItem(ACE_UINT32 hip)
{
	return m_arp_table.FreeBlockByKey(hip);
}

int CPF_ArpTable::DeleteItem(const void * macaddr)
{
	int count = 0;

	ACE_UINT64 int_mac = GET_UINT64_MAC(macaddr);

	int index = m_arp_table.GetHeadBlock();

	while( index != -1 )
	{
		int prev_index = index;

		const ArpTableItem * pItem = m_arp_table.GetNextBlockFromHead(index);

		if( pItem->mac == int_mac )
		{
			m_arp_table.FreeBlock(prev_index);

			++count;
		}
	}

	return count;

}

BOOL CPF_ArpTable::QuaryMac(ACE_UINT32 hip,void * macaddr,ACE_UINT32& vlan_id,ACE_UINT16& pppoe_sesion_id,int the_time) const
{
	if( m_arp_table.GetTotalCount() == 0 )
		return false;

	int index = m_arp_table.FindFirst(hip);

	while (index != -1)
	{
		ArpTableItem * pItem = (ArpTableItem *)m_arp_table.FindNext(hip,index);

		if(pItem)
		{
			memcpy(macaddr,&pItem->mac,6);

			vlan_id = pItem->vlan_id;
			pppoe_sesion_id = pItem->pppoe_sesion_id;

			if( the_time != 0 )
			{
				pItem->last_data_time = the_time;
			}

			return true;
		}
	}

	return false;

}

BOOL CPF_ArpTable::InsertArpItem(ACE_UINT32 hip,const void * macaddr,ACE_UINT32 vlan_id,ACE_UINT16 pppoe_sesion_id,int the_time,int static_type,int * change_type)
{
	if( m_arp_table.GetTotalCount() == 0 )
		return false;

	if( hip == 0 )
		return false;

	ArpTableItem * pItem = m_arp_table.FindOnly(hip);

	BOOL bnew = false;

	if( !pItem )
	{
		pItem = m_arp_table.AllocBlockByKey(hip);

		if( !pItem )
			return false;

		pItem->hip = hip;

		bnew = true;
	}

	if( bnew )
	{
		pItem->last_arp_time = the_time;
		pItem->last_data_time = the_time;

		if( vlan_id == 0 )
		{
			pItem->vlan_id = -1;
		}
		else
		{
			pItem->vlan_id = vlan_id;
		}

		if( pppoe_sesion_id == 0 )
		{
			pItem->pppoe_sesion_id = (ACE_UINT16)-1;
		}
		else
		{
			pItem->pppoe_sesion_id = pppoe_sesion_id;
		}
	}
	else
	{
		if( the_time != 0 )
		{
			pItem->last_arp_time = the_time;

			pItem->last_data_time = the_time;
		}

		if( vlan_id != 0 )
		{
			pItem->vlan_id = vlan_id;
		}

		if( pppoe_sesion_id != 0 )
		{
			pItem->pppoe_sesion_id = pppoe_sesion_id;
		}
	}

	if( bnew )
	{
		if( change_type )
			*change_type = 1;

		pItem->static_type = static_type;

		pItem->mac = GET_UINT64_MAC(macaddr);
	}
	else
	{
		if( pItem->static_type && !static_type )
		{//如果以前是静态的,并且现在不是静态的

			return false;
		}

		if( change_type )
		{
			if( memcmp((void *)&pItem->mac,macaddr,6) == 0 )
			{
				*change_type = 0;
			}
			else
			{
				pItem->mac = GET_UINT64_MAC(macaddr);

				*change_type = 2;
			}
		}
		else
		{
			pItem->mac = GET_UINT64_MAC(macaddr);
		}
	}

	return true;
}

void CPF_ArpTable::ResetAllTable()
{
	m_arp_table.FreeAllBlock();

	return;
}

int CPF_ArpTable::ResetTable(int static_type)
{
	int count = 0;

	int index = m_arp_table.GetHeadBlock();

	while( index != -1 )
	{
		int prev_index = index;

		ArpTableItem * pItem = m_arp_table.GetNextBlockFromHead(index);

		if( pItem->static_type == static_type )
		{
			m_arp_table.FreeBlock(prev_index);

			++count;
		}
	}

	return count;
}

