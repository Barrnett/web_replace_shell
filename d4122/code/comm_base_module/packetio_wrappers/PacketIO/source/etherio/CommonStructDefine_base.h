/////////////////////////////////////////////////////////////////
//	creater:		someone
//	filename:		CommonStructDefine_base.h
//	describe:		���Ͳ���ͨ�ýṹ����
//	create data:	2003-06-17
/////////////////////////////////////////////////////////////////

#ifndef _COMMON_STRUCT_DEFINE_BASE_H_2014_11_04
#define _COMMON_STRUCT_DEFINE_BASE_H_2014_11_04



// ���в��԰忨�Ƿ�֧��ReceivePacketHandler�ص�����
//#define DEF_RECV_PACKET_TEST	1


// �ⲿ����win����ʹ��
#ifdef OS_WINDOWS
#	include "windef.h"
#endif


// �±���linux����ʹ��
#ifdef OS_LINUX

// �±���linux��������ʹ��
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

	//���ʹ����cent_os�£�ʹ��pf_ring����Ҫ����	#define iPF_PACKET 28
	//���ʹ����unbuntu�£���Ҫ����	#define iPF_PACKET PF_PACKET_1
	//�����Ҫ�ر�ע��

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
	EN_MAX_ETHER_NIC_NUM = 16,	// ϵͳ�����֧�ֵ�������
	EN_MAX_NAME_STRING_LEN = 256,
};

// ��ʱ�������ʹ��
#define _OFFSET_SEC        ((ULONGLONG)(400*365+97)*24*60*60-978278400-28800)
#define _OFFSET_100NS      (_OFFSET_SEC*10000000);   /*100ns units*/

#define _MIN_INTERFRAME_100NS_1000M	7 /*7us*/
#define _MIN_INTERFRAME_100NS_100M	67 /*7us*/
#define _MIN_INTERFRAME_100NS_10M	670 /*7us*/

#define PREAMBLE_BIT		8*8		// ��̫֡��ʼ�ֶ�
#define SPACE_BIT			12*8		// ��̫֡��ʼ�ֶ�
#define PREAMBLE_BYTE		8		// ��̫֡��ʼ�ֶ�
#define SPACE_BYTE			12		// ��̫֡��ʼ�ֶ�

#define TIME_20MS_100NS		200000
#define BYTE_CRC_FRAMEABLE_SPACE	(4+8+12)


// ��ʱ�������ʹ��

typedef enum _EN_TIME_MODE_
{
	EN_TIME_STAMP_MODE_NONE = 0,	// ��ʱ���
	EN_TIME_STAMP_MODE_LOW = 1,		// �;��ȣ�ֱ��ȥʱ�䣬�������ʱ��Ư��
	EN_TIME_STAMP_MODE_HIGH	= 2,	// �߾��ȣ�����δ��������������ʱ��Ư��
	EN_TIME_STAMP_MODE_LOW_NO,		// �;���ʱ��ꡣ����Ҳ���Ϊ����
	EN_TIME_STAMP_MODE_HIGH_NO,		// �߾���ʱ��ꡣ����Ҳ���Ϊ����

	EN_TIME_STAMP_MODE_RDTSC,		// ����x86 Cpu��rdtsc�ж϶�ȡʱ�䣬
	EN_TIME_STAMP_MODE_RDTSC_NO,

	EN_TIME_STAMP_MODE_LOW_TICK,		// �;��ȣ�����δ��������������ʱ��Ư��
	EN_TIME_STAMP_MODE_LOW_TICK_NO,	

}EN_TIME_MODE;

#pragma pack(push)
#pragma pack(1)

typedef struct _ST_FRAME_HEAD_
{
	UINT_PTR				OriginalLength;		// ԭʼ���ݰ����ȣ�����CRC��������FRAME_HEAD
	UINT_PTR				IncludeLength;		// ��Ч���ݳ��ȣ�����CRC��������FRAME_HEAD; (����ԭʼ���ݰ����ض���)
	LARGE_INTEGER			Timestamp;
	BYTE					card_index;
	BYTE					up_or_down;//0��ʾ���У�1��ʾ���С�
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

// ���Ͳ���

#define MAX_RECV_QUEUE_NUMS 32


typedef enum _EN_SEND_MODE_
{
	EN_SENDMODE_NONE = 0,
	EN_SENDMODE_FIFO = 1,
	EN_SENDMODE_BLOCK = 2,
	EN_SENDMODE_SINGLE = 3,		// �������ͷ�ʽ�����ַ�ʽ�£���Ҫ�������豸����

}EN_SEND_MODE;



// ���ط��͹����е�״̬��Ϣ
typedef struct _ST_GET_SEND_INFO_
{
	DWORD			iSendMode;			// 0-none, 1-fifo, 2-block, Ӧ�ò�����Ϊ0
	DWORD			iSendStatus;		// 0-stop, 1-start
	
	LARGE_INTEGER	llRequestFrames;	//׼�����͵İ���
	LARGE_INTEGER	llRequestBytes;

	LARGE_INTEGER	llFinishedFrames;//�Ѿ����͵İ���
	LARGE_INTEGER	llFinishedBytes;
	
}GET_SEND_INFO, *PGET_SEND_INFO;


// ö����������
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

// ���������е�ͳ����Ϣ
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

	ULONGLONG		recvFrames;		// �������յ����ܰ���
	ULONGLONG		recvBytes;		// �������յ������ֽ���

	ULONGLONG		lostFrames;		// ���������ĵ��ܰ���
	ULONGLONG		lostBytes;		// ���������ĵ����ֽ���

	// ��������Ķ�������, �͵��� lostFrames
	ULONGLONG		lostNotAllocMemory;	// û�д��������ڴ�
	ULONGLONG		lostHeadSize;		// ���ݰ�ͷ����
	ULONGLONG		lostBufferFull;		// �����������¶���
	ULONGLONG		lostResource;		// ϵͳ��Դ����

	// ���ýӿڵļ������͵���recvFrames
	ULONGLONG		recvHandle;		// ReceiveHandler �ص������յ��İ� = recvFrames
	ULONGLONG		recvPktHandle;	// ReceivePacketHandler �ص������յ��İ�

	// ���䷽ʽ�ļ������͵���recvHandle
	ULONGLONG		recvAllData;	// pLookaheadBuffer������������
	ULONGLONG		recvNeedTrans;	// �������NdisTransferData��ȡ����

} DRIVER_STSS_INFO,*PDRIVER_STSS_INFO;

// ���Ͳ��ֵ�ͳ��
typedef struct _EN_SEND_STSS_INFO_
{
	ULONGLONG		sendTooLong;	// �����͵����ݰ����ȴ�����������󳤶�
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
	EN_TIME_MODE	enTimeStampMode;	// EN_TIME_STAMP_MODE_NONE LOW HIGH�е�һ��
}EN_SET_TIME_MODE;


#pragma pack(pop)

#endif//_COMMON_STRUCT_DEFINE_BASE_H_2014_11_04

