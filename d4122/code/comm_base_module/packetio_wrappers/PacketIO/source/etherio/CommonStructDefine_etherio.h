/////////////////////////////////////////////////////////////////
//	creater:		someone
//	filename:		CommonStructDefine_etherio.h
//	describe:		发送部分通用结构定义
//	create data:	2003-06-17
/////////////////////////////////////////////////////////////////

#ifndef _COMMON_STRUCT_DEFINE_ETHERIO_H_2014_11_07
#define _COMMON_STRUCT_DEFINE_ETHERIO_H_2014_11_07

#pragma once

////////////////////////////////////////////////////////////////////////

#include "CommonStructDefine_base.h"


#pragma pack(push)
#pragma pack(1)

////////////////////////////////////////////////////////////////////////

// 发送参数
typedef struct _ST_RECV_CREATE_PARA_
{
	DWORD dwMemberLst;	// 成员
	DWORD dwBuffLen;
	PVOID hEvent;		//用于上层获知底层驱动来了数据，这样上层程序等待就可以不用很长时间
	DWORD watermark;

	DWORD dwTeamIndex;	// 组编号(驱动返回)，再创建内存，释放等操作时需要使用到
	PUCHAR pUserAddr;	// 缓冲区地址(驱动返回)
}ST_RECV_CREATE_PARA;

// 应用程序设定发送参数的时候使用
// 发送参数结构
// #define	SEND_MODE_CYCLE		0
// #define SEND_MODE_TIMES		1
// #define MAX_SEND_MODE		1
// 
// #define MAX_SEND_SPEED_KB	1024*100		// 100M = 100*1024

// 
// 创建缓冲区发送方式
//
typedef struct _ST_SEND_CREATE_PARA_
{
#ifdef __cplusplus
	_ST_SEND_CREATE_PARA_()
	{	reset(0);	}
	_ST_SEND_CREATE_PARA_(UINT index)
	{	reset(index);	}
	void reset(UINT index)
	{
		dwCardIndex = index;
		enSendMode = EN_SENDMODE_FIFO;
		dwBufferLen = 0x500000;
		pUserAddress = 0;
		watermark = 3;

		bWaitPacketEvent = TRUE;
	}
#endif
	// 卡编号
	IN DWORD				dwCardIndex;

	// 发送模式，FIFO/BLOCK/FIFO_TIMESTAMP/BLOCK_TIMESTAMP
	IN EN_SEND_MODE		enSendMode;

	// FIFO_TIMESTAMP/BLOCK_TIMESTAMP模式下，这个字段指定时间戳的精度
	//	0 == EN_TIME_STAMP_MODE_NONE, 1=low, 2=high
//	IN EN_TIME_MODE		enTimeStampMode;

	// FIFO/FIFO_TIMESTAMP模式下，这个字段指定指定缓冲区大小
	IN DWORD				dwBufferLen;

	// FIFO/FIFO_TIMESTAMP模式下，这个字段用来输出缓冲区在用户空间的地址
	OUT PVOID			pUserAddress;

	// 是否不使用LoopBack的发送
	IN DWORD				dwDontLoopBack;

	//当发包缓冲区中的包数不高于该watermark 时，通知内核线程
	IN DWORD				watermark;


	IN DWORD				bWaitPacketEvent; 

	// windows 使用
	// 从ETHERIO_OPEN_CONTEXT挪过来的
	//队列已经不空了，写入者用这个信号唤醒读者
	IN OUT HANDLE		sendEventNotEmpty;


}SEND_CREATE_PARA;


// 接收时共享内存中的包类型定义
// #define PACKET_TYPE_INVALID			0
// #define PACKET_TYPE_VIRTUAL			-1
// #define PACKET_TYPE_TIMED			-2
// #define PACKET_TYPE_MEM_ERROR		-3
// #define PACKET_TYPE_PKT_ERROR		-4

// block 发送时设置待发送数据使用
#define MAX_TX_FRAME_NUMBER		256
#define MAX_TX_FRAME_LENGTH		2048
//#define MAX_TX_FRAME_TIMES		0x0FFFFFFF

/*
typedef struct _ST_BLOCK_SEND_DATA_
{
	ULONG	dwCardIndex;	// 在APP和驱动中无意义，DLL用这个来指定设置那个卡

	ULONG	dwTxFrameCount;	//数据帧个数
	ULONG	dwTxFrameLength[MAX_TX_FRAME_NUMBER];	//数据帧长度
	UCHAR	TxBuffer[MAX_TX_FRAME_NUMBER][MAX_TX_FRAME_LENGTH];
}BLOCK_SEND_DATA;
*/
typedef struct _ST_BLOCK_SEND_DATA_STAMP_
{
#ifdef __cplusplus	
	_ST_BLOCK_SEND_DATA_STAMP_()
	{
		memset(this, 0x0, sizeof(*this));	

		for(int i = 0; i < MAX_TX_FRAME_NUMBER; ++i)
		{
			dwTimeStampOffset[i] = (DWORD)-1;
		}
	}
#endif

	DWORD	dwCardIndex;	// 在APP和驱动中无意义，DLL用这个来指定设置那个卡

	DWORD	dwTxFrameCount;	//数据帧个数
	DWORD	dwTxFrameLength[MAX_TX_FRAME_NUMBER];	//数据帧长度
	DWORD	dwTimeStampOffset[MAX_TX_FRAME_NUMBER]; // 插入时间的偏移位置
	DWORD	TxBuffer[MAX_TX_FRAME_NUMBER][MAX_TX_FRAME_LENGTH];
}BLOCK_SEND_DATA_STAMP;

typedef struct _ST_BLOCK_SEND_PARA_
{
	DWORD	dwCardIndex;		// 在APP和驱动中无意义，DLL用这个来指定设置那个卡

	DWORD	dwSendMode;			//0=循环发送，1=按次数发送，(2=按时间发送，不支持)
	DWORD	dwSendSpeed;		//发送的速率，0为不限速发送，单位是kbps，由此计算发送的间隔.范围是100Mbps~1Kbps
	DWORD	dwCycleCount;		//发送的次数；如果循环发送，此设定无效;范围是2^24~1
	//	DWORD	dwSendTotalTimes;	//按时间发送的总时间；由此来计算发送的次数，单位为ms；范围是1Hour~1ms
} BLOCK_SEND_PARA;


// 驱动程序中的统计信息
#define DRIVER_STSS_INFO_QUERY	0
#define DRIVER_STSS_INFO_CLEAR	1

#pragma pack(pop)

#endif//_COMMON_STRUCT_DEFINE_ETHERIO_H_2014_11_07