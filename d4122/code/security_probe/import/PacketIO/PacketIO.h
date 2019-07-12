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

//这个结构不是存储在文件中结构，是从文件读出来后，恢复出来的结构。
typedef struct tagRECV_PACKET
{
	PACKET_HEADER *	pHeaderinfo;

	void *			pPacket;

	int	nPktlen; //数据包的本来长度
	int	nCaplen;//数据包的实际捕获或者存储长度


	////上层协议的数据的开始位置是pPacket＋nOffsetBit(位)
	int nOffsetBit;

	//上层协议的数据长度是(nPktlen*8+ nPacketAddiBitLen)(位）
	int			nPktAddiBitLen;

	//上层协议的实际捕获长度是(nCaplen*8+ nCapAddiBitLen)(位）
	int			nCapAddiBitLen;

	int			nReserved;

}RECV_PACKET;

#pragma pack(pop)

typedef enum
{
	//这是一个定时数据包，在从硬件读取数据的时候，每隔一秒钟产生这么一个数据包
	//返回的数据结构中，Time_Stamp ts有效，其他的都是无效的。
	PACKET_TIMER=-1,

	//这个包无效，返回的数据结构内容全部无效
	//主要用于在底层板卡的数据被读暂时读空了
	//应用程序应该sleep(5)来释放cpu资源
	PACKET_EMPTY=0,

	//这个是一个正常的有效数据包，返回的数据结构内容全部有效
	//应用程序可以使用这个数据。但是注意不要越界使用
	PACKET_OK = 1,

	//这个包无效，返回的数据结构内容全部无效
	//数据包全部结束，用于从文件读取数据的时候，表示这个数据读结束了
	PACKET_NO_MORE = 2,//以后没有数据包了

	//驱动出错了
	PACKET_DRV_ERROR = 3,

	//这个包无效，返回的数据结构内容全部无效
	//主要用于其它的一些控制。
	//应用程序在得到这个返回值后，不要调用sleep来释放资源，应该立即继续调用getpacket.
	PACKET_CALL_NEXT = 4,

	PACKET_SERIAL_EMPTY = 10,
	PACKET_SERIAL_OK = 11,

}PACKET_LEN_TYPE;

//返回所需内存的大小,不计算字节对齐。
//如果pbuffer为NULL,则只返回所需内存的大小。不进行拷贝
PACKETIO_CLASS int Package_OnePacket(OUT void * pbuffer,
								   IN PACKET_LEN_TYPE type,
								   IN const RECV_PACKET& packet);
//返回跳过内存的大小,不计算字节对齐
PACKETIO_CLASS int UnPackage_OnePacket(IN const void * pbuffer,
									 OUT PACKET_LEN_TYPE& type,
									 OUT RECV_PACKET*& packet);

//返回所需内存的大小,不计算字节对齐。
//如果pbuffer为NULL,则只返回所需内存的大小。不进行拷贝
PACKETIO_CLASS int Package_OnePacket(OUT void * pbuffer,
								   IN PACKET_LEN_TYPE type,
								   IN const RECV_PACKET& packet,
								   IN const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
								   IN int attach_len,
								   IN const void * attach_data);

//解码Package_OnePacket的打包。请看函数具体实现
PACKETIO_CLASS void UnPackage_OnePacket(IN OUT RECV_PACKET& packet,
									  IN OUT CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
									  OUT int& attach_len,
									  OUT void*& attach_data);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_PACKET_IO_H_2006_05_09
