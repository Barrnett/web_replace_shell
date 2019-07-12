#ifndef _PACKET_IO_H_2006_05_09
#define _PACKET_IO_H_2006_05_09
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"
#include "cpf/CommonMacTCPIPDecode.h"

#if defined(PACKETIO_EXPORTS)
#define PACKETIO_API		MPLAT_API_Export_Flag
#define PACKETIO_CLASS		MPLAT_CLASS_Export_Flag
#else
#define PACKETIO_API		MPLAT_API_Import_Flag
#define PACKETIO_CLASS		MPLAT_CLASS_Import_Flag
#endif

typedef enum
{
	DRIVER_TYPE_MULTI_FILE = -2,
	DRIVER_TYPE_NONE = -1,
	DRIVER_TYPE_WINPCAP_NET = 0,
	DRIVER_TYPE_WINPCAP_TAP = 1,
	DRIVER_TYPE_FILE_ETHEREAL = 2,
	DRIVER_TYPE_FILE_ZCCPT = 3,
	DRIVER_TYPE_ETHERIO = 4,
	DRIVER_TYPE_MEMORY = 5,
	DRIVER_TYPE_Pagefile_Memory_Pool = 6,
	DRIVER_TYPE_EMPTY = 7,

}DRIVER_TYPE;

#include "cpf/TimeStamp32.h"

#pragma pack(push)
#pragma pack(1)

typedef struct 
{
	BYTE 			btHeaderLength;//HEADER_INFO
	BYTE			btVersion;
	WORD			wdProto;
	BYTE 			btHardwareType;
	BYTE 			btCardNo;
	BYTE 			btInterfaceNo;
	BYTE 			btReserved;

/*
	struct
	{

		ACE_UINT32 Tx:1;
		ACE_UINT32 Rx:1;

	} nFlag;
*/
	ACE_UINT32		nFlag;
	ACE_UINT32		dwError;
	Time_Stamp		stamp;
} PACKET_HEADER;

//����ṹ���Ǵ洢���ļ��нṹ���Ǵ��ļ��������󣬻ָ������Ľṹ��
typedef struct tagRECV_PACKET
{
	PACKET_HEADER *	pHeaderinfo;

	void *			pPacket;

	int	nPktlen; //���ݰ��ı�������
	int	nCaplen;//���ݰ���ʵ�ʲ�����ߴ洢����


	////�ϲ�Э������ݵĿ�ʼλ����pPacket��nOffsetBit(λ)
	int nOffsetBit;

	//�ϲ�Э������ݳ�����(nPktlen*8+ nPacketAddiBitLen)(λ��
	int			nPktAddiBitLen;

	//�ϲ�Э���ʵ�ʲ��񳤶���(nCaplen*8+ nCapAddiBitLen)(λ��
	int			nCapAddiBitLen;

	int			nReserved;

}RECV_PACKET;

#pragma pack(pop)

typedef enum
{
	//����һ����ʱ���ݰ����ڴ�Ӳ����ȡ���ݵ�ʱ��ÿ��һ���Ӳ�����ôһ�����ݰ�
	//���ص����ݽṹ�У�Time_Stamp ts��Ч�������Ķ�����Ч�ġ�
	PACKET_TIMER=-1,

	//�������Ч�����ص����ݽṹ����ȫ����Ч
	//��Ҫ�����ڵײ�忨�����ݱ�����ʱ������
	//Ӧ�ó���Ӧ��sleep(5)���ͷ�cpu��Դ
	PACKET_EMPTY=0,

	//�����һ����������Ч���ݰ������ص����ݽṹ����ȫ����Ч
	//Ӧ�ó������ʹ��������ݡ�����ע�ⲻҪԽ��ʹ��
	PACKET_OK = 1,

	//�������Ч�����ص����ݽṹ����ȫ����Ч
	//���ݰ�ȫ�����������ڴ��ļ���ȡ���ݵ�ʱ�򣬱�ʾ������ݶ�������
	PACKET_NO_MORE = 2,//�Ժ�û�����ݰ���

	//����������
	PACKET_DRV_ERROR = 3,

	//�������Ч�����ص����ݽṹ����ȫ����Ч
	//��Ҫ����������һЩ���ơ�
	//Ӧ�ó����ڵõ��������ֵ�󣬲�Ҫ����sleep���ͷ���Դ��Ӧ��������������getpacket.
	PACKET_CALL_NEXT = 4,

	PACKET_SERIAL_EMPTY = 10,
	PACKET_SERIAL_OK = 11,

}PACKET_LEN_TYPE;

//���������ڴ�Ĵ�С,�������ֽڶ��롣
//���pbufferΪNULL,��ֻ���������ڴ�Ĵ�С�������п���
PACKETIO_CLASS int Package_OnePacket(OUT void * pbuffer,
								   IN PACKET_LEN_TYPE type,
								   IN const RECV_PACKET& packet);
//���������ڴ�Ĵ�С,�������ֽڶ���
PACKETIO_CLASS int UnPackage_OnePacket(IN const void * pbuffer,
									 OUT PACKET_LEN_TYPE& type,
									 OUT RECV_PACKET*& packet);

//���������ڴ�Ĵ�С,�������ֽڶ��롣
//���pbufferΪNULL,��ֻ���������ڴ�Ĵ�С�������п���
PACKETIO_CLASS int Package_OnePacket(OUT void * pbuffer,
								   IN PACKET_LEN_TYPE type,
								   IN const RECV_PACKET& packet,
								   IN const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
								   IN int attach_len,
								   IN const void * attach_data);

//����Package_OnePacket�Ĵ�����뿴��������ʵ��
PACKETIO_CLASS void UnPackage_OnePacket(IN OUT RECV_PACKET& packet,
									  IN OUT CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
									  OUT int& attach_len,
									  OUT void*& attach_data);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_PACKET_IO_H_2006_05_09
