//////////////////////////////////////////////////////////////////////////
//tcpip_mgr_common_init.h


#ifndef _TCPIP_MGR_COMMON_INIT_H_2006_07_04
#define _TCPIP_MGR_COMMON_INIT_H_2006_07_04
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "tcpip_mgr_common.h"
#include "cpf/ACE_Memmap_Log_Msg.h"
#include "cpf/PrivateNet.h"
#include "cpf/addr_tools.h"
#include "PacketIO/PacketIO.h"

TCPIP_MGR_COMMON_CLASS 
void tcpip_mgr_common_init(int live_log_size,unsigned int live_log_level=-1);

TCPIP_MGR_COMMON_CLASS 
void tcpip_mgr_common_fini();


extern TCPIP_MGR_COMMON_CLASS ACE_Memmap_Log_Msg *	g_pACE_Memmap_Log_Msg;
extern TCPIP_MGR_COMMON_CLASS unsigned int			g_live_log_level;

#define DO_TCPIP_MEMMAP_ACE_DEBUG(level,x)	if(g_pACE_Memmap_Log_Msg && (level)>=g_live_log_level) {MY_ACE_DEBUG(g_pACE_Memmap_Log_Msg,x);}

//全局变量控制；
extern TCPIP_MGR_COMMON_CLASS bool g_bDisnableAllInterrupt;		//禁用所有的阻断
extern TCPIP_MGR_COMMON_CLASS int  g_nAuditSavelevel;				//保存审计的级别
extern TCPIP_MGR_COMMON_CLASS int  g_nHttpAuditType;				//保存审计的级别
extern TCPIP_MGR_COMMON_CLASS int  g_save_html_only_have_title;			
extern TCPIP_MGR_COMMON_CLASS bool g_bDisnableAllWarninglevel;	//禁用所有的告警级别
extern TCPIP_MGR_COMMON_CLASS bool g_bDisnableActionStat;		//禁用行为统计
extern TCPIP_MGR_COMMON_CLASS bool g_bAuditWebLogon;			//是否审计web登陆


#define AUDIT_FLAG_WEBGET_BROWSER	((ACE_UINT64)0x00000001)
#define AUDIT_FLAG_WEBGET_L0GON		((ACE_UINT64)0x00000002)
#define AUDIT_FLAG_WEBGET_WEBMAIL	((ACE_UINT64)0x00000004)
#define AUDIT_FLAG_WEBGET_DOWNFILE	((ACE_UINT64)0x00000008)
#define AUDIT_FLAG_WEBGET_RESERVED2	((ACE_UINT64)0x00000010)
#define AUDIT_FLAG_WEBGET_RESERVED3	((ACE_UINT64)0x00000020)

#define AUDIT_FLAG_WEBPOST_POST			((ACE_UINT64)0x00000040)
#define AUDIT_FLAG_WEBPOST_LOGON		((ACE_UINT64)0x00000080)
#define AUDIT_FLAG_WEBPOST_WEBMAIL		((ACE_UINT64)0x00000100)
#define AUDIT_FLAG_WEBPOST_UPFILE		((ACE_UINT64)0x00000200)
#define AUDIT_FLAG_WEBPOST_RESERVED1	((ACE_UINT64)0x00000400)
#define AUDIT_FLAG_WEBPOST_RESERVED2	((ACE_UINT64)0x00000800)

#define AUDIT_FLAG_MAIL_SMTP			((ACE_UINT64)0x00001000)
#define AUDIT_FLAG_MAIL_POP3			((ACE_UINT64)0x00002000)
#define AUDIT_FLAG_MAIL_IMAP			((ACE_UINT64)0x00004000)
#define AUDIT_FLAG_MAIL_RESERVED1		((ACE_UINT64)0x00008000)
#define AUDIT_FLAG_MAIL_RESERVED2		((ACE_UINT64)0x00010000)

#define AUDIT_FLAG_IM_MSN				((ACE_UINT64)0x00020000)
#define AUDIT_FLAG_IM_QQ				((ACE_UINT64)0x00040000)
#define AUDIT_FLAG_IM_YMSG				((ACE_UINT64)0x00080000)
#define AUDIT_FLAG_IM_RESERVED2			((ACE_UINT64)0x00100000)
#define AUDIT_FLAG_IM_RESERVED3			((ACE_UINT64)0x00200000)
#define AUDIT_FLAG_IM_RESERVED4			((ACE_UINT64)0x00400000)
#define AUDIT_FLAG_IM_RESERVED5			((ACE_UINT64)0x00800000)

#define AUDIT_FLAG_CMD_FTP				((ACE_UINT64)0x01000000)
#define AUDIT_FLAG_CMD_TELNET			((ACE_UINT64)0x02000000)
#define AUDIT_FLAG_CMD_RESERVED1		((ACE_UINT64)0x04000000)
#define AUDIT_FLAG_CMD_RESERVED2		((ACE_UINT64)0x08000000)


#define AUDIT_FLAG_USERD_1				((ACE_UINT64)0x10000000)
#define AUDIT_FLAG_USERD_2				((ACE_UINT64)0x20000000)
#define AUDIT_FLAG_USERD_3				((ACE_UINT64)0x40000000)
#define AUDIT_FLAG_USERD_4				((ACE_UINT64)0x80000000)


#define DEF_STAT_TXT_FLAG_BEGIN			((ACE_UINT64)0x4141414141414141)	//"AAAAAAAA"
#define DEF_STAT_TXT_FLAG_END			((ACE_UINT64)0x4242424242424242)	//"BBBBBBBB"


typedef struct tagIPv4_CONNECT_ADDR_EX 
{
	ACE_UINT32			vm_id;
	IPv4_CONNECT_ADDR	connAddr;

}IPv4_CONNECT_ADDR_EX;

typedef struct tagPACKET_INFO_EX
{
	tagPACKET_INFO_EX()
	{
		pDynBuffer = NULL;
	}

	inline void reset()
	{
		if_line_index = -1;
		vm_id = -1;

		dir_phy = DIR_IO_UNKNOWN;

		pkt_len_type = PACKET_OK;
		packet = NULL;


		tcpipContext.reset();

		if( pDynBuffer )
		{
			CPF::MyMemFree(pDynBuffer);
			pDynBuffer = NULL;
		}
	}

	~tagPACKET_INFO_EX()
	{
		if( pDynBuffer )
		{
			CPF::MyMemFree(pDynBuffer);
			pDynBuffer = NULL;
		}
	}

	ACE_UINT32		vm_id;//流量控制中的虚拟链路号

	//如果是透明桥模式，则是配置的线路对的号
	//如果是路由模式，这个值是出口wan_line_index,即出口线路号
	int				if_line_index;

	PACKET_LEN_TYPE pkt_len_type;
	RECV_PACKET	*	packet;

	DIR_IO			dir_phy;

	CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT				tcpipContext;
	CCommonEtherDecode::ARP_DECODE_INFO 						m_arp_decode_info;// 存储ARP报文的信息

	CTimeStamp32											tsCurPkt;

	//数据的动态缓存，如果是分片数据，需要缓存，pDynBuffer不为0，析构的时候需要释放内存
	//如果是正常的数据不缓存，这个值为0
	BYTE *													pDynBuffer;

public:
	inline const BYTE * GetInnerMac() const
	{
		return tcpipContext.GetInnerMac(dir_phy);
	}

	inline const BYTE * GetOutterMac() const
	{
		return tcpipContext.GetOutterMac(dir_phy);
	}

	inline ACE_UINT32 GetInnerIP() const
	{
		return tcpipContext.conn_addr.GetInnerIP(dir_phy);
	}

	inline ACE_UINT16 GetInnerPort() const
	{
		return tcpipContext.conn_addr.GetInnerPort(dir_phy);
	}

	inline ACE_UINT32 GetOutterIP() const
	{
		return tcpipContext.conn_addr.GetOutterIP(dir_phy);
	}

	inline ACE_UINT16 GetOutterPort() const
	{
		return tcpipContext.conn_addr.GetOutterPort(dir_phy);
	}

}PACKET_INFO_EX;




#define DEF_JUDGE_STAT_TXT_HEAD_FLAG(_value, _file_handle)	\
	if (DEF_STAT_TXT_FLAG_BEGIN != _value)	\
	{	\
		int _i_return = feof(_file_handle) ? 0 : -2;		\
		if (0 != _i_return)						\
		{										\
			ACE_ASSERT(FALSE);					\
		}										\
		fclose(_file_handle);					\
		return _i_return;						\
	}


#define DEF_JUDGE_STAT_TXT_TAIL_FLAG(_value, _file_handle)	\
	if (DEF_STAT_TXT_FLAG_END != _value)							\
	{	\
		ACE_ASSERT(FALSE);					\
		fclose(_file_handle);	\
		return -2; \
	}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_TCPIP_MGR_COMMON_INIT_H_2006_07_04