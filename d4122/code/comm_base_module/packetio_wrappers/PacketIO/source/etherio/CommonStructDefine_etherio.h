/////////////////////////////////////////////////////////////////
//	creater:		someone
//	filename:		CommonStructDefine_etherio.h
//	describe:		���Ͳ���ͨ�ýṹ����
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

// ���Ͳ���
typedef struct _ST_RECV_CREATE_PARA_
{
	DWORD dwMemberLst;	// ��Ա
	DWORD dwBuffLen;
	PVOID hEvent;		//�����ϲ��֪�ײ������������ݣ������ϲ����ȴ��Ϳ��Բ��úܳ�ʱ��
	DWORD watermark;

	DWORD dwTeamIndex;	// ����(��������)���ٴ����ڴ棬�ͷŵȲ���ʱ��Ҫʹ�õ�
	PUCHAR pUserAddr;	// ��������ַ(��������)
}ST_RECV_CREATE_PARA;

// Ӧ�ó����趨���Ͳ�����ʱ��ʹ��
// ���Ͳ����ṹ
// #define	SEND_MODE_CYCLE		0
// #define SEND_MODE_TIMES		1
// #define MAX_SEND_MODE		1
// 
// #define MAX_SEND_SPEED_KB	1024*100		// 100M = 100*1024

// 
// �������������ͷ�ʽ
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
	// �����
	IN DWORD				dwCardIndex;

	// ����ģʽ��FIFO/BLOCK/FIFO_TIMESTAMP/BLOCK_TIMESTAMP
	IN EN_SEND_MODE		enSendMode;

	// FIFO_TIMESTAMP/BLOCK_TIMESTAMPģʽ�£�����ֶ�ָ��ʱ����ľ���
	//	0 == EN_TIME_STAMP_MODE_NONE, 1=low, 2=high
//	IN EN_TIME_MODE		enTimeStampMode;

	// FIFO/FIFO_TIMESTAMPģʽ�£�����ֶ�ָ��ָ����������С
	IN DWORD				dwBufferLen;

	// FIFO/FIFO_TIMESTAMPģʽ�£�����ֶ�����������������û��ռ�ĵ�ַ
	OUT PVOID			pUserAddress;

	// �Ƿ�ʹ��LoopBack�ķ���
	IN DWORD				dwDontLoopBack;

	//�������������еİ��������ڸ�watermark ʱ��֪ͨ�ں��߳�
	IN DWORD				watermark;


	IN DWORD				bWaitPacketEvent; 

	// windows ʹ��
	// ��ETHERIO_OPEN_CONTEXTŲ������
	//�����Ѿ������ˣ�д����������źŻ��Ѷ���
	IN OUT HANDLE		sendEventNotEmpty;


}SEND_CREATE_PARA;


// ����ʱ�����ڴ��еİ����Ͷ���
// #define PACKET_TYPE_INVALID			0
// #define PACKET_TYPE_VIRTUAL			-1
// #define PACKET_TYPE_TIMED			-2
// #define PACKET_TYPE_MEM_ERROR		-3
// #define PACKET_TYPE_PKT_ERROR		-4

// block ����ʱ���ô���������ʹ��
#define MAX_TX_FRAME_NUMBER		256
#define MAX_TX_FRAME_LENGTH		2048
//#define MAX_TX_FRAME_TIMES		0x0FFFFFFF

/*
typedef struct _ST_BLOCK_SEND_DATA_
{
	ULONG	dwCardIndex;	// ��APP�������������壬DLL�������ָ�������Ǹ���

	ULONG	dwTxFrameCount;	//����֡����
	ULONG	dwTxFrameLength[MAX_TX_FRAME_NUMBER];	//����֡����
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

	DWORD	dwCardIndex;	// ��APP�������������壬DLL�������ָ�������Ǹ���

	DWORD	dwTxFrameCount;	//����֡����
	DWORD	dwTxFrameLength[MAX_TX_FRAME_NUMBER];	//����֡����
	DWORD	dwTimeStampOffset[MAX_TX_FRAME_NUMBER]; // ����ʱ���ƫ��λ��
	DWORD	TxBuffer[MAX_TX_FRAME_NUMBER][MAX_TX_FRAME_LENGTH];
}BLOCK_SEND_DATA_STAMP;

typedef struct _ST_BLOCK_SEND_PARA_
{
	DWORD	dwCardIndex;		// ��APP�������������壬DLL�������ָ�������Ǹ���

	DWORD	dwSendMode;			//0=ѭ�����ͣ�1=���������ͣ�(2=��ʱ�䷢�ͣ���֧��)
	DWORD	dwSendSpeed;		//���͵����ʣ�0Ϊ�����ٷ��ͣ���λ��kbps���ɴ˼��㷢�͵ļ��.��Χ��100Mbps~1Kbps
	DWORD	dwCycleCount;		//���͵Ĵ��������ѭ�����ͣ����趨��Ч;��Χ��2^24~1
	//	DWORD	dwSendTotalTimes;	//��ʱ�䷢�͵���ʱ�䣻�ɴ������㷢�͵Ĵ�������λΪms����Χ��1Hour~1ms
} BLOCK_SEND_PARA;


// ���������е�ͳ����Ϣ
#define DRIVER_STSS_INFO_QUERY	0
#define DRIVER_STSS_INFO_CLEAR	1

#pragma pack(pop)

#endif//_COMMON_STRUCT_DEFINE_ETHERIO_H_2014_11_07