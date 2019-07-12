/*******************************************************************************
  CapSave.h

	创 建 者:	孙向光
	创建时间:	2002/8/20 
	内容描述：  捕获文件头相关定义。


		文件结构：

┌─────────────┬──┬──┬──┬────────┬────┬──┬─────┐
│文件头（CAPFILE_HEAD结构）│帧1 │... │帧n │帧结束标志0xFFFF│索引值1 │... │ 索引值m  │
└─────────────┴──┴──┴──┴────────┴────┴──┴─────┘
			
		索引结构：

		每隔nIndexStep个帧建立一个索引值，索引帧序号＝nIndexStep*当前索引下标，
		索引值为该帧在文件中的位置。索引值为0时比较特殊，不是帧在文件中的实际位置，
		该帧应该在文件头后。


	修改记录：
	修改时间	    修改者		修改内容

*********************************************************************************/


#ifndef _CAPFILEHEAD_H_
#define _CAPFILEHEAD_H_
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"
#include "cpf/TimeStamp32.h"


#if defined(OS_WINDOWS)
#define _MAX_ULONGLONG  0xffffffffffffffff
#define _MIN_ULONGLONG  0x0
#else
#define _MAX_ULONGLONG  0xffffffffffffffffull
#define _MIN_ULONGLONG  0x0ull
#endif
#define _MAX_DWORD      0xffffffff
#define _MIN_DWORD      0x0
#define _MAX_WORD	0xffff
#define _MIN_WORD	0x0
#define _MAX_SHORT	0x7fff
#define _MIN_SHORT	(-0x7fff)
#define _MAX_LONG       0x7fffffff
#define _MIN_LONG       (-0x7fffffff)
#define _MAX_INT        0x7fffffff
#define _MIN_INT        (-0x7fffffff)

#if defined(OS_LINUX)
typedef void *				HANDLE;
typedef int               	FILE_ID;
#define ZCTT_INVALID_HANDLE	(-1)
#define CLOSEFILE(x)		close(x)
#define CLOSE_MAP(x)		
#elif defined(OS_WINDOWS)

typedef HANDLE             	FILE_ID;
#define ZCTT_INVALID_HANDLE	INVALID_HANDLE_VALUE
#define CLOSEFILE(x)		CloseHandle(x)
#define CLOSE_MAP(x)		CloseHandle(x)	
typedef LONGLONG            off64_t;
typedef LONGLONG            loff_t;
#endif//#if defined(OS_LINUX)

#define FILENAME_LEN1AND2	100
#define FILENAME_LEN3		50

#define CAPFILE_ID1		"ZCXC CPT 1"
#define CAPFILE_ID2		"ZCXC CPT 2"
#define CAPFILE_ID3		"ZCXC CPT 3"
#define CAPFILE_ID4		"ZCXC CPT 4"

#define MAX_RAW_PACKETLEN		4096    //最大原始包长度

#pragma pack(1)

typedef struct
{
	BYTE 			btHeaderLength;//HEADER_INFO
	BYTE			btVersion;
	WORD			wdProto;
	BYTE 			btHardwareType;
	BYTE 			btBureauNo;
	BYTE 			btFrontMachineNo;
	BYTE 			btCardNo;
	BYTE 			btInterfaceNo;

	struct
	{
		BYTE Ex:1;
		BYTE Tx:1;
		BYTE Rx:1;
		BYTE Re:1;
		BYTE L:1;
		BYTE Ts:1;
		BYTE Sp1:1;
		BYTE Sp2:1;
	} btFlag;

	DWORD 			dwError;
	TIMESTAMP32 	stamp;

	void* 			pHeaderSpec;//也要存盘

	
}HEADER_INFO;

#pragma pack()

typedef struct	tagPACKET_CONTEXT
{
	tagPACKET_CONTEXT()
	{
		dwLengthPacket = 0;
		pPacket = NULL;
		dwOffsetBit = 0;
		dwAddiBitLen = 0;
	}

	DWORD			dwLengthPacket;	//数据包的字节长度
	BYTE*  			pPacket;			//数据包的首地址
	HEADER_INFO*	pHeaderInfo;		//数据包头部信息

	DWORD			dwOffsetBit;//为了支持3G的位解码，上层协议的数据的开始位置是pPacket＋dwOffsetBit(位)
	DWORD			dwAddiBitLen;//为了支持3G的位解码，上层协议的数据长度是(dwLengthPacket*8+dwAddiBitLen)(位）


}PACKET_CONTEXT;


#pragma pack(1)


typedef struct 
{
	char	ID[10];						// 捕获文件格式标识 CAPFILE_ID1
	ACE_UINT32	timeBegin;					// 开始捕捉的时间
	ACE_UINT32	timeEnd;					// 停止捕捉的时间
	BOOL	bSerialFiles;				// 是否是一系列的文件中的一个
	DWORD	dwPacketCount;				// 记录的包的个数
	int		nNo;						// 是第几个文件,从0开始
	char	PrevFileName[FILENAME_LEN3]; // 前一个文件名
	char	NextFileName[FILENAME_LEN3]; // 后一个文件名
	DWORD	dwIndexStep;				// 多少个帧建立一个索引
	DWORD	dwIndexCount;				// 索引值个数
	DWORD	dwIndexPosInFile;			// 文件中存放索引的开始位置
	BOOL	bFileCreating;				// 文件正在创建中，有一个写对象在使用它
	DWORD	dwAddtionDataCount;			// 附加数据个数
	DWORD	dwAddtionDataPos;			// 附加数据开始位置
	char	Reserved[100];				// 保留,必须以0填充
	WORD	dwHeadCheckSum;				// 头的校验和,使用函数CPTFileHeadComputeCheckSum
} CAPFILE_HEAD;		// 捕获文件头格式，Size=256字节

#pragma pack()

#define CAPFILE_FRAME_END_FLAG	0xFFFF			// 包结束标志
#define CPTFILE_FRAME_END_STRING "CPT PKT END"	// 包结束串,CPT 版本 3 以后使用,跟在CAPFILE_FRAME_END_FLAG后
#define CPTFILE_FRAME_END_STRING_LEN	12		// 包结束串的长度,CPT 版本 3 以后使用

// 每次写入包后最小剩余空间（如果小于它就认为文件满）
// 其中包括对齐浪费4、CPTFILE_FRAME_END_STRING的长度和包结束标志2等
#define CPTFILE_MIN_FREE	(CPTFILE_FRAME_END_STRING_LEN+8)	

// CPT文件中的最大附加数据量 100 Mb。写入包到CPT文件中时，
// 如果到达4G-CPTFILE_MAX_ADDTION_DATA，也算文件写满。
// 以便以后加入附加数据后，该文件也保持 4 GB 以内
// 也就是说，没有附加数据的CPT文件最大为 4GB - CPTFILE_MAX_ADDTION_DATA
// 有附加数据的一个CPT文件不会超过 4GB。记录超过4GB的数据可以使用
// 连续记录多个CPT文件的方法。 
#define CPTFILE_MAX_ADDTION_DATA	(100*1024*1024) 

#define DEFAULT_INDEX_STEP	1000		// 缺省多少个帧建立一个索引



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
#endif//_CAPFILEHEAD_H_

