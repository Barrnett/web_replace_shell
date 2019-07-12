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

	//����������ĵĳ��Ȳ���18���ֽ� , ���¼���󲢷���
	if(dwPacketLen < 18)
	{
		macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
		return;
	}

	{
		const ACE_UINT32 * pLongPacket = (ACE_UINT32*)pPacket;

		// �Ƿ�ΪISL��VLAN���ݰ�
		if( (pLongPacket[0]== *(ACE_UINT32 *)islLabel1 && pPacket[4]==islLabel1[4])
			||  (pLongPacket[0]== *(ACE_UINT32 *)islLabel2 && pPacket[4]==islLabel2[4]) )
		{
			macInfo.vlan_id  = ((pPacket[20]&0X7F) << 8);// ISL��VLAN IDռ��15λ
			macInfo.vlan_id |= pPacket[21];

			// �޸İ�ͷָ��ͳ���,�±߾Ϳ���ʹ����ǰ�Ĵ���
			pPacket += 26;
			dwPacketLen -= (26+4);	// ISLͷ�ͽ�βFCS
			macInfo.vlan_type = VLAN_TYPE_ISL;
		}
	}

	macInfo.pMacAddrHeader = (unsigned char *)pPacket;
	
	// �ж��Ƿ�Ϊ��ISL��VLA802.1Q���ݰ�

	do{
		if(		(*(ACE_UINT16 *)(pPacket+12) == 0X0081 )
			&&	(macInfo.vlan_type==VLAN_TYPE_NONE||macInfo.vlan_type==VLAN_TYPE_8021Q)
		  )
		{
			macInfo.vlan_id  = ((pPacket[14]&0X0F) << 8);//802.1Q��VLAN IDռ��12λ
			macInfo.vlan_id |= pPacket[15];

			pPacket += 4;	// 802.1Q�����VLAN tag����
			dwPacketLen -= 4;
			macInfo.vlan_type = VLAN_TYPE_8021Q;

			if ( (dwPacketLen) <= (12+4) )	/*mac+vlan,��ֹ�쳣���ݵ���Խ��*/
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

	//ȡ�������ֶ�,֮���Է�Ϊ������Ϊ�˽�������ֽ���
	WORD typeOrLen = NBUF_TO_WORD(pPacket+12);

	if(typeOrLen >= 1500) // ethernet_II
	{
		macInfo.dwCapsulation = ETHERNET_II;
		macInfo.dwHeaderLength = 14;
		macInfo.nProtoIndex = GetSNAPOrEthernetIndex(typeOrLen);
	
	}
	else// LLC or raw 8023
	{
		if (dwPacketLen < 20)//ͷ�����Ȳ���
		{
			macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
			return;
		}
		
		if(*(WORD*)(pPacket+14) == PID_ETHERTYPE_RAWIPX) // ethernet_802.3
		{
			macInfo.dwCapsulation = ETHERNET_8023;
			macInfo.dwHeaderLength = 14;//0xFFFF��IPXͷ��һ����
			macInfo.nProtoIndex = INDEX_PID_MAC_IPX;
		}
		else //LLC
		{
			if (dwPacketLen < 26)//ͷ�����Ȳ���
			{
				macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
				return;
			}
			
			BYTE DSAP = pPacket[14] & 0xFE;
			if(DSAP == LLC_SAP_SNAP) // ethernet_snap
			{
				macInfo.dwCapsulation = ETHERNET_SNAP;
				macInfo.dwHeaderLength = 22; //12 MAC addr + 2 len + 3 LLC + 5 SNAP

				// �ж��Ƿ�ΪISL��802.1Q��VLAN���ݰ�
				//if( (pPacket[20]==0x81) && (pPacket[21]==0x00) )
				if( *(ACE_UINT16 *)(pPacket+20) == 0X0081 )
				{
					macInfo.vlan_type = VLAN_TYPE_8021Q;
					macInfo.vlan_id  = ((pPacket[22]&0X0F) << 8);//802.1Q��VLAN IDռ��12λ
					macInfo.vlan_id |= pPacket[23];

					pPacket +=4;
					dwPacketLen -= 4;
				}

				typeOrLen = NBUF_TO_WORD(pPacket+20);
				macInfo.nProtoIndex = GetSNAPOrEthernetIndex(typeOrLen);
			
			}
			else // ethernet_8022 LLC
			{
				if (dwPacketLen < 21)//ͷ�����Ȳ���
				{
					macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
					return;
				}

				// �ж��Ƿ�ΪAAL��802.1Q��VLAN���ݰ�
				if( (pPacket[17]==0x81) && (pPacket[18]==0x00) )
				{
					macInfo.vlan_type = VLAN_TYPE_8021Q;
					macInfo.vlan_id  = ((pPacket[19]&0X0F) << 8);//802.1Q��VLAN IDռ��12λ
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

	//���Ͻ�����ȷ, ֤�����ı����޴�, ���ڴӱ����г�ȡ��Ӧ����Ϣ
	//�ӱ����л����Ӧ����Ϣ��������ͳ����

	if(dwPacketLen <= macInfo.dwHeaderLength+4 )
	{
		macInfo.nErrorStyle = MACERRSTYLE_SHORT_HEADER;
		return;
	}

	macInfo.dwUpperProtoLength = dwPacketLen - (macInfo.dwHeaderLength+4);
	macInfo.pUpperData = (BYTE *)pPacket + macInfo.dwHeaderLength;

	//�ж�MAC��������
	if(IS_BROADCAST_ADDRESS_MAC(pPacket))
		macInfo.nPackStyle = MACPACKSTYLE_BROADCAST;//�㲥
	else if(IS_GROUP_ADDRESS_MAC(pPacket))
		macInfo.nPackStyle = MACPACKSTYLE_GROUPCAST;//�鲥
	else
		macInfo.nPackStyle = MACPACKSTYLE_OTHER;//��ͨ

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

