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

		m_nCardStat = 1;//EA32已连接
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
	// 关闭旧串口
	ace_read_dev.dump();
	ace_con.dump();

	// 打开LMB屏的串口
	int result = ace_con.connect(ace_read_dev, ACE_DEV_Addr(pCardName));
	if (result < 0) 
	{ 
		return -1; 
	}

	// 配置串口
	// 波特率
	serial_param.baudrate = 57600;
	// 数据位类型。合法值为5, 6, 7, 8。Win32上支持4位
	serial_param.databits = 8;//每个字节有 8 位
	// 奇偶校验
	serial_param.paritymode = "none";
	// 停止位。合法值为1和2
	serial_param.stopbits = 1;
	// 在XON字符被发送之前，输入缓冲区的最小字节
	serial_param.xonlim = 0;
	// 在XOFF字符被发送之前，输入缓冲区的最大字节
	serial_param.xofflim = 0;
	// 对于POSIX非标准的读最少个数的字符
	serial_param.readmincharacters = 0;
	// 在从read返回之前，等待的时间
	serial_param.readtimeoutmsec = 10; // 10ms
	// 是否支持CTS模式。确定是否可通过查询 Clear To Send(CTS) 线的状态发送数据。
	serial_param.ctsenb = FALSE;
	// 使用和设置RTS。0标示不使用RTS 1标示使用RTS 2标示使用RTS流控制握手(win32)
	// 3标示如果有了足够的用于传输的字节，RTS线变高。传输之后RTS变成低(win32)
	serial_param.rtsenb = 0;
	// 是否在输入上使用软件流控制
	serial_param.xinenb = FALSE;
	// 是否在输出上使用软件流控制
	serial_param.xoutenb = FALSE;
	// 设备是否是POSIX调制模式
	serial_param.modem = FALSE;
	// 是否使用receiver(POSIX)
	serial_param.rcvenb = TRUE;
	// Data Set Ready(DSR) 线的状态是否起作用(WIN32)
	serial_param.dsrenb = FALSE;
	// Data Terminal Ready状态是否起作用
	serial_param.dtrdisable = FALSE;

	// 载入配置
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

	//E1协转板会发送告警上报帧，循环读取直至读取到查询应答帧
	do{
		memset(m_aResponse, 0, sizeof(m_aResponse));
		
		// 3字节头
		int nRecvLen = ace_read_dev.recv(m_aResponse, 3);
		if (3 > nRecvLen)//串口数据读空
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

		// 按长度读取
		nRecvLen = ace_read_dev.recv(m_aResponse+nRecvLen, m_aResponse[2]);
		if (nRecvLen != m_aResponse[2])
		{
			continue;
		}
	}
	while((0x0A != m_aResponse[3]) || (0x00 != m_aResponse[4]));

	//帧校验

	//读取到查询应答帧
	for (ACE_UINT32 i=0; i<m_pSglCfg->m_E1Max; i++)
	{
		m_vtE1Stat[i] = m_aResponse[5+i];
	}	

	return TRUE;
}

