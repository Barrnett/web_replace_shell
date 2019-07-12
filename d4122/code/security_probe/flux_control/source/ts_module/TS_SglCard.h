#if !defined(__TS_SGLCARD__)
#define __TS_SGLCARD__

#include "cpf/ostypedef.h"
#include "flux_control_common.h"
#include "tcpip_mgr_common_init.h"
#include "TS_Config.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "TS_define.h"
#include "ace/DEV_Addr.h"
#include "ace/DEV_Connector.h"
#include "ace/TTY_IO.h"


const unsigned char CMD_CONSOLE_GET[7] = {0x7E, 0xFF, 0x04, 0x0A, 0x00, 0x0D, 0xF1}; 

#define E1STAT_LOS		0x01
#define E1STAT_AIS		0x02
#define E1STAT_LOOP		0x04
#define E1STAT_MLOF		0x08
#define E1STAT_RDI		0x10
#define E1STAT_CRC		0x20

class CTS_SglCard
{
public:
	CTS_SglCard();
	virtual ~CTS_SglCard();
	
public:
	CTS_CONFIG*			m_pTsCfg;
	CTS_SGL_CONFIG*		m_pSglCfg;
	ACE_Log_File_Msg*	m_pLogFile;
	
	ACE_TTY_IO ace_read_dev;
	ACE_DEV_Connector ace_con;
	ACE_TTY_IO::Serial_Params serial_param;

	unsigned char m_aResponse[32];
	
	//接口卡连接状态: 0未连接；1已连接
	unsigned int m_nCardStat;

	//每个bit置位"1"代表一种告警状态:LOS(bit0)/AIS(bit1)/LOOP(bit2)/MLOF(bit3)/RDI(bit4)/CRC(bit5)
	std::vector <unsigned char> m_vtE1Stat;

public:
	virtual BOOL f_ModInit(CTS_CONFIG * pTsConfig, CTS_SGL_CONFIG * pSglConfig, ACE_Log_File_Msg * pLogFile);
	virtual BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg, CTS_SGL_CONFIG * pNewSglConfig);
	virtual int f_ConnetSglCard(char * pCardName);
	virtual BOOL f_GetE1Stat(void);
};
#endif

