
#include "AppSubtypeReader.h"

#include "cpf/IniFile.h"
#include "cpf/path_tools.h"
#include "cpf/TinyXmlEx.h"
#include "ace/Guard_T.h"

//////////////////////////////////////////////////////////////////////////

//
//if( appid == CAppSubtypeReader::APP_EX_WEB //一般的浏览
//   || appid == CAppSubtypeReader::APP_EX_WEB+2 )//web_mail的浏览
//{
//	return new CWebAuditTask(appid);
//}
//
//if( appid == CAppSubtypeReader::APP_EX_WEB+3)//web搜索
//{
//	return new CWebSearchAuditTask(appid);
//}
//
//if( appid == CAppSubtypeReader::APP_EX_WEB+20//一般post
//   || appid == CAppSubtypeReader::APP_EX_WEB+21//网页登陆
//   || appid == CAppSubtypeReader::APP_EX_WEB+22 )//webmail_post
//{
//	return new CWebPostAuditTask(appid);
//}
//
//if( appid == CAppSubtypeReader::APP_EX_WEB+23//微博
//   || appid == CAppSubtypeReader::APP_EX_WEB+24 )//bbs-论坛
//{
//	return new CWebBlogPostAuditTask(appid);
//}

//CAppSubtypeReader::APP_EX_WEB+25表示web_logon

const ACE_UINT32 CAppSubtypeReader::ALL_APP_TYPE = -1;

//没有进行判断的业务，注意要和UNKNOWN区分开
const ACE_UINT32 CAppSubtypeReader::APP_EXTERN_NOT_JUDGE= 100;



// 互联网业务 //
const ACE_UINT32 CAppSubtypeReader::APP_EXTERN			= 10000000;


const ACE_UINT32 CAppSubtypeReader::APP_EX_WEB			= 10010000;

//<smtp disp="发邮件" appid="10020001" />
//<pop3 disp="收邮件" appid="10020002" />
//<smtps disp="加密发邮件" appid="10020003" />
//<pop3s disp="加密收邮件" appid="10020004" />
//<imap disp="IMAP" appid="10020005" />
//<imaps disp="IMAPS" appid="10020006" />
//<webmail disp="WEB_MAILI"" appid="10020007" />

const ACE_UINT32 CAppSubtypeReader::APP_EX_MAIL			= 10020000;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_SMTP			= 10020001;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_POP3			= 10020002;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IMAP			= 10020003;	
	const ACE_UINT32 CAppSubtypeReader::APP_EX_WEB_MAIL		= 10020007;

const ACE_UINT32 CAppSubtypeReader::APP_EX_FILE_TRANS			= 10030000;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_FILE_TRANS_FTP			= 10030001;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_FILE_TRANS_HTTP			= 10030002;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_FILE_TRANS_MULTI_FTP		= 10030003;		// MULTI_FTP,多线程FTP下载
	const ACE_UINT32 CAppSubtypeReader::APP_EX_FILE_TRANS_MULTI_HTTP	= 10030004;		// MULTI_HTTP,多线程http下载
	const ACE_UINT32 CAppSubtypeReader::APP_EX_FILE_TRANS_QQ_SHARE_FILE	= 10030005;		// QQ文件共享
	const ACE_UINT32 CAppSubtypeReader::APP_EX_FILE_TRANS_PSEUDO_HTTP	= 10030006;		// 伪http下载
	const ACE_UINT32 CAppSubtypeReader::APP_EX_FILE_TRANS_PSEUDO_HTTPS	= 10030007;		// 伪https下载

const ACE_UINT32 CAppSubtypeReader::APP_EX_IM				= 10080000;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IM_MSN			= 10080001;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IM_QQ			= 10080002;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IM_FETION		= 10080003;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IM_YMSG			= 10080005;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IM_QQ_TM			= 10080018;

	const ACE_UINT32 CAppSubtypeReader::APP_EX_IM_MSN_FILE		= 10080501;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IM_QQ_FILE		= 10080502;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IM_FETION_FILE	= 10080503;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IM_YMSG_FILE		= 10080505;


const ACE_UINT32 CAppSubtypeReader::APP_EX_DB				= 19070000;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_DB_ORACLE	= 19070001;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_DB_SYBASE	= 19070002;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_DB_SQLSERVER	= 19070003;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_DB_MYSQL		= 19070004;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_DB_INFORMIX	= 19070005;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_DB_DB2		= 19070006;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_DB_POSTGRESQL= 19070007;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_DB_ACCESS	= 19070008;


const ACE_UINT32 CAppSubtypeReader::APP_EX_P2P				= 10070000;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_CRY_P2P		= 10070014;

const ACE_UINT32 CAppSubtypeReader::APP_EX_GAME				= 10090000;
const ACE_UINT32 CAppSubtypeReader::APP_EX_STOCK			= 10100000;


const ACE_UINT32 CAppSubtypeReader::APP_EX_VOIP				= 10110000;		// voip


const ACE_UINT32 CAppSubtypeReader::APP_EX_NETVIDEO			= 10120000;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_WEB_VIDEO		= 10120019;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_WEB_AUDIO		= 10120024;


	//对应数据中：APP_EX_RESERVED1
const ACE_UINT32 CAppSubtypeReader::APP_EX_VPN				= 10130000;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_VPN_PPTP		= 10130001;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_VPN_L2TP		= 10130002;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_VPN_GRE		= 10130003;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_VPN_IPSEC	= 10130004;


//以下5个应用是预留的，对应的数据库表在db_v35版本已经建立了。
//const ACE_UINT32 CAppSubtypeReader::APP_EX_RESERVED1			= 10130000;
//const ACE_UINT32 CAppSubtypeReader::APP_EX_RESERVED2			= 10140000;
//const ACE_UINT32 CAppSubtypeReader::APP_EX_RESERVED3			= 10150000;
//const ACE_UINT32 CAppSubtypeReader::APP_EX_RESERVED4			= 10160000;
//const ACE_UINT32 CAppSubtypeReader::APP_EX_RESERVED5			= 10170000;


const ACE_UINT32 CAppSubtypeReader::APP_EX_USERD			= 19000000;

const ACE_UINT32 CAppSubtypeReader::APP_EX_UK_DATA		= 19010000;		// Unknown_data
	const ACE_UINT32 CAppSubtypeReader::APP_EX_UK_DATA_TCP= 19010001;		// Unknown_data_tcp
	const ACE_UINT32 CAppSubtypeReader::APP_EX_UK_DATA_UDP= 19010002;		// Unknown_data_udp

const ACE_UINT32 CAppSubtypeReader::APP_EX_UK_SERVICE		= 19020000;		// Unknown_service
	const ACE_UINT32 CAppSubtypeReader::APP_EX_UK_SERVICE_TCP= 19020001;		// Unknown_service_tcp
	const ACE_UINT32 CAppSubtypeReader::APP_EX_UK_SERVICE_UDP= 19020002;		// Unknown_service_udp

const ACE_UINT32 CAppSubtypeReader::APP_EX_UJ_DATA				= 19030000;		// Unjudge_data
	const ACE_UINT32 CAppSubtypeReader::APP_EX_UJ_DATA_TCP		= 19030001;		// Unjudge_data_tcp
	const ACE_UINT32 CAppSubtypeReader::APP_EX_UJ_DATA_UDP		= 19030002;		// Unjudge_data_udp

const ACE_UINT32 CAppSubtypeReader::APP_EX_UJ_SERVICE			= 19040000;		// Unjudge_service
	const ACE_UINT32 CAppSubtypeReader::APP_EX_UJ_SERVICE_TCP	= 19040001;		// Unjudge_service_tcp
	const ACE_UINT32 CAppSubtypeReader::APP_EX_UJ_SERVICE_UDP	= 19040002;		// Unjudge_service_udp

const ACE_UINT32 CAppSubtypeReader::APP_EX_NOT_TCPUDP_DATA		= 19080000;		// 
	const ACE_UINT32 CAppSubtypeReader::APP_EX_NOT_IP_OLD	= 19080001;	//非IP包
	const ACE_UINT32 CAppSubtypeReader::APP_EX_IP_ERROR		= 19080002;	//错误的IP包
	const ACE_UINT32 CAppSubtypeReader::APP_EX_ICMP			= 19080003;//icmp
	const ACE_UINT32 CAppSubtypeReader::APP_EX_OTHER_TRANS	=19080004;//（非TCP/UDP/ICMP的数据包）

const ACE_UINT32 CAppSubtypeReader::APP_EX_NOT_IP_GRP			= 19050000;		// 
	const ACE_UINT32 CAppSubtypeReader::APP_EX_MAC_ARP		= 19050001;	
	const ACE_UINT32 CAppSubtypeReader::APP_EX_MAC_NETBIOS	= 19050002;	
	const ACE_UINT32 CAppSubtypeReader::APP_EX_MAC_BPDU		= 19050003;	
	const ACE_UINT32 CAppSubtypeReader::APP_EX_MAC_DNA		= 19050004;	
	const ACE_UINT32 CAppSubtypeReader::APP_EX_MAC_PPPOE	= 19050005;	
	const ACE_UINT32 CAppSubtypeReader::APP_EX_OTHER_NOT_IP	= 19050010;	////（其他的非IP包）


const ACE_UINT32 CAppSubtypeReader::APP_EX_OTHER_COMMON_DATA		= 19090000;		// 
	const ACE_UINT32 CAppSubtypeReader::APP_EX_DNS					= 19090001;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_TELNET				= 19090002;
	const ACE_UINT32 CAppSubtypeReader::APP_EX_HTTPS				= 19090014;//<https disp="https" appid="19090014" />

const ACE_UINT32 CAppSubtypeReader::APP_EX_REMOTE_CTRL				= 19100000;

const ACE_UINT32 CAppSubtypeReader::APP_EX_MOBILE_APP				= 19110000;//移动app

const ACE_UINT32 CAppSubtypeReader::APP_EX_OVER_MALL				= 19120000;//翻墙软件

// 内网业务 //
const ACE_UINT32 CAppSubtypeReader::APP_INTERN			= 20000000;

const ACE_UINT32 CAppSubtypeReader::APP_IN_WEB			= 20010000;
const ACE_UINT32 CAppSubtypeReader::APP_IN_MAIL			= 20020000;
	const ACE_UINT32 CAppSubtypeReader::APP_IN_SMTP			= 20020001;
	const ACE_UINT32 CAppSubtypeReader::APP_IN_POP3			= 20020002;
const ACE_UINT32 CAppSubtypeReader::APP_IN_FTP			= 20030000;
const ACE_UINT32 CAppSubtypeReader::APP_IN_TELNET			= 20050000;
const ACE_UINT32 CAppSubtypeReader::APP_IN_SMB			= 20040000;
const ACE_UINT32 CAppSubtypeReader::APP_IN_RCTRL			= 20060000;

const ACE_UINT32 CAppSubtypeReader::APP_IN_DB				= 20110000;
const ACE_UINT32 CAppSubtypeReader::APP_IN_DB_ORACLE		= 20110001;
const ACE_UINT32 CAppSubtypeReader::APP_IN_DB_SYBASE		= 20110002;
const ACE_UINT32 CAppSubtypeReader::APP_IN_DB_SQLSERVER	= 20110003;
const ACE_UINT32 CAppSubtypeReader::APP_IN_DB_MYSQL		= 20110004;
const ACE_UINT32 CAppSubtypeReader::APP_IN_DB_INFORMIX	= 20110005;
const ACE_UINT32 CAppSubtypeReader::APP_IN_DB_DB2			= 20110006;
const ACE_UINT32 CAppSubtypeReader::APP_IN_DB_POSTGRESQL	= 20110007;
const ACE_UINT32 CAppSubtypeReader::APP_IN_DB_ACCESS		= 20110008;

const ACE_UINT32 CAppSubtypeReader::APP_IN_DHCP			= 20130000;
const ACE_UINT32 CAppSubtypeReader::APP_IN_DNS			= 20140000;

const ACE_UINT32 CAppSubtypeReader::APP_IN_VOIP			= 20150000;		// voip

const ACE_UINT32 CAppSubtypeReader::APP_IN_USERD			= 29000000;

const ACE_UINT32 CAppSubtypeReader::APP_IN_UK_DATA		= 29010000;		// Unknown_data
	const ACE_UINT32 CAppSubtypeReader::APP_IN_UK_DATA_TCP= 29010001;		// Unknown_data_tcp
	const ACE_UINT32 CAppSubtypeReader::APP_IN_UK_DATA_UDP= 29010002;		// Unknown_data_udp

const ACE_UINT32 CAppSubtypeReader::APP_IN_UK_SERVICE		= 29020000;		// Unknown_service
	const ACE_UINT32 CAppSubtypeReader::APP_IN_UK_SERVICE_TCP= 29020001;		// Unknown_service_tcp
	const ACE_UINT32 CAppSubtypeReader::APP_IN_UK_SERVICE_UDP= 29020002;		// Unknown_service_udp

const ACE_UINT32 CAppSubtypeReader::APP_IN_UJ_DATA			= 29030000;		// Unjudge_data
	const ACE_UINT32 CAppSubtypeReader::APP_IN_UJ_DATA_TCP	= 29030001;		// Unjudge_data_tcp
	const ACE_UINT32 CAppSubtypeReader::APP_IN_UJ_DATA_UDP	= 29030002;		// Unjudge_data_udp

const ACE_UINT32 CAppSubtypeReader::APP_IN_UJ_SERVICE		= 29040000;		// Unjudge_service
	const ACE_UINT32 CAppSubtypeReader::APP_IN_UJ_SERVICE_TCP	= 29040001;		// Unjudge_service_tcp
	const ACE_UINT32 CAppSubtypeReader::APP_IN_UJ_SERVICE_UDP	= 29040002;		// Unjudge_service_udp


//////////////////////////////////////////////////////////////////////////

CAppSubtypeReader::PROTO_TREE_ITEM *				CAppSubtypeReader::ms_root_item = NULL;

CTHashFinderEx<CAppSubtypeReader::LPPROTO_TREE_ITEM,ACE_UINT32,CAppSubtypeReader::HashCompFun>		CAppSubtypeReader::ms_hash_finder;

ACE_Recursive_Thread_Mutex							CAppSubtypeReader::m_tm;

CAppSubtypeReader::CAppSubtypeReader(void)
{
}

CAppSubtypeReader::~CAppSubtypeReader(void)
{
}

void CAppSubtypeReader::ReadTreeInfo(CTinyXmlEx& xml_file,
									 TiXmlNode* parentnode,
									 PROTO_TREE_ITEM * parent_item)
{
	for(TiXmlNode * pNode = parentnode->FirstChildElement();
		pNode;
		pNode = pNode->NextSiblingElement())
	{
		const char * app_name = pNode->Value();

		if (0 == ACE_OS::strcmp(pNode->Value(), "version_info") )
		{
			continue;
		}

		if( strlen(app_name) >= 32 )
		{
			ACE_ASSERT(FALSE);
			continue;
		}

		std::string disp_name;

		if( !xml_file.GetAttr(pNode,"disp",disp_name) )
		{
			ACE_ASSERT(FALSE);
			continue;
		}

		if( disp_name.length() >= 64 )
		{
			ACE_ASSERT(FALSE);
			continue;
		}
		
		ACE_UINT32 appid = 0;

		if( !xml_file.GetAttr(pNode,"appid",appid) )
		{
			ACE_ASSERT(FALSE);
			continue;
		}

		int bvisible = 1;
		xml_file.GetAttr(pNode,"visible",bvisible);

		PROTO_TREE_ITEM * item = InsertItem(app_name,disp_name.c_str(),appid,bvisible,parent_item);

		if( appid == CAppSubtypeReader::APP_EXTERN )
		{
			ms_root_item = item;
		}

		ReadTreeInfo(xml_file,pNode,item);
	}

	return;

}

CAppSubtypeReader::PROTO_TREE_ITEM * 
CAppSubtypeReader::InsertItem(const char * app_name,
							  const char * disp_name,
							  ACE_UINT32 appid,
							  int bvisible,
							  PROTO_TREE_ITEM * parent_item)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	if( FindItemByAppid(appid) )
	{
		ACE_ASSERT(FALSE);
		return NULL;
	}

	if( FindItemByAppName(app_name) )
	{
		ACE_ASSERT(FALSE);
		return NULL;
	}

	if( FindItemByAppDesc(disp_name) )
	{
		ACE_ASSERT(FALSE);
		return NULL;
	}

	PROTO_TREE_ITEM * item = new PROTO_TREE_ITEM;

	item->app_id = appid;
	item->bvisible = bvisible;
	item->parent_item = parent_item;
	strcpy(item->app_name,app_name);
	strcpy(item->disp_name,disp_name);

	if( parent_item )
	{
		parent_item->vt_sub_item.push_back(item);
	}

	LPPROTO_TREE_ITEM * ppitem = ms_hash_finder.AllocBlockByKey(appid);
	
	if( ppitem )
	{
		*ppitem = item;
	}

	return item;
}


void CAppSubtypeReader::CloseAppTypeTree()
{	
	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	ms_hash_finder.Destroy();

	if( ms_root_item )
	{
		for(size_t i = 0; i < ms_root_item->vt_sub_item.size(); ++i)
		{
			const CAppSubtypeReader::PROTO_TREE_ITEM * pGrpItem = ms_root_item->vt_sub_item[i];

			for(size_t j = 0; j < pGrpItem->vt_sub_item.size(); ++j)
			{
				delete pGrpItem->vt_sub_item[j];
			}

			delete pGrpItem;
		}

		delete ms_root_item;
		ms_root_item = NULL;
	}

	return;
}

BOOL CAppSubtypeReader::ReadAppTypeTree()
{
	CloseAppTypeTree();

	ms_hash_finder.Create(1000,65536/4,false,NULL);

	CTinyXmlEx xml_file;

	std::string full_name = CPF::GetModuleOtherFileName("pat","proto_tree.xml");
	if( 0 != xml_file.open(full_name.c_str()) )
	{
		return false;
	}

	if( !xml_file.GetRootNode() )
	{
		return false;
	}

	ReadTreeInfo(xml_file,xml_file.GetRootNode(),NULL);

	ACE_ASSERT( ms_hash_finder.GetUnUsedCount() >= 100 );

	return true;
}

BOOL CAppSubtypeReader::AddUserDefApp(const char * app_name,const char *disp_name,ACE_UINT32 appid,int bvisible)
{
	CAppSubtypeReader::PROTO_TREE_ITEM * pUserAppDefRoot =
		(CAppSubtypeReader::PROTO_TREE_ITEM *)FindItemByAppid(CAppSubtypeReader::APP_EX_USERD);

	if( !pUserAppDefRoot )
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	return (InsertItem(app_name,disp_name,appid,bvisible,pUserAppDefRoot) != NULL);
}

void CAppSubtypeReader::DeleteAllUserDefApp()
{
	PROTO_TREE_ITEM * pItem = (PROTO_TREE_ITEM *)FindItemByAppid(CAppSubtypeReader::APP_EX_USERD);

	if( !pItem )
	{
		ACE_ASSERT(FALSE);
		return;
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	for(size_t i = 0; i < pItem->vt_sub_item.size(); ++i)
	{
		PROTO_TREE_ITEM * pSubItem = pItem->vt_sub_item[i];

		ms_hash_finder.FreeBlockByKey(pSubItem->app_id);

		delete pSubItem;
	}

	pItem->vt_sub_item.clear();

	return;
}

void CAppSubtypeReader::GetAllExtGrpAppType(std::vector<ACE_UINT32>& vt_subtype)
{
	vt_subtype.clear();

	const PROTO_TREE_ITEM * pitem = FindItemByAppid(CAppSubtypeReader::APP_EXTERN);

	ACE_ASSERT(pitem);

	if( !pitem )
	{
		return;
	}

	vt_subtype.reserve(pitem->vt_sub_item.size());

	for(size_t i = 0; i < pitem->vt_sub_item.size(); ++i)
	{
		vt_subtype.push_back(pitem->vt_sub_item[i]->app_id);
	}

	return;

}


const CAppSubtypeReader::PROTO_TREE_ITEM *	
CAppSubtypeReader::FindItemByAppName(const char * name)
{
	if( !ms_root_item )
	{
		return NULL;
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	for(size_t i = 0; i < ms_root_item->vt_sub_item.size(); ++i)
	{
		const CAppSubtypeReader::PROTO_TREE_ITEM * pGrpItem = ms_root_item->vt_sub_item[i];

		if( stricmp(pGrpItem->app_name,name) == 0 )
		{
			return pGrpItem;
		}

		for(size_t j = 0; j < pGrpItem->vt_sub_item.size(); ++j)
		{
			if( stricmp(pGrpItem->vt_sub_item[j]->app_name,name) == 0 )
			{
				return pGrpItem->vt_sub_item[j];
			}
		}
	}
	
	return NULL;
}


const char * CAppSubtypeReader::FindDispNameByAppid(ACE_UINT32 appid)
{	
	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	const PROTO_TREE_ITEM * pitem = Inner_FindItemByAppid(appid);

	if( pitem )
	{
		return pitem->disp_name;
	}

	static const char unjudeg_proto[] = "系统暂未判定";

	if( CAppSubtypeReader::APP_EXTERN_NOT_JUDGE == appid )
	{
		return unjudeg_proto;
	}

	static const char deleted_user_def_proto[] = "已删除自定义应用";

	if( CAppSubtypeReader::IsUserDefSubApp(appid) )
	{
		return deleted_user_def_proto;
	}

	static const char undef_proto[] = "系统未定义应用";

	return undef_proto;
}

ACE_UINT32 CAppSubtypeReader::FindAppIdByAppName(const char * name)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	const PROTO_TREE_ITEM * pitem = FindItemByAppName(name);

	if( pitem )
	{
		return pitem->app_id;
	}

	return 0;
}

const CAppSubtypeReader::PROTO_TREE_ITEM *	
CAppSubtypeReader::FindItemByAppDesc(const char * disp)
{
	if( !ms_root_item )
	{
		return NULL;
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	for(size_t i = 0; i < ms_root_item->vt_sub_item.size(); ++i)
	{
		const CAppSubtypeReader::PROTO_TREE_ITEM * pGrpItem = ms_root_item->vt_sub_item[i];

		if( stricmp(pGrpItem->disp_name,disp) == 0 )
		{
			return pGrpItem;
		}

		for(size_t j = 0; j < pGrpItem->vt_sub_item.size(); ++j)
		{
			if( stricmp(pGrpItem->vt_sub_item[j]->disp_name,disp) == 0 )
			{
				return pGrpItem->vt_sub_item[j];
			}
		}
	}

	return NULL;
}

size_t CAppSubtypeReader::GetSubAppType(ACE_UINT32 apptype,std::vector<ACE_UINT32>* vt_subtype)
{
	if( !IsAppGrpType(apptype) )
	{
		ACE_ASSERT(false);
		return 0;
	}
	
	if( vt_subtype )
	{
		vt_subtype->clear();
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	const PROTO_TREE_ITEM * pitem = Inner_FindItemByAppid(apptype);

	ACE_ASSERT(pitem);

	if( !pitem )
	{
		return 0;
	}

	if( vt_subtype )
	{		
		vt_subtype->reserve(pitem->vt_sub_item.size());

		for(size_t i = 0; i < pitem->vt_sub_item.size(); ++i)
		{
			vt_subtype->push_back(pitem->vt_sub_item[i]->app_id);
		}
	}

	return (int)pitem->vt_sub_item.size();
	
}

ACE_UINT32 CAppSubtypeReader::GetExtGrpAppType(ACE_UINT32 subAppId)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex>	obj(m_tm);

	const CAppSubtypeReader::PROTO_TREE_ITEM *  pItem = Inner_FindItemByAppid(subAppId);

	if( !pItem )
	{
		if( CAppSubtypeReader::IsUserDefSubApp(subAppId) )
		{
			return CAppSubtypeReader::APP_EX_USERD;
		}

		if( subAppId != CAppSubtypeReader::APP_EXTERN_NOT_JUDGE )
		{
			ACE_ASSERT(FALSE);
		}
		
		return 0;
	}

	if( !pItem->parent_item )
	{
		return 0;
	}

	if( pItem->parent_item->app_id != CAppSubtypeReader::APP_EXTERN )
	{
		return pItem->parent_item->app_id;
	}

	return 0;

}

BOOL CAppSubtypeReader::HasChildAppType(ACE_UINT32 apptype)
{
	if( !IsAppGrpType(apptype) )
	{
		return FALSE;
	}

	if( CAppSubtypeReader::APP_EX_WEB == apptype
		|| CAppSubtypeReader::APP_EX_VOIP == apptype )
	{
		return FALSE;
	}

	return TRUE;
}
