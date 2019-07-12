#if  !defined(__TS_CONFIG__)
#define __TS_CONFIG__

#include "stdio.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/ostypedef.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "flux_control_common.h"
#include "TS_define.h"
#include "UNS_define.h"

// 网络部署模式
typedef enum TS_NETWORK_MODE
{
	L2_MODE = 1,		 // 二层交换机模式
	L3_MODE,			// 三层交换机模式
	LAN_MODE,		//局域网延伸
	//TWO_R,       // 双路由器模式
}TS_NETWORK_MODE_E;

typedef enum TS_FORWARD_MODE
{
	ASYNC_MODE,	//异步模式，主断备传
	SYNC_MODE	//同步模式，主备同传
}TS_FORWARD_MODE_E;

class FLUX_CONTROL_COMMON_CLASS CTS_GLOBAL_CONFIG
{
public:
	CTS_GLOBAL_CONFIG();
	~CTS_GLOBAL_CONFIG();
	
public:
	// 网络部署模式
	TS_NETWORK_MODE_E m_eNetworkMode;

	// 传输方式，0:主断备传 1:主备同传
	TS_FORWARD_MODE_E m_eForwardMode;

	// 可靠传输的缓存空间(字节)
	unsigned int m_nQueueSize;
	
public:
	BOOL operator=(const CTS_GLOBAL_CONFIG &other) ;
	BOOL operator==(const CTS_GLOBAL_CONFIG &other) const;
	BOOL operator!=(const CTS_GLOBAL_CONFIG &other) const;

};

// TS(流量切换器)镜像口配置
class FLUX_CONTROL_COMMON_CLASS CTS_MIRROR_CONFIG
{
public:
	CTS_MIRROR_CONFIG();
	~CTS_MIRROR_CONFIG();

public:
	// 占用网卡名字
	std::string m_strCardName;

public:
	BOOL operator=(const CTS_MIRROR_CONFIG &other) ;
	BOOL operator==(const CTS_MIRROR_CONFIG &other) const;
	BOOL operator!=(const CTS_MIRROR_CONFIG &other) const;
};
//////////////////

// TS(流量切换器)监测口配置
class FLUX_CONTROL_COMMON_CLASS CTS_MAINNET_CONFIG
{
public:
	CTS_MAINNET_CONFIG();
	~CTS_MAINNET_CONFIG();

public:
	// 占用网卡名字
	std::string m_strCardName;

	// 使用IP
	ACE_UINT32 m_hlIpAddr;
	ACE_UINT32 m_hlNetmask;

	// 网关(路由器R)IP
	ACE_UINT32 m_hlGateway;

	
	ACE_UINT32 m_nDetectPeriod;// 远程网络的探测频率
	ACE_UINT32 m_nDetectQueue; // 表的记录个数
	ACE_UINT32 m_nSwitchLostThreshold; //切换丢包率门限(丢包个数)
	ACE_UINT32 m_nRecoverLostThreshold;//恢复丢包率门限(丢包个数)
	ACE_UINT32 m_nRecoverTimeout;//稳定的时间标准

public:
	void operator=(const CTS_MAINNET_CONFIG &other);
	BOOL operator==(const CTS_MAINNET_CONFIG &other) const;
	BOOL operator!=(const CTS_MAINNET_CONFIG &other) const;
};
//////////////////

// TS(流量切换器)B 口配置
class FLUX_CONTROL_COMMON_CLASS CTS_BACKNET_CONFIG
{
public:
	CTS_BACKNET_CONFIG();
	~CTS_BACKNET_CONFIG();
public:
	// 占用网卡名字
	std::string m_strCardName;

	// 使用IP
	ACE_UINT32 m_hlIpAddr;
	ACE_UINT32 m_hlNetmask;

	// 网关IP
	ACE_UINT32 m_hlGateway;

	// 远程网络的探测频率
	ACE_UINT32 m_nDetectPeriod;
	ACE_UINT32 m_nDetectQueue; //表的记录个数
	ACE_UINT32 m_nSwitchLostThreshold; //切换丢包率门限(丢包个数)
	ACE_UINT32 m_nRecoverLostThreshold;//恢复丢包率门限(丢包个数)
	ACE_UINT32 m_nRecoverTimeout;//稳定的时间标准

	std::vector<ACE_UINT32> m_vtFilterId;
	
public:
	void operator=(const CTS_BACKNET_CONFIG &other) ;
	BOOL operator==(const CTS_BACKNET_CONFIG &other) const;
	BOOL operator!=(const CTS_BACKNET_CONFIG &other) const;
};

class FLUX_CONTROL_COMMON_CLASS CTS_BACKCOM_CONFIG
{
public:
	CTS_BACKCOM_CONFIG();
	~CTS_BACKCOM_CONFIG();

public:
	// 串口名字, "/dev/ttyM2"
	std::string m_strCardName;
	// 波特率
	unsigned int m_nSpeed;
	// 数据位
	unsigned int m_nCs;
	// 奇偶校验
	unsigned int m_nParity;
	// 停止位
	unsigned int m_nStop;

	std::vector<ACE_UINT32> m_vtFilterId;
	
public:
	void operator=(const CTS_BACKCOM_CONFIG &other) ;
	BOOL operator==(const CTS_BACKCOM_CONFIG &other) const;
	BOOL operator!=(const CTS_BACKCOM_CONFIG &other) const;
};

// 目标网络配置
class FLUX_CONTROL_COMMON_CLASS CTS_PEER_CONFIG
{
public:
	CTS_PEER_CONFIG();
	~CTS_PEER_CONFIG();

public:
	ACE_UINT32 m_nPeerId;
	
	// 主网的测试IP，唯一的IP
	std::string 	m_strMainInt;
	ACE_UINT32 	m_nMainTestIp;

	// B1的测试IP
	std::string 	m_strB1Int;
	ACE_UINT32 	m_nB1VcgId;
	ACE_UINT32 	m_nB1TestIp;

	// B2的测试IP
	std::string 	m_strB2Int;
	ACE_UINT32 	m_nB2VcgId;
	ACE_UINT32 	m_nB2TestIp;
	
	// 网内的主机IP段,例如: "192.168.0.1-192.168.0.255"的IP地址范围
	std::vector< std::pair<ACE_UINT32, ACE_UINT32> > m_vtPeerIpRange;

public:
	//BOOL operator=(const CTS_PEER_CONFIG &other) ;
	BOOL operator==(const CTS_PEER_CONFIG &other) const;
	BOOL operator!=(const CTS_PEER_CONFIG &other) const;
};

// IP首部协议标识
typedef enum __ENUM_IP_PROTOCOL__
{
	IP_PROTOCOL_ICMP = 1,
	IP_PROTOCOL_IGMP = 2,
	IP_PROTOCOL_TCP = 6,
	IP_PROTOCOL_UDP = 17
}ENUM_IP_PROTOCOL;

//过滤器配置
class FLUX_CONTROL_COMMON_CLASS CTS_FILTER_CONFIG
{
public:
	CTS_FILTER_CONFIG();
	~CTS_FILTER_CONFIG();

public:
	// 
	ACE_UINT32 id;

	// 
	std::string name;

	// 网内的主机IP段,例如: "192.168.0.1-192.168.0.255"的IP地址范围
	std::vector< std::pair<ACE_UINT32, ACE_UINT32> > src_ip_rang;

	// 网内的主机IP段,例如: "192.168.0.1-192.168.0.255"的IP地址范围
	std::vector< std::pair<ACE_UINT32, ACE_UINT32> > dst_ip_rang;

	// 
	ACE_UINT32 protocol_id;

	// 
	std::vector< std::pair<ACE_UINT16, ACE_UINT16> > port;

public:
	BOOL operator=(const CTS_FILTER_CONFIG &other) ;
	BOOL operator==(const CTS_FILTER_CONFIG &other) const;
	BOOL operator!=(const CTS_FILTER_CONFIG &other) const;
};
//////////////////

//管理端口配置
class FLUX_CONTROL_COMMON_CLASS CTS_MGMT_CONFIG
{
public:
	CTS_MGMT_CONFIG();
	~CTS_MGMT_CONFIG();

public:
	// 占用网卡名字
	std::string m_strCardName;

	// 使用IP
	ACE_UINT32 m_hlIpAddr;

	// 使用IP
	ACE_UINT32 m_hlNetmask;

	// 网关IP
	ACE_UINT32 m_hlGateway;

public:
	BOOL operator=(const CTS_MGMT_CONFIG &other) ;
	BOOL operator==(const CTS_MGMT_CONFIG &other) const;
	BOOL operator!=(const CTS_MGMT_CONFIG &other) const;
};

//VCG配置
class FLUX_CONTROL_COMMON_CLASS CE1
{
public:
	CE1()
	{
		m_ID = 0;
		m_Name.clear();
	}
	~CE1()
	{
		m_Name.clear();
	}
public:
	ACE_UINT32 m_ID;
	std::string m_Name;
public:
	BOOL operator=(const CE1 &other)
	{
		m_ID = other.m_ID;
		m_Name = other.m_Name;

		return TRUE;
	}
	BOOL operator==(const CE1 &other) const
	{
		if (m_ID != other.m_ID){return FALSE;}
		if (m_Name != other.m_Name){return FALSE;}

		return TRUE;
	}
	BOOL operator!=(const CE1 &other) const
	{
		return(!(*this == other));
	}
};
class FLUX_CONTROL_COMMON_CLASS CVcg
{
public:
	CVcg()
	{
		m_ID = 0;
		m_ChannelIndex = 0;
		m_Name.clear();
		m_Vlan = 0;
		m_vtE1Member.clear();
	}
	~CVcg()
	{
		m_Name.clear();
		m_vtE1Member.clear();
	}
public:
	ACE_UINT32 m_ID;//对应vcg_config.xml的VCG ID
	ACE_UINT32 m_ChannelIndex;//配置E1汇聚板时由E1汇聚板生成，给SNMP管理使用
	std::string m_Name;
	ACE_UINT32 m_Vlan;
	std::vector<ACE_UINT32>	m_vtE1Member;
public:
	BOOL operator==(const CVcg &other) const
	{
		if (m_ID != other.m_ID){return FALSE;}
		if (m_ChannelIndex != other.m_ChannelIndex){return FALSE;}
		if (m_Name != other.m_Name){return FALSE;}
		if (m_Vlan != other.m_Vlan){return FALSE;}
		if (m_vtE1Member != other.m_vtE1Member){return FALSE;}

		return TRUE;
	}
	BOOL operator!=(const CVcg &other) const
	{
		return(!(*this == other));
	}
};
class FLUX_CONTROL_COMMON_CLASS CTS_SGL_CONFIG
{

public:
	CTS_SGL_CONFIG();
	~CTS_SGL_CONFIG();

public:
	std::string m_strCardName;
	ACE_UINT32 m_E1Max;
	
	std::vector< CE1 >		m_vtE1List;
	std::vector< CVcg >		m_vtVcgList;
	
public:
	BOOL operator=(const CTS_SGL_CONFIG &other) ;
	BOOL operator==(const CTS_SGL_CONFIG &other) const;
	BOOL operator!=(const CTS_SGL_CONFIG &other) const;
};


// TS 的配置信息
class FLUX_CONTROL_COMMON_CLASS CTS_CONFIG
{
public:
	CTS_CONFIG();
	~CTS_CONFIG();

public:
	
	ACE_Log_File_Msg* m_pLogFile;

	CTS_GLOBAL_CONFIG m_iGlobalCfg;

	CTS_MIRROR_CONFIG m_iMirrorCfg;

	CTS_MAINNET_CONFIG m_iMainNetCfg;

	CTS_BACKNET_CONFIG m_iBackNet1Cfg;

	CTS_BACKNET_CONFIG m_iBackNet2Cfg;

	CTS_BACKCOM_CONFIG m_iBackCom1Cfg;

	CTS_BACKCOM_CONFIG m_iBackCom2Cfg;

	// 管理端口配置
	CTS_MGMT_CONFIG m_iLocalMgmtCfg;

	// 目标网络配置
	std::vector<CTS_PEER_CONFIG> m_vtPeerCfg;

	// 本地主机IP范围
	std::vector< std::pair<ACE_UINT32, ACE_UINT32> > m_vtLocalHostCfg;

	// 过滤器配置
	std::vector<CTS_FILTER_CONFIG> m_vtFilterCfg;

	// VCG配置
	CTS_SGL_CONFIG m_iSglCard1;
	CTS_SGL_CONFIG m_iSglCard2;

private:
	 // xml文件读取类
	CTinyXmlEx xml_doc;

public:
	BOOL f_Init(ACE_Log_File_Msg * pLogFile);
	int f_GetLocalMgmtConfig(TiXmlNode * RootNode);
	int f_GetTsGlobalConfig(TiXmlNode * RootNode);
	int f_GetLocalHostConfig(TiXmlNode * RootNode);
	int f_GetTsMirrorConfig(TiXmlNode * RootNode);
	int f_GetMainNetConfig(TiXmlNode * RootNode);
	int f_GetBackNet1Config(TiXmlNode * RootNode);
	int f_GetBackNet2Config(TiXmlNode * RootNode);
	int f_GetBackCom1Config(TiXmlNode * RootNode);
	int f_GetBackCom2Config(TiXmlNode * RootNode);
	int f_GetLocalConfig(char * xml_fname);
	int f_GetPeerConfig(char * xml_fname);
	int f_GetFilterConfig(char * xml_fname);
	int f_GetVcgConfig(char * xml_fname);
	int f_ReadConfigFile(void);
	
	BOOL f_IsPeerHostIp(ACE_UINT32 hlHostIp, ACE_UINT32 & peer_index);
	BOOL f_IsLocalHostIp(ACE_UINT32 hlHostIp);
	CTS_PEER_CONFIG* f_FindPeerCfg(ACE_UINT32 id);
	CTS_FILTER_CONFIG* f_FindFilter(ACE_UINT32 id);
	CVcg* f_FindVcg(ACE_UINT32 id);
	BOOL f_FindAllCardName(std::string& all_card_name);
	BOOL f_SetLocalMgmt(void);
	BOOL f_SetMainNetMgmt(void);
	BOOL f_SetBackNet1Mgmt(void);
	BOOL f_SetBackNet2Mgmt(void);
};
//////////////////

#include "ace/Singleton.h"
#include "ace/Null_Mutex.h"

class FLUX_CONTROL_COMMON_CLASS CTS_CONFIG_Singleton : public ACE_Singleton<CTS_CONFIG, ACE_Null_Mutex>
{
public:
	static CTS_CONFIG * instance()
	{
		return ACE_Singleton<CTS_CONFIG, ACE_Null_Mutex>::instance();
	}

};

#endif
