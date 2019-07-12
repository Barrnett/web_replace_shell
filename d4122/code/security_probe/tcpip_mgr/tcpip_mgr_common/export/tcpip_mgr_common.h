//////////////////////////////////////////////////////////////////////////
//tcpip_mgr_common.h

#pragma once

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"
#include "cpf/Data_Stream.h"

#if defined(TCPIP_MGR_COMMON_EXPORTS)
#define TCPIP_MGR_COMMON_API		MPLAT_API_Export_Flag
#define TCPIP_MGR_COMMON_CLASS		MPLAT_CLASS_Export_Flag
#else
#define TCPIP_MGR_COMMON_API		MPLAT_API_Import_Flag
#define TCPIP_MGR_COMMON_CLASS		MPLAT_CLASS_Import_Flag
#endif


typedef ACE_UINT64	TYPE_DID;

#define NULL_DID	((ACE_UINT64)0)
#define ALL_DID		((ACE_UINT64)(-1))

//比所有的MAC地址要大，为MAC和IP直接作为DID预留

#define MIN_REAL_DID	((ACE_UINT64)0xffffffffffff+1)

#define MIN_IP_DID		((ACE_UINT64)(0X01))
#define MAX_IP_DID		((ACE_UINT64)(0xffffffff))

#define MIN_MAC_DID		((ACE_UINT64)MAX_IP_DID+1)
#define MAX_MAC_DID		((ACE_UINT64)0xffffffffffff)

#define PUT_DID(pdata,did)	CData_Stream_LE::PutFixString(pdata,sizeof(TYPE_DID),(const char *)&did);
#define GET_DID(pdata,did) memcpy(&did,CData_Stream_LE::GetFixString(pdata,sizeof(TYPE_DID)),sizeof(TYPE_DID));

#define MAX_EVENT_LEVEL		5						//最大的告警事件级别；

#define VIRTUAL_HOST_IP		4294967295				//外网统一的主机虚拟IP地址(255.255.255.255)；

#define VIRTUAL_HOST_ACC	"EXTERNAL_USER"			//外网统一的主机虚拟帐号；

#define UNKNOWN_USER_NAME	"~unknown"
#define UNKNOWN_WUSER_NAME	L"~unknown"

/*
enum PROBE_IMP_TYPE
{
	// 互联网业务 //
	IMP_EXTER_UJ_DATA_TCP	= 100,					//Unjudge_DATA
	IMP_EXTER_UJ_DATA_UDP	= 101,					//Unjudge_DATA
	IMP_EXTER_UJ_SERVICE_TCP	= 102,				//Unjudge_SERVICE	
	IMP_EXTER_UJ_SERVICE_UDP	= 103,				//Unjudge_SERVICE

	IMP_EXTER_HTTP			= 1000,					//HTTP
	IMP_EXTER_WEB			= 1001,
	IMP_EXTER_WEBMAIL		= 1002,
	IMP_EXTER_WEIBO			= 1003,
	IMP_EXTER_BBS			= 1004,
	

	IMP_EXTER_POP3			= 1010,					//POP3
	IMP_EXTER_SMTP			= 1011,					//SMTP
	
	IMP_EXTER_FTP_CONTROL	= 1020,					//FTP
	IMP_EXTER_FTP_FILE		= 1021,
	
	IMP_EXTER_TELNET		= 1030,					//Telnet

	IMP_EXTER_MSN_MESS		= 1040,					//MSN
	IMP_EXTER_MSN_FILE		= 1041,

	IMP_EXTER_QQ_MESS		= 1050,					//QQ
	IMP_EXTER_QQ_FILE		= 1051,


	IMP_EXTER_YMSG			= 1060,					//yahoo通
	IMP_EXTER_YMSG_FILE		= 1061,

	IMP_EXTER_VOIP			= 1070,					//voip

	IMP_EXTER_TCP8080		= 1080,
	IMP_EXTER_FETION		= 1081,


	// P2P (1100～1149)
	IMP_EXTER_P2P_BASE		= 1100,					//P2P

	// 游戏 (1150～1199)
	IMP_EXTER_GAME_BASE		= 1200,					//Games

	//网络视频
	IMP_EXTER_NetVideo_BASE = 1300,

	// 自定义业务 (1700～1899)
	IMP_EXTER_USERDEF_BASE	= 1700,					//Undecode

	// 其它所有Unknown的协议；
	IMP_EXTER_UK_DATA_TCP	= 1900,					//Unknown_DATA
	IMP_EXTER_UK_DATA_UDP	= 1901,					//Unknown_DATA
	IMP_EXTER_UK_SERVICE_TCP	= 1902,				//Unknown_SERVICE	
	IMP_EXTER_UK_SERVICE_UDP	= 1903,				//Unknown_SERVICE

	// Uncare不关心的协议；
	IMP_EXTER_UNCARE		= 1910,

	//////////////////////////////////////////////////////////////////////////

	// 内网业务 //
	IMP_INTER_UJ_DATA_TCP	= 200,					//Unjudge_DATA
	IMP_INTER_UJ_DATA_UDP	= 201,					//Unjudge_DATA
	IMP_INTER_UJ_SERVICE_TCP	= 202,				//Unjudge_SERVICE	
	IMP_INTER_UJ_SERVICE_UDP	= 203,				//Unjudge_SERVICE


	IMP_INTER_HTTP			= 2000,					//HTTP
	IMP_INTER_WEB			= 2001,
	IMP_INTER_WEBMAIL		= 2002,

	IMP_INTER_POP3			= 2003,					//POP3
	IMP_INTER_SMTP			= 2004,					//SMTP

	IMP_INTER_FTP_CONTROL	= 2005,					//FTP
	IMP_INTER_FTP_FILE		= 2006,

	IMP_INTER_TELNET		= 2007,					//Telnet

	IMP_INTER_SMB			= 2008,					//SMB

	IMP_INTER_TCP8080		= 2009,


	// 数据库 (2100～2149)
	IMP_INTER_DB_BASE		= 2100,					//Database

	// 自定义业务 (2700～2899)
	IMP_INTER_USERDEF_BASE	= 2700,					//Undecode

	// 其它所有Unknown的协议；
	IMP_INTER_UK_DATA_TCP	= 2900,					//Unknown_DATA
	IMP_INTER_UK_DATA_UDP	= 2901,					//Unknown_DATA
	IMP_INTER_UK_SERVICE_TCP	= 2902,				//Unknown_SERVICE	
	IMP_INTER_UK_SERVICE_UDP	= 2903,				//Unknown_SERVICE

	// Uncare不关心的协议；
	IMP_INTER_UNCARE		= 2910,

};


// 协议的分组编号
enum PROBE_IMP_GROUPID
{
	IMP_GROUPID_HTTP	= 0,
	IMP_GROUPID_FTP,
	IMP_GROUPID_TELNET,
	IMP_GROUPID_MAIL,
	IMP_GROUPID_IM,
	IMP_GROUPID_SMB,
	IMP_GROUPID_DB,
	IMP_GROUPID_P2P,
	IMP_GROUPID_NetVideo,
	IMP_GROUPID_VOIP,
	IMP_GROUPID_GAME,
	IMP_GROUPID_STOCK,
	IMP_GROUPID_RCTRL,
	IMP_GROUPID_UK_DATA,
	IMP_GROUPID_UK_SERVICE,
	IMP_GROUPID_UJ_DATA,
	IMP_GROUPID_UJ_SERVICE,
	IMP_GROUPID_TCP8080,
	IMP_GROUPID_MAX
};

*/