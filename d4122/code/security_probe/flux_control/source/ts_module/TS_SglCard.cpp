#include "stdafx.h"
#include "TS_SglCard.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
///////////////////////////////////////

CTS_SglCard::CTS_SglCard()
{
	m_nCardStat = 0;
	m_vtE1Stat.clear();
}

CTS_SglCard::~CTS_SglCard()
{
	ace_read_dev.dump();
	ace_con.dump();
	ace_read_dev.close();
	m_vtE1Stat.clear();
}

BOOL CTS_SglCard::f_ModInit(CTS_CONFIG* pTsConfig, CTS_SGL_CONFIG* pSglConfig, ACE_Log_File_Msg* pLogFile)
{
	m_pTsCfg = pTsConfig;
	m_pSglCfg = pSglConfig;
	m_pLogFile= pLogFile;

	BOOL ret;
	
	//ping test
	ret = f_ConnetSglCard(pSglConfig->m_strCardName.c_str());
	if (0 > ret)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]f_ModInit: f_ConnetSglCard %s failed.\n"), pSglConfig->m_strCardName.c_str()));
	}
	else
	{		
		//init E1 stat
		unsigned char ucE1Stat = 0xFF;//unknown
		for (ACE_UINT32 i=0; i<pSglConfig->m_E1Max; i++)
		{
			m_vtE1Stat.push_back(ucE1Stat);
		}

		m_nCardStat = 1;//EA32������
	}

	return TRUE;
}

BOOL CTS_SglCard::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg, CTS_SGL_CONFIG* pNewSglConfig)
{
	m_pTsCfg = pNewTsCfg;
	m_pSglCfg = pNewSglConfig;

	return TRUE;
}

int CTS_SglCard::f_ConnetSglCard(char* pCardName)
{
	// �رվɴ���
	ace_read_dev.dump();
	ace_con.dump();

	// ��LMB���Ĵ���
	int result = ace_con.connect(ace_read_dev, ACE_DEV_Addr(pCardName));
	if (result < 0) 
	{ 
		return -1; 
	}

	// ���ô���
	// ������
	serial_param.baudrate = 57600;
	// ����λ���͡��Ϸ�ֵΪ5, 6, 7, 8��Win32��֧��4λ
	serial_param.databits = 8;//ÿ���ֽ��� 8 λ
	// ��żУ��
	serial_param.paritymode = "none";
	// ֹͣλ���Ϸ�ֵΪ1��2
	serial_param.stopbits = 1;
	// ��XON�ַ�������֮ǰ�����뻺��������С�ֽ�
	serial_param.xonlim = 0;
	// ��XOFF�ַ�������֮ǰ�����뻺����������ֽ�
	serial_param.xofflim = 0;
	// ����POSIX�Ǳ�׼�Ķ����ٸ������ַ�
	serial_param.readmincharacters = 0;
	// �ڴ�read����֮ǰ���ȴ���ʱ��
	serial_param.readtimeoutmsec = 10; // 10ms
	// �Ƿ�֧��CTSģʽ��ȷ���Ƿ��ͨ����ѯ Clear To Send(CTS) �ߵ�״̬�������ݡ�
	serial_param.ctsenb = FALSE;
	// ʹ�ú�����RTS��0��ʾ��ʹ��RTS 1��ʾʹ��RTS 2��ʾʹ��RTS����������(win32)
	// 3��ʾ��������㹻�����ڴ�����ֽڣ�RTS�߱�ߡ�����֮��RTS��ɵ�(win32)
	serial_param.rtsenb = 0;
	// �Ƿ���������ʹ�����������
	serial_param.xinenb = FALSE;
	// �Ƿ��������ʹ�����������
	serial_param.xoutenb = FALSE;
	// �豸�Ƿ���POSIX����ģʽ
	serial_param.modem = FALSE;
	// �Ƿ�ʹ��receiver(POSIX)
	serial_param.rcvenb = TRUE;
	// Data Set Ready(DSR) �ߵ�״̬�Ƿ�������(WIN32)
	serial_param.dsrenb = FALSE;
	// Data Terminal Ready״̬�Ƿ�������
	serial_param.dtrdisable = FALSE;

	// ��������
	result = ace_read_dev.control(ACE_TTY_IO::SETPARAMS, &serial_param);
	if (result < 0) 
	{ 
		return -2; 
	}

	return 0;
}

BOOL CTS_SglCard::f_GetE1Stat(void)
{	
	if (0 == m_nCardStat)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]f_GetE1Stat: %s unconnected.\n"), m_pSglCfg->m_strCardName.c_str()));
		return FALSE;
	}

	ace_read_dev.send(CMD_CONSOLE_GET, 7);

	//E1Эת��ᷢ�͸澯�ϱ�֡��ѭ����ȡֱ����ȡ����ѯӦ��֡
	do{
		memset(m_aResponse, 0, sizeof(m_aResponse));
		
		// 3�ֽ�ͷ
		int nRecvLen = ace_read_dev.recv(m_aResponse, 3);
		if (3 > nRecvLen)//�������ݶ���
		{ 
			MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]f_GetE1Stat: %s recv head failed.\n"), m_pSglCfg->m_strCardName.c_str()));
			return FALSE;
		}

		if ((0x7E != m_aResponse[0]) || (0xFF != m_aResponse[1]))
		{ 
			MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]f_GetE1Stat: %s head check failed.\n"), m_pSglCfg->m_strCardName.c_str()));
			continue;
		}

		if (29 < m_aResponse[2])
		{
			continue;
		}

		// �����ȶ�ȡ
		nRecvLen = ace_read_dev.recv(m_aResponse+nRecvLen, m_aResponse[2]);
		if (nRecvLen != m_aResponse[2])
		{
			continue;
		}
	}
	while((0x0A != m_aResponse[3]) || (0x00 != m_aResponse[4]));

	//֡У��

	//��ȡ����ѯӦ��֡
	for (ACE_UINT32 i=0; i<m_pSglCfg->m_E1Max; i++)
	{
		m_vtE1Stat[i] = m_aResponse[5+i];
	}	

	return TRUE;
}

