/////////////////////////////////////////////////////////////////
//	creater:		someone
//	filename:		CommonStructDefine_base.h
//	describe:		发送部分通用结构定义
//	create data:	2003-06-17
/////////////////////////////////////////////////////////////////

#ifndef _COMMON_STRUCT_DEFINE_BASE_H_2014_11_04
#define _COMMON_STRUCT_DEFINE_BASE_H_2014_11_04



// 进行测试板卡是否支持ReceivePacketHandler回调函数
//#define DEF_RECV_PACKET_TEST	1


// 这部分是win编译使用
#ifdef OS_WINDOWS
#	include "windef.h"
#endif


// 下边是linux编译使用
#ifdef OS_LINUX

// 下边是linux驱动编译使用
#	ifdef OS_LINUX_DLL
#		include <string.h>
#		include <linux/stddef.h>
#	endif // OS_LINUX_DLL

	#define IN
	#define OUT

	#define FALSE 0
	#define TRUE  1

///packet family macro --  tmp

	#define PF_PACKET_1 37
	#define PF_PACKET_2 38
	#define PF_PACKET_3 39
	#define PF_PACKET_4 40
	#define PF_PACKET_5 41

	//如果使用在cent_os下，使用pf_ring，需要定义	#define iPF_PACKET 28
	//如果使用在unbuntu下，需要定义	#define iPF_PACKET PF_PACKET_1
	//这个需要特别注意

	#define iPF_PACKET 28	//PF_PACKET_1

	#define PACKED  __attribute__ ((packed))

	// Put platform dependent declaration here
	// For example, linux type definition
	typedef unsigned char		UINT8;
	typedef unsigned short		UINT16;
	typedef unsigned int		UINT32;
	typedef unsigned long long	UINT64;
	typedef int				INT32;
	typedef long long 			INT64;

	typedef unsigned char *		PUINT8;
	typedef unsigned short *		PUINT16;
	typedef unsigned int *			PUINT32;
	typedef unsigned long long *	PUINT64;
	typedef int	*				PINT32;
	typedef long long * 			PINT64;

	typedef signed char			CHAR;
	typedef signed short		SHORT;
	typedef signed int			INT;
	typedef signed long			LONG;


	typedef unsigned char		BYTE;
	typedef unsigned short		USHORT;
	typedef unsigned int		UINT;
	typedef unsigned long		ULONG;
#if !( (defined OS_LINUX) && (defined _X64)  )
	typedef unsigned long long	ULONGLONG;
	typedef signed long long	LONGLONG;
#endif


	typedef unsigned char		BOOLEAN;
	typedef void				VOID;

	typedef VOID *				PVOID;
	typedef CHAR *				PCHAR;
	typedef USHORT *			PUSHORT;
	typedef LONG *				PLONG;
	typedef ULONG *				PULONG;
	typedef UINT *				PUINT;

	typedef unsigned int 			DWORD;
	typedef int             			BOOL;

	typedef unsigned int	NDIS_MEDIA_STATE;

#ifndef LARGE_INTEGER
	typedef union _LARGE_INTEGER {
		struct {
			DWORD	LowPart;
			DWORD	HighPart;
		} u;
		LONGLONG QuadPart;
	} LARGE_INTEGER;
#endif


	#ifndef ASSERT
	#	ifdef _DEBUG
	#		include "assert.h"
	#		define ASSERT(_x_)	assert(_x_)
	#	else
	#		define ASSERT(_x_)
	#	endif
	#endif

	typedef unsigned char		UCHAR;
	typedef UCHAR * 			PUCHAR;
	
	typedef unsigned long		UINT_PTR;
	typedef unsigned long		ULONG_PTR;
	typedef long				INT_PTR;
	typedef long				LONG_PTR;
	typedef void*				HANDLE;

	#define INVALID_HANDLE_VALUE -1

#endif	//#ifdef OS_LINUX

typedef struct
{
	DWORD sec; 
	DWORD ns;
} EIO_TIME;



//#define ASSERT(a)                     if (!(a)) *(char *)0 = 0

enum
{
	EN_MAX_ETHER_NIC_NUM = 16,	// 系统中最大支持的网卡数
	EN_MAX_NAME_STRING_LEN = 256,
};

// 做时间戳调整使用
#define _OFFSET_SEC        ((ULONGLONG)(400*365+97)*24*60*60-978278400-28800)
#define _OFFSET_100NS      (_OFFSET_SEC*10000000);   /*100ns units*/

#define _MIN_INTERFRAME_100NS_1000M	7 /*7us*/
#define _MIN_INTERFRAME_100NS_100M	67 /*7us*/
#define _MIN_INTERFRAME_100NS_10M	670 /*7us*/

#define PREAMBLE_BIT		8*8		// 以太帧起始字段
#define SPACE_BIT			12*8		// 以太帧起始字段
#define PREAMBLE_BYTE		8		// 以太帧起始字段
#define SPACE_BYTE			12		// 以太帧起始字段

#define TIME_20MS_100NS		200000
#define BYTE_CRC_FRAMEABLE_SPACE	(4+8+12)


// 做时间戳调整使用

typedef enum _EN_TIME_MODE_
{
	EN_TIME_STAMP_MODE_NONE = 0,	// 无时间戳
	EN_TIME_STAMP_MODE_LOW = 1,		// 低精度，直接去时间，会随机器时间漂移
	EN_TIME_STAMP_MODE_HIGH	= 2,	// 高精度，计算滴答数，不会随机器时间漂移
	EN_TIME_STAMP_MODE_LOW_NO,		// 低精度时间辍，并且不人为调整
	EN_TIME_STAMP_MODE_HIGH_NO,		// 高精度时间辍，并且不人为调整

	EN_TIME_STAMP_MODE_RDTSC,		// 调用x86 Cpu的rdtsc中断读取时间，
	EN_TIME_STAMP_MODE_RDTSC_NO,

	EN_TIME_STAMP_MODE_LOW_TICK,		// 低精度，计算滴答数，不会随机器时间漂移
	EN_TIME_STAMP_MODE_LOW_TICK_NO,	

}EN_TIME_MODE;

#pragma pack(push)
#pragma pack(1)

typedef struct _ST_FRAME_HEAD_
{
	UINT_PTR				OriginalLength;		// 原始数据包长度，包含CRC，不包括FRAME_HEAD
	UINT_PTR				IncludeLength;		// 有效数据长度，包含CRC，不包括FRAME_HEAD; (可能原始数据包被截断了)
	LARGE_INTEGER			Timestamp;
	BYTE					card_index;
	BYTE					up_or_down;//0表示上行，1表示下行。
	BYTE					Reserved[6];	
}FRAME_HEAD,*PFRAME_HEAD;

typedef struct _ST_ETHERIO_OID_DATA_
{
	ULONG			CardIndex;
	ULONG           Oid;
	ULONG           Length;
	UCHAR           Data[sizeof(ULONG)];
}ETHERIO_OID_DATA, *PETHERIO_OID_DATA;

////////////////////////////////////////////////////////////////////////

// 发送参数

#define MAX_RECV_QUEUE_NUMS 32


typedef enum _EN_SEND_MODE_
{
	EN_SENDMODE_NONE = 0,
	EN_SENDMODE_FIFO = 1,
	EN_SENDMODE_BLOCK = 2,
	EN_SENDMODE_SINGLE = 3,		// 单包发送方式，这种方式下，需要打开网卡设备对象

}EN_SEND_MODE;



// 返回发送过程中的状态信息
typedef struct _ST_GET_SEND_INFO_
{
	DWORD			iSendMode;			// 0-none, 1-fifo, 2-block, 应该不可能为0
	DWORD			iSendStatus;		// 0-stop, 1-start
	
	LARGE_INTEGER	llRequestFrames;	//准备发送的包数
	LARGE_INTEGER	llRequestBytes;

	LARGE_INTEGER	llFinishedFrames;//已经发送的包数
	LARGE_INTEGER	llFinishedBytes;
	
}GET_SEND_INFO, *PGET_SEND_INFO;


// 枚举连接速率
typedef enum __ENUM_ETHERNET_LINK_SPEED__
{
	ETHERNET_LINK_DISCONNECT = 0,
	ETHERNET_LINK_SPEED_10M = 1,
	ETHERNET_LINK_SPEED_100M = 2,
	ETHERNET_LINK_SPEED_1000M = 3,
	ETHERNET_LINK_SPEED_10000M = 4
}ENUM_ETHERNET_LINK_SPEED;


#define MACRO_STSS_RECV(stss, len, type)	\
	stss.recvFrames ++;	\
	stss.recvBytes += len;	\
	stss.type ++;

#define MACRO_STSS_LOST(stss, len, cause)	\
	stss.lostFrames ++;	\
	stss.lostBytes += len;	\
	stss.cause ++;

// 驱动程序中的统计信息
#define DRIVER_STSS_INFO_QUERY	0
#define DRIVER_STSS_INFO_CLEAR	1
typedef struct _EN_DRIVER_STSS_INFO_
{
#ifdef __cplusplus
	_EN_DRIVER_STSS_INFO_()
	{
		memset(this, 0x0, sizeof(*this));
	}
	void AddIt(_EN_DRIVER_STSS_INFO_* b)
	{
		this->recvFrames += b->recvFrames;
		this->recvBytes += b->recvBytes;
		this->lostFrames += b->lostFrames;
		this->lostBytes += b->lostBytes;
		this->lostNotAllocMemory += b->lostNotAllocMemory;
		this->lostHeadSize += b->lostHeadSize;
		this->lostBufferFull += b->lostBufferFull;
		this->lostResource += b->lostResource;
		this->recvHandle += b->recvHandle;
		this->recvPktHandle += b->recvPktHandle;
		this->recvAllData += b->recvAllData;
		this->recvNeedTrans += b->recvNeedTrans;
	}
#endif
	DWORD			queue_index;
	DWORD			dwReserved;

	ULONGLONG		recvFrames;		// 驱动接收到的总包数
	ULONGLONG		recvBytes;		// 驱动接收到的总字节数

	ULONGLONG		lostFrames;		// 驱动丢掉的的总包数
	ULONGLONG		lostBytes;		// 驱动丢掉的的总字节数

	// 具体情况的丢包计数, 和等于 lostFrames
	ULONGLONG		lostNotAllocMemory;	// 没有创建共享内存
	ULONGLONG		lostHeadSize;		// 数据包头不对
	ULONGLONG		lostBufferFull;		// 缓冲区满导致丢包
	ULONGLONG		lostResource;		// 系统资源不足

	// 调用接口的计数，和等于recvFrames
	ULONGLONG		recvHandle;		// ReceiveHandler 回调函数收到的包 = recvFrames
	ULONGLONG		recvPktHandle;	// ReceivePacketHandler 回调函数收到的包

	// 传输方式的计数，和等于recvHandle
	ULONGLONG		recvAllData;	// pLookaheadBuffer包括所有数据
	ULONGLONG		recvNeedTrans;	// 必须调用NdisTransferData获取数据

} DRIVER_STSS_INFO,*PDRIVER_STSS_INFO;

// 发送部分的统计
typedef struct _EN_SEND_STSS_INFO_
{
	ULONGLONG		sendTooLong;	// 待发送的数据包长度大于网卡的最大长度
}EN_SEND_STSS_INFO;


typedef struct _EN_ENABLE_TIMED_
{
	int		iTeamIndex;
	BOOL	bEnable;
}EN_ENABLE_TIMED;

typedef struct _EN_SET_TIME_MODE_
{
	union
	{
		int			iTeamIndex;	// recv team index or 
		int			iCardIndex;	// send card index
	};
	EN_TIME_MODE	enTimeStampMode;	// EN_TIME_STAMP_MODE_NONE LOW HIGH中的一个
}EN_SET_TIME_MODE;


#pragma pack(pop)

#endif//_COMMON_STRUCT_DEFINE_BASE_H_2014_11_04

