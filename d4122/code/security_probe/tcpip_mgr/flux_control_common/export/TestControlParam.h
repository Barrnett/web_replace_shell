//////////////////////////////////////////////////////////////////////////
//TestControlParam.h

#pragma once

#include "flux_control_common.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/addr_tools.h"
#include "cpf/AddrAndPortSet.h"

class FLUX_CONTROL_COMMON_CLASS CTestControlParam
{
public:
	CTestControlParam(void);
	~CTestControlParam(void);

public:
	int					m_bypass_time_out;
	BOOL				m_bResetWDTInLoop;
	BOOL				m_bResetWDTInUI;

	BOOL				m_bSyncOnAddStat;
	BOOL				m_bSyncOnDelStat;

	int					m_live_log_size;//单位KB
	unsigned int		m_live_log_level;//记录级别,大于该级别的都要记录，-1表示都不记录

	int					m_bTraceCode;
	BOOL				m_bCreateDump;
	int					m_log_flux;
	
	//<!--淘汰掉旧的统计的间隔时间-->	
	int					m_update_stat_interval;

public:
	int				m_uk_server_stat_nums;
	int				m_uk_port_stat_nums;

	int				m_peer_server_stat_nums;
	int				m_peer_port_stat_nums;

	int				m_webvideo_time;	// 秒
	int				m_webvideo_speed;	// kbps

public:
	typedef struct tagLINK_TIME_PARAM
	{
		int operator==(const tagLINK_TIME_PARAM& other) const
		{
			return (
				begin_ip == other.begin_ip
				&& end_ip == other.end_ip
				&& begin_port == other.begin_port
				&& end_port == other.end_port
				&& timeout_sec == other.timeout_sec 
				);
		}

		ACE_UINT32		begin_ip;
		ACE_UINT32		end_ip;
		unsigned int	begin_port;
		unsigned int	end_port;
		ACE_UINT32		timeout_sec;
	}LINK_TIME_PARAM;
	
	std::vector<LINK_TIME_PARAM>	m_vt_tcp_timeout_param;
	std::vector<LINK_TIME_PARAM>	m_vt_udp_timeout_param;

	ACE_UINT32				m_default_tcp_timeout;//tcp的超时时间，单位second
	ACE_UINT32				m_default_udp_timeout;//udp的超时时间，单位second

	ACE_UINT32				m_tcp_syn_timeout;//tcp建立连接超时，单位second
	ACE_UINT32				m_udp_syn_timeout;//tcp建立连接超时，单位second

public:
	int				m_empty_sleep_time;//空转等待时间

public:
	int				m_b_automap;//是否启动智能学习

	int				m_set_cfg_in_protect_mode;//保护模式下是否设置网卡地址

	int				m_bCopyDrvData;
	int				m_buffer_pkt_thread;

	int				m_nProtoDepth;//协议识别的深度，0表示最深，1表示只根据端口来识别，不做更深的识别。其他的暂时没有定义
	int				m_bDFI;

	int				m_live_app_stat;//是不是做实时流量统计

	int				m_flux_control_handle_data;//流控是否处理数据，缺省是1，如果不处理，则不进行流量控制和流量分析

public:
	int				m_http_line_ingore;//http头识别的忽略
	int				m_http_data_ingore;//http数据的忽略
	
	int				m_http_header_find_optimize;
	int				m_http_line_find_optimize;


	//加密P2P识别的参数
	unsigned int	m_cryp2p_param;

public:
	//0表示不发送数据到审计模块，1表示全部发送，2表示按照下面条件发送
	int						m_data_to_audit_type;
	CAddrAndPortSetVector	m_data_to_audit_addr_port_cond;

private:
	void close();

	void add_defalut_bypaas_port();

public:
	void init_default(BOOL bXiaocao);

private:
	std::vector< std::pair<unsigned int,std::string> >	m_vt_bypass_udp_port;
	std::vector< std::pair<unsigned int,std::string> >	m_vt_bypass_tcp_port;

	unsigned int	m_array_bypass_udp_port[100];
	int				m_bypass_udp_port_nums;

	unsigned int	m_array_bypass_tcp_port[100];
	int				m_bypass_tcp_port_nums;


private:
	inline static BOOL IsByPassport(const unsigned int *array_bypass_port,int nums,unsigned int port1,unsigned int port2)
	{
		for( int i = 0; i < nums; ++i)
		{
			if( (array_bypass_port[i] == port1 || array_bypass_port[i] == port2) )
			{
				return true;
			}
		}

		return false;
	}

private:
	void Format_bypass_udp_port();
	void Format_bypass_tcp_port();

public:
	const std::vector< std::pair<unsigned int,std::string> >& GetBypassUdpPort() const
	{
		return m_vt_bypass_udp_port;
	}
	const std::vector< std::pair<unsigned int,std::string> >& GetBypassTcpPort() const
	{
		return m_vt_bypass_tcp_port;
	}

	void SetBypassUdpPort(std::vector< std::pair<unsigned int,std::string> >& vt_bypass_udp_port);
	void SetBypassTcpPort(std::vector< std::pair<unsigned int,std::string> >& vt_bypass_tcp_port);
	
public:
	BOOL IsByPassport(int type,unsigned int port1,unsigned int port2) const;

public:
	int operator == (const CTestControlParam& other) const;

	int operator != (const CTestControlParam& other) const
	{
		return !(*this == other);
	}

	CTestControlParam& operator=(const CTestControlParam& other);

	//变化很大，需要重新启动程序
	BOOL IsBigChanged(const CTestControlParam& other) const;

public:
	BOOL Write(const char * xml_file_name);
	BOOL Write();
	BOOL Read(const char * xml_file_name);

private:
	BOOL WriteByPassPort(CTinyXmlEx& xml_file,TiXmlNode * parent);
	void ReadByPassPort(CTinyXmlEx& xml_file,TiXmlNode * parent);

private:
	BOOL WriteLinkTimeOutParam(CTinyXmlEx& xml_file,TiXmlNode * parent);
	void ReadLinkTimeOutParam(CTinyXmlEx& xml_file,TiXmlNode * parent);

public:
	ACE_UINT32 GetLinkTimeout_Tcp(const IPv4_CONNECT_ADDR& addr) const;
	ACE_UINT32 GetLinkTimeout_Udp(const IPv4_CONNECT_ADDR& addr) const;

	ACE_UINT32 GetLinkTimeout_Tcp(const IPv4_TRANSPORT_ADDR& addr) const;
	ACE_UINT32 GetLinkTimeout_Udp(const IPv4_TRANSPORT_ADDR& addr) const;

private:
	static inline BOOL IsInLinkTimeoutParam(const LINK_TIME_PARAM& link_timeout_param,const IPv4_CONNECT_ADDR& addr)
	{
		if( IsInLinkTimeoutParam(link_timeout_param,addr.server) )
		{
			return true;
		}

		if( IsInLinkTimeoutParam(link_timeout_param,addr.client) )
		{
			return true;
		}

		return false;
	}

	static inline BOOL IsInLinkTimeoutParam(const LINK_TIME_PARAM& link_timeout_param,const IPv4_TRANSPORT_ADDR& trans_addr)
	{
		if( trans_addr.dwIP >= link_timeout_param.begin_ip && trans_addr.dwIP <= link_timeout_param.end_ip
			&& trans_addr.wdPort >= link_timeout_param.begin_port && trans_addr.wdPort <= link_timeout_param.end_port )
		{
			return true;
		}

		return false;
	}

private:
	char	m_file_name[MAX_PATH];

};

#include "ace/Singleton.h"
#include "ace/Null_Mutex.h"

class FLUX_CONTROL_COMMON_CLASS CCTestControlParam_Singleton : public ACE_Singleton<CTestControlParam,ACE_Null_Mutex>
{
public:
	static CTestControlParam * instance()
	{
		return ACE_Singleton<CTestControlParam,ACE_Null_Mutex>::instance();
	}

};
