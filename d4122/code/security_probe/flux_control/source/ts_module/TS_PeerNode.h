#if !defined(__TS_PEERNODE__)
#define __TS_PEERNODE__

#include "flux_control_common.h"
#include "TS_Config.h"

typedef enum TS_PEER_STAT
{
	DISCONNECT = 0,
	MAIN_NET_RUN,
	BACK_NET1_RUN,
	BACK_NET2_RUN,
	BACK_COM1_RUN,
	BACK_COM2_RUN
}TS_PEER_STAT_E;

class CTS_PeerNode
{
public:
	CTS_PeerNode();
	virtual ~CTS_PeerNode();

public:
	CTS_CONFIG* m_pTsCfg;
	ACE_Log_File_Msg* m_pLogFile;
	
	std::vector<TS_PEER_STAT_E> m_vtPeerStat;
	
public:
	BOOL f_ModInit(CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile);
	BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);
};

#endif

