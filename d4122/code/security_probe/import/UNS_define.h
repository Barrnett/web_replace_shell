#pragma once

//#include "cpf/ostypedef.h"

// 流量统计的宏, add by xumx 2016-8-13
//#define USE_TS_FLUX_STATISTIC

//#define TS_HTONL(X) (ACE_UINT32)ACE_HTONL(X)

//#define PRODUCT_Z4160

#define	FUNC_IP_CHANGE_NODITY	0

typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;
typedef unsigned long long	UINT64;
typedef int				INT32;
typedef long long 			INT64;


#define UNS_STAT_SHM_NAME		"/home/ts/stat"

#define MAX_LAN_NUM		4
#define MAX_E1_NUM			12
#define MAX_PEER_NUM		1
#define MAX_FLOW_NUM		100

#pragma pack(4)
typedef struct UNS_SYSTEM_INFO
{
	UINT8	nCpuRate;
	UINT8	nReserve[3];
	UINT32	nMemUsed;
	UINT32	nMemTotal;
	UINT32	nHddUsed;//G
	UINT32	nHddTotal;//G
	UINT32	nSysStartTime;
	UINT32	nSysRunTime;
}UNS_SYSTEM_INFO_S;

typedef struct UNS_LAN_STAT
{
	UINT8	nPhyStat;
	UINT8	nReserve[3];
}UNS_LAN_STAT_S;

typedef struct UNS_E1_STAT
{
	UINT8	nE1Stat;
	UINT8	nReserve[3];
}UNS_E1_STAT_S;

typedef struct UNS_PEER_STAT
{
	UINT32	nPeerId;
	UINT8	nMainStat;
	UINT8	nB1Stat;
	UINT8	nB2Stat;
	UINT8	nB3Stat;
	UINT8	nB4Stat;
	UINT8	nReserve[3];
}UNS_PEER_STAT_S;

typedef struct UNS_FLOW_STAT
{
	UINT32	nFlowId;
	UINT64	nUpBytes;
	UINT64	nUpPackets;
}UNS_FLOW_STAT_S;

//共享内存数据结构
typedef struct UNS_STAT_SHM
{
	UINT8	aLocalMgmtIf[4];//eth编号
	UINT8	aLocalMgmtIp[4];
	
	UNS_SYSTEM_INFO_S 	tagSysInfo;

	UINT32				nLanNum;
	UNS_LAN_STAT_S		tagLanStat[MAX_LAN_NUM];

	UINT32				nE1Num;
	UNS_E1_STAT_S		tagE1Stat[MAX_E1_NUM];

	UINT32				nPeerNum;
	UNS_PEER_STAT_S	tagPeerStat;

	UINT32				nFlowNum;
	UNS_FLOW_STAT_S	tagFlowStat[MAX_FLOW_NUM];
}UNS_STAT_SHM_S;

#pragma pack()

///////////
