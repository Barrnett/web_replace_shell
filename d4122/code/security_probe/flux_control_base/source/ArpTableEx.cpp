//////////////////////////////////////////////////////////////////////////
//ArpTableEx.cpp

#include "stdafx.h"
#include "ArpTableEx.h"
#include "cpf/ostypedef.h"
#include "cpf/ConstructPacket.h"
#include "cpf/proto_struct_def.h"
#include "TS_interfaceCfg.h"

CArpTableEx::CArpTableEx(CTS_InterfaceCfg * pTS_InterfaceCfg)
{
	m_pTS_InterfaceCfg = pTS_InterfaceCfg;
}

CArpTableEx::~CArpTableEx(void)
{
}

int CArpTableEx::OnPacketTimer(const CTimeStamp32& cur_ts)
{
	if( cur_ts.SecBetween(m_last_check_ts) > 5 )
	{
		m_last_check_ts = cur_ts;

		RefreshArp(cur_ts);

		return CheckTimeout_DataTime(cur_ts.GetSEC());
	}

	return 0;

}

void CArpTableEx::RefreshArp(const CTimeStamp32& cur_ts)
{
	int index = m_arp_table.GetHeadBlock();

	while( index != -1 )
	{
		CPF_ArpTable::ArpTableItem * pTableItem = m_arp_table.GetNextBlockFromHead(index);

		if( pTableItem->static_type || IS_PPPOE(pTableItem->pppoe_sesion_id) )
		{
			continue;
		}
		
		if( cur_ts.GetSEC() - pTableItem->last_arp_time < 30 )
		{
			continue;
		}

		ACE_UINT32 sender_ip = 0;
		ACE_UINT64 sender_mac = 0;
		int		   sender_card_index = -1;

		/*
		if( pTableItem->card_type == CARD_TYPE_WAN )
		{
			if( m_pTS_InterfaceCfg->GetRouterModeConfig().m_vt_wan[pTableItem->cfg_index].addr_type == ONE_WAN_LINE_INFO::ADDR_TYPE_PPPOE )
			{
				continue;
			}

			if( m_pTS_InterfaceCfg->GetRouterModeConfig().m_vt_wan[pTableItem->cfg_index].IsBridgeType() )
			{
				continue;
			}

			sender_ip = m_pTS_InterfaceCfg->GetRouterModeConfig().m_vt_wan[pTableItem->cfg_index].GetIP();
			sender_mac = m_pTS_InterfaceCfg->GetCardMacByConfigIndex(pTableItem->cfg_index,pTableItem->card_type);

			sender_card_index = m_pTS_InterfaceCfg->GetRouterModeConfig().m_vt_wan[pTableItem->cfg_index].cardindex;
		}
		else if( pTableItem->card_type == CARD_TYPE_LAN )
		{
			if( !m_pTS_InterfaceCfg->GetRouterModeConfig().m_vt_lan[pTableItem->cfg_index].IsInSameSubNet_NOTPPPOE(pTableItem->hip) )
			{
				continue;
			}

			if( m_pTS_InterfaceCfg->IsBridgeType(pTableItem->card_type,pTableItem->cfg_index) )
			{
				continue;
			}

			sender_ip = m_pTS_InterfaceCfg->GetRouterModeConfig().m_vt_lan[pTableItem->cfg_index].ipaddr;
			sender_mac = m_pTS_InterfaceCfg->GetCardMacByConfigIndex(pTableItem->cfg_index,pTableItem->card_type);

			sender_card_index = m_pTS_InterfaceCfg->GetRouterModeConfig().m_vt_lan[pTableItem->cfg_index].cardindex;
		}

		if( sender_ip != pTableItem->hip )
		{
			BYTE arp_buf[512];

			unsigned int arp_pkt_len = CConstructPacket::MAC_Construct_Arp_Req_Packet(
				arp_buf,(const BYTE *)&sender_mac,ACE_HTONL(sender_ip),ACE_HTONL(pTableItem->hip),NULL,pTableItem->vlan_id);

			IWriteDataStream * pWriteDataSteam = m_pTS_InterfaceCfg->GetWriteDataStream();

			if( pWriteDataSteam )
			{
				pWriteDataSteam->WritePacket(PACKET_OK,(const char *)arp_buf,arp_pkt_len+4,(1<<sender_card_index),1);
			}
		}
		*/
	}

	return;
}

BOOL CArpTableEx::GetArpListInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	int count = m_arp_table.GetUsedCount();

	int one_item_size = (1+1+1+4+6+4+4);

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,4+count*one_item_size);

	if( !pRetMsgBlock )
	{
		return false;
	}

	char * write_data = CMsgBlockManager::WritePtr(*pRetMsgBlock);
	char * begin_data = write_data;

	CData_Stream_LE::PutUint16(write_data,0);//code=0，表示正确

	char * count_pos = write_data;
	CData_Stream_LE::PutUint32(write_data,0);

	int real_count = 0;

	int index = m_arp_table.GetHeadBlock();

	while( index != -1 && real_count < count )
	{
		const CPF_ArpTable::ArpTableItem * pTableItem = m_arp_table.GetNextBlockFromHead(index);

		CData_Stream_LE::PutUint8(write_data,pTableItem->static_type);
		CData_Stream_LE::PutUint32(write_data,pTableItem->hip);
		CData_Stream_LE::PutFixString(write_data,6,(const char *)&pTableItem->mac);
		CData_Stream_LE::PutUint32(write_data,pTableItem->last_arp_time);
		CData_Stream_LE::PutUint32(write_data,pTableItem->last_data_time);

		++real_count;
	}

	CData_Stream_LE::PutUint32(count_pos,real_count);

	pRetMsgBlock->nDataLen = (int)(write_data-begin_data);

	return true;
}
