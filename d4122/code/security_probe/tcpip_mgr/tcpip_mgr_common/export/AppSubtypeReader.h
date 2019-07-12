#pragma once

#include "cpf/ostypedef.h"
#include "tcpip_mgr_common.h"
#include <string>
#include "cpf/TinyXmlEx.h"
#include "cpf/THashFinderEx.h"
#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"

class TCPIP_MGR_COMMON_CLASS CAppSubtypeReader
{
public:
	typedef struct tagPROTO_TREE_ITEM
	{
		ACE_UINT32	app_id;
		
		int			bvisible;//是否可见

		char		app_name[32];//唯一的名字
		char		disp_name[64];//用于显示的名字

		tagPROTO_TREE_ITEM *				parent_item;
		std::vector<tagPROTO_TREE_ITEM *>	vt_sub_item;
		

	}PROTO_TREE_ITEM,*LPPROTO_TREE_ITEM;

public:
	CAppSubtypeReader(void);
	~CAppSubtypeReader(void);

public:
	//判断是否是业务大类
	static inline BOOL IsAppGrpType(ACE_UINT32 apptype)
	{
		return ((apptype%10000)==0);
	}

	//是否有孩子节点业务
	static BOOL HasChildAppType(ACE_UINT32 apptype);

	static size_t GetSubAppType(ACE_UINT32 apptype,std::vector<ACE_UINT32>* vt_subtype);

	//得到所有的组业务id
	static void GetAllExtGrpAppType(std::vector<ACE_UINT32>& vt_subtype);

	//由子业务id，得到对应的组业务id
	static ACE_UINT32 GetExtGrpAppType(ACE_UINT32 subappid);

	inline static BOOL IsExtAppSubType(ACE_UINT32 subappid)
	{
		return ( GetExtGrpAppType(subappid) != 0 );
	}


	static inline BOOL IsExUkSubAppType(ACE_UINT32 uVirtualServPort)
	{
		return (CAppSubtypeReader::APP_EX_UK_DATA_TCP==uVirtualServPort
			|| CAppSubtypeReader::APP_EX_UK_SERVICE_TCP==uVirtualServPort
			|| CAppSubtypeReader::APP_EX_UK_DATA_UDP==uVirtualServPort
			|| CAppSubtypeReader::APP_EX_UK_SERVICE_UDP==uVirtualServPort);
	}

	static inline BOOL IsExUkGrpAppType(ACE_UINT32 uVirtualServPort)
	{
		return (CAppSubtypeReader::APP_EX_UK_DATA==uVirtualServPort
			|| CAppSubtypeReader::APP_EX_UK_SERVICE==uVirtualServPort);
	}

	static inline BOOL IsUserDefSubApp(ACE_UINT32 subappid)
	{
		return ( subappid > CAppSubtypeReader::APP_EX_USERD 
			&& subappid <= (CAppSubtypeReader::APP_EX_USERD + 9999) );
	}	

public:
	//所有应用
	static const ACE_UINT32	ALL_APP_TYPE;

	static const ACE_UINT32 APP_EXTERN_NOT_JUDGE;

	//没有进行判断的业务，注意要和UNKNOWN区分开
	static const ACE_UINT32 APP_EX_UJ_DATA;		// Unjudge_data
		static const ACE_UINT32 APP_EX_UJ_DATA_TCP;		// Unjudge_data_tcp
		static const ACE_UINT32 APP_EX_UJ_DATA_UDP;		// Unjudge_data_udp
	static const ACE_UINT32 APP_EX_UJ_SERVICE;		// Unjudge_service
		static const ACE_UINT32 APP_EX_UJ_SERVICE_TCP;		// Unjudge_service_tcp
		static const ACE_UINT32 APP_EX_UJ_SERVICE_UDP;		// Unjudge_service_udp

	// 互联网业务 //
	static const ACE_UINT32 APP_EXTERN;		//外网整体

	static const ACE_UINT32 APP_EX_WEB;		// Web
	static const ACE_UINT32 APP_EX_MAIL;		// Mail
		static const ACE_UINT32 APP_EX_SMTP;		// SMTP
		static const ACE_UINT32 APP_EX_POP3;		// POP3
		static const ACE_UINT32 APP_EX_IMAP;		// POP3
		static const ACE_UINT32 APP_EX_WEB_MAIL;		// POP3

	static const ACE_UINT32 APP_EX_FILE_TRANS;		// 文件传输
		static const ACE_UINT32 APP_EX_FILE_TRANS_FTP;		// FTP
		static const ACE_UINT32 APP_EX_FILE_TRANS_HTTP;		// HTTP
		static const ACE_UINT32 APP_EX_FILE_TRANS_MULTI_FTP;		// MULTI_FTP,多线程FTP下载
		static const ACE_UINT32 APP_EX_FILE_TRANS_MULTI_HTTP;		// MULTI_HTTP,多线程http下载
		static const ACE_UINT32 APP_EX_FILE_TRANS_QQ_SHARE_FILE;		// QQ文件共享
		static const ACE_UINT32 APP_EX_FILE_TRANS_PSEUDO_HTTP;		// 伪http下载
		static const ACE_UINT32 APP_EX_FILE_TRANS_PSEUDO_HTTPS;		// 伪https下载


	static const ACE_UINT32 APP_EX_IM;		// IM
		static const ACE_UINT32 APP_EX_IM_MSN;
		static const ACE_UINT32 APP_EX_IM_QQ;
		static const ACE_UINT32 APP_EX_IM_QQ_TM;//企业qq
		static const ACE_UINT32 APP_EX_IM_FETION;
		static const ACE_UINT32 APP_EX_IM_YMSG;
		static const ACE_UINT32 APP_EX_IM_MSN_FILE;
		static const ACE_UINT32 APP_EX_IM_QQ_FILE;
		static const ACE_UINT32 APP_EX_IM_FETION_FILE;
		static const ACE_UINT32 APP_EX_IM_YMSG_FILE;

	static const ACE_UINT32 APP_EX_DB;		// Database
		static const ACE_UINT32 APP_EX_DB_ORACLE;
		static const ACE_UINT32 APP_EX_DB_SYBASE;
		static const ACE_UINT32 APP_EX_DB_SQLSERVER;
		static const ACE_UINT32 APP_EX_DB_MYSQL;
		static const ACE_UINT32 APP_EX_DB_INFORMIX;
		static const ACE_UINT32 APP_EX_DB_DB2;
		static const ACE_UINT32 APP_EX_DB_POSTGRESQL;
		static const ACE_UINT32 APP_EX_DB_ACCESS;
	
	static const ACE_UINT32 APP_EX_P2P;		// P2P	
		static const ACE_UINT32 APP_EX_CRY_P2P;

	static const ACE_UINT32 APP_EX_NETVIDEO;
		static const ACE_UINT32 APP_EX_WEB_VIDEO;
		static const ACE_UINT32 APP_EX_WEB_AUDIO;

	static const ACE_UINT32 APP_EX_VPN;
		static const ACE_UINT32 APP_EX_VPN_PPTP;
		static const ACE_UINT32 APP_EX_VPN_L2TP;
		static const ACE_UINT32 APP_EX_VPN_GRE;
		static const ACE_UINT32 APP_EX_VPN_IPSEC;

	static const ACE_UINT32 APP_EX_GAME;		// Game
	static const ACE_UINT32 APP_EX_STOCK;		// Stock

	static const ACE_UINT32 APP_EX_USERD;		// User Defined

	//1．APP_EX_OTHER_DATA其他数据传输：对于目的端口在1024以上，并且连接超过40个数据包后仍然未能识别的业务
	//	2．APP_EX_OTHER_SERVICE其他系统服务：对于目的端口在1024以下，并且连接超过40个数据包后仍然未能识别的业务
	static const ACE_UINT32 APP_EX_UK_DATA;		// Unknown_data
		static const ACE_UINT32 APP_EX_UK_DATA_TCP;		// Unknown_data_tcp
		static const ACE_UINT32 APP_EX_UK_DATA_UDP;		// Unknown_data_udp
	static const ACE_UINT32 APP_EX_UK_SERVICE;		// Unknown_service
		static const ACE_UINT32 APP_EX_UK_SERVICE_TCP;		// Unknown_service_tcp
		static const ACE_UINT32 APP_EX_UK_SERVICE_UDP;		// Unknown_service_udp

	static const ACE_UINT32 APP_EX_VOIP;		// 

	static const ACE_UINT32 APP_EX_NOT_TCPUDP_DATA;		// 
		static const ACE_UINT32 APP_EX_NOT_IP_OLD;//非IP包
		static const ACE_UINT32 APP_EX_IP_ERROR;//错误的IP包
		static const ACE_UINT32 APP_EX_ICMP;//icmp
		static const ACE_UINT32 APP_EX_OTHER_TRANS;//（非TCP/UDP/ICMP的数据包）

	static const ACE_UINT32 APP_EX_NOT_IP_GRP;//非IP包
		static const ACE_UINT32 APP_EX_MAC_ARP;//ARP包
		static const ACE_UINT32 APP_EX_MAC_NETBIOS;
		static const ACE_UINT32 APP_EX_MAC_BPDU;
		static const ACE_UINT32 APP_EX_MAC_DNA;
		static const ACE_UINT32 APP_EX_MAC_PPPOE;
		static const ACE_UINT32 APP_EX_OTHER_NOT_IP;//（其他的非ip包）


	static const ACE_UINT32 APP_EX_OTHER_COMMON_DATA;
		static const ACE_UINT32 APP_EX_DNS;
		static const ACE_UINT32 APP_EX_TELNET;
		static const ACE_UINT32 APP_EX_HTTPS;

	static const ACE_UINT32 APP_EX_REMOTE_CTRL;

	static const ACE_UINT32 APP_EX_MOBILE_APP;//移动app

	static const ACE_UINT32 APP_EX_OVER_MALL;//翻墙软件
	//////////////////////////////////////////////////////////////////////////

	// 内网业务 //

	//没有进行判断的业务，注意要和UNKNOWN区分开
	static const ACE_UINT32 APP_IN_UJ_DATA;		// Unjudge_data
		static const ACE_UINT32 APP_IN_UJ_DATA_TCP;		// Unjudge_data_tcp
		static const ACE_UINT32 APP_IN_UJ_DATA_UDP;		// Unjudge_data_udp
	static const ACE_UINT32 APP_IN_UJ_SERVICE;		// Unjudge_service
		static const ACE_UINT32 APP_IN_UJ_SERVICE_TCP;		// Unjudge_service_tcp
		static const ACE_UINT32 APP_IN_UJ_SERVICE_UDP;		// Unjudge_service_udp

	static const ACE_UINT32 APP_INTERN;		//内网整体

	static const ACE_UINT32 APP_IN_WEB;		// Web
	static const ACE_UINT32 APP_IN_MAIL;		// Mail
		static const ACE_UINT32 APP_IN_SMTP;		// SMTP
		static const ACE_UINT32 APP_IN_POP3;		// POP3
	static const ACE_UINT32 APP_IN_FTP;		// FTP
	static const ACE_UINT32 APP_IN_TELNET;	// Telnet
	static const ACE_UINT32 APP_IN_SMB;		// SMB

	static const ACE_UINT32 APP_IN_DB;		// Database
		static const ACE_UINT32 APP_IN_DB_ORACLE;
		static const ACE_UINT32 APP_IN_DB_SYBASE;
		static const ACE_UINT32 APP_IN_DB_SQLSERVER;
		static const ACE_UINT32 APP_IN_DB_MYSQL;
		static const ACE_UINT32 APP_IN_DB_INFORMIX;
		static const ACE_UINT32 APP_IN_DB_DB2;
		static const ACE_UINT32 APP_IN_DB_POSTGRESQL;
		static const ACE_UINT32 APP_IN_DB_ACCESS;

	static const ACE_UINT32 APP_IN_RCTRL;		// RCtrl
	static const ACE_UINT32 APP_IN_DHCP;		// DHCP
	static const ACE_UINT32 APP_IN_DNS;		// DNS

	static const ACE_UINT32 APP_IN_USERD;		// User Defined

	static const ACE_UINT32 APP_IN_UK_DATA;		// Unknown_data
		static const ACE_UINT32 APP_IN_UK_DATA_TCP;		// Unknown_data_tcp
		static const ACE_UINT32 APP_IN_UK_DATA_UDP;		// Unknown_data_udp
	static const ACE_UINT32 APP_IN_UK_SERVICE;		// Unknown_service
		static const ACE_UINT32 APP_IN_UK_SERVICE_TCP;		// Unknown_service_tcp
		static const ACE_UINT32 APP_IN_UK_SERVICE_UDP;		// Unknown_service_udp
		
	static const ACE_UINT32 APP_IN_VOIP;		// 

	static const ACE_UINT32 APP_IN_NOT_IP;		// 

public:
	static BOOL ReadAppTypeTree();
	static void CloseAppTypeTree();

	static const char * FindDispNameByAppid(ACE_UINT32 appid);
	static ACE_UINT32	FindAppIdByAppName(const char * name);

	inline static const PROTO_TREE_ITEM *	FindItemByAppid(ACE_UINT32 appid)
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

		return Inner_FindItemByAppid(appid);
	}

	static const PROTO_TREE_ITEM *	FindItemByAppName(const char * name);
	static const PROTO_TREE_ITEM *	FindItemByAppDesc(const char * desc);

private:
	//没有同步
	inline static const CAppSubtypeReader::PROTO_TREE_ITEM * Inner_FindItemByAppid(ACE_UINT32 appid)
	{
		LPPROTO_TREE_ITEM * ppItem = ms_hash_finder.FindOnly(appid);

		if( !ppItem )
			return NULL;

		return *ppItem;
	}

private:
	static PROTO_TREE_ITEM * InsertItem(const char * name,
		const char * disp,ACE_UINT32 appid,int bvisible,
		PROTO_TREE_ITEM * parent_item);

public:
	static BOOL AddUserDefApp(const char * name,const char *desc,ACE_UINT32 appid,int bvisible=true);
	static void DeleteAllUserDefApp();

private:
	static void ReadTreeInfo(CTinyXmlEx& xml_file,
		TiXmlNode* parentnode,
		PROTO_TREE_ITEM * parent_item);

	class HashCompFun
	{
	public:		
		inline int Hash(const ACE_UINT32& protoid) const
		{
			return (int)(((protoid%1000)<<8)+(protoid/1000));
		}

		inline int Cmp(const LPPROTO_TREE_ITEM& data,const ACE_UINT32& protoid) const
		{
			return (data->app_id == protoid);
		}
	};


	static CTHashFinderEx<LPPROTO_TREE_ITEM,ACE_UINT32,HashCompFun>		ms_hash_finder;


private:
	static PROTO_TREE_ITEM *					ms_root_item;

private:
	static ACE_Recursive_Thread_Mutex			m_tm;


};