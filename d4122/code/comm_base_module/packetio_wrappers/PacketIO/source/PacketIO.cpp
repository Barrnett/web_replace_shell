//////////////////////////////////////////////////////////////////////////
//PacketIO.cpp

#include "PacketIO/PacketIO.h"
#include "PacketIO/MyProtocolID.h"
#include "cpf/Common_Func_Macor.h"

//返回所需内存的大小。
PACKETIO_CLASS int Package_OnePacket(OUT void * pbuffer,
								   IN PACKET_LEN_TYPE type,
								   IN const RECV_PACKET& packet)
{
	if( pbuffer )
	{
		char * pdata = (char *)pbuffer;

		ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

		*(ULONG_PTR *)pdata = (ULONG_PTR)type;
		pdata += sizeof(ULONG_PTR);

		memcpy(pdata,&packet,sizeof(packet));
		pdata += sizeof(packet);

		ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

		memcpy(pdata,packet.pHeaderinfo,sizeof(*packet.pHeaderinfo));
		pdata += sizeof(*packet.pHeaderinfo);

		ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

		if( type == PACKET_OK )
		{
			memcpy(pdata,packet.pPacket,packet.nCaplen);
			pdata += packet.nCaplen;
		}

		return (int)(pdata-(char *)pbuffer);
	}
	else
	{
		int nsize = 0;
		nsize += sizeof(ULONG_PTR);//type
		nsize += sizeof(packet);
		nsize += sizeof(*packet.pHeaderinfo);
		nsize += packet.nCaplen;

		return nsize;
	}

}

//返回跳过内存的大小,不计算字节对齐
PACKETIO_CLASS int UnPackage_OnePacket(IN const void * pbuffer,
									 OUT PACKET_LEN_TYPE& type,
									 OUT RECV_PACKET*& packet)
{
	char * pdata = (char *)pbuffer;

	ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

	type = (PACKET_LEN_TYPE)(*(ULONG_PTR *)pdata);
	pdata += sizeof(ULONG_PTR);

	packet = (RECV_PACKET *)pdata;
	pdata += sizeof(RECV_PACKET);

	ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

	packet->pHeaderinfo = (PACKET_HEADER *)pdata;
	pdata += sizeof(PACKET_HEADER);

	ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

	packet->pPacket = (void *)pdata;
	pdata += packet->nCaplen;

	return (int)(pdata-(char *)pbuffer);

}


PACKETIO_CLASS int Package_OnePacket(OUT void * pbuffer,
									 IN PACKET_LEN_TYPE type,
									 IN const RECV_PACKET& packet,
									 IN const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
									 IN int attach_len,
									 IN const void * attach_data)
{
	if( pbuffer )
	{
		char * pdata = (char *)pbuffer;

		ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

		*(ULONG_PTR *)pdata = (ULONG_PTR)type;
		pdata += sizeof(ULONG_PTR);

		RECV_PACKET* temp_pkt = (RECV_PACKET *)pdata;

		temp_pkt->nCaplen = packet.nCaplen + sizeof(context) + sizeof(ULONG_PTR) + GET_BYTE_ALIGN(attach_len,sizeof(void *));
		temp_pkt->nPktlen = packet.nPktlen + sizeof(context) + sizeof(ULONG_PTR) + GET_BYTE_ALIGN(attach_len,sizeof(void *));

		temp_pkt->nOffsetBit = packet.nOffsetBit;
		temp_pkt->nPktAddiBitLen = packet.nPktAddiBitLen;
		temp_pkt->nCapAddiBitLen = packet.nCapAddiBitLen;

		pdata += sizeof(packet);

		ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

		memcpy(pdata,packet.pHeaderinfo,sizeof(*packet.pHeaderinfo));
		((PACKET_HEADER *)pdata)->wdProto = MY_PROTOCOL_MACTCPIPCONTEXT;//修改协议号
		pdata += sizeof(*packet.pHeaderinfo);

		ACE_ASSERT( IS_ALLIGN_PTR(pdata) );
		memcpy(pdata,&context,sizeof(context));
		pdata += sizeof(context);

		ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

		*(ULONG_PTR *)pdata = attach_len;
		pdata += sizeof(ULONG_PTR);

		if( attach_len > 0 )
		{
			ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

			memcpy(pdata,attach_data,attach_len);
			pdata += GET_BYTE_ALIGN(attach_len,sizeof(void *));
		}

		if( type == PACKET_OK )
		{
			ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

			memcpy(pdata,packet.pPacket,packet.nCaplen);
			pdata += packet.nCaplen;
		}

		return (int)(pdata-(char *)pbuffer);
	}
	else
	{
		int nsize = 0;
		nsize += sizeof(ULONG_PTR);//type
		nsize += sizeof(packet);
		nsize += sizeof(*packet.pHeaderinfo);
		nsize += sizeof(context);

		nsize += sizeof(ULONG_PTR);//attach_len

		nsize += GET_BYTE_ALIGN(attach_len,sizeof(void *));

		nsize += packet.nCaplen;

		return nsize;
	}

}

PACKETIO_CLASS void UnPackage_OnePacket(RECV_PACKET& packet,
										CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
										OUT int& attach_len,
										OUT void*& attach_data)
{
	packet.nCaplen -= sizeof(context)+sizeof(ULONG_PTR);
	packet.nPktlen -= sizeof(context)+sizeof(ULONG_PTR);

	packet.pHeaderinfo->wdProto = MY_PROTOCOL_MAC;

	const char * pdata = (const char *)packet.pPacket;

	ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

	memcpy(&context,(const BYTE *)pdata,sizeof(context));
	pdata += sizeof(context);

	ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

	attach_len = (int)(*(ULONG_PTR *)pdata);
	pdata += sizeof(ULONG_PTR);

	if( attach_len > 0 )
	{
		attach_data = (void *)pdata;

		pdata += GET_BYTE_ALIGN(attach_len,sizeof(void *));

		packet.nCaplen -= GET_BYTE_ALIGN(attach_len,sizeof(void *));
		packet.nPktlen -= GET_BYTE_ALIGN(attach_len,sizeof(void *));
	}
	else
	{
		attach_data = NULL;
	}

	ACE_ASSERT( IS_ALLIGN_PTR(pdata) );

	packet.pPacket = (void *)pdata;

	CCommonMacTCPIPDecode::RecalDataPointer(context,(const BYTE *)packet.pPacket);

	return;
}

