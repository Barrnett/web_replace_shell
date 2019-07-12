
#include "stdafx.h"
#include "TS_Mirror.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
#include "FluxControlObserver.h"

CTS_Mirror::CTS_Mirror()
{
	m_nIfIndex = 0xFFFF;
}

CTS_Mirror::~CTS_Mirror()
{
}

BOOL CTS_Mirror::f_ModInit(CTS_CONFIG* pTsConfig, ACE_Log_File_Msg* pLogFile)
{
	m_pTsCfg = pTsConfig;
	m_pLogFile = pLogFile;

	// M网口使用的物理网口编号
	m_nIfIndex = (ACE_UINT32)EtherIO_GetOneIndexByShowName(pTsConfig->m_iMirrorCfg.m_strCardName.c_str());

	EtherIO_GetDeviceMac(m_nIfIndex, m_aMacAddr);
	
	return TRUE;
}

BOOL CTS_Mirror::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (m_pTsCfg->m_iMirrorCfg.m_strCardName != pNewTsCfg->m_iMirrorCfg.m_strCardName)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: mirror interface changed.\n")));
		ACE_OS::system("reboot -p");
		return FALSE;
	}
	
	m_pTsCfg = pNewTsCfg;
	
	return TRUE;
}

BOOL CTS_Mirror::f_HandleArp(PACKET_INFO_EX& packet_info)
{
	return TRUE;
}

BOOL CTS_Mirror::f_HandleIp(PACKET_INFO_EX& packet_info)
{
	return FALSE;
}

BOOL CTS_Mirror::f_HandlePacket(PACKET_INFO_EX& packet_info)
{
	return FALSE;
}
