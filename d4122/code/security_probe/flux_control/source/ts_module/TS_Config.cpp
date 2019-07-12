//#include "StdAfx.h"
#include "TS_Config.h"
#include "config_center.h"

CTS_GLOBAL_CONFIG::CTS_GLOBAL_CONFIG()
{
	m_eNetworkMode = L2_MODE;
	m_eForwardMode = ASYNC_MODE;
	m_nQueueSize = 16000;
}

CTS_GLOBAL_CONFIG::~CTS_GLOBAL_CONFIG()
{
}

BOOL CTS_GLOBAL_CONFIG::operator=(const CTS_GLOBAL_CONFIG &other)
{
	m_eNetworkMode = other.m_eNetworkMode;
	m_eForwardMode = other.m_eForwardMode;
	m_nQueueSize = other.m_nQueueSize;
	
	return TRUE;
}

BOOL CTS_GLOBAL_CONFIG::operator==(const CTS_GLOBAL_CONFIG &other) const
{
	if (m_eNetworkMode != other.m_eNetworkMode){return FALSE;}
	if (m_eForwardMode != other.m_eForwardMode){return FALSE;}
	if (m_nQueueSize != other.m_nQueueSize){return FALSE;}
	
	return TRUE;
}

BOOL CTS_GLOBAL_CONFIG::operator!=(const CTS_GLOBAL_CONFIG &other) const
{
	return(!(*this == other));
}

CTS_MIRROR_CONFIG::CTS_MIRROR_CONFIG()
{
	m_strCardName.clear();
}

CTS_MIRROR_CONFIG::~CTS_MIRROR_CONFIG()
{
}

BOOL CTS_MIRROR_CONFIG::operator=(const CTS_MIRROR_CONFIG &other)
{
	m_strCardName = other.m_strCardName;
	return(TRUE);
}

BOOL CTS_MIRROR_CONFIG::operator==(const CTS_MIRROR_CONFIG &other) const
{
	if(m_strCardName != other.m_strCardName){ return(FALSE); }
	return(TRUE);
}

BOOL CTS_MIRROR_CONFIG::operator!=(const CTS_MIRROR_CONFIG &other) const
{
	return(!(*this == other));
}

CTS_MAINNET_CONFIG::CTS_MAINNET_CONFIG()
{
}

CTS_MAINNET_CONFIG::~CTS_MAINNET_CONFIG()
{
}

void CTS_MAINNET_CONFIG::operator=(const CTS_MAINNET_CONFIG &other) 
{
	m_strCardName = other.m_strCardName;
	m_hlIpAddr = other.m_hlIpAddr;
	m_hlNetmask = other.m_hlNetmask;
	m_hlGateway = other.m_hlGateway;
	m_nDetectPeriod = other.m_nDetectPeriod;
	m_nDetectQueue = other.m_nDetectQueue;
	m_nSwitchLostThreshold = other.m_nSwitchLostThreshold;
	m_nRecoverLostThreshold = other.m_nRecoverLostThreshold;
	m_nRecoverTimeout = other.m_nRecoverTimeout;
}

BOOL CTS_MAINNET_CONFIG::operator==(const CTS_MAINNET_CONFIG &other) const
{
	if(m_strCardName != other.m_strCardName){ return(FALSE); }
	if(m_hlIpAddr != other.m_hlIpAddr){ return(FALSE); }
	if(m_hlNetmask != other.m_hlNetmask){ return(FALSE); }
	if(m_hlGateway != other.m_hlGateway){ return(FALSE); }
	if(m_nDetectPeriod != other.m_nDetectPeriod){return FALSE;}
	if(m_nDetectQueue != other.m_nDetectQueue){return FALSE;}
	if(m_nSwitchLostThreshold != other.m_nSwitchLostThreshold){return FALSE;}
	if(m_nRecoverLostThreshold != other.m_nRecoverLostThreshold){return FALSE;}
	if(m_nRecoverTimeout != other.m_nRecoverTimeout){return FALSE;}

	return(TRUE);
}

BOOL CTS_MAINNET_CONFIG::operator!=(const CTS_MAINNET_CONFIG &other) const
{
	return(!(*this == other));
}


CTS_BACKNET_CONFIG::CTS_BACKNET_CONFIG()
{
}

CTS_BACKNET_CONFIG::~CTS_BACKNET_CONFIG()
{
}

void CTS_BACKNET_CONFIG::operator=(const CTS_BACKNET_CONFIG &other) 
{
	m_strCardName = other.m_strCardName;
	m_hlIpAddr = other.m_hlIpAddr;
	m_hlNetmask = other.m_hlNetmask;
	m_hlGateway = other.m_hlGateway;
	m_nDetectPeriod = other.m_nDetectPeriod;
	m_nDetectQueue = other.m_nDetectQueue;
	m_nSwitchLostThreshold = other.m_nSwitchLostThreshold;
	m_nRecoverLostThreshold = other.m_nRecoverLostThreshold;
}

BOOL CTS_BACKNET_CONFIG::operator==(const CTS_BACKNET_CONFIG &other) const
{
	if(m_strCardName != other.m_strCardName){ return(FALSE); }
	if(m_hlIpAddr != other.m_hlIpAddr){ return(FALSE); }
	if(m_hlNetmask != other.m_hlNetmask){ return(FALSE); }
	if(m_hlGateway != other.m_hlGateway){ return(FALSE); }
	if(m_nDetectPeriod != other.m_nDetectPeriod){return FALSE;}
	if(m_nDetectQueue != other.m_nDetectQueue){return FALSE;}
	if(m_nSwitchLostThreshold != other.m_nSwitchLostThreshold){return FALSE;}
	if(m_nRecoverLostThreshold != other.m_nRecoverLostThreshold){return FALSE;}

	return TRUE;
}

BOOL CTS_BACKNET_CONFIG::operator!=(const CTS_BACKNET_CONFIG &other) const
{
	return(!(*this == other));
}


CTS_BACKCOM_CONFIG::CTS_BACKCOM_CONFIG()
{
}

CTS_BACKCOM_CONFIG::~CTS_BACKCOM_CONFIG()
{
}

void CTS_BACKCOM_CONFIG::operator=(const CTS_BACKCOM_CONFIG &other) 
{
	m_strCardName = other.m_strCardName;
	m_nSpeed = other.m_nSpeed;
	m_nCs = other.m_nCs;
	m_nParity = other.m_nParity;
	m_nStop= other.m_nStop;
}

BOOL CTS_BACKCOM_CONFIG::operator==(const CTS_BACKCOM_CONFIG &other) const
{
	if(m_strCardName != other.m_strCardName){ return(FALSE); }
	if(m_nSpeed != other.m_nSpeed){ return(FALSE); }
	if(m_nCs != other.m_nCs){ return(FALSE); }
	if(m_nParity != other.m_nParity){ return(FALSE); }
	if(m_nStop != other.m_nStop){return FALSE;}

	return TRUE;
}

BOOL CTS_BACKCOM_CONFIG::operator!=(const CTS_BACKCOM_CONFIG &other) const
{
	return(!(*this == other));
}

CTS_PEER_CONFIG::CTS_PEER_CONFIG()
{
	m_nPeerId = 0;
	
	m_strMainInt.clear();
	m_nMainTestIp = 0;

	m_strB1Int.clear();
	m_nB1VcgId = 0;
	m_nB1TestIp = 0;

	m_strB2Int.clear();
	m_nB2VcgId = 0;
	m_nB2TestIp = 0;
	
	m_vtPeerIpRange.clear();
}

CTS_PEER_CONFIG::~CTS_PEER_CONFIG()
{
}

BOOL CTS_PEER_CONFIG::operator==(const CTS_PEER_CONFIG &other) const
{
	if(m_nPeerId != other.m_nPeerId){return FALSE;}
	if(m_strMainInt != other.m_strMainInt){return FALSE;}
	if(m_nMainTestIp != other.m_nMainTestIp){ return(FALSE); }
	if(m_strB1Int != other.m_strB1Int){ return(FALSE); }
	if(m_nB1VcgId != other.m_nB1VcgId){ return(FALSE); }
	if(m_nB1TestIp != other.m_nB1TestIp){ return(FALSE); }
	if(m_strB2Int != other.m_strB2Int){ return(FALSE); }
	if(m_nB2VcgId != other.m_nB2VcgId){ return(FALSE); }
	if(m_nB2TestIp != other.m_nB2TestIp){ return(FALSE); }
	if(m_vtPeerIpRange != other.m_vtPeerIpRange){ return(FALSE); }

	return(TRUE);
}

BOOL CTS_PEER_CONFIG::operator!=(const CTS_PEER_CONFIG &other) const
{
	return(!(*this == other));
}

CTS_FILTER_CONFIG::CTS_FILTER_CONFIG()
{
	id = 0;
	name.clear();
	src_ip_rang.clear();
	dst_ip_rang.clear();
	protocol_id=0xFF;//255为保留，表示不判断协议号
	port.clear();
}

CTS_FILTER_CONFIG::~CTS_FILTER_CONFIG()
{
}

BOOL CTS_FILTER_CONFIG::operator=(const CTS_FILTER_CONFIG &other) 
{
	id = other.id;
	name = other.name;
	src_ip_rang = other.src_ip_rang;
	dst_ip_rang = other.dst_ip_rang;
	protocol_id = other.protocol_id;
	port = other.port;

	return(TRUE);
}

BOOL CTS_FILTER_CONFIG::operator==(const CTS_FILTER_CONFIG &other) const
{
	if(id != other.id){ return(FALSE); }
	//if(name != other.name){ return(FALSE); }
	if(src_ip_rang != other.src_ip_rang){ return(FALSE); }
	if(dst_ip_rang != other.dst_ip_rang){ return(FALSE); }
	if(protocol_id != other.protocol_id){ return(FALSE); }
	if(port != other.port){ return(FALSE); }

	return(TRUE);
}

BOOL CTS_FILTER_CONFIG::operator!=(const CTS_FILTER_CONFIG &other) const
{
	return(!(*this == other));
}

CTS_MGMT_CONFIG::CTS_MGMT_CONFIG()
{
	m_strCardName.clear();
	m_hlIpAddr=0;
	m_hlNetmask=0;
	m_hlGateway = 0;
}

CTS_MGMT_CONFIG::~CTS_MGMT_CONFIG()
{
}

BOOL CTS_MGMT_CONFIG::operator=(const CTS_MGMT_CONFIG &other) 
{
	m_strCardName = other.m_strCardName;
	m_hlIpAddr = other.m_hlIpAddr;
	m_hlNetmask = other.m_hlNetmask;
	m_hlGateway = other.m_hlGateway;

	return(TRUE);
}

BOOL CTS_MGMT_CONFIG::operator==(const CTS_MGMT_CONFIG &other) const
{
	if(m_strCardName != other.m_strCardName){ return(FALSE); }
	if(m_hlIpAddr != other.m_hlIpAddr){ return(FALSE); }
	if(m_hlNetmask != other.m_hlNetmask){ return(FALSE); }
	if(m_hlGateway != other.m_hlGateway){ return(FALSE); }

	return(TRUE);
}

BOOL CTS_MGMT_CONFIG::operator!=(const CTS_MGMT_CONFIG &other) const
{
	return(!(*this == other));
}

CTS_SGL_CONFIG::CTS_SGL_CONFIG()
{
	m_strCardName.clear();
	m_E1Max=0;
	m_vtE1List.clear();
	m_vtVcgList.clear();
}

CTS_SGL_CONFIG::~CTS_SGL_CONFIG()
{
	m_strCardName.clear();
	m_vtE1List.clear();
	m_vtVcgList.clear();
}

BOOL CTS_SGL_CONFIG::operator=(const CTS_SGL_CONFIG &other) 
{
	m_strCardName = other.m_strCardName;
	m_E1Max = other.m_E1Max;
	m_vtE1List = other.m_vtE1List;
	m_vtVcgList = other.m_vtVcgList;

	return(TRUE);
}

BOOL CTS_SGL_CONFIG::operator==(const CTS_SGL_CONFIG &other) const
{
	if(m_strCardName != other.m_strCardName){ return(FALSE); }
	if(m_E1Max != other.m_E1Max){ return(FALSE); }
	if(m_vtE1List != other.m_vtE1List){ return(FALSE); }
	if(m_vtVcgList != other.m_vtVcgList){ return(FALSE); }

	return(TRUE);
}

BOOL CTS_SGL_CONFIG::operator!=(const CTS_SGL_CONFIG &other) const
{
	return(!(*this == other));
}

CTS_CONFIG::CTS_CONFIG()
{
	m_pLogFile = NULL;
	m_vtPeerCfg.clear();
	m_vtLocalHostCfg.clear();
	m_vtFilterCfg.clear();
}

CTS_CONFIG::~CTS_CONFIG()
{
}

BOOL CTS_CONFIG::f_Init(ACE_Log_File_Msg* pLogFile)
{
	if (NULL == pLogFile)
	{
		return FALSE;
	}
	
	m_pLogFile = pLogFile;

	return TRUE;
}

// 本地管理
int CTS_CONFIG::f_GetLocalMgmtConfig(TiXmlNode *RootNode)
{
	TiXmlNode* pLocalMgmtNode = xml_doc.FirstChildElement(RootNode, "LOCAL_MANAGEMENT_CONFIG");
	if (NULL == pLocalMgmtNode)
	{ 
		return -1; 
	}

	// 网卡名字
	xml_doc.GetValueLikeIni(pLocalMgmtNode, "INTERFACE",  m_iLocalMgmtCfg.m_strCardName);  
	if (TRUE == m_iLocalMgmtCfg.m_strCardName.empty())
	{
		return -2; 
	}
	// IP地址
	xml_doc.GetValueLikeIni_HostOrderIP(pLocalMgmtNode, "IPADDR",  m_iLocalMgmtCfg.m_hlIpAddr);  
	if (0 == m_iLocalMgmtCfg.m_hlIpAddr)
	{ 
		return -3; 
	}
	// 掩码
	xml_doc.GetValueLikeIni_HostOrderIP(pLocalMgmtNode, "NETMASK",  m_iLocalMgmtCfg.m_hlNetmask);  
	if (0 == m_iLocalMgmtCfg.m_hlNetmask)
	{ 
		return -4; 
	}
	// 网关
	xml_doc.GetValueLikeIni_HostOrderIP(pLocalMgmtNode, "GATEWAY",  m_iLocalMgmtCfg.m_hlGateway);  
	if (0 == m_iLocalMgmtCfg.m_hlGateway)
	{ 
		//return -5; 
	}

	return 0;
}

int CTS_CONFIG::f_GetTsGlobalConfig(TiXmlNode *RootNode)
{
	TiXmlNode *pGlobalNode = xml_doc.FirstChildElement(RootNode, "GLOBAL_CONFIG");
	if (NULL == pGlobalNode)
	{ 
		return -1; 
	}

	ACE_UINT32 nValue;
	xml_doc.GetValueLikeIni(pGlobalNode, "NETWORK_MODE", nValue);
	m_iGlobalCfg.m_eNetworkMode = nValue;
	xml_doc.GetValueLikeIni(pGlobalNode, "FORWARD_MODE", nValue);
	m_iGlobalCfg.m_eForwardMode = nValue;
	xml_doc.GetValueLikeIni(pGlobalNode, "QUEUE_SIZE", m_iGlobalCfg.m_nQueueSize);

	return 0;
}

// 本局保护网段
int CTS_CONFIG::f_GetLocalHostConfig(TiXmlNode *RootNode)
{
	TiXmlNode* pLocalHostNode = xml_doc.FirstChildElement(RootNode, "LOCAL_HOST_CONFIG");
	if (NULL == pLocalHostNode)
	{ 
		return -1; 
	}

	xml_doc.GetValue_HostOrderIPRangeVector(pLocalHostNode, m_vtLocalHostCfg, "ip_range");
	if (0 == m_vtLocalHostCfg.size())
	{ 
		return -2; 
	}

	// 统一顺序
	size_t ii_max = m_vtLocalHostCfg.size();
	for(size_t ii=0; ii<ii_max; ii++)
	{
		// 网络字节序
		//m_vtLocalHostCfg[ii].first = TS_HTONL(m_vtLocalHostCfg[ii].first);
		//m_vtLocalHostCfg[ii].second = TS_HTONL(m_vtLocalHostCfg[ii].second);

		// 统一顺序
		if (m_vtLocalHostCfg[ii].first > m_vtLocalHostCfg[ii].second)
		{
			std::swap(m_vtLocalHostCfg[ii].first, m_vtLocalHostCfg[ii].second);
		}
	} // for(ii)

	return 0;
}

int CTS_CONFIG::f_GetTsMirrorConfig(TiXmlNode *RootNode)
{
	TiXmlNode* pMirrorNode = xml_doc.FirstChildElement(RootNode, "MIRROR_CONFIG");
	if (NULL == pMirrorNode)
	{ 
		return -1; 
	}

	xml_doc.GetValueLikeIni(pMirrorNode,  "INTERFACE", m_iMirrorCfg.m_strCardName);  // 网卡名字
	if (TRUE == m_iMirrorCfg.m_strCardName.empty())
	{ 
		return -2; 
	}

	return 0;
}

// 主网监测CK模块的配置
int CTS_CONFIG::f_GetMainNetConfig(TiXmlNode *RootNode)
{
	TiXmlNode* pMainNetNode = xml_doc.FirstChildElement(RootNode, "MAIN_NET_CONFIG");
	if (NULL == pMainNetNode)
	{ 
		return -1; 
	}

	xml_doc.GetValueLikeIni(pMainNetNode, "INTERFACE",  m_iMainNetCfg.m_strCardName);  // 网卡名字
	if (TRUE == m_iMainNetCfg.m_strCardName.empty())
	{ 
		return -2; 
	}

	xml_doc.GetValueLikeIni_HostOrderIP(pMainNetNode, "IPADDR",  m_iMainNetCfg.m_hlIpAddr);  // 使用ip
	if (0 == m_iMainNetCfg.m_hlIpAddr)
	{ 
		return -3; 
	}

	xml_doc.GetValueLikeIni_HostOrderIP(pMainNetNode, "NETMASK", m_iMainNetCfg.m_hlNetmask);
	if (0 == m_iMainNetCfg.m_hlNetmask)
	{ 
		return -4; 
	}

	xml_doc.GetValueLikeIni_HostOrderIP(pMainNetNode, "GATEWAY",  m_iMainNetCfg.m_hlGateway);  // 路由器IP
	if (0 == m_iMainNetCfg.m_hlGateway)
	{ 
		return -5; 
	}


	// 远程网络的探测频率
	xml_doc.GetValueLikeIni(pMainNetNode, "DETECT_SEND_PERIOD", m_iMainNetCfg.m_nDetectPeriod);
	xml_doc.GetValueLikeIni(pMainNetNode, "DETECT_SEND_QUEUE", m_iMainNetCfg.m_nDetectQueue);
	xml_doc.GetValueLikeIni(pMainNetNode, "SWITCH_LOST_THRESHOLD", m_iMainNetCfg.m_nSwitchLostThreshold);
	xml_doc.GetValueLikeIni(pMainNetNode, "RECOVER_LOST_THRESHOLD", m_iMainNetCfg.m_nRecoverLostThreshold);
	xml_doc.GetValueLikeIni(pMainNetNode, "RECOVER_STABLE_TIMER",  m_iMainNetCfg.m_nRecoverTimeout);  // 路由器稳定时间标准

	return 0;
}

// B1模块的配置
int CTS_CONFIG::f_GetBackNet1Config(TiXmlNode *RootNode)
{
	TiXmlNode* pBackNet1Node = xml_doc.FirstChildElement(RootNode, "BACK_NET1_CONFIG");
	if (NULL == pBackNet1Node)
	{ 
		return -1; 
	}

	xml_doc.GetValueLikeIni(pBackNet1Node, "INTERFACE",  m_iBackNet1Cfg.m_strCardName);  // 网卡名字
	if (TRUE == m_iBackNet1Cfg.m_strCardName.empty())
	{ 
		return -2; 
	}

	xml_doc.GetValueLikeIni_HostOrderIP(pBackNet1Node, "IPADDR",  m_iBackNet1Cfg.m_hlIpAddr);   // 使用ip
	if (0 == m_iBackNet1Cfg.m_hlIpAddr)
	{ 
		return -3; 
	}

	xml_doc.GetValueLikeIni_HostOrderIP(pBackNet1Node, "NETMASK",  m_iBackNet1Cfg.m_hlNetmask);
	if (0 == m_iBackNet1Cfg.m_hlNetmask)
	{ 
		return -4; 
	}

	xml_doc.GetValueLikeIni_HostOrderIP(pBackNet1Node, "GATEWAY",  m_iBackNet1Cfg.m_hlGateway);   // 网关IP
	if (0 == m_iBackNet1Cfg.m_hlGateway)
	{ 
		//return -5; //备网为局域网时可以不配网关
	}


	// 远程网络的探测频率
	xml_doc.GetValueLikeIni(pBackNet1Node, "DETECT_SEND_PERIOD", m_iBackNet1Cfg.m_nDetectPeriod);
	xml_doc.GetValueLikeIni(pBackNet1Node, "DETECT_SEND_QUEUE", m_iBackNet1Cfg.m_nDetectQueue);
	xml_doc.GetValueLikeIni(pBackNet1Node, "SWITCH_LOST_THRESHOLD", m_iBackNet1Cfg.m_nSwitchLostThreshold);
	xml_doc.GetValueLikeIni(pBackNet1Node, "RECOVER_LOST_THRESHOLD", m_iBackNet1Cfg.m_nRecoverLostThreshold);
	xml_doc.GetValueLikeIni(pBackNet1Node, "RECOVER_STABLE_TIMER", m_iBackNet1Cfg.m_nRecoverTimeout);

	//			
	TiXmlNode* pFilter = xml_doc.FirstChildElement(pBackNet1Node, "FILTER");
	xml_doc.GetValue_Uint32Vector(pFilter, m_iBackNet1Cfg.m_vtFilterId, "FILTER_ID");
	
	return 0;
}

// B2模块的配置
int CTS_CONFIG::f_GetBackNet2Config(TiXmlNode *RootNode)
{
	TiXmlNode* pBackNet2Node = xml_doc.FirstChildElement(RootNode, "BACK_NET2_CONFIG");
	if (NULL == pBackNet2Node)
	{ 
		return -1; 
	}

	xml_doc.GetValueLikeIni(pBackNet2Node, "INTERFACE",  m_iBackNet2Cfg.m_strCardName);  // 网卡名字
	if (TRUE == m_iBackNet2Cfg.m_strCardName.empty())
	{ 
		return -2; 
	}

	xml_doc.GetValueLikeIni_HostOrderIP(pBackNet2Node, "IPADDR",  m_iBackNet2Cfg.m_hlIpAddr);   // 使用ip
	if (0 == m_iBackNet2Cfg.m_hlIpAddr)
	{ 
		return -2; 
	}

	xml_doc.GetValueLikeIni_HostOrderIP(pBackNet2Node, "NETMASK",  m_iBackNet2Cfg.m_hlNetmask);
	if (0 == m_iBackNet2Cfg.m_hlNetmask)
	{ 
		return -2; 
	}

	xml_doc.GetValueLikeIni_HostOrderIP(pBackNet2Node, "GATEWAY",  m_iBackNet2Cfg.m_hlGateway);   // 网关IP
	if (0 == m_iBackNet2Cfg.m_hlGateway)
	{ 
		//return -5; //备网为局域网时可以不配网关
	}

	// 远程网络的探测频率
	xml_doc.GetValueLikeIni(pBackNet2Node, "DETECT_SEND_PERIOD", m_iBackNet2Cfg.m_nDetectPeriod);
	xml_doc.GetValueLikeIni(pBackNet2Node, "DETECT_SEND_QUEUE", m_iBackNet2Cfg.m_nDetectQueue);
	xml_doc.GetValueLikeIni(pBackNet2Node, "SWITCH_LOST_THRESHOLD", m_iBackNet2Cfg.m_nSwitchLostThreshold);
	xml_doc.GetValueLikeIni(pBackNet2Node, "RECOVER_LOST_THRESHOLD", m_iBackNet2Cfg.m_nRecoverLostThreshold);
	xml_doc.GetValueLikeIni(pBackNet2Node, "RECOVER_STABLE_TIMER", m_iBackNet2Cfg.m_nRecoverTimeout);

	//			
	TiXmlNode* pFilter = xml_doc.FirstChildElement(pBackNet2Node, "FILTER");
	xml_doc.GetValue_Uint32Vector(pFilter, m_iBackNet2Cfg.m_vtFilterId, "FILTER_ID");
	
	return 0;
}

int CTS_CONFIG::f_GetBackCom1Config(TiXmlNode *RootNode)
{
	TiXmlNode* pBackCom1Node = xml_doc.FirstChildElement(RootNode, "BACK_COM1_CONFIG");
	if (NULL == pBackCom1Node)
	{ 
		return -1; 
	}
	
	// 串口名字
	xml_doc.GetValueLikeIni(pBackCom1Node, "SERIAL_NAME",  m_iBackCom1Cfg.m_strCardName);
	if (TRUE == m_iBackCom1Cfg.m_strCardName.empty())
	{ 
		return -2; 
	}

	xml_doc.GetValueLikeIni(pBackCom1Node, "SPEED",  m_iBackCom1Cfg.m_nSpeed);
	xml_doc.GetValueLikeIni(pBackCom1Node, "CS",  m_iBackCom1Cfg.m_nCs);
	xml_doc.GetValueLikeIni(pBackCom1Node, "PARITYMODE",  m_iBackCom1Cfg.m_nParity);
	xml_doc.GetValueLikeIni(pBackCom1Node, "STOPBITS",  m_iBackCom1Cfg.m_nStop);

	//			
	TiXmlNode* pFilter = xml_doc.FirstChildElement(pBackCom1Node, "FILTER");
	xml_doc.GetValue_Uint32Vector(pFilter, m_iBackCom1Cfg.m_vtFilterId, "FILTER_ID");
	
	return 0;
}

int CTS_CONFIG::f_GetBackCom2Config(TiXmlNode *RootNode)
{
	TiXmlNode* pBackCom2Node = xml_doc.FirstChildElement(RootNode, "BACK_COM2_CONFIG");
	if (NULL == pBackCom2Node)
	{ 
		return -1; 
	}
	
	// 串口名字
	xml_doc.GetValueLikeIni(pBackCom2Node, "SERIAL_NAME",  m_iBackCom2Cfg.m_strCardName);
	if (TRUE == m_iBackCom2Cfg.m_strCardName.empty())
	{ 
		return -2; 
	}

	xml_doc.GetValueLikeIni(pBackCom2Node, "SPEED",  m_iBackCom2Cfg.m_nSpeed);
	xml_doc.GetValueLikeIni(pBackCom2Node, "CS",  m_iBackCom2Cfg.m_nCs);
	xml_doc.GetValueLikeIni(pBackCom2Node, "PARITYMODE",  m_iBackCom2Cfg.m_nParity);
	xml_doc.GetValueLikeIni(pBackCom2Node, "STOPBITS",  m_iBackCom2Cfg.m_nStop);

	//			
	TiXmlNode* pFilter = xml_doc.FirstChildElement(pBackCom2Node, "FILTER");
	xml_doc.GetValue_Uint32Vector(pFilter, m_iBackCom2Cfg.m_vtFilterId, "FILTER_ID");
	
	return 0;
}

int CTS_CONFIG::f_GetLocalConfig(char *xml_fname)
{
	xml_doc.close();
	
	// 打开文件
	int ret = xml_doc.open_for_readonly(xml_fname);
	if (0 > ret)
	{ 
		return -1; 
	}

	// XML文件的根节点
	TiXmlNode* pXmlRootNode = xml_doc.GetRootNode();

	// 本地管理
	ret = f_GetLocalMgmtConfig(pXmlRootNode);
	if (0 > ret)
	{
		return -2;
	}
	
	ret = f_GetTsGlobalConfig(pXmlRootNode);
	if (0 > ret)
	{
		return -3;
	}

	// 本局保护网段
	ret = f_GetLocalHostConfig(pXmlRootNode);
	if (0 > ret)
	{
		return -4;
	}

	// 镜像口M配置
	ret = f_GetTsMirrorConfig(pXmlRootNode);
	if (0 > ret)
	{
		return -5;
	}
	
	// 主网CK口的配置
	ret = f_GetMainNetConfig(pXmlRootNode);
	if (0 > ret)
	{
		return -6;
	}

	// B1口的配置
	ret = f_GetBackNet1Config(pXmlRootNode);
	if (0 > ret)
	{
		return -7;
	}

	// B2口的配置
	ret = f_GetBackNet2Config(pXmlRootNode);
	if (0 > ret)
	{
		return -8;
	}

	//
	ret = f_GetBackCom1Config(pXmlRootNode);
	if (0 > ret)
	{
		return -9;
	}

	//
	ret = f_GetBackCom2Config(pXmlRootNode);
	if (0 > ret)
	{
		return -10;
	}
	
	return 0;
}

// 目标网络的配置
int CTS_CONFIG::f_GetPeerConfig(char *xml_fname)
{
	xml_doc.close();

	// 打开文件
	int ret = xml_doc.open_for_readonly(xml_fname);
	if (0 > ret)
	{ 
		return -1; 
	}

	// XML文件的根节点
	TiXmlNode *pXmlRootNode = xml_doc.GetRootNode();
	
	// 逐个读取每个节点
	TiXmlNode * pPeerNode = xml_doc.FirstChildElement(pXmlRootNode, "PEER_NETWORK");
	if (NULL != pPeerNode)
	{
		CTS_PEER_CONFIG iPeerInstance;

		//对局ID		
		xml_doc.GetValueLikeIni(pPeerNode, "PEER_ID", iPeerInstance.m_nPeerId);

		if (0 != iPeerInstance.m_nPeerId)//对局ID为0表示不启用
		{
			TiXmlNode* pPeerIp = xml_doc.FirstChildElement(pPeerNode, "PEER_IP");
			xml_doc.GetValue_HostOrderIPRangeVector(pPeerIp, iPeerInstance.m_vtPeerIpRange, "IP_RANGE");
			
			// 主网的测试IP
			TiXmlNode* pMainCfg = xml_doc.FirstChildElement(pPeerNode, "MAIN_CONFIG");
			xml_doc.GetValueLikeIni(pMainCfg, "MAIN_INTERFACE",  iPeerInstance.m_strMainInt);
			xml_doc.GetValueLikeIni_HostOrderIP(pMainCfg, "MAIN_TEST_IP",  iPeerInstance.m_nMainTestIp);
			
			// B1的配置
			TiXmlNode* pB1Cfg = xml_doc.FirstChildElement(pPeerNode, "B1_CONFIG");
			xml_doc.GetValueLikeIni(pB1Cfg, "B1_INTERFACE",  iPeerInstance.m_strB1Int);
			xml_doc.GetValueLikeIni(pB1Cfg, "B1_VCG_ID",  iPeerInstance.m_nB1VcgId);
			xml_doc.GetValueLikeIni_HostOrderIP(pB1Cfg, "B1_TEST_IP",  iPeerInstance.m_nB1TestIp);
			
			// B2的配置
			TiXmlNode* pB2Cfg = xml_doc.FirstChildElement(pPeerNode, "B2_CONFIG");
			xml_doc.GetValueLikeIni(pB2Cfg, "B2_INTERFACE",  iPeerInstance.m_strB2Int);
			xml_doc.GetValueLikeIni(pB2Cfg, "B2_VCG_ID",  iPeerInstance.m_nB2VcgId);
			xml_doc.GetValueLikeIni_HostOrderIP(pB2Cfg, "B2_TEST_IP",  iPeerInstance.m_nB2TestIp);

			// 加入vector
			m_vtPeerCfg.push_back(iPeerInstance);
		}
	}

	if (0 == m_vtPeerCfg.size())
	{ 
		return -2; 
	}

	return 0;
}

// 本地业务过滤器
int CTS_CONFIG::f_GetFilterConfig(char *xml_fname)
{
	xml_doc.close();

	// 打开文件
	int ret = xml_doc.open_for_readonly(xml_fname);
	if (0 > ret)
	{ 
		return -1; 
	}

	// XML文件的根节点
	TiXmlNode* pXmlRootNode = xml_doc.GetRootNode();

	// 逐个读取每个节点
	TiXmlNode* pFilterNode = xml_doc.FirstChildElement(pXmlRootNode, "FILTER");   // 当前"目标网络配置"的根节点
	while(NULL != pFilterNode)
	{
		CTS_FILTER_CONFIG cur_Filter_config;

		 // 内部使用的ID，全局唯一
		if (FALSE == xml_doc.GetValueLikeIni(pFilterNode, "ID",  cur_Filter_config.id)) 
		{
			pFilterNode = xml_doc.NextSiblingElement(pFilterNode, "FILTER");
			continue; 
		}

		// 业务名称，不允许为空
		/*if (FALSE == xml_doc.GetValueLikeIni(Filter_node, "NAME",  cur_Filter_config.name))
		{
			// 继续下一个
			Filter_node = xml_doc.NextSiblingElement(Filter_node, "FILTER");
			continue; 
		}
		if(TRUE == cur_Filter_config.name.empty())
		{
			// 继续下一个
			Filter_node = xml_doc.NextSiblingElement(Filter_node, "FILTER");
			continue; 
		}*/
		
		TiXmlNode* pSrcIpNode = xml_doc.FirstChildElement(pFilterNode, "SOURCE_IP");
		if (NULL != pSrcIpNode)
		{
			xml_doc.GetValue_HostOrderIPRangeVector(pSrcIpNode, cur_Filter_config.src_ip_rang, "ADDRESS");
		}

		TiXmlNode* pDstIpNode = xml_doc.FirstChildElement(pFilterNode, "DESTINATION_IP");
		if (NULL != pDstIpNode)
		{
			xml_doc.GetValue_HostOrderIPRangeVector(pDstIpNode, cur_Filter_config.dst_ip_rang, "ADDRESS");
		}

		// IP层协议号
		xml_doc.GetValueLikeIni(pFilterNode, "PROTOCOL_ID",  cur_Filter_config.protocol_id);
		
		TiXmlNode* pPortNode = xml_doc.FirstChildElement(pFilterNode, "PORT");
		if (NULL != pPortNode)
		{
			xml_doc.GetValue_Uint16RangeVector(pPortNode, cur_Filter_config.port, "NUM");
		}

		// 加入vector
		m_vtFilterCfg.push_back(cur_Filter_config);

		// 继续下一个
		pFilterNode = xml_doc.NextSiblingElement(pFilterNode, "FILTER");
	} // while(Filter_node)

	return 0;
}

int CTS_CONFIG::f_GetVcgConfig(char *xml_fname)
{
	xml_doc.close();

	// 打开文件
	int ret = xml_doc.open_for_readonly(xml_fname);
	if (0 > ret)
	{ 
		return -1; 
	}

	// XML文件的根节点
	TiXmlNode *pXmlRootNode = xml_doc.GetRootNode();

	TiXmlNode *pVcgCardRootNode = xml_doc.FirstChildElement(pXmlRootNode, "VCG_CARD");
	if (NULL == pVcgCardRootNode)
	{ 
		return -2; 
	}

	// 网卡名字
	xml_doc.GetValueLikeIni(pVcgCardRootNode, "PORT",  m_iSglCard1.m_strCardName);  
	if (TRUE == m_iSglCard1.m_strCardName.empty())
	{
		return -3; 
	}

	xml_doc.GetValueLikeIni(pVcgCardRootNode, "E1_MAX", m_iSglCard1.m_E1Max);
	if (8 != m_iSglCard1.m_E1Max)
	{
		return -4;
	}

	/*TiXmlNode* pE1ListNode = xml_doc.FirstChildElement(pVcgCardRootNode, "E1_LIST");
	
	TiXmlNode* pE1Node = xml_doc.FirstChildElement(pE1ListNode, "E1");
	while (NULL != pE1Node)
	{
		CE1 iE1Instance;

		xml_doc.GetValueLikeIni(pE1Node, "ID", iE1Instance.m_ID);
		xml_doc.GetValueLikeIni(pE1Node, "NAME", iE1Instance.m_Name);

		m_iSglCard1.m_vtE1List.push_back(iE1Instance);

		pE1Node = xml_doc.NextSiblingElement(pE1Node, "E1");
	}

	TiXmlNode* pVcgListNode = xml_doc.FirstChildElement(pVcgCardRootNode, "VCG_LIST");

	TiXmlNode* pVcgNode = xml_doc.FirstChildElement(pVcgListNode, "VCG");
	while (NULL != pVcgNode)
	{
		CVcg iVcgInstance;

		xml_doc.GetValueLikeIni(pVcgNode, "ID", iVcgInstance.m_ID);
		//xml_doc.GetValueLikeIni(pVcgNode, "NAME", iVcgInstance.m_Name);
		xml_doc.GetAttr(pVcgNode, "name", iVcgInstance.m_Name);
		xml_doc.GetValueLikeIni(pVcgNode, "VLAN", iVcgInstance.m_Vlan);

		TiXmlNode* pMember = xml_doc.FirstChildElement(pVcgNode, "MEMBERS");
		xml_doc.GetValue_Uint32Vector(pMember, iVcgInstance.m_vtE1Member, "E1_ID");

		m_iSglCard1.m_vtVcgList.push_back(iVcgInstance);

		pVcgNode = xml_doc.NextSiblingElement(pVcgNode, "VCG");
	}*/

	pVcgCardRootNode = xml_doc.NextSiblingElement(pVcgCardRootNode, "VCG_CARD");
	if (NULL == pVcgCardRootNode)
	{ 
		return -12; 
	}

	// 网卡名字
	xml_doc.GetValueLikeIni(pVcgCardRootNode, "PORT",  m_iSglCard2.m_strCardName);  
	if (TRUE == m_iSglCard2.m_strCardName.empty())
	{
		return -13; 
	}

	xml_doc.GetValueLikeIni(pVcgCardRootNode, "E1_MAX", m_iSglCard2.m_E1Max);
	if (4 != m_iSglCard2.m_E1Max)
	{
		return -14;
	}	
		
	return 0;
}

int CTS_CONFIG::f_ReadConfigFile(void)
{
	int ret;

	//
	ret = f_GetLocalConfig((char *)(ConfigCenter_Obj()->GetProbeFullConfigName("local_config.xml").c_str()));
	if (0 > ret)
	{ 
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]f_GetLocalConfig failed. return:%d\n"), ret));
		return -1; 
	}

	ret = f_GetPeerConfig((char *)(ConfigCenter_Obj()->GetProbeFullConfigName("peer_config.xml").c_str()));
	if (0 > ret)
	{ 
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]f_GetPeerConfig failed. return:%d\n"), ret));
		return -2; 
	}

	// 本地业务过滤器
	ret = f_GetFilterConfig((char *)(ConfigCenter_Obj()->GetProbeFullConfigName("filter_config.xml").c_str()));
	if (0 > ret)
	{ 
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]f_GetFilterConfig failed. return:%d\n"), ret));
		return -3; 
	}

	//
	ret = f_GetVcgConfig((char *)(ConfigCenter_Obj()->GetProbeFullConfigName("vcg_config.xml").c_str()));
	if (0 > ret)
	{ 
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]f_GetVcgConfig failed. return:%d\n"), ret));
		return -4; 
	}
	
	return 0;
}

BOOL CTS_CONFIG::f_IsPeerHostIp(ACE_UINT32 hlHostIp, ACE_UINT32 &peer_index)
{
	//特殊地址要在前面判断，因为主机地址范围有可能配成全网段
	//if ((0xE0000000 <= hlHostIp) && (0xEFFFFFFF >= hlHostIp))
	if ((0xE0000000 <= hlHostIp)
		|| (0x0 == hlHostIp))
	{
		peer_index = 0;
		return TRUE;
	}
	
	size_t ii_max = m_vtPeerCfg.size();
	for (size_t ii = 0; ii < ii_max; ii++)
	{
		if (hlHostIp == m_vtPeerCfg[ii].m_nMainTestIp)
		{
			return FALSE;
		}
		
		// IP 段 
		size_t kk_max = m_vtPeerCfg[ii].m_vtPeerIpRange.size();
		for(size_t kk=0; kk<kk_max; kk++)
		{
			if
			(
				(hlHostIp >= m_vtPeerCfg[ii].m_vtPeerIpRange[kk].first)
			 &&  (hlHostIp <= m_vtPeerCfg[ii].m_vtPeerIpRange[kk].second)
			)
			{
				peer_index = (ACE_UINT32)ii + 1;
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CTS_CONFIG::f_IsLocalHostIp(ACE_UINT32 hlHostIp)
{
	if (hlHostIp == m_iMainNetCfg.m_hlIpAddr)
	{
		return FALSE;
	}

	//if ((0xE0000000 <= hlHostIp) && (0xEFFFFFFF >= hlHostIp))
	if ((0xE0000000 <= hlHostIp)
		|| (0x0 == hlHostIp))
	{
		return TRUE;
	}
	
	size_t max = m_vtLocalHostCfg.size();
	for (size_t i=0; i<max; i++)
	{
		if ((hlHostIp >= m_vtLocalHostCfg[i].first) && (hlHostIp <= m_vtLocalHostCfg[i].second))
		{
			return TRUE;
		}
	}

	return FALSE;
}

CTS_PEER_CONFIG* CTS_CONFIG::f_FindPeerCfg(ACE_UINT32 id)
{
	size_t max = m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		if (id == m_vtPeerCfg[i].m_nPeerId)
		{
			return ((CTS_PEER_CONFIG*)(&(m_vtPeerCfg[i])));
		}
	}

	return NULL;
}

// 通过ID值查找过滤器
CTS_FILTER_CONFIG* CTS_CONFIG::f_FindFilter(ACE_UINT32 id)
{
	size_t max = m_vtFilterCfg.size();
	for (size_t i=0; i<max; i++)
	{
		if (id == m_vtFilterCfg[i].id)
		{
			return ((CTS_FILTER_CONFIG*)(&(m_vtFilterCfg[i])));
		}
	}

	return NULL;
}

CVcg* CTS_CONFIG::f_FindVcg(ACE_UINT32 id)
{
	size_t max = m_iSglCard1.m_vtVcgList.size();
	for (size_t i=0; i<max; i++)
	{
		if (id == m_iSglCard1.m_vtVcgList[i].m_ID)
		{
			return ((CVcg*)(&(m_iSglCard1.m_vtVcgList[i])));
		}
	}

	max = m_iSglCard2.m_vtVcgList.size();
	for (size_t i=0; i<max; i++)
	{
		if (id == m_iSglCard2.m_vtVcgList[i].m_ID)
		{
			return ((CVcg*)(&(m_iSglCard2.m_vtVcgList[i])));
		}
	}

	return NULL;
}

BOOL CTS_CONFIG::f_FindAllCardName(std::string& all_card_name)
{
	//去掉重复的网卡，每个网卡以|分隔
	std::vector< std::string > card_list;

	// M
	card_list.push_back( m_iMirrorCfg.m_strCardName );

	// CK
	card_list.push_back( m_iMainNetCfg.m_strCardName );

	// B1
	card_list.push_back( m_iBackNet1Cfg.m_strCardName );

	// B2
	card_list.push_back( m_iBackNet2Cfg.m_strCardName );
	
	// 本地管理
	card_list.push_back(m_iLocalMgmtCfg.m_strCardName);

	std::sort(card_list.begin(), card_list.end()); // 先排序
	std::vector< std::string >::iterator iter = std::unique(card_list.begin(), card_list.end());
	if(iter != card_list.end()){ card_list.erase(iter, card_list.end()); }  // 删除重复的

	if(card_list.size() == 0){ return(FALSE); }
	////////

	// 形成指定的格式串
	all_card_name.clear();
	all_card_name += card_list[0];
	for(size_t ii=1; ii<card_list.size(); ii++)
	{
		all_card_name += "|";
		all_card_name += card_list[ii];
	}
	///////////

	return(TRUE);
}

BOOL CTS_CONFIG::f_SetLocalMgmt(void)
{
	char str[100];

	//set port
	memset(str, 0, sizeof(str));
	ACE_OS::sprintf(str, "ifconfig %s ", m_iLocalMgmtCfg.m_strCardName.c_str());
	
	//set ip
	ACE_UINT32 netorder_ip = TS_HTONL(m_iLocalMgmtCfg.m_hlIpAddr);
	ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_ip));
	
	//set netmask
	ACE_OS::strcat(str, ACE_TEXT(" netmask "));
	ACE_UINT32 netorder_netmask = TS_HTONL(m_iLocalMgmtCfg.m_hlNetmask);
	ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_netmask));
	ACE_OS::system(str);

//本地管理口暂不支持网关
#if 0
	//set gateway
	//memset(str, 0, sizeof(str));
	//ACE_OS::sprintf(str, "route del default");
	ACE_OS::system("route del default");
	if (0 != m_hlGateway)
	{
		memset(str, 0, sizeof(str));
		ACE_OS::sprintf(str, "route add default gw ");
		ACE_UINT32 netorder_gateway = TS_HTONL(m_hlGateway);
		ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_gateway));
		ACE_OS::system(str);
	}
#endif


	return TRUE;
}

BOOL CTS_CONFIG::f_SetMainNetMgmt(void)
{
	ACE_UINT32 netorder_ip = 0;
	ACE_UINT32 netorder_netmask = 0;
	char str[100];

	//探测口带内管理
	//set port
	memset(str, 0, sizeof(str));
	ACE_OS::sprintf(str, "ifconfig %s ", m_iMainNetCfg.m_strCardName.c_str());
	//set ip
	netorder_ip = TS_HTONL(m_iMainNetCfg.m_hlIpAddr);
	ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_ip));
	//set netmask
	ACE_OS::strcat(str, ACE_TEXT(" netmask "));
	netorder_netmask = TS_HTONL(m_iMainNetCfg.m_hlNetmask);
	ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_netmask));
	ACE_OS::system(str);

	//set gateway
	ACE_OS::system("route del default");
	if (0 != m_iMainNetCfg.m_hlGateway)
	{
		memset(str, 0, sizeof(str));
		ACE_OS::sprintf(str, "route add default gw ");
		ACE_UINT32 netorder_gateway = TS_HTONL(m_iMainNetCfg.m_hlGateway);
		ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_gateway));
		ACE_OS::system(str);
	}

	return TRUE;
}

BOOL CTS_CONFIG::f_SetBackNet1Mgmt(void)
{
	ACE_UINT32 netorder_ip = 0;
	ACE_UINT32 netorder_netmask = 0;
	char str[100];

	//set port
	memset(str, 0, sizeof(str));
	ACE_OS::sprintf(str, "ifconfig %s ", m_iBackNet1Cfg.m_strCardName.c_str());
	//set ip
	netorder_ip = TS_HTONL(m_iBackNet1Cfg.m_hlIpAddr);
	ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_ip));
	//set netmask
	ACE_OS::strcat(str, ACE_TEXT(" netmask "));
	netorder_netmask = TS_HTONL(m_iBackNet1Cfg.m_hlNetmask);
	ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_netmask));
	ACE_OS::system(str);

	return TRUE;
}

BOOL CTS_CONFIG::f_SetBackNet2Mgmt(void)
{
	ACE_UINT32 netorder_ip = 0;
	ACE_UINT32 netorder_netmask = 0;
	char str[100];

	//set port
	memset(str, 0, sizeof(str));
	ACE_OS::sprintf(str, "ifconfig %s ", m_iBackNet2Cfg.m_strCardName.c_str());
	//set ip
	netorder_ip = TS_HTONL(m_iBackNet2Cfg.m_hlIpAddr);
	ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_ip));
	//set netmask
	ACE_OS::strcat(str, ACE_TEXT(" netmask "));
	netorder_netmask = TS_HTONL(m_iBackNet2Cfg.m_hlNetmask);
	ACE_OS::strcat(str, ACE_OS::inet_ntoa(*(in_addr *)&netorder_netmask));
	ACE_OS::system(str);

	return TRUE;
}

