//////////////////////////////////////////////////////////////////////////
//MyProtocolID.h

#ifndef MY_PROTOCOL_ID_H
#define MY_PROTOCOL_ID_H
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#define PROTOID_MAX			65536

/*
 *	非Proto,控制过程使用,要大于PROTOID_MAX
 */
#define PARSE_ERROR				0xf0000001
#define NO_UPPER_PROTO			0xf0000003
#define PARSE_BUFFER_REORDER	0xf0000004//乱序缓存
#define PARSE_BUFFER_FRAGMENT	0xf0000005//分片缓存
#define PROTO_STACK_NULL		0xf0000007//m_NotOverProtoStack为空，HandleNextPacket时使用作为返回值


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define MY_PROTOCOL_MACTCPIPCONTEXT	0x5000

#define MY_PROTOCOL_PHY             0x0001
#define MY_PROTOCOL_MAC				0x0002
#define MY_PROTOCOL_PPP			    0x0003
#define MY_PROTOCOL_IPv4			0x0004
#define MY_PROTOCOL_IPX				0x0005
#define MY_PROTOCOL_UDP				0x0006
#define MY_PROTOCOL_TCP				0x0007
#define MY_PROTOCOL_HTTP			0x0008
#define MY_PROTOCOL_X25_PACKET		0x0009//x.25 packet
#define MY_PROTOCOL_SPX				0x000A
#define MY_PROTOCOL_X25				0x000B//x.25 lapb
#define MY_PROTOCOL_X25_LAPB		MY_PROTOCOL_X25	
#define MY_PROTOCOL_FR				0x000C
#define MY_PROTOCOL_ATM				0x000D
#define MY_PROTOCOL_ARP				0x000E
#define MY_PROTOCOL_XNS				0x000F
#define MY_PROTOCOL_XNS1			0x0010
#define MY_PROTOCOL_APPLETALK_ARP	0x0011
#define MY_PROTOCOL_VINES2			0x0012
#define MY_PROTOCOL_BANYAN_ECHO1	0x0013
#define MY_PROTOCOL_BANYAN_ECHO2	0x0014
#define MY_PROTOCOL_DNA				0x0015
#define MY_PROTOCOL_RARP			0x0016
#define MY_PROTOCOL_APPLETALK		0x0017
#define MY_PROTOCOL_SNA				0x0018
#define MY_PROTOCOL_IPX1			0x0019
#define MY_PROTOCOL_LLC				0x001A
#define MY_PROTOCOL_ICMP			0x001B
#define MY_PROTOCOL_IPIGMP			0x001C
#define MY_PROTOCOL_IPGGP			0x001D
#define MY_PROTOCOL_IPSTREAM		0x001E
#define MY_PROTOCOL_IGP				0x001F
#define MY_PROTOCOL_NVPII			0x0020
#define MY_PROTOCOL_PUP				0x0021
#define MY_PROTOCOL_CHAOS			0x0022
#define MY_PROTOCOL_SEP				0x0023
#define MY_PROTOCOL_DDP				0x0024
#define MY_PROTOCOL_RSVP			0x0025
#define MY_PROTOCOL_IGRP			0x0026
#define MY_PROTOCOL_IPOSPF			0x0027
#define MY_PROTOCOL_FTPDATA			0x0028
#define MY_PROTOCOL_FTPCTRL			0x0029
#define MY_PROTOCOL_KLOGIN			0x002A
#define MY_PROTOCOL_WEBSTER			0x002B
#define MY_PROTOCOL_NETBIOS_SSN		0x002C
#define MY_PROTOCOL_POP2			0x002D
#define MY_PROTOCOL_POP3			0x002E
#define MY_PROTOCOL_SUNRPC			0x002F
#define MY_PROTOCOL_TELNET			0x0030
#define MY_PROTOCOL_GOPHER			0x0031
#define MY_PROTOCOL_LOGIN			0x0032
#define MY_PROTOCOL_DNS				0x0033
#define MY_PROTOCOL_FINGER			0x0034
#define MY_PROTOCOL_ECHO			0x0035
#define MY_PROTOCOL_OSQLNET			0x0036
#define MY_PROTOCOL_BOOTPSERVER		0x0037
#define MY_PROTOCOL_BOOTPCLIENT		0x0038
#define MY_PROTOCOL_TFTP			0x0039
#define MY_PROTOCOL_IPNTP			0x003A
#define MY_PROTOCOL_NETBIOS_NS		0x003B
#define MY_PROTOCOL_NETBIOS_DGM		0x003C
#define MY_PROTOCOL_SQLNET			0x003D
#define MY_PROTOCOL_SQLSRV			0x003E
#define MY_PROTOCOL_SNMP161			0x003F
#define MY_PROTOCOL_SNMP162			0x0040
#define MY_PROTOCOL_BGP				0x0041
#define MY_PROTOCOL_LDAP			0x0042
#define MY_PROTOCOL_IPRIP	   		0x0043
#define MY_PROTOCOL_WHO				0x0044
#define MY_PROTOCOL_H225RAS			0x0045
#define MY_PROTOCOL_H225DISGK		0x0046
#define MY_PROTOCOL_RTP				0x0047
#define MY_PROTOCOL_RTCP			0x0048
#define MY_PROTOCOL_SMTP			0x0049
#define MY_PROTOCOL_NETBIOS			0x004A
#define MY_PROTOCOL_CISCO_ISL		0x004B
#define	MY_PROTOCOL_8021Q			0x004C
#define MY_PROTOCOL_CISCO_HDLC	    0x004D

#define MY_PROTOCOL_NONE			0x0100
#define MY_PROTOCOL_SPECIAL			0x0101
#define MY_PROTOCOL_SNAP			0x0102
#define MY_PROTOCOL_IPEGP			0x0103
#define MY_PROTOCOL_MINIMAL_IP		0x0104
#define MY_PROTOCOL_GRE				0x0105
#define MY_PROTOCOL_MIP				0x0106
#define MY_PROTOCOL_IPXSAP			0x0107
#define MY_PROTOCOL_IPXSPX			0x0108
#define MY_PROTOCOL_RIPX			0x0109
#define MY_PROTOCOL_IPXNCP			0x010A
#define MY_PROTOCOL_IPXBMP			0x010B
#define MY_PROTOCOL_IPXMSG			0x010C
#define MY_PROTOCOL_IPXECHO			0x010D
#define MY_PROTOCOL_IPXERROR		0x010E
#define MY_PROTOCOL_IPXSER			0x010F
#define MY_PROTOCOL_IPXDIAG			0x0110
#define MY_PROTOCOL_BPDU			0x0111
#define MY_PROTOCOL_APPLETALK_LAP	0x0112
#define MY_PROTOCOL_APPLETALK_DDP	0x0113
#define MY_PROTOCOL_APPLETALK_NBP	0x0114
#define MY_PROTOCOL_APPLETALK_ZIP	0x0115
#define MY_PROTOCOL_APPLETALK_RTMP	0x0116
#define MY_PROTOCOL_APPLETALK_AEP	0x0117
#define MY_PROTOCOL_APPLETALK_ATP	0x0118
#define MY_PROTOCOL_APPLETALK_ADSP	0x0119
#define MY_PROTOCOL_BANYAN_ECHO		0x011A
#define MY_PROTOCOL_BANYAN_VIP		0x011B
#define MY_PROTOCOL_BANYAN_ARP		0x011C
#define MY_PROTOCOL_BANYAN_ICP		0x011D
#define MY_PROTOCOL_BANYAN_SPP		0x011E
#define MY_PROTOCOL_BANYAN_IPC		0x011F
#define MY_PROTOCOL_BANYAN_RTP		0x0120
#define MY_PROTOCOL_NETBEUI		    0x0121
#define MY_PROTOCOL_IBMSMB			0x0122
#define MY_PROTOCOL_Q922			0x0123
#define MY_PROTOCOL_Q933			0x0124
#define MY_PROTOCOL_RFC1490			0x0125
#define MY_PROTOCOL_ETHER_TYPE		0x0126
#define MY_PROTOCOL_DHCP			0x0128
#define MY_PROTOCOL_RADIUS			0x0129
#define MY_PROTOCOL_PPP_LCP			0x012A
#define MY_PROTOCOL_PPP_CHAP		0x012B
#define MY_PROTOCOL_PPP_CCP			0x012C
#define MY_PROTOCOL_PPP_IPCP		0x012D
#define MY_PROTOCOL_PPP_NBFCP		0x012E
#define MY_PROTOCOL_PPP_IPXCP		0x012F
#define MY_PROTOCOL_PPP_BACP		0x0130
#define MY_PROTOCOL_PPP_BAP			0x0131
#define MY_PROTOCOL_PPP_BCP			0x0132
#define MY_PROTOCOL_PPP_1STCC		0x0133
#define MY_PROTOCOL_PPP_ATCP		0x0134
#define MY_PROTOCOL_PPP_BVCP		0x0135
#define MY_PROTOCOL_PPP_ECP			0x0136
#define MY_PROTOCOL_PPP_PAP			0x0137
#define MY_PROTOCOL_PPP_LQR			0x0138
#define MY_PROTOCOL_PPP_DNCP		0x0139
#define MY_PROTOCOL_PPP_IPv6CP		0x013A
#define MY_PROTOCOL_PPP_SDCP		0x013B
#define MY_PROTOCOL_PPP_SNACP		0x013C
#define MY_PROTOCOL_PPP_OSINLCP		0x013D
#define MY_PROTOCOL_PPP_MP			0x013E
#define MY_PROTOCOL_SMPP			0x013F
#define MY_PROTOCOL_CMPP			0x0140
#define MY_PROTOCOL_NO7_MTP			0x0141
#define MY_PROTOCOL_NO7_MSU			0x0142
#define MY_PROTOCOL_NO7_LSSU		0x0143
#define MY_PROTOCOL_NO7_ISUP		0x0144
#define MY_PROTOCOL_NO7_FISU		0x0145
#define MY_PROTOCOL_MPLS			0x0146
#define MY_PROTOCOL_A11				0x0147
#define MY_PROTOCOL_WSP				0x0148
#define MY_PROTOCOL_WTP				0x0149
#define MY_PROTOCOL_PPPSTREAM		0x014A
#define MY_PROTOCOL_RFC1144			0x014B
#define MY_PROTOCOL_MPLS_LDP	    0x014C
#define MY_PROTOCOL_IPV4_STREAM		0x014D
#define MY_PROTOCOL_RSV_FOR_OBCFT	0x014E
#define MY_PROTOCOL_PPP_VJ_UNCMP	0x0150
#define MY_PROTOCOL_PPP_VJ_CMP		0x0151
#define MY_PROTOCOL_GPRS_GTP		0x0152
#define	MY_PROTOCOL_CDMA_COURSE		0x0153
#define MY_PROTOCOL_LAN				0x0154

#define MY_PROTOCOL_AAL5			0x0155 //AAL5

#define MY_PROTOCOL_80211A			0x0156
#define MY_PROTOCOL_80211B			0x0157
#define MY_PROTOCOL_80211G			0x0158

#define MY_PROTOCOL_80211A_PHY		0x0159
#define MY_PROTOCOL_80211B_PHY		0x015A
#define MY_PROTOCOL_80211G_PHY		0x015B

#define MY_PROTOCOL_MGCP            0x015C
#define MY_PROTOCOL_SIP             0x015D
#define MY_PROTOCOL_H248            0x015E
#define MY_PROTOCOL_SIGTRAN         0x015F
#define MY_PROTOCOL_BICC            0x0160
#define MY_PROTOCOL_MEGACO			MY_PROTOCOL_H248
#define MY_PROTOCOL_SDP				0x0161

#define MY_PROTOCOL_EMPP			0x0162
#define MY_PROTOCOL_SMIAS			0x0163
#define MY_PROTOCOL_SGIP			0x0164
#define MY_PROTOCOL_MMSE			0x0165
#define MY_PROTOCOL_PUSH			0x0166
#define MY_PROTOCOL_VTP				0x0167
#define MY_PROTOCOL_WTPS			0x0168
#define MY_PROTOCOL_NO7_TUP			0x0169
#define MY_PROTOCOL_TDS				0x016A

// For ATM
#define MY_PROTOCOL_ATM_UNI			0x016B
#define MY_PROTOCOL_ATM_NNI			0x016C
#define MY_PROTOCOL_ASN_TEST		0x016D //专门用来测试与ASN.1方式的兼容性
#define MY_PROTOCOL_AAL2			0x016E //AAL2
#define MY_PROTOCOL_PPPOE_DISCOVERY	0x016F 
#define MY_PROTOCOL_PPPOE_SESSION	0x0170 
#define MY_PROTOCOL_SSCOP			0x0171

// For WLAN
#define MY_PROTOCOL_WLAN_NET		0x0172      // WLAN网络测试业务实现
#define MY_PROTOCOL_EAP			    0x0173
#define MY_PROTOCOL_EAPOL			0x0174
#define MY_PROTOCOL_EAPORADIUS		0x0175
#define MY_PROTOCOL_PORTAL			0x0176

// For 3G
#define MY_PROTOCOL_MTP3B			0x0177
#define MY_PROTOCOL_SCCP			0x0178
#define MY_PROTOCOL_RR				0x0179
#define MY_PROTOCOL_RANAP			0x017A
#define MY_PROTOCOL_GMM				0x017B
#define MY_PROTOCOL_RRC				0x017C
#define MY_PROTOCOL_ALCAP			0x017D
#define MY_PROTOCOL_IUUP			0x017E
#define MY_PROTOCOL_CC				0x017F
#define MY_PROTOCOL_MM				0x0180
#define MY_PROTOCOL_SM				0x0181
#define MY_PROTOCOL_TS24008			0x0182
#define MY_PROTOCOL_NBAP            0x0183
#define MY_PROTOCOL_SMSCP           0x0184
#define MY_PROTOCOL_SMSRP           0x0185
#define MY_PROTOCOL_MAC_3G          0x0186
#define MY_PROTOCOL_RLC             0x0187
#define MY_PROTOCOL_SMSTP           0x0188
#define MY_PROTOCOL_FP              0x0189
#define MY_PROTOCOL_SS				0x0190
#define MY_PROTOCOL_SCMG			0x0191
#define MY_PROTOCOL_SNT				0x0192
#define MY_PROTOCOL_SNM				0x0193
#define MY_PROTOCOL_RLP 			0x0194
#define MY_PROTOCOL_RNSAP			0x0195
#define MY_PROTOCOL_RLC_FIX			0x0196
#define MY_PROTOCOL_IUB_INTERFACE	0x0197
#define MY_PROTOCOL_IUR_INTERFACE	0x0198
#define MY_PROTOCOL_IUCS_INTERFACE	0x0199
#define MY_PROTOCOL_IUPS_INTERFACE	0x019A
#define MY_PROTOCOL_H223            0x019B  // cheng.j.w
#define MY_PROTOCOL_H245            0x019C  // cheng.j.w
#define MY_PROTOCOL_SRP             0x019D  // cheng.j.w
#define MY_PROTOCOL_H223AAL         0x019E  //  cheng.j.w
#define MY_PROTOCOL_H223FIXAAL      0x019F  // cheng.j.w
#define MY_PROTOCOL_CCSRL           0x01A0  // cheng.j.w
#define MY_PROTOCOL_UMTS_3G			0x01A1

// -----------------------------------------------
// Range (0x0177 - 0x01ff) is reserved for ATM/3G.
// -----------------------------------------------

// New
#define MY_PROTOCOL_SIGTRAN_IUA		0x0200
#define MY_PROTOCOL_SIGTRAN_M2UA	0x0201
#define MY_PROTOCOL_SIGTRAN_M3UA	0x0202
#define MY_PROTOCOL_SIGTRAN_SCTP	MY_PROTOCOL_SCTP//0x0203
#define MY_PROTOCOL_RTSP			0x0204
#define MY_PROTOCOL_IPV6			0x0205

#define MY_PROTOCOL_SMGP			0x0206		//chench,2004-3-23
#define MY_PROTOCOL_GPRS_GTP_V1		0x0207 

#define MY_PROTOCOL_AAL2SSSAR		0x0208 

#define MY_PROTOCOL_SMC		        0x0209

// -----------------------------------------------
// Range (0x0210 - 0x02ff) is reserved for No 7.
// -----------------------------------------------
#define MY_PROTOCOL_NO7_400         0x0210
#define MY_PROTOCOL_NO7_430         0x0211

#define MY_PROTOCOL_SCTP			0x0212
#define MY_PROTOCOL_M3UA_ISUP		0x0213
#define MY_PROTOCOL_RFC3015			0x0214
#define MY_PROTOCOL_NO7_MAP			0x0215
#define MY_PROTOCOL_NO7_TCAP		0x0216


// For APT-3G板卡SAR后的包类型
#define MY_PROTOCOL_APT3G_AAL		0x0217

// -----------------------------------------------
// Range (0x0210 - 0x02ff) is reserved for No 7.
// -----------------------------------------------


#define MY_PROTOCOL_RSV_SMS_MAC		0x1001

#define MY_PROTOCOL_RS232COM		0x1002
#define MY_PROTOCOL_ATC				0x1003
#define MY_PROTOCOL_ATC_STREAM		0x1004

// 用来区分E1设备的链路配置不同
#define MY_PROTOCOL_E1_64K			0x1005
#define MY_PROTOCOL_E1_HS2M			0x1006

#define MY_PROTOCOL_BCTP			0x1007
#define MY_PROTOCOL_GBLLC			0x1008
#define MY_PROTOCOL_GBNS			0x1009
#define MY_PROTOCOL_GBSSGP			0x100A
#define MY_PROTOCOL_IPBCP			0x100B
#define MY_PROTOCOL_VLAN			0x100C
#define MY_PROTOCOL_MY_HEADER		0x100D
#define MY_PROTOCOL_V5UA			0x100E
#define MY_PROTOCOL_M2PA			0x100F
#define MY_PROTOCOL_SIGTRAN_SUA		0x1010
#define MY_PROTOCOL_DIAMETER		0x1011
#define MY_PROTOCOL_CNGP			0x1012
#define MY_PROTOCOL_SMRSE			0x1013
#define MY_PROTOCOL_CNPP			0x1014
#define MY_PROTOCOL_CMPP30			0x1015


//////////////////////////////////////////////////////////////////////////////
#define MY_PROTOCOL_LAST_PREDEFINE		0x1016

// 历史数据
#define MY_PROBE_HISTORY_DATA			0x1020
#define MY_PROTOCOL_SELFMON				0x1021

#define MY_PROTOCOL_EXTEND				0xF000
#define MY_PROTOCOL_USER				0xF100  
#define USER_PROTOCOL_COUNT				20			// 用户可以局部使用的固定协议号，用户模块要保证是局部使用的
#define MY_PROTOCOL_INVALIDATE_PROTO	0xFFFF    //无效协议

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#endif//MY_PROTOCOL_ID_H
