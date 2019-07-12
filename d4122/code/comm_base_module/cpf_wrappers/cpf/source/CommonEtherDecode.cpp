//////////////////////////////////////////////////////////////////////////
//CommonEtherDecode.cpp

#include "cpf/CommonEtherDecode.h"

CCommonEtherDecode::CCommonEtherDecode()
{
}

CCommonEtherDecode::~CCommonEtherDecode()
{
}


void CCommonEtherDecode::MACProtocolDecode(const BYTE* pPacket, DWORD dwPacketLen, MAC_DECODE_INFO& macInfo)							   
{    
	static const BYTE islLabel1[] = {0x01,0x00,0x0c,0x00,0x00};
	static const BYTE islLabel2[] = {0x03,0x00,0x0c,0x00,0x00};

	macInfo.dwPackLen = dwPacketLen;

	macInfo.vlan_type = VLAN_TYPE_NONE;
	macInfo.vlan_id = -1;

	macInfo.nErrorStyle = MACERRSTYLE_OTHER_ERROR;

	//如果整个报文的长度不足18个字节 , 则记录错误并返回
	if(dwPacketLen < 18)
	{
		macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
		return;
	}

	{
		const ACE_UINT32 * pLongPacket = (ACE_UINT32*)pPacket;

		// 是否为ISL的VLAN数据包
		if( (pLongPacket[0]== *(ACE_UINT32 *)islLabel1 && pPacket[4]==islLabel1[4])
			||  (pLongPacket[0]== *(ACE_UINT32 *)islLabel2 && pPacket[4]==islLabel2[4]) )
		{
			macInfo.vlan_id  = ((pPacket[20]&0X7F) << 8);// ISL的VLAN ID占高15位
			macInfo.vlan_id |= pPacket[21];

			// 修改包头指针和长度,下边就可以使用以前的代码
			pPacket += 26;
			dwPacketLen -= (26+4);	// ISL头和结尾FCS
			macInfo.vlan_type = VLAN_TYPE_ISL;
		}
	}

	macInfo.pMacAddrHeader = (unsigned char *)pPacket;
	
	// 判断是否为非ISL的VLA802.1Q数据包

	do{
		if(		(*(ACE_UINT16 *)(pPacket+12) == 0X0081 )
			&&	(macInfo.vlan_type==VLAN_TYPE_NONE||macInfo.vlan_type==VLAN_TYPE_8021Q)
		  )
		{
			macInfo.vlan_id  = ((pPacket[14]&0X0F) << 8);//802.1Q的VLAN ID占低12位
			macInfo.vlan_id |= pPacket[15];

			pPacket += 4;	// 802.1Q插入的VLAN tag部分
			dwPacketLen -= 4;
			macInfo.vlan_type = VLAN_TYPE_8021Q;

			if ( (dwPacketLen) <= (12+4) )	/*mac+vlan,防止异常数据导致越界*/
			{
				macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
				return;
			}
		}
		else
		{
			break;
		}

	}while(1);

	//取出类型字段,之所以分为两步是为了解决网络字节序
	WORD typeOrLen = NBUF_TO_WORD(pPacket+12);

	if(typeOrLen >= 1500) // ethernet_II
	{
		macInfo.dwCapsulation = ETHERNET_II;
		macInfo.dwHeaderLength = 14;
		macInfo.nProtoIndex = GetSNAPOrEthernetIndex(typeOrLen);
	
	}
	else// LLC or raw 8023
	{
		if (dwPacketLen < 20)//头部长度不够
		{
			macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
			return;
		}
		
		if(*(WORD*)(pPacket+14) == PID_ETHERTYPE_RAWIPX) // ethernet_802.3
		{
			macInfo.dwCapsulation = ETHERNET_8023;
			macInfo.dwHeaderLength = 14;//0xFFFF是IPX头的一部份
			macInfo.nProtoIndex = INDEX_PID_MAC_IPX;
		}
		else //LLC
		{
			if (dwPacketLen < 26)//头部长度不够
			{
				macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
				return;
			}
			
			BYTE DSAP = pPacket[14] & 0xFE;
			if(DSAP == LLC_SAP_SNAP) // ethernet_snap
			{
				macInfo.dwCapsulation = ETHERNET_SNAP;
				macInfo.dwHeaderLength = 22; //12 MAC addr + 2 len + 3 LLC + 5 SNAP

				// 判断是否为ISL的802.1Q的VLAN数据包
				//if( (pPacket[20]==0x81) && (pPacket[21]==0x00) )
				if( *(ACE_UINT16 *)(pPacket+20) == 0X0081 )
				{
					macInfo.vlan_type = VLAN_TYPE_8021Q;
					macInfo.vlan_id  = ((pPacket[22]&0X0F) << 8);//802.1Q的VLAN ID占低12位
					macInfo.vlan_id |= pPacket[23];

					pPacket +=4;
					dwPacketLen -= 4;
				}

				typeOrLen = NBUF_TO_WORD(pPacket+20);
				macInfo.nProtoIndex = GetSNAPOrEthernetIndex(typeOrLen);
			
			}
			else // ethernet_8022 LLC
			{
				if (dwPacketLen < 21)//头部长度不够
				{
					macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
					return;
				}

				// 判断是否为AAL的802.1Q的VLAN数据包
				if( (pPacket[17]==0x81) && (pPacket[18]==0x00) )
				{
					macInfo.vlan_type = VLAN_TYPE_8021Q;
					macInfo.vlan_id  = ((pPacket[19]&0X0F) << 8);//802.1Q的VLAN ID占低12位
					macInfo.vlan_id |= pPacket[20];

					pPacket +=4;
					dwPacketLen -= 4;
				}

				macInfo.dwCapsulation = ETHERNET_8022;
				macInfo.dwHeaderLength = 17;
				macInfo.nProtoIndex = Get8022Index(DSAP);
			}//else // ethernet_8022 LLC
		}//else //LLC
	}

	//以上解码正确, 证明报文本身无错, 现在从报文中抽取相应的信息
	//从报文中获得相应的信息，以利于统计用

	if(dwPacketLen <= macInfo.dwHeaderLength+4 )
	{
		macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
		return;
	}

	macInfo.dwUpperProtoLength = dwPacketLen - (macInfo.dwHeaderLength+4);
	macInfo.pUpperData = (BYTE *)pPacket + macInfo.dwHeaderLength;

	//判断MAC包的类型
	if(IS_BROADCAST_ADDRESS_MAC(pPacket))
		macInfo.nPackStyle = MACPACKSTYLE_BROADCAST;//广播
	else if(IS_GROUP_ADDRESS_MAC(pPacket))
		macInfo.nPackStyle = MACPACKSTYLE_GROUPCAST;//组播
	else
		macInfo.nPackStyle = MACPACKSTYLE_OTHER;//普通

	macInfo.nErrorStyle = MACERRSTYLE_NOERROR;

	return;
}


void CCommonEtherDecode::ARPProtocolDecode(const BYTE* pPacket, DWORD dwPacketLen, ARP_DECODE_INFO& arpInfo)
{
	if( dwPacketLen < 28 )
	{	
		arpInfo.nErrorStyle = 1;
		return;
	}

	arpInfo.hardType = (pPacket[0]<<8);
	arpInfo.hardType |= pPacket[1];

	if( arpInfo.hardType != 1 )
	{
		arpInfo.nErrorStyle = 2;
		return;
	}

	arpInfo.protoType = (pPacket[2]<<8);
	arpInfo.protoType |= pPacket[3];

	if( arpInfo.protoType != 0x00000800 )
	{
		arpInfo.nErrorStyle = 3;
		return;
	}
	
	arpInfo.hardAddrLen = pPacket[4];
	arpInfo.protoAddrLen = pPacket[5];

	if( arpInfo.hardAddrLen != 6 || arpInfo.protoAddrLen != 4 )
	{
		arpInfo.nErrorStyle = 4;
		return;
	}

	arpInfo.operation = (pPacket[6]<<8);
	arpInfo.operation |= pPacket[7];

	memcpy( arpInfo.senderMacAddr, pPacket + 8 , 6);
	arpInfo.senderNetOrderIPAddr = *(DWORD *)(pPacket+14);

	memcpy( arpInfo.destMacAddr, pPacket + 18 , 6);
	arpInfo.destNetOrderIPAddr = *(DWORD *)(pPacket+24);

	arpInfo.nErrorStyle = 0;

	return;
}

