// flux_control_base.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "flux_control_base.h"

#ifdef OS_WINDOWS

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#endif


//���������ڴ�Ĵ�С,�������ֽڶ��롣
//���pbufferΪNULL,��ֻ���������ڴ�Ĵ�С�������п���
int Package_OnePacketInfoEx(IN char * pbuffer,const PACKET_INFO_EX& packet_ex)
{
	char * orig_ptr = (char *)pbuffer;

	if( pbuffer )
	{
		*(ACE_UINT32 *)pbuffer = packet_ex.vm_id;
		pbuffer += sizeof(ACE_UINT32);

		*(int *)pbuffer = packet_ex.if_line_index;
		pbuffer += sizeof(int);

		*(PACKET_LEN_TYPE *)pbuffer = packet_ex.pkt_len_type;
		pbuffer += sizeof(PACKET_LEN_TYPE);

		*(DIR_IO *)pbuffer = packet_ex.dir_phy;
		pbuffer += sizeof(DIR_IO);

		memcpy(pbuffer,packet_ex.packet,sizeof(RECV_PACKET));
		pbuffer += sizeof(RECV_PACKET);

		memcpy(pbuffer,packet_ex.packet->pHeaderinfo,sizeof(PACKET_HEADER));
		pbuffer += sizeof(PACKET_HEADER);
	
		memcpy(pbuffer,packet_ex.packet->pPacket,packet_ex.packet->nCaplen);
		pbuffer += packet_ex.packet->nCaplen;
		
		memcpy(pbuffer,&packet_ex.tcpipContext,sizeof(packet_ex.tcpipContext));
		pbuffer += sizeof(packet_ex.tcpipContext);

		//ע�ⲻ�ܰ�ʱ���������ʱ��Ӧ�õ���Ҫ���͵�ʱ�򣬰��յ�ʱ��ʱ����лָ�
		return (int)(pbuffer-orig_ptr);
	}
	else
	{
		//*(ACE_UINT32 *)pbuffer = packet_ex.vm_id;
		pbuffer += sizeof(ACE_UINT32);

		//*(int *)pbuffer = packet_ex.if_line_index;
		pbuffer += sizeof(int);

		//*(PACKET_LEN_TYPE *)pbuffer = packet_ex.pkt_len_type;
		pbuffer += sizeof(PACKET_LEN_TYPE);

		//*(DIR_IO *)pbuffer = packet_ex.dir_phy;
		pbuffer += sizeof(DIR_IO);

		//memcpy(pbuffer,*packet_ex.packet,sizeof(RECV_PACKET));
		pbuffer += sizeof(RECV_PACKET);

		//memcpy(pbuffer,packet_ex.packet->pHeaderinfo,sizeof(PACKET_HEADER));
		pbuffer += sizeof(PACKET_HEADER);

		//memcpy(pbuffer,packet_ex.packet->pPacket,packet_ex.packet->nCaplen);
		pbuffer += packet_ex.packet->nCaplen;


		//memcpy(pbuffer,&packet_ex.tcpipContext,sizeof(packet_ex.tcpipContext));
		pbuffer += sizeof(packet_ex.tcpipContext);

		//ע�ⲻ�ܰ�ʱ���������ʱ��Ӧ�õ���Ҫ���͵�ʱ�򣬰��յ�ʱ��ʱ����лָ�

		return (int)(pbuffer-orig_ptr);
	}
}

//���������ڴ�Ĵ�С,�������ֽڶ���
int UnPackage_OnePacketInfoEx(IN const char * pbuffer,PACKET_INFO_EX& packet_ex,
							  const CTimeStamp32& cur_ts)
{
	const char * orig_ptr = (const char *)pbuffer;

	packet_ex.vm_id = *(ACE_UINT32 *)pbuffer;
	pbuffer += sizeof(ACE_UINT32);

	packet_ex.if_line_index = *(int *)pbuffer;
	pbuffer += sizeof(int);

	packet_ex.pkt_len_type = *(PACKET_LEN_TYPE *)pbuffer;
	pbuffer += sizeof(PACKET_LEN_TYPE);

	packet_ex.dir_phy = *(DIR_IO *)pbuffer;
	pbuffer += sizeof(DIR_IO);

	packet_ex.packet = (RECV_PACKET * )pbuffer;
	pbuffer += sizeof(RECV_PACKET);

	packet_ex.packet->pHeaderinfo = (PACKET_HEADER *)pbuffer;
	pbuffer += sizeof(PACKET_HEADER);

	packet_ex.packet->pPacket = (void *)pbuffer;
	pbuffer += packet_ex.packet->nCaplen;

	memcpy(&packet_ex.tcpipContext,pbuffer,sizeof(packet_ex.tcpipContext));
	pbuffer += sizeof(packet_ex.tcpipContext);

	CCommonMacTCPIPDecode::RecalDataPointer(packet_ex.tcpipContext,(const BYTE *)packet_ex.packet->pPacket);

	packet_ex.tsCurPkt = cur_ts;
	packet_ex.packet->pHeaderinfo->stamp.sec = cur_ts.GetSEC();
	packet_ex.packet->pHeaderinfo->stamp.ns = cur_ts.GetNS();

	return (int)(pbuffer-orig_ptr);
}

