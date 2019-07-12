#if  !defined(__TS_MAINNETL2__)
#define __TS_MAINNETL2__

#include "TS_MainNet.h"

class CTS_MainNetL2 : public CTS_MainNet
{
public:
	CTS_MainNetL2();
	~CTS_MainNetL2();

public:
	BOOL f_ModInit(CTS_InterfaceCfg * pTsInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile, CArpTableEx * pArpTable);
	BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);

	void f_HandleMyIcmp(PACKET_INFO_EX & packet_info);
	BOOL f_HandleArp(PACKET_INFO_EX & packet_info);
	BOOL f_HandleIp(PACKET_INFO_EX & packet_info);
	BOOL f_HandlePacket(PACKET_INFO_EX& packet_info);

	void f_PeerDetect(ACE_UINT32 now_time);
	void f_ArpSpoof(void);
	void f_ArpRestore(void);
	void f_MainNetCheck(ACE_UINT32 now_time);
	//int f_ForwardPacket(PACKET_INFO_EX & packet_info);
	int f_ForwardIp(PACKET_INFO_EX & packet_info);
	int f_ForwardArp(PACKET_INFO_EX & packet_info);
};

#endif

