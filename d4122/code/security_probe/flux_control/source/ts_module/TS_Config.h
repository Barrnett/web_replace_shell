#if  !defined(__TS_CONFIG__)
#define __TS_CONFIG__

#include "stdio.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/ostypedef.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "flux_control_common.h"
#include "TS_define.h"
#include "UNS_define.h"

// ���粿��ģʽ
typedef enum TS_NETWORK_MODE
{
	L2_MODE = 1,		 // ���㽻����ģʽ
	L3_MODE,			// ���㽻����ģʽ
	LAN_MODE,		//����������
	//TWO_R,       // ˫·����ģʽ
}TS_NETWORK_MODE_E;

typedef enum TS_FORWARD_MODE
{
	ASYNC_MODE,	//�첽ģʽ�����ϱ���
	SYNC_MODE	//ͬ��ģʽ������ͬ��
}TS_FORWARD_MODE_E;

class FLUX_CONTROL_COMMON_CLASS CTS_GLOBAL_CONFIG
{
public:
	CTS_GLOBAL_CONFIG();
	~CTS_GLOBAL_CONFIG();
	
public:
	// ���粿��ģʽ
	TS_NETWORK_MODE_E m_eNetworkMode;

	// ���䷽ʽ��0:���ϱ��� 1:����ͬ��
	TS_FORWARD_MODE_E m_eForwardMode;

	// �ɿ�����Ļ���ռ�(�ֽ�)
	unsigned int m_nQueueSize;
	
public:
	BOOL operator=(const CTS_GLOBAL_CONFIG &other) ;
	BOOL operator==(const CTS_GLOBAL_CONFIG &other) const;
	BOOL operator!=(const CTS_GLOBAL_CONFIG &other) const;

};

// TS(�����л���)���������
class FLUX_CONTROL_COMMON_CLASS CTS_MIRROR_CONFIG
{
public:
	CTS_MIRROR_CONFIG();
	~CTS_MIRROR_CONFIG();

public:
	// ռ����������
	std::string m_strCardName;

public:
	BOOL operator=(const CTS_MIRROR_CONFIG &other) ;
	BOOL operator==(const CTS_MIRROR_CONFIG &other) const;
	BOOL operator!=(const CTS_MIRROR_CONFIG &other) const;
};
//////////////////

// TS(�����л���)��������
class FLUX_CONTROL_COMMON_CLASS CTS_MAINNET_CONFIG
{
public:
	CTS_MAINNET_CONFIG();
	~CTS_MAINNET_CONFIG();

public:
	// ռ����������
	std::string m_strCardName;

	// ʹ��IP
	ACE_UINT32 m_hlIpAddr;
	ACE_UINT32 m_hlNetmask;

	// ����(·����R)IP
	ACE_UINT32 m_hlGateway;

	
	ACE_UINT32 m_nDetectPeriod;// Զ�������̽��Ƶ��
	ACE_UINT32 m_nDetectQueue; // ��ļ�¼����
	ACE_UINT32 m_nSwitchLostThreshold; //�л�����������(��������)
	ACE_UINT32 m_nRecoverLostThreshold;//�ָ�����������(��������)
	ACE_UINT32 m_nRecoverTimeout;//�ȶ���ʱ���׼

public:
	void operator=(const CTS_MAINNET_CONFIG &other);
	BOOL operator==(const CTS_MAINNET_CONFIG &other) const;
	BOOL operator!=(const CTS_MAINNET_CONFIG &other) const;
};
//////////////////

// TS(�����л���)B ������
class FLUX_CONTROL_COMMON_CLASS CTS_BACKNET_CONFIG
{
public:
	CTS_BACKNET_CONFIG();
	~CTS_BACKNET_CONFIG();
public:
	// ռ����������
	std::string m_strCardName;

	// ʹ��IP
	ACE_UINT32 m_hlIpAddr;
	ACE_UINT32 m_hlNetmask;

	// ����IP
	ACE_UINT32 m_hlGateway;

	// Զ�������̽��Ƶ��
	ACE_UINT32 m_nDetectPeriod;
	ACE_UINT32 m_nDetectQueue; //��ļ�¼����
	ACE_UINT32 m_nSwitchLostThreshold; //�л�����������(��������)
	ACE_UINT32 m_nRecoverLostThreshold;//�ָ�����������(��������)
	ACE_UINT32 m_nRecoverTimeout;//�ȶ���ʱ���׼

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
	// ��������, "/dev/ttyM2"
	std::string m_strCardName;
	// ������
	unsigned int m_nSpeed;
	// ����λ
	unsigned int m_nCs;
	// ��żУ��
	unsigned int m_nParity;
	// ֹͣλ
	unsigned int m_nStop;

	std::vector<ACE_UINT32> m_vtFilterId;
	
public:
	void operator=(const CTS_BACKCOM_CONFIG &other) ;
	BOOL operator==(const CTS_BACKCOM_CONFIG &other) const;
	BOOL operator!=(const CTS_BACKCOM_CONFIG &other) const;
};

// Ŀ����������
class FLUX_CONTROL_COMMON_CLASS CTS_PEER_CONFIG
{
public:
	CTS_PEER_CONFIG();
	~CTS_PEER_CONFIG();

public:
	ACE_UINT32 m_nPeerId;
	
	// �����Ĳ���IP��Ψһ��IP
	std::string 	m_strMainInt;
	ACE_UINT32 	m_nMainTestIp;

	// B1�Ĳ���IP
	std::string 	m_strB1Int;
	ACE_UINT32 	m_nB1VcgId;
	ACE_UINT32 	m_nB1TestIp;

	// B2�Ĳ���IP
	std::string 	m_strB2Int;
	ACE_UINT32 	m_nB2VcgId;
	ACE_UINT32 	m_nB2TestIp;
	
	// ���ڵ�����IP��,����: "192.168.0.1-192.168.0.255"��IP��ַ��Χ
	std::vector< std::pair<ACE_UINT32, ACE_UINT32> > m_vtPeerIpRange;

public:
	//BOOL operator=(const CTS_PEER_CONFIG &other) ;
	BOOL operator==(const CTS_PEER_CONFIG &other) const;
	BOOL operator!=(const CTS_PEER_CONFIG &other) const;
};

// IP�ײ�Э���ʶ
typedef enum __ENUM_IP_PROTOCOL__
{
	IP_PROTOCOL_ICMP = 1,
	IP_PROTOCOL_IGMP = 2,
	IP_PROTOCOL_TCP = 6,
	IP_PROTOCOL_UDP = 17
}ENUM_IP_PROTOCOL;

//����������
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

	// ���ڵ�����IP��,����: "192.168.0.1-192.168.0.255"��IP��ַ��Χ
	std::vector< std::pair<ACE_UINT32, ACE_UINT32> > src_ip_rang;

	// ���ڵ�����IP��,����: "192.168.0.1-192.168.0.255"��IP��ַ��Χ
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

//����˿�����
class FLUX_CONTROL_COMMON_CLASS CTS_MGMT_CONFIG
{
public:
	CTS_MGMT_CONFIG();
	~CTS_MGMT_CONFIG();

public:
	// ռ����������
	std::string m_strCardName;

	// ʹ��IP
	ACE_UINT32 m_hlIpAddr;

	// ʹ��IP
	ACE_UINT32 m_hlNetmask;

	// ����IP
	ACE_UINT32 m_hlGateway;

public:
	BOOL operator=(const CTS_MGMT_CONFIG &other) ;
	BOOL operator==(const CTS_MGMT_CONFIG &other) const;
	BOOL operator!=(const CTS_MGMT_CONFIG &other) const;
};

//VCG����
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
	ACE_UINT32 m_ID;//��Ӧvcg_config.xml��VCG ID
	ACE_UINT32 m_ChannelIndex;//����E1��۰�ʱ��E1��۰����ɣ���SNMP����ʹ��
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


// TS ��������Ϣ
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

	// ����˿�����
	CTS_MGMT_CONFIG m_iLocalMgmtCfg;

	// Ŀ����������
	std::vector<CTS_PEER_CONFIG> m_vtPeerCfg;

	// ��������IP��Χ
	std::vector< std::pair<ACE_UINT32, ACE_UINT32> > m_vtLocalHostCfg;

	// ����������
	std::vector<CTS_FILTER_CONFIG> m_vtFilterCfg;

	// VCG����
	CTS_SGL_CONFIG m_iSglCard1;
	CTS_SGL_CONFIG m_iSglCard2;

private:
	 // xml�ļ���ȡ��
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
