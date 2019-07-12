//////////////////////////////////////////////////////////////////////////
//CommonIPv4Decode.cpp

#include "cpf/CommonIPv4Decode.h"
#include "cpf/crc_tools.h"

////////////////////////////////////////////////////////////////////////////
//

#define IP_FRAGMENT(pIPHeader) ((*((ACE_UINT16*)(pIPHeader+6)))&0xFF7F)
#define IP_TTL_1(pIPHeader) (pIPHeader[8])
#define IP_ID(pIPHeader) (*(ACE_UINT16*)(pIPHeader+4))
#define IP_ADDRESS_SRC(pkt) (*((ACE_UINT32*)(pkt+12)))
#define IP_ADDRESS_DST(pkt) (*((ACE_UINT32*)(pkt+16)))


CCommonIPv4Decode::CCommonIPv4Decode()
{

}
CCommonIPv4Decode::~CCommonIPv4Decode()
{

}


void CCommonIPv4Decode::IPv4ProtocolDecode(const BYTE* pPacket, unsigned int dwPacketLen, 
										   IPv4_DECODE_INFO& ipv4Info,BOOL bCalCheckSum)
{
	//(����ϢӦ���������, �����ĳ��ִ����, ���и��������Ч��)
	ipv4Info.dwPackLen = dwPacketLen;

	ipv4Info.nErrorStyle = IPv4ERRSTYLE_OTHER_ERROR;

	//����������ĵĳ��Ȳ���20���ֽ� , ���¼���󲢷���
	if(dwPacketLen < 20)
	{
		ipv4Info.nErrorStyle = IPv4ERRSTYLE_SHORT_HEADER;
		return;
	}

	//��IPͷ��ȡ��ͷ������
	ipv4Info.dwHeaderLength = ((pPacket[0] & 0x0F) << 2);
	ipv4Info.pUpperData = (BYTE *)pPacket + ipv4Info.dwHeaderLength;
	if (dwPacketLen < ipv4Info.dwHeaderLength || ipv4Info.dwHeaderLength < 20)
	{
		ipv4Info.nErrorStyle = IPv4ERRSTYLE_SHORT_HEADER;
		return;
	}

	if( (pPacket[0]&0xF0) != 0x40 )
	{
		ipv4Info.nErrorStyle= IPv4ERRSTYLE_VERSION;
		return;
	}

	if( *(WORD *)(pPacket+10) != 0 && bCalCheckSum )
	{
		if (CPF::ComputeCheckSum_NetOrder((BYTE*)pPacket, ipv4Info.dwHeaderLength) != 0)
		{
			ipv4Info.nErrorStyle = IPv4ERRSTYLE_CHECKSUM;
			return;
		}
	}

	//��IPͷ��ȡ��IP�����ܳ���
	DWORD dwIPPacketLen = NBUF_TO_WORD(pPacket+2);

	{//add by zhuzhp 2012-02-02
		if( dwIPPacketLen == 0 )
			dwIPPacketLen = dwPacketLen;

	}

	if(dwPacketLen < dwIPPacketLen)//IPͷ���������󣬵������Խ���IPͷ
	{
		ipv4Info.dwUpperProtoLength = 0;//IP�ϲ�Э�������
		ipv4Info.nErrorStyle = IPv4ERRSTYLE_PACKETSHORT;
		return;
	}
	else
	{
		ipv4Info.dwUpperProtoLength = dwIPPacketLen - ipv4Info.dwHeaderLength;
	}

	ipv4Info.nProtoIndex = GetProtoIndex(pPacket[9]);

	//�ӱ����л����Ӧ����Ϣ��������ͳ����
	ipv4Info.ipSrc = (DWORD)IP_ADDRESS_SRC(pPacket);//get the src-addr
	ipv4Info.ipDst = IP_ADDRESS_DST(pPacket);//get the dest-addr

	ipv4Info.dwTTL = IP_TTL_1(pPacket);//get the ttl-value
	ipv4Info.dwFragment = IP_FRAGMENT(pPacket);//get the fragment-sig
	ipv4Info.dwID = IP_ID(pPacket);

	//�ж�ip��������(ע�������ֽ�������)
	if(ipv4Info.ipDst == 0xffffffff)//group addr
	{
		ipv4Info.nPackStyle = IPv4PACKSTYLE_BROADCAST;
	}
	else 
	{
		if(ipv4Info.ipDst & 0x00000080)//not A class
		{
			if (ipv4Info.ipDst & 0x00000040)//not B class
				ipv4Info.nPackStyle = ((ipv4Info.ipDst&0xff000000)==0xff000000) ? IPv4PACKSTYLE_BROADCAST : IPv4PACKSTYLE_OTHER;
			else//B class
				ipv4Info.nPackStyle = ((ipv4Info.ipDst&0xffff0000)==0xffff0000) ? IPv4PACKSTYLE_BROADCAST : IPv4PACKSTYLE_OTHER;
		}
		else//A class
		{
			ipv4Info.nPackStyle = ((ipv4Info.ipDst&0xffffff00)==0xffffff00) ? IPv4PACKSTYLE_BROADCAST : IPv4PACKSTYLE_OTHER;
		}
	}	

	ipv4Info.nErrorStyle = IPv4ERRSTYLE_NOERROR;

	return;
}
