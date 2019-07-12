//////////////////////////////////////////////////////////////////////////
//Flux_Control_Common_Struct.h

#pragma once

#pragma warning(disable : 4996)

#include "cpf/cpf.h"
#include "flux_control_common.h"
#include "cpf/TIME_QUERY_COND.h"


//用于内网业务以及互联网整体业务的性能指标
typedef enum {

	IF_DATA_TYPE_NONE = -1,
	IF_DATA_TYPE_INNER = 0,
	IF_DATA_TYPE_OUTTER = 1,
	IF_DATA_TYPE_BEFORE_TRANS = 2,
	IF_DATA_TYPE_AFTER_TRANS = 3,
	IF_DATA_TYPE_TRANS_TO_PA = 4,//给业务系统的数据，可能这个数据拷贝失败，因为业务系统的速度问题
	IF_DATA_TYPE_TRANS_SUCC_TO_PA = 5//成功拷贝给业务系统的数据

}IF_DATA_TYPE;


typedef enum{
	LINKSTAT_SUCCESS = 0,	//建立成功的连接
	LINKSTAT_FAIL,			//建立失败的连接
	LINKSTAT_EMPTY,			//空连接(只握手)
	LINKSTAT_RESET,			//强制中断的连接
	LINKSTAT_TIMEOUT		//超时异常中断的连接
}LINK_STAT_TYPE;

#pragma pack(push)
#pragma pack(1)

class MINI_CHANNEL_PERF_STAT_EX
{
public:
	inline MINI_CHANNEL_PERF_STAT_EX()
	{
		reset();
	}

	inline void reset()
	{
		memset(this, 0, sizeof(*this));
	}
public:
	//TCP + UDP
	ACE_UINT64	nSendPackets;			//发送报文包数(内网流出到外网、内网间发送)；
	ACE_UINT64	nSendBytes;				//发送字节数(内网流出到外网、内网间发送)；

	ACE_UINT64	nRecvPackets;			//接收报文包数(外网流入到内网、内网间接收)；
	ACE_UINT64	nRecvBytes;				//接收字节数(外网流入到内网、内网间接收)；

	ACE_UINT64	nRepeatPackets;			//TCP重传的报文包数；

public:
	ACE_UINT64	nOrigSendPackets;			//发送报文包数(内网流出到外网、内网间发送)；
	ACE_UINT64	nOrigSendBytes;				//发送字节数(内网流出到外网、内网间发送)；

	ACE_UINT64	nOrigRecvPackets;			//接收报文包数(外网流入到内网、内网间接收)；
	ACE_UINT64	nOrigRecvBytes;				//接收字节数(外网流入到内网、内网间接收)；

	inline ACE_UINT64 get_send_bytes(IF_DATA_TYPE data_type) const
	{
		switch(data_type)
		{
		case IF_DATA_TYPE_INNER:
			return (nOrigSendBytes);
			break;

		case IF_DATA_TYPE_OUTTER:
			return (nSendBytes);

		case IF_DATA_TYPE_AFTER_TRANS:
			return (nSendBytes);

		case IF_DATA_TYPE_BEFORE_TRANS:
			return (nOrigSendBytes);

		default:
			ACE_ASSERT(FALSE);
			return (nOrigSendBytes);
		}
	}

	inline ACE_UINT64 get_recv_bytes(IF_DATA_TYPE data_type) const
	{
		switch(data_type)
		{
		case IF_DATA_TYPE_INNER:
			return (nRecvBytes);
			break;

		case IF_DATA_TYPE_OUTTER:
			return (nOrigRecvBytes);

		case IF_DATA_TYPE_AFTER_TRANS:
			return (nRecvBytes);

		case IF_DATA_TYPE_BEFORE_TRANS:
			return (nOrigRecvBytes);

		default:
			ACE_ASSERT(FALSE);
			return (nOrigSendBytes);
		}
	}

	inline ACE_UINT64 total_bytes(IF_DATA_TYPE data_type=IF_DATA_TYPE_BEFORE_TRANS) const
	{
		switch(data_type)
		{
		case IF_DATA_TYPE_INNER:
			return (nOrigSendBytes+nRecvBytes);
			break;

		case IF_DATA_TYPE_OUTTER:
			return (nOrigRecvBytes+nSendBytes);

		case IF_DATA_TYPE_AFTER_TRANS:
			return (nSendBytes+nRecvBytes);

		case IF_DATA_TYPE_BEFORE_TRANS:
			return (nOrigRecvBytes+nOrigSendBytes);

		default:
			ACE_ASSERT(FALSE);
			return (nOrigRecvBytes+nOrigSendBytes);
		}
	}

	inline ACE_UINT64 total_packets(IF_DATA_TYPE data_type=IF_DATA_TYPE_BEFORE_TRANS) const
	{
		switch(data_type)
		{
		case IF_DATA_TYPE_INNER:
			return (nOrigSendPackets+nRecvPackets);
			break;

		case IF_DATA_TYPE_OUTTER:
			return (nOrigRecvPackets+nSendPackets);

		case IF_DATA_TYPE_AFTER_TRANS:
			return (nSendPackets+nRecvPackets);

		case IF_DATA_TYPE_BEFORE_TRANS:
			return (nOrigRecvPackets+nOrigSendPackets);

		default:
			ACE_ASSERT(FALSE);
			return (nOrigRecvPackets+nOrigSendPackets);
		}
	}

	inline void AddStat_Flux(ACE_UINT32 nBytes, BOOL bSend, BOOL bRepeat, BOOL bPass)
	{
		if( bSend )
		{
			nOrigSendPackets ++;
			nOrigSendBytes += nBytes;
		}
		else
		{
			nOrigRecvPackets ++;
			nOrigRecvBytes += nBytes;
		}

		if( bPass )
		{
			if( bSend )
			{
				nSendPackets ++;
				nSendBytes += nBytes;
			}
			else
			{
				nRecvPackets ++;
				nRecvBytes += nBytes;
			}
		}

		if( bRepeat )
		{
			nRepeatPackets ++;
		}

		return;
	}

public:
	inline bool IsEmpty() const
	{
		return total_bytes(IF_DATA_TYPE_BEFORE_TRANS) == 0 ;
	}

public:
	inline MINI_CHANNEL_PERF_STAT_EX& operator = (const MINI_CHANNEL_PERF_STAT_EX& other)
	{
		if (&other != this)
		{
			this->nSendPackets		= other.nSendPackets;
			this->nSendBytes		= other.nSendBytes;
			this->nRecvPackets		= other.nRecvPackets;
			this->nRecvBytes		= other.nRecvBytes;

			this->nRepeatPackets	= other.nRepeatPackets;

			this->nOrigSendPackets = other.nOrigSendPackets;
			this->nOrigSendBytes = other.nOrigSendBytes;
			this->nOrigRecvPackets = other.nOrigRecvPackets;
			this->nOrigRecvBytes = other.nOrigRecvBytes;
		}

		return *this;
	};

	MINI_CHANNEL_PERF_STAT_EX& desc2(const MINI_CHANNEL_PERF_STAT_EX& other)
	{
		this->nSendPackets		-= other.nSendPackets;
		this->nSendBytes		-= other.nSendBytes;
		this->nRecvPackets		-= other.nRecvPackets;
		this->nRecvBytes		-= other.nRecvBytes;
	
		this->nRepeatPackets	-= other.nRepeatPackets;

		this->nOrigSendPackets -= other.nOrigSendPackets;
		this->nOrigSendBytes -= other.nOrigSendBytes;
		this->nOrigRecvPackets -= other.nOrigRecvPackets;
		this->nOrigRecvBytes -= other.nOrigRecvBytes;

		return *this;
	}

	MINI_CHANNEL_PERF_STAT_EX& operator += (const MINI_CHANNEL_PERF_STAT_EX& other)
	{
		this->nSendPackets		+= other.nSendPackets;
		this->nSendBytes		+= other.nSendBytes;
		this->nRecvPackets		+= other.nRecvPackets;
		this->nRecvBytes		+= other.nRecvBytes;

		this->nRepeatPackets	+= other.nRepeatPackets;
		
		this->nOrigSendPackets += other.nOrigSendPackets;
		this->nOrigSendBytes += other.nOrigSendBytes;
		this->nOrigRecvPackets += other.nOrigRecvPackets;
		this->nOrigRecvBytes += other.nOrigRecvBytes;

		return *this;
	};
};

typedef struct tagCHANNEL_PERF_STAT_EX
{
public:
	//TCP + UDP
	ACE_UINT64	nSendPackets;			//发送报文包数(内网流出到外网、内网间发送)；
	ACE_UINT64	nSendBytes;				//发送字节数(内网流出到外网、内网间发送)；

	ACE_UINT64	nRecvPackets;			//接收报文包数(外网流入到内网、内网间接收)；
	ACE_UINT64	nRecvBytes;				//接收字节数(外网流入到内网、内网间接收)；

	//TCP
	ACE_UINT64	nSuccessLinks;			//建立成功的连接数；
	ACE_UINT64	nFailedLinks;			//建立失败的连接数；
	ACE_UINT64	nEmptyLinks;			//空会话(只有握手没有传输数据)的连接数；
	ACE_UINT64	nResetLinks;			//强制中断(Reset)的连接数；
	ACE_UINT64	nTimeoutLinks;			//异常中断(TimeOut)的连接数；

	ACE_UINT64	nClientRespTimes;		//客户端响应时间(网络响应时间)汇总；
	ACE_UINT64	nServerRespTimes;		//服务器响应时间汇总；单位毫秒；

	ACE_UINT64	nRepeatPackets;			//TCP重传的报文包数；

	//UDP
	//	ACE_UINT64	nUdpCreatedLinks;		//UDP:总共尝试建立的连接数；

	ACE_UINT64	nBroadcastPackets;		//广播包总数；
	ACE_UINT64	nBroadcastBytes;		//广播字节总数；

	ACE_UINT64	nCurTCPConnects;
	ACE_UINT64	nCurUDPConnects;
public:
	ACE_UINT64	nOrigSendPackets;			//发送报文包数(内网流出到外网、内网间发送)；
	ACE_UINT64	nOrigSendBytes;				//发送字节数(内网流出到外网、内网间发送)；

	ACE_UINT64	nOrigRecvPackets;			//接收报文包数(外网流入到内网、内网间接收)；
	ACE_UINT64	nOrigRecvBytes;				//接收字节数(外网流入到内网、内网间接收)；

	ACE_UINT64	nOrigBroadcastPackets;		//接收字节数(外网流入到内网、内网间接收)；
	ACE_UINT64	nOrigBroadcastBytes;		//接收报文包数(外网流入到内网、内网间接收)；

public:
	inline tagCHANNEL_PERF_STAT_EX()
	{
		reset();
	}

	inline void reset()
	{
		memset(this, 0, sizeof(*this));
	}

	inline ACE_UINT64 get_send_bytes(IF_DATA_TYPE data_type=IF_DATA_TYPE_BEFORE_TRANS) const
	{
		switch(data_type)
		{
		case IF_DATA_TYPE_INNER:
			return (nOrigSendBytes);
			break;

		case IF_DATA_TYPE_OUTTER:
			return (nSendBytes);

		case IF_DATA_TYPE_AFTER_TRANS:
			return (nSendBytes);

		case IF_DATA_TYPE_BEFORE_TRANS:
			return (nOrigSendBytes);

		default:
			ACE_ASSERT(FALSE);
			return (nOrigSendBytes);
		}
	}

	inline ACE_UINT64 get_recv_bytes(IF_DATA_TYPE data_type=IF_DATA_TYPE_BEFORE_TRANS) const
	{
		switch(data_type)
		{
		case IF_DATA_TYPE_INNER:
			return (nRecvBytes);
			break;

		case IF_DATA_TYPE_OUTTER:
			return (nOrigRecvBytes);

		case IF_DATA_TYPE_AFTER_TRANS:
			return (nRecvBytes);

		case IF_DATA_TYPE_BEFORE_TRANS:
			return (nOrigRecvBytes);

		default:
			ACE_ASSERT(FALSE);
			return (nOrigSendBytes);
		}
	}

	inline ACE_UINT64 total_bytes(IF_DATA_TYPE data_type=IF_DATA_TYPE_BEFORE_TRANS) const
	{
		switch(data_type)
		{
		case IF_DATA_TYPE_INNER:
			return (nOrigSendBytes+nRecvBytes);
			break;

		case IF_DATA_TYPE_OUTTER:
			return (nOrigRecvBytes+nSendBytes);

		case IF_DATA_TYPE_AFTER_TRANS:
			return (nSendBytes+nRecvBytes);

		case IF_DATA_TYPE_BEFORE_TRANS:
			return (nOrigRecvBytes+nOrigSendBytes);

		default:
			ACE_ASSERT(FALSE);
			return (nOrigRecvBytes+nOrigSendBytes);
		}
	}

	inline ACE_UINT64 total_packets(IF_DATA_TYPE data_type=IF_DATA_TYPE_BEFORE_TRANS) const
	{
		switch(data_type)
		{
		case IF_DATA_TYPE_INNER:
			return (nOrigSendPackets+nRecvPackets);
			break;

		case IF_DATA_TYPE_OUTTER:
			return (nOrigRecvPackets+nSendPackets);

		case IF_DATA_TYPE_AFTER_TRANS:
			return (nSendPackets+nRecvPackets);

		case IF_DATA_TYPE_BEFORE_TRANS:
			return (nOrigRecvPackets+nOrigSendPackets);

		default:
			ACE_ASSERT(FALSE);
			return (nOrigRecvPackets+nOrigSendPackets);
		}
	}

	inline ACE_UINT64 total_created_links() const
	{
		return (total_created_tcp_links()+total_created_dup_links());
	}

	inline ACE_UINT64 total_created_tcp_links() const
	{
		return (nSuccessLinks+nFailedLinks);
	}
	inline ACE_UINT64 total_created_dup_links() const
	{
		return 0;
	}

	inline ACE_UINT64 total_cur_links() const
	{
		return (nCurTCPConnects+nCurUDPConnects);
	}

	inline bool IsEmpty() const
	{
		if( total_bytes(IF_DATA_TYPE_BEFORE_TRANS) > 0 )
			return false;

		return ((nFailedLinks + nEmptyLinks + nResetLinks + nTimeoutLinks) == 0);
	}

	inline tagCHANNEL_PERF_STAT_EX& operator = (const tagCHANNEL_PERF_STAT_EX& other)
	{
		if (&other != this)
		{
			//	memcpy(this, &other, sizeof(tagWHOLE_LIVE_STAT));
			this->nSendPackets		= other.nSendPackets;
			this->nSendBytes		= other.nSendBytes;
			this->nRecvPackets		= other.nRecvPackets;
			this->nRecvBytes		= other.nRecvBytes;
			//	this->nActiveLinks		= other.nActiveLinks;
			this->nEmptyLinks		= other.nEmptyLinks;
			this->nSuccessLinks		= other.nSuccessLinks;
			this->nFailedLinks		= other.nFailedLinks;
			this->nResetLinks		= other.nResetLinks;
			this->nTimeoutLinks		= other.nTimeoutLinks;
			this->nClientRespTimes	= other.nClientRespTimes;
			this->nServerRespTimes	= other.nServerRespTimes;
			this->nRepeatPackets	= other.nRepeatPackets;
			//	this->nUdpActiveLinks	= other.nUdpActiveLinks;
			//	this->nUdpCreatedLinks	= other.nUdpCreatedLinks;
			this->nBroadcastPackets	= other.nBroadcastPackets;
			this->nBroadcastBytes	= other.nBroadcastBytes;

			this->nCurTCPConnects	= other.nCurTCPConnects;
			this->nCurUDPConnects	= other.nCurUDPConnects;


			this->nOrigSendPackets = other.nOrigSendPackets;
			this->nOrigSendBytes = other.nOrigSendBytes;
			this->nOrigRecvPackets = other.nOrigRecvPackets;
			this->nOrigRecvBytes = other.nOrigRecvBytes;

			this->nOrigBroadcastPackets = other.nOrigBroadcastPackets;
			this->nOrigBroadcastBytes = other.nOrigBroadcastBytes;
		}
		return *this;
	};

	tagCHANNEL_PERF_STAT_EX operator + (const tagCHANNEL_PERF_STAT_EX& other) const
	{
		tagCHANNEL_PERF_STAT_EX new_stat = *this;

		new_stat += other;

		return new_stat;
	}

	tagCHANNEL_PERF_STAT_EX& operator += (const tagCHANNEL_PERF_STAT_EX& other)
	{
		this->nSendPackets		+= other.nSendPackets;
		this->nSendBytes		+= other.nSendBytes;
		this->nRecvPackets		+= other.nRecvPackets;
		this->nRecvBytes		+= other.nRecvBytes;
		//	this->nActiveLinks		+= other.nActiveLinks;
		this->nEmptyLinks		+= other.nEmptyLinks;
		this->nSuccessLinks		+= other.nSuccessLinks;
		this->nFailedLinks		+= other.nFailedLinks;
		this->nResetLinks		+= other.nResetLinks;
		this->nTimeoutLinks		+= other.nTimeoutLinks;
		this->nClientRespTimes	+= other.nClientRespTimes;
		this->nServerRespTimes	+= other.nServerRespTimes;
		this->nRepeatPackets	+= other.nRepeatPackets;
		//	this->nUdpActiveLinks	+= other.nUdpActiveLinks;
		//	this->nUdpCreatedLinks	+= other.nUdpCreatedLinks;
		this->nBroadcastPackets	+= other.nBroadcastPackets;
		this->nBroadcastBytes	+= other.nBroadcastBytes;

		this->nCurTCPConnects	+= other.nCurTCPConnects;
		this->nCurUDPConnects	+= other.nCurUDPConnects;

		this->nOrigSendPackets += other.nOrigSendPackets;
		this->nOrigSendBytes += other.nOrigSendBytes;
		this->nOrigRecvPackets += other.nOrigRecvPackets;
		this->nOrigRecvBytes += other.nOrigRecvBytes;

		this->nOrigBroadcastPackets += other.nOrigBroadcastPackets;
		this->nOrigBroadcastBytes += other.nOrigBroadcastBytes;

		return *this;
	};
	
	tagCHANNEL_PERF_STAT_EX operator - (const tagCHANNEL_PERF_STAT_EX& other) const
	{
		tagCHANNEL_PERF_STAT_EX new_stat = *this;

		new_stat -= other;
	
		return new_stat;
	}

	tagCHANNEL_PERF_STAT_EX& operator -= (const tagCHANNEL_PERF_STAT_EX& other)
	{
		desc2(other);

		this->nCurTCPConnects	-= other.nCurTCPConnects;
		this->nCurUDPConnects	-= other.nCurUDPConnects;

		return *this;
	};

	tagCHANNEL_PERF_STAT_EX& desc2(const tagCHANNEL_PERF_STAT_EX& other)
	{
		this->nSendPackets		-= other.nSendPackets;
		this->nSendBytes		-= other.nSendBytes;
		this->nRecvPackets		-= other.nRecvPackets;
		this->nRecvBytes		-= other.nRecvBytes;
		//	this->nActiveLinks		-= other.nActiveLinks;
		this->nEmptyLinks		-= other.nEmptyLinks;
		this->nSuccessLinks		-= other.nSuccessLinks;
		this->nFailedLinks		-= other.nFailedLinks;
		this->nResetLinks		-= other.nResetLinks;
		this->nTimeoutLinks		-= other.nTimeoutLinks;
		this->nClientRespTimes	-= other.nClientRespTimes;
		this->nServerRespTimes	-= other.nServerRespTimes;
		this->nRepeatPackets	-= other.nRepeatPackets;
		//	this->nUdpActiveLinks	-= other.nUdpActiveLinks;
		//	this->nUdpCreatedLinks	-= other.nUdpCreatedLinks;
		this->nBroadcastPackets	-= other.nBroadcastPackets;
		this->nBroadcastBytes	-= other.nBroadcastBytes;

		//下面两个语句不要执行：
		//this->nCurTCPConnects	-= other.nCurTCPConnects;//该语句不能执行
		//this->nCurUDPConnects	-= other.nCurUDPConnects;//该语句不能执行

		this->nOrigSendPackets -= other.nOrigSendPackets;
		this->nOrigSendBytes -= other.nOrigSendBytes;
		this->nOrigRecvPackets -= other.nOrigRecvPackets;
		this->nOrigRecvBytes -= other.nOrigRecvBytes;

		this->nOrigBroadcastPackets -= other.nOrigBroadcastPackets;
		this->nOrigBroadcastBytes -= other.nOrigBroadcastBytes;

		return *this;
	}

	tagCHANNEL_PERF_STAT_EX sub2(const tagCHANNEL_PERF_STAT_EX& other) const
	{
		tagCHANNEL_PERF_STAT_EX new_stat = *this;

		new_stat.desc2(other);

		return new_stat;
	}

public:
	inline void AddStat_Flux(const MINI_CHANNEL_PERF_STAT_EX& mini_data)
	{
		this->nSendPackets		+= mini_data.nSendPackets;
		this->nSendBytes		+= mini_data.nSendBytes;
		this->nRecvPackets		+= mini_data.nRecvPackets;
		this->nRecvBytes		+= mini_data.nRecvBytes;

		this->nRepeatPackets	+= mini_data.nRepeatPackets;
	
		this->nOrigSendPackets += mini_data.nOrigSendPackets;
		this->nOrigSendBytes += mini_data.nOrigSendBytes;
		this->nOrigRecvPackets += mini_data.nOrigRecvPackets;
		this->nOrigRecvBytes += mini_data.nOrigRecvBytes;
	}

public:
	inline void AddStat_Flux(ACE_UINT32 nBytes, BOOL bSend, BOOL bRepeat, BOOL bPass)
	{
		if( bSend )
		{
			nOrigSendPackets ++;
			nOrigSendBytes += nBytes;
		}
		else
		{
			nOrigRecvPackets ++;
			nOrigRecvBytes += nBytes;
		}

		if( bPass )
		{
			if( bSend )
			{
				nSendPackets ++;
				nSendBytes += nBytes;
			}
			else
			{
				nRecvPackets ++;
				nRecvBytes += nBytes;
			}
		}

		if( bRepeat )
		{
			nRepeatPackets ++;
		}

		return;
	}


	inline void AddStat_TcpLink(LINK_STAT_TYPE type)
	{
		switch( type )
		{
		case LINKSTAT_SUCCESS:
			nSuccessLinks ++;
			break;
		case LINKSTAT_FAIL:
			nFailedLinks ++;
			break;
		case LINKSTAT_EMPTY:
			nEmptyLinks ++;
			break;
		case LINKSTAT_RESET:
			nResetLinks ++;
			break;
		case LINKSTAT_TIMEOUT:
			nTimeoutLinks ++;
			break;
		default:
			break;
		}

		return;
	}

	inline void Inc_Connect(int linkType)
	{
		if( linkType == 0x01 )
		{
			++nCurTCPConnects;
		}
		else if( linkType == 0x02 )
		{
			++nCurUDPConnects;
		}
		else
		{
			ACE_ASSERT(FALSE);
		}

	}
	inline void Dsc_Connect(int linkType)
	{
		if( linkType == 0x01 )
		{
			--nCurTCPConnects;

			if( (ACE_INT64)nCurTCPConnects < 0 )
				nCurTCPConnects = 0;

		}
		else if( linkType == 0x02 )
		{
			--nCurUDPConnects;

			if( (ACE_INT64)nCurUDPConnects < 0 )
				nCurUDPConnects = 0;
		}
		else
		{
			ACE_ASSERT(FALSE);
		}
	}

	inline void AddStat_RespTime(ACE_UINT32 nCliRspTime, ACE_UINT32 nSrvRspTime)
	{
		nClientRespTimes += nCliRspTime;
		nServerRespTimes += nSrvRspTime;

		return;
	}

}CHANNEL_PERF_STAT_EX,*PCHANNEL_PERF_STAT_EX;

typedef struct tagREPTILE_URL_STAT
{
	tagREPTILE_URL_STAT()
	{
		reset();
	}

	inline void reset()
	{
		nURLReqNums = 0;
		nURLFailedNums = 0;
		for(int i=0; i<EN_HTTP_ERR_MAX; ++i)
			nHttpRespErr[i] = 0;
	}

public:
	ACE_UINT64	nURLReqNums;
	ACE_UINT64	nURLFailedNums;

	typedef enum _EN_HTTP_ERR_TYPE{
		EN_HTTP_ERR_200=0,	// 客户端请求成功
		EN_HTTP_ERR_400,	// Bad Request  //客户端请求有语法错误，不能被服务器所理解
		EN_HTTP_ERR_401,	// Unauthorized //请求未经授权，这个状态代码必须和WWW-Authenticate报头域一起使用 
		EN_HTTP_ERR_403,	// Forbidden  //服务器收到请求，但是拒绝提供服务
		EN_HTTP_ERR_404,	// Not Found  //请求资源不存在，eg：输入了错误的URL
		EN_HTTP_ERR_500,	// Internal Server Error //服务器发生不可预期的错误
		EN_HTTP_ERR_503,	// Server Unavailable  //服务器当前不能处理客户端的请求，一段时间后可能恢复正常
		EN_HTTP_ERR_OTHER,

		EN_HTTP_ERR_MAX,
	}EN_HTTP_ERR_TYPE;
	ACE_UINT64	nHttpRespErr[EN_HTTP_ERR_MAX];

}REPTILE_URL_STAT,*PREPTILE_URL_STAT;

typedef struct tagCHANNEL_PERF_STAT_EX2 : public CHANNEL_PERF_STAT_EX
{
public:
	tagCHANNEL_PERF_STAT_EX2()
	{
		reset();
	}

	inline void reset()
	{
		CHANNEL_PERF_STAT_EX::reset();
		url.reset();
	}

public:
	inline bool IsEmpty() const
	{
		return( CHANNEL_PERF_STAT_EX::IsEmpty() );
	}

	inline tagCHANNEL_PERF_STAT_EX2& operator = (const tagCHANNEL_PERF_STAT_EX2& other)
	{
		if (&other != this)
		{
			*(tagCHANNEL_PERF_STAT_EX *)this = *(const tagCHANNEL_PERF_STAT_EX *)&other;

			url.nURLReqNums = other.url.nURLReqNums;
			url.nURLFailedNums = other.url.nURLFailedNums;
			for(int i=0; i<REPTILE_URL_STAT::EN_HTTP_ERR_MAX; ++i)
				url.nHttpRespErr[i] = other.url.nHttpRespErr[i];
		}

		return *this;
	};

	tagCHANNEL_PERF_STAT_EX2& operator += (const tagCHANNEL_PERF_STAT_EX2& other)
	{
		*(tagCHANNEL_PERF_STAT_EX *)this += *(const tagCHANNEL_PERF_STAT_EX *)&other;

		url.nURLReqNums += other.url.nURLReqNums;
		url.nURLFailedNums += other.url.nURLFailedNums;

		for(int i=0; i<REPTILE_URL_STAT::EN_HTTP_ERR_MAX; ++i)
			url.nHttpRespErr[i] += other.url.nHttpRespErr[i];

		return *this;
	};

	tagCHANNEL_PERF_STAT_EX2 operator - (const tagCHANNEL_PERF_STAT_EX2& other) const
	{
		tagCHANNEL_PERF_STAT_EX2 new_stat = *this;

		new_stat -= other;

		return new_stat;
	}

	tagCHANNEL_PERF_STAT_EX2& operator -= (const tagCHANNEL_PERF_STAT_EX2& other)
	{
		desc2(other);

		this->nCurTCPConnects	-= other.nCurTCPConnects;
		this->nCurUDPConnects	-= other.nCurUDPConnects;

		return *this;
	};

	tagCHANNEL_PERF_STAT_EX2& desc2(const tagCHANNEL_PERF_STAT_EX2& other)
	{
		(*(tagCHANNEL_PERF_STAT_EX *)this).desc2(*(const tagCHANNEL_PERF_STAT_EX *)&other);

		url.nURLReqNums -= other.url.nURLReqNums;
		url.nURLFailedNums -= other.url.nURLFailedNums;
		for(int i=0; i<REPTILE_URL_STAT::EN_HTTP_ERR_MAX; ++i)
			this->url.nHttpRespErr[i] -= other.url.nHttpRespErr[i];



		return *this;
	}

	tagCHANNEL_PERF_STAT_EX2 sub2(const tagCHANNEL_PERF_STAT_EX2& other) const
	{
		tagCHANNEL_PERF_STAT_EX2 new_stat = *this;

		new_stat.desc2(other);

		return new_stat;
	}


public:
	void AddStat_URLRequest(int failed_code)
	{
		++url.nURLReqNums;

/*
		if( failed_code != 0 )
		{
			++url.nURLFailedNums;
		}
*/
	}

	void AddStat_URLResponse(int failed_code)
	{
		if (failed_code>REPTILE_URL_STAT::EN_HTTP_ERR_OTHER || failed_code<0)
			failed_code = REPTILE_URL_STAT::EN_HTTP_ERR_OTHER;

		url.nHttpRespErr[failed_code] ++;

		if (REPTILE_URL_STAT::EN_HTTP_ERR_200 != failed_code)
			url.nURLFailedNums ++;
	}

public:
	REPTILE_URL_STAT url;

}CHANNEL_PERF_STAT_EX2,*PCHANNEL_PERF_STAT_EX2;

#pragma pack(pop)

FLUX_CONTROL_COMMON_API
void PutStream_TimeQueryCond(char *& pdata,const TIME_QUERY_COND& cond);

FLUX_CONTROL_COMMON_API
void GetStream_TimeQueryCond(const char *& pdata,TIME_QUERY_COND& cond);

//得到probe数据的入库多少次。
//比如每5分钟入库一次，则小时表是60/5=12次，天表是12*24,..
FLUX_CONTROL_COMMON_API
int GetProbeToDBTimes(TIME_QUERY_COND::QUERY_TIME_TABLE_TYPE table_type);

//得到的间隔时间end_time-begin_time，
FLUX_CONTROL_COMMON_API
int GetHistInterval_TimeQueryCond(const TIME_QUERY_COND& cond);

//得到表的间隔时间，小时表类型=3600，天表= 3600*24
FLUX_CONTROL_COMMON_API
int GetHistInterval_TableType(TIME_QUERY_COND::QUERY_TIME_TABLE_TYPE table_type);

FLUX_CONTROL_COMMON_CLASS
std::string GetIfDataTypeString(int nCapDataType);

