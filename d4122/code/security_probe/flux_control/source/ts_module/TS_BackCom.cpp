#include "stdafx.h"
#include "TS_BackCom.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"

CTS_BackCom::CTS_BackCom()
{
	m_nComIndex = 0;

	m_nChkPeriod = 0;
	
	m_vtPeerDct.clear();
	
	m_piPeerPktFilter = NULL;
}

CTS_BackCom::~CTS_BackCom()
{
	//
	size_t max = m_vtPeerDct.size();
	for (size_t i=0; i<max; i++)
	{
		delete m_vtPeerDct[i];
	}
	m_vtPeerDct.clear();

	//
	if (m_piPeerPktFilter)
	{
		delete m_piPeerPktFilter;
	}
}

BOOL CTS_BackCom::f_ModInit(CSerialInterface* pTsSerialInterface, CTS_CONFIG* pTsConfig, CTS_BACKCOM_CONFIG* pBackComConfig, ACE_Log_File_Msg* pLogFile)
{
	m_pTsSerialInterface = pTsSerialInterface;
	m_pTsCfg = pTsConfig;
	m_pBackComCfg = pBackComConfig;
	m_pLogFile = pLogFile;

	// 备用网口使用的物理网口编号
	int ret = m_pTsSerialInterface->f_Open(pBackComConfig->m_strCardName, pBackComConfig->m_nSpeed, pBackComConfig->m_nCs, pBackComConfig->m_nParity, pBackComConfig->m_nStop);
	if (0 > ret)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]m_pTsSerialInterface->f_Open failed. return:%d\n"), ret));
	}
	else
	{
		m_nComIndex = (ACE_UINT8)ret;
	}

	m_nChkPeriod = 0;
		
	f_PeerInit(pTsConfig);
	
	f_FilterInit(pTsConfig, pBackComConfig);
	
	return TRUE;
}

BOOL CTS_BackCom::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg, CTS_BACKCOM_CONFIG* pNewBackComConfig)
{
	if (m_pBackComCfg->m_strCardName != pNewBackComConfig->m_strCardName)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: back com interface changed.\n")));
		ACE_OS::system("reboot -p");
	}

	if (*m_pBackComCfg != *pNewBackComConfig)
	{
		int ret = m_pTsSerialInterface->f_Set(pNewBackComConfig->m_strCardName,
										 pNewBackComConfig->m_nSpeed,
										 pNewBackComConfig->m_nCs,
										 pNewBackComConfig->m_nParity,
										 pNewBackComConfig->m_nStop);
		if (0 > ret)
		{
			MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]f_ModUpdateCfg: %s is not found.\n"), pNewBackComConfig->m_strCardName.c_str()));
		}
		else
		{
			MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]f_ModUpdateCfg: back com %s.\n"), pNewBackComConfig->m_strCardName.c_str()));
		}
	}

	f_FilterUpdateCfg(pNewTsCfg, pNewBackComConfig);

	m_pTsCfg = pNewTsCfg;
	m_pBackComCfg = pNewBackComConfig;
	
	return TRUE;
}

BOOL CTS_BackCom::f_PeerInit(CTS_CONFIG* pTsConfig)
{
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_HOST_DETECT* piBackComChk = new CTS_HOST_DETECT;
		if (NULL == piBackComChk)
		{
			return FALSE;
		}

		piBackComChk->f_Init(0, 1);

		m_vtPeerDct.push_back(piBackComChk);
	}

	return TRUE;
}

BOOL CTS_BackCom::f_FilterInit(CTS_CONFIG* pTsConfig, CTS_BACKCOM_CONFIG* pBackComConfig)
{
	m_piPeerPktFilter = new CTS_PACKET_FILTER;
	if (NULL == m_piPeerPktFilter)
	{
		return FALSE;
	}
	
	size_t max = pBackComConfig->m_vtFilterId.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_FILTER_CONFIG* pFilter = pTsConfig->f_FindFilter(pBackComConfig->m_vtFilterId[i]);
		if (NULL == pFilter)
		{
			return FALSE;
		}

		m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
	}

	return TRUE;
}

BOOL CTS_BackCom::f_FilterUpdateCfg(CTS_CONFIG* pNewTsCfg, CTS_BACKCOM_CONFIG* pNewBackComConfig)
{
	size_t nCurMax = m_pBackComCfg->m_vtFilterId.size();
	size_t nNewMax = pNewBackComConfig->m_vtFilterId.size();
	size_t i;

	if (nCurMax <= nNewMax)
	{
		for (i=0; i<nCurMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewBackComConfig->m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		for (i=nCurMax; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewBackComConfig->m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
		}
	}
	else
	{
		for (i=0; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewBackComConfig->m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		m_piPeerPktFilter->m_vtFilter.erase(m_piPeerPktFilter->m_vtFilter.begin()+nNewMax, m_piPeerPktFilter->m_vtFilter.end());
	}

	return TRUE;
}

void CTS_BackCom::f_SendEchoReq()
{
	unsigned char ping_packet[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	
	m_pTsSerialInterface->m_piSerialDataSend->f_SendPacket(ping_packet, 6, m_nComIndex);
}

void CTS_BackCom::f_BackComCheck(ACE_UINT32 now_time)
{
	if (4 > m_nChkPeriod)
	{
		m_nChkPeriod++;
		return;
	}
	else
	{
		m_nChkPeriod = 0;
	}
	
	size_t max = m_vtPeerDct.size();
	for (size_t i=0; i<max; i++)
	{
		switch (m_vtPeerDct[i]->m_eHostStat)
		{
			case UNKNOWN:
				m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
				f_SendEchoReq();
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(1, now_time);
				
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
					ACE_TEXT("[Info][%D]back com(%d@%s) connectivity changed. %s to %s\n"),
					i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
					"\"UNKNOWN\"", "\"ARP_CHECKING\""));
				break;

			case ECHO_CHECKING:
				if (1 <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_OK;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back com(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_CHECKING\"", "\"ECHO_OK\""));
				}
				else if (1 <= m_vtPeerDct[i]->m_iHostTest.CheckTimeout(now_time, 0))
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_FAILED;
										
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back com(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_CHECKING\"", "\"ECHO_FAILED\""));
				}
				f_SendEchoReq();
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(1, now_time);
				break;

			case ECHO_OK:
				if (1 <= m_vtPeerDct[i]->m_iHostTest.CheckTimeout(now_time, 0))
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_FAILED;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back com(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_OK\"", "\"ECHO_FAILED\""));
				}
				f_SendEchoReq();
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(1, now_time);
				break;

			case ECHO_FAILED:
				if (1 <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back com(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_FAILED\"", "\"ECHO_CHECKING\""));
				}				
				f_SendEchoReq();
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(1, now_time);
				break;

			default:
				break;
		}
	}
}

