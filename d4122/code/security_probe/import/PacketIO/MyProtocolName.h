//////////////////////////////////////////////////////////////////////////
//MyProtocolName.h

#ifndef MY_PROTOCOL_NAME_H
#define MY_PROTOCOL_NAME_H
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




//常用协议的协议名称
#define PROTONAME_RESERVE_FOR_OBJECTCFT		"GENERAL_PROTOCOL"

#define PROTONAME_MAC		                "MAC"
#define PROTONAME_TCP				        "TCP"
#define PROTONAME_UDP				        "UDP"
#define PROTONAME_PPP					    "PPP"
#define PROTONAME_X25						"X25_LAPB"
#define PROTONAME_X25_LAPB					"X25_LAPB"
#define PROTONAME_X25_PACKET				"X25_Packet"
#define PROTONAME_FR				        "FR"
#define PROTONAME_ATM				        "ATM"
#define PROTONAME_ARP				        "ARP"
#define PROTONAME_XNS					    "XNS"
#define PROTONAME_CISCO_HDLC			    "CISCO_HDLC"

#define PROTONAME_XNS1						"XNS1"
#define PROTONAME_APPLETALK_ARP				"APPLETALK_ARP"
#define PROTONAME_BANYAN_VINES2				"BANYAN_VINES2"
#define PROTONAME_BANYAN_ECHO1				"BANYAN_ECHO1"
#define PROTONAME_BANYAN_ECHO2				"BANYAN_ECHO2"
#define PROTONAME_DNA						"DNA"
#define PROTONAME_RARP						"RARP"
#define PROTONAME_APPLETALK					"APPLETALK"
#define PROTONAME_SNA						"SNA"


#define PROTONAME_IPX1						"IPX1"
#define PROTONAME_LLC						"LLC"
#define PROTONAME_IPv4						"IPv4"
#define PROTONAME_IPX						"IPX"
#define PROTONAME_SPX						"SPX"


#define PROTONAME_ICMP						"ICMP"
#define PROTONAME_IGMP						"IGMP"
#define PROTONAME_GGP						"GGP"
#define PROTONAME_STREAM					"STREAM"
#define PROTONAME_IGP						"IGP"
#define PROTONAME_NVPII						"NVPII"
#define PROTONAME_PUP						"PUP"
#define PROTONAME_CHAOS						"CHAOS"
#define PROTONAME_SEP						"SEP"
#define PROTONAME_DDP						"DDP"
#define PROTONAME_RSVP						"RSVP"
#define PROTONAME_IGRP						"IGRP"
#define PROTONAME_OSPF						"OSPF"

#define PROTONAME_CISCO_ISL					"CISCO_ISL"
#define PROTONAME_8021Q						"802.1Q"

#define PROTONAME_HTTP						"HTTP"
#define PROTONAME_FTPDATA					"FTPDATA"
#define PROTONAME_FTPCTRL					"FTPCTRL"
#define PROTONAME_KLOGIN					"KLOGIN"
#define PROTONAME_WEBSTER					"WEBSTER"
#define PROTONAME_NETBIOS_SSN				"NETBIOS_SSN"
#define PROTONAME_POP2						"POP2"
#define PROTONAME_POP3						"POP3"
#define PROTONAME_SUNRPC					"SUNRPC"
#define PROTONAME_TELNET					"TELNET"


#define PROTONAME_GOPHER					"GOPHER"
#define PROTONAME_LOGIN						"LOGIN"
#define PROTONAME_DNS						"DNS"
#define PROTONAME_FINGER					"FINGER"


#define PROTONAME_ECHO						"ECHO"
#define PROTONAME_OSQLNET					"OSQLNET"
#define PROTONAME_BOOTPSERVER				"BOOTPSERVER"
#define PROTONAME_BOOTPCLIENT				"BOOTPCLIENT"
#define PROTONAME_TFTP						"TFTP"
#define PROTONAME_NTP						"NTP"
#define PROTONAME_NETBIOS_NS				"NETBIOS_NS"
#define PROTONAME_NETBIOS_DGM				"NETBIOS_DGM"
#define PROTONAME_SQLNET					"SQLNET"
#define PROTONAME_SQLSRV					"SQLSRV"
#define PROTONAME_SNMP161					"SNMP161"
#define PROTONAME_SNMP162					"SNMP162"
#define PROTONAME_BGP						"BGP"
#define PROTONAME_LDAP						"LDAP"
#define PROTONAME_RIP						"RIP"
#define PROTONAME_WHO						"WHO"
#define PROTONAME_H225RAS					"H225RAS"
#define PROTONAME_H225DISGK					"H225DISGK"
#define PROTONAME_RTP						"RTP"
#define PROTONAME_RTCP						"RTCP"
#define PROTONAME_SMTP						"SMTP"
#define PROTONAME_NETBIOS					"NETBIOS"

#define PROTONAME_LAN						"LAN"
#define PROTONAME_USER						"USER"

//定义协议解释库所支持的其他协议名
#define PROTONAME_NONE						"NONE"
#define PROTONAME_SPECIAL					"SPECIAL"
#define PROTONAME_SNAP						"SNAP"
#define PROTONAME_EGP						"EGP"
#define PROTONAME_MINIMAL_IP				"MINIMAL_IP"
#define PROTONAME_GRE						"GRE"
#define PROTONAME_MIP						"MIP"
#define PROTONAME_A11						"A11"

#define PROTONAME_IPX_SAP					"IPX_SAP"
#define PROTONAME_IPX_SPX					"IPX_SPX"
#define PROTONAME_RIPX						"RIPX"
#define PROTONAME_IPX_NCP					"IPX_NCP"
#define PROTONAME_IPX_BMP					"IPX_BMP"
#define PROTONAME_IPX_MSG					"IPX_MSG"
#define PROTONAME_IPX_ECHO					"IPX_ECHO"
#define PROTONAME_IPX_ERROR					"IPX_ERROR"
#define PROTONAME_IPX_SER					"IPX_SER"
#define PROTONAME_IPX_DIAG					"IPX_DIAG"
#define PROTONAME_BPDU						"BPDU"
#define PROTONAME_APPLETALK_LAP				"APPLETALK_LAP"
#define PROTONAME_APPLETALK_DDP				"APPLETALK_DDP"
#define PROTONAME_APPLETALK_NBP				"APPLETALK_NBP"
#define PROTONAME_APPLETALK_ZIP				"APPLETALK_ZIP"
#define PROTONAME_APPLETALK_RTMP			"APPLETALK_RTMP"
#define PROTONAME_APPLETALK_AEP		        "APPLETALK_AEP"
#define PROTONAME_APPLETALK_ATP		        "APPLETALK_ATP"
#define PROTONAME_APPLETALK_ADSP			"APPLETALK_ADSP"
#define PROTONAME_BANYAN_ECHO				"BANYAN_ECHO"
#define PROTONAME_BANYAN_VIP				"BANYAN_VIP"
#define PROTONAME_BANYAN_ARP				"BANYAN_ARP"
#define PROTONAME_BANYAN_ICP				"BANYAN_ICP"
#define PROTONAME_BANYAN_SPP				"BANYAN_SPP"
#define PROTONAME_BANYAN_IPC				"BANYAN_IPC"


#define PROTONAME_BANYAN_RTP				"BANYAN_RTP"
#define PROTONAME_NETBEUI					"NETBEUI"
#define PROTONAME_IBMSMB					"IBMSMB"
#define PROTONAME_Q922						"Q922"
#define PROTONAME_Q933						"Q933"
#define PROTONAME_RFC1490					"RFC1490"
#define PROTONAME_ETHER_TYPE				"ETHER_TYPE"
#define PROTONAME_DHCP						"DHCP"
#define PROTONAME_RADIUS					"RADIUS"
#define PROTONAME_PPP_LCP					"PPP_LCP"
#define PROTONAME_PPP_CHAP					"PPP_CHAP"
#define PROTONAME_PPP_CCP					"PPP_CCP"
#define PROTONAME_PPP_IPCP					"PPP_IPCP"
#define PROTONAME_PPP_NBFCP					"PPP_NBFCP"
#define PROTONAME_PPP_IPXCP					"PPP_IPXCP"
#define PROTONAME_PPP_BACP					"PPP_BACP"
#define PROTONAME_PPP_BAP					"PPP_BAP"
#define PROTONAME_PPP_BCP					"PPP_BCP"
#define PROTONAME_PPP_1STCC					"PPP_1STCC"
#define PROTONAME_PPP_ATCP					"PPP_ATCP"
#define PROTONAME_PPP_BVCP					"PPP_BVCP"
#define PROTONAME_PPP_ECP					"PPP_ECP"
#define PROTONAME_PPP_PAP					"PPP_PAP"
#define PROTONAME_PPP_LQR					"PPP_LQR"
#define PROTONAME_PPP_DNCP					"PPP_DNCP"
#define PROTONAME_PPP_IPv6CP				"PPP_IPv6CP"
#define PROTONAME_PPP_SDCP					"PPP_SDCP"
#define PROTONAME_PPP_SNACP					"PPP_SNACP"
#define PROTONAME_PPP_OSINLCP				"PPP_OSINLCP"
#define PROTONAME_PPP_MP					"PPP_MP"
#define PROTONAME_SMPP						"SMPP"
#define PROTONAME_CMPP						"CMPP"
#define PROTONAME_NO7_MTP					"NO7_MTP"
#define PROTONAME_NO7_MSU					"NO7_MSU"
#define PROTONAME_NO7_LSSU					"NO7_LSSU"
#define PROTONAME_NO7_ISUP					"ISUP"
#define PROTONAME_NO7_FISU					"NO7_FISU"

#define PROTONAME_PPPSTREAM					"PPP_STREAM"
#define PROTONAME_RFC1144					"RFC1144"
#define PROTONAME_IPV4_STREAM				"IPV4_STREAM"

#define PROTONAME_INVALIDATE_PROTO			"INVALID_PROTOCOL"

#define PROTONAME_CDMA_COURSE				"CDMA_COURSE"

#define PROTONAME_AAL5						"AAL5" //AAL5
#define PROTONAME_AAL2						"AAL2" //AAL2

#define PROTONAME_80211A					"802.11a"
#define PROTONAME_80211B					"802.11b"
#define PROTONAME_80211G					"802.11g"

#define PROTONAME_80211A_PHY				"802.11a_PHY"
#define PROTONAME_80211B_PHY				"802.11b_PHY"
#define PROTONAME_80211G_PHY				"802.11g_PHY"

#define PROTONAME_MGCP                      "MGCP"
#define PROTONAME_SIP                       "SIP"
#define PROTONAME_H248                      "H248"
#define PROTONAME_SIGTRAN                   "SIGTRAN"
#define PROTONAME_BICC                      "BICC"
#define PROTONAME_MEGACO                    PROTONAME_H248
#define PROTONAME_SDP						"SDP"

#define PROTONAME_EMPP						"EMPP"
#define PROTONAME_SMIAS						"SMIAS"
#define PROTONAME_SGIP						"SGIP"
#define PROTONAME_MMSE						"MMSE"
#define PROTONAME_PUSH						"PUSH"
#define PROTONAME_VTP						"VTP"
#define PROTONAME_WTPS						"WTPS"
#define PROTONAME_NO7_TUP					"TUP"
#define PROTONAME_TDS						"TDS"
#define PROTONAME_ASN_TEST		        "ASN_TEST"//专门用来测试与ASN.1方式的兼容性

#define PROTONAME_GPRS_GTP                  "GPRS_GTP"
#define PROTONAME_PPPOE_DISCOVERY           "PPPOE_DISCOVERY"
#define PROTONAME_PPPOE_SESSION             "PPPOE_SESSION"

#define PROTONAME_WLAN_NET					"WLAN_NET"
#define PROTONAME_EAP						"EAP"
#define PROTONAME_EAPOL						"EAPOL"
#define PROTONAME_EAPORADIUS	            "EAPORADIUS"
#define PROTONAME_PORTAL					"PORTAL"
#define PROTONAME_EXTEND                    "EXTEND"
#define PROTONAME_PPP_VJ_CMP                "PPP_VJ_CMP"

#define PROTONAME_PPP_VJ_UNCMP              "PPP_VJ_UNCMP"
#define PROTONAME_MPLS_LDP                  "MPLS_LDP"
#define PROTONAME_WTP                       "WTP"
#define PROTONAME_WSP                       "WSP"
#define PROTONAME_MPLS                      "MPLS"

#define PROTONAME_SIGTRAN_IUA               "SIGTRAN_IUA"
#define PROTONAME_SIGTRAN_M2UA              "SIGTRAN_M2UA"
#define PROTONAME_SIGTRAN_M3UA              "SIGTRAN_M3UA"
#define PROTONAME_SIGTRAN_SCTP              "SIGTRAN_SCTP"
#define PROTONAME_RTSP                      "RTSP"
#define PROTONAME_IPV6                      "IPV6"
#define PROTONAME_SMGP                      "SMGP"
#define PROTONAME_GPRS_GTP_V1               "GPRS_GTP_V1"
        
#define PROTONAME_PPPOE_DISCOVERY			"PPPOE_DISCOVERY"
#define PROTONAME_PPPOE_SESSION				"PPPOE_SESSION"

#define PROTONAME_WLAN_NET					"WLAN_NET"
#define PROTONAME_EAP						"EAP"
#define PROTONAME_EAPOL						"EAPOL"
#define PROTONAME_EAPORADIUS				"EAPORADIUS"
#define PROTONAME_PORTAL					"PORTAL"

// For 3G
#define PROTONAME_GTP						"GTP"
#define PROTONAME_MTP3B						"MTP3b"
#define PROTONAME_SCCP						"SCCP"
#define PROTONAME_SSCOP						"SSCOP"
#define PROTONAME_RANAP						"RANAP"
#define PROTONAME_NBAP						"NBAP"
#define PROTONAME_GMM						"GMM"
#define PROTONAME_RRC						"RRC"
#define PROTONAME_ALCAP						"ALCAP"
#define PROTONAME_IUUP						"IuUP"
#define PROTONAME_CC						"CC"
#define PROTONAME_RR						"RR"
#define PROTONAME_MM						"MM"
#define PROTONAME_SM						"SM"
#define PROTONAME_SMSCP						"SMSCP"
#define PROTONAME_SMSRP						"SMSRP"
#define PROTONAME_SMSTP						"SMSTP"
#define PROTONAME_MAC_3G					"MAC_3G"
#define PROTONAME_FP						"FP"
#define PROTONAME_SS						"SS"
#define PROTONAME_SCMG						"SCMG"
#define PROTONAME_SNT						"SNT"
#define PROTONAME_SNM						"SNM"
#define PROTONAME_RLP						"RLP"
#define PROTONAME_TS24008					"3GPP TS 24.008"
#define PROTONAME_RLC						"RLC"
#define PROTONAME_RNSAP						"RNSAP"
#define PROTONAME_RLC_FIX					"RLCFix"
#define PROTONAME_IUB_INTERFACE				"Iub"
#define PROTONAME_IUR_INTERFACE				"Iur"
#define PROTONAME_IUCS_INTERFACE			"IuCS"
#define PROTONAME_IUPS_INTERFACE			"IuPS"	
#define PROTONAME_H223                      "H223"     // cheng.j.w
#define PROTONAME_H245                      "H245"     // cheng.j.w
#define PROTONAME_SRP                       "SRP"      // cheng.j.w
#define PROTONAME_H223AAL                   "H223AAL"  // cheng.j,w
#define PROTONAME_H223FIXAAL                "H223FIXAAL"    // cheng.j.w
#define PROTONAME_CCSRL                     "CCSRL"         // cheng.j.w 
#define PROTONAME_UMTS_3G					"UMTS/3G"

// For ATM
#define PROTONAME_ATM_UNI					"ATM_UNI"
#define PROTONAME_ATM_NNI					"ATM_NNI"
#define PROTONAME_ASN_TEST					"ASN_TEST"//专门用来测试与ASN.1方式的兼容性
#define PROTONAME_SAAL						"SAAL"
#define PROTONAME_AAL5						"AAL5" //AAL5
#define PROTONAME_AAL2						"AAL2" //AAL2
#define PROTONAME_AAL2SSSAR					"AAL2SSSAR" // ITU-T I.366.1

// For No7
#define PROTONAME_NO7_400                   "NO7_400"
#define PROTONAME_NO7_430                   "NO7_430"

#define PROTONAME_SMC	                    "SMC"

#define PROTONAME_RS232COM                  "RS232COM"


#define PROTONAME_SCTP						"SCTP"
#define PROTONAME_M3UA_ISUP					"M3UA_ISUP"


// for E1 config
#define PROTONAME_E1_64K					"E1_64K"
#define PROTONAME_E1_HS2M					"E1_HS2M"

#define PROTONAME_APT3G_AAL					"APT3G_AAL"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#endif//MY_PROTOCOL_NAME_H
