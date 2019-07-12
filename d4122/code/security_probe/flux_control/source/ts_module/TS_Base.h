#if  !defined(__TS_BASE__)
#define __TS_BASE__

#pragma once

#include "TS_interfaceCfg.h"
#include "ArpTableEx.h"
#include "TS_Config.h"
#include "EtherioDllExport.h"
#include "tcpip_mgr_common_init.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "TS_OneWanLineStatusManager.h"
#include "cpf/ostypedef.h"
#include "cpf/CommonQueueManager.h"
#include "flux_control_common.h"
#include "tcpip_mgr_common_init.h"

BOOL f_cmp_srcip_with_filter(ACE_UINT32 ip, const CTS_FILTER_CONFIG * p_filter);
BOOL f_cmp_dstip_with_filter(ACE_UINT32 ip, const CTS_FILTER_CONFIG * p_filter);
BOOL f_cmp_port_with_filter(ACE_UINT16 port, const CTS_FILTER_CONFIG * p_filter);
BOOL f_cmp_pkt_with_filter(PACKET_INFO_EX& packet_info, const CTS_FILTER_CONFIG  *p_filter);

typedef enum TS_HOST_STAT
{
	UNKNOWN = 0,
	ARP_CHECKING,
	ARP_OK,
	ARP_FAILED,
	ECHO_CHECKING,
	ECHO_OK,
	ECHO_FAILED
}TS_HOST_STAT_E;

class CTS_HOST_DETECT
{
public:
	CTS_HOST_DETECT();
	virtual ~CTS_HOST_DETECT();
	
public:
	ACE_UINT32 m_hlHostIp;
	
	TS_HOST_STAT_E m_eHostStat;
	
	ACE_UINT8 m_aHostMac[6+2];

	ACE_UINT32 m_nHostRecoverTimer;

	COneTypeTestInfo m_iHostTest;

public:
	BOOL f_Init(ACE_UINT32 nHostIp, ACE_UINT32 nTestNum);
	void f_ResetIp(ACE_UINT32 nHostIp);
	void f_ResetTestNum(ACE_UINT32 nTestNum);
};

class CTS_PACKET_FILTER
{
public:
	CTS_PACKET_FILTER();
	virtual ~CTS_PACKET_FILTER();

public:
	std::vector<CTS_FILTER_CONFIG*> m_vtFilter;

public:
	BOOL f_ApplyFilter(PACKET_INFO_EX & packet_info);
};

// 可靠传输用的缓存队列
class CTS_PACKET_QUEUE
{
public:
	CTS_PACKET_QUEUE();
	virtual ~CTS_PACKET_QUEUE();

public:
	unsigned char *m_pBuf;
	//TAG_COMMOM_QUEUE_MANAGER m_queue_write_handle;
	//TAG_COMMOM_QUEUE_MANAGER m_queue_read_handle;
	TAG_COMMOM_QUEUE_MANAGER m_tagHandle;
public:
	BOOL f_Init(unsigned int buffer_size);
	BOOL f_ClearQueue(void);
};


#pragma pack(push)
#pragma pack(1)
// ICMP header
typedef struct TS_ihdr {
	BYTE	i_type;
	BYTE	i_code; /* type sub code */
	USHORT	i_cksum;
	USHORT	i_id;
	USHORT	i_seq;

	char	i_data[1];

}TS_IcmpHeader;
#pragma pack(pop)

#endif

