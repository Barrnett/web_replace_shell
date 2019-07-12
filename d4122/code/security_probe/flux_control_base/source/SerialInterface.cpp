//////////////////////////////////////////////////////////////////////////
//SerialInterface.cpp

#include "stdafx.h"
#include <unistd.h>
#include <fcntl.h> /* File control definitions */
#include <errno.h>
#include <termios.h>
#include "SerialInterface.h"
//#include "cpf/IfIPTool.h"
//#include "cpf/HostEtherCardMgr.h"
//#include "PacketIO/DataSourceHelper.h"
//#include "cpf/adapter_info_query.h"

CSerialInterface::CSerialInterface()
{
	m_piSerialDataRecv = NULL;
	m_piSerialDataSend = NULL;
	m_vtSerialIfInfo.clear();
}

CSerialInterface::~CSerialInterface()
{
	if (m_piSerialDataRecv)
	{
		delete m_piSerialDataRecv;
	}

	if (m_piSerialDataSend)
	{
		delete m_piSerialDataSend;
	}

	m_vtSerialIfInfo.clear();
}

int CSerialInterface::f_Init()
{
	m_piSerialDataRecv = new CSerialDataRecv;
	if (NULL == m_piSerialDataRecv)
	{
		return -1;
	}
	m_piSerialDataRecv->f_Init();

	m_piSerialDataSend = new CSerialDataSend;
	if (NULL == m_piSerialDataSend)
	{
		return -2;
	}
	m_piSerialDataSend->f_Init();

	return 0;
}

int CSerialInterface::f_Open(std::string& strSerialName, ACE_UINT32 nBitrate, ACE_UINT32 nCs, ACE_UINT32 nParity, ACE_UINT32 nStopbit)
{
	int ret;
	ACE_UINT8 nSerialIndex = 0;
	
	size_t max = m_vtSerialIfInfo.size();
	for (size_t i=0; i<max; i++)
	{
		if (m_vtSerialIfInfo[i].strSerialName == strSerialName)
		{
			nSerialIndex = m_vtSerialIfInfo[i].nSerialIndex;
			break;
		}
	}

	if (0 == nSerialIndex)
	{
		nSerialIndex = max + 1;

		//
		int nSerialFd = open(strSerialName.c_str(), O_RDWR | O_NOCTTY);
		if (0 > nSerialFd)
		{
			return -10;
		}

		struct termios serial_param;
		
		tcgetattr(nSerialFd, &serial_param);
		
		//������		
		if (4800 == nBitrate)
		{
			cfsetispeed(&serial_param, B4800);
			cfsetospeed(&serial_param, B4800);
		}
		else if (9600 == nBitrate)
		{
			cfsetispeed(&serial_param, B9600);
			cfsetospeed(&serial_param, B9600);
		}
		else if (19200 == nBitrate)
		{
			cfsetispeed(&serial_param, B19200);
			cfsetospeed(&serial_param, B19200);
		}
		else if (38400 == nBitrate)
		{
			cfsetispeed(&serial_param, B38400);
			cfsetospeed(&serial_param, B38400);
		}
		else if (57600 == nBitrate)
		{
			cfsetispeed(&serial_param, B57600);
			cfsetospeed(&serial_param, B57600);
		}
		else if (115200 == nBitrate)
		{
			cfsetispeed(&serial_param, B115200);
			cfsetospeed(&serial_param, B115200);
		}
		else
		{
			cfsetispeed(&serial_param, B115200);
			cfsetospeed(&serial_param, B115200);
		}

		//����λ
		if (7 == nCs)
		{
			serial_param.c_cflag &= ~CSIZE;
			serial_param.c_cflag |= CS7;
		}
		else if (8 == nCs)
		{
			serial_param.c_cflag &= ~CSIZE;
			serial_param.c_cflag |= CS8;
		}
		else
		{
			serial_param.c_cflag &= ~CSIZE;
			serial_param.c_cflag |= CS8;
		}

		//��żУ��
		if (0 == nParity)
		{
			serial_param.c_cflag |= PARENB;
			serial_param.c_cflag |= PARODD;
		}
		else if (1 == nParity)
		{
			serial_param.c_cflag |= PARENB;
			serial_param.c_cflag &= ~PARODD;
		}
		else if (2 == nParity)
		{
			serial_param.c_cflag &= ~PARENB;
		}
		else
		{
			serial_param.c_cflag &= ~PARENB;
		}

		//ֹͣλ
		if (1 == nStopbit)
		{
			serial_param.c_cflag &= ~CSTOPB;
		}
		else
		{
			serial_param.c_cflag |= CSTOPB;
		}
		
		// ��ռ�� | �ܹ���ȡ
		serial_param.c_cflag |= (CLOCAL | CREAD);

		//��ʹ��������
		serial_param.c_cflag &= ~CRTSCTS;

		// �޸����ģʽ��ԭʼ�������������
  		serial_param.c_oflag &= ~OPOST;
		serial_param.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  // input

		// ȡ�������ַ��Ŀ���,������ú���Ҫ�������Ͷ���������ʱ����0x0d,0x11��0x13ȴ�ᱻ����
		// �ο� http://blog.sina.com.cn/s/blog_55b759e60102vvp2.html
		//serial_param.c_iflag &= ~(ICRNL | IXON);
		serial_param.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

		 // �����������������������ݣ����ǲ��ٶ�ȡ
		tcflush(nSerialFd,TCIFLUSH);

		// ����������Ч
		tcsetattr(nSerialFd, TCSANOW, &serial_param);

		//
		SERIAL_INTERFACE_INFO_S tagSerialIntInfo;

		tagSerialIntInfo.strSerialName = strSerialName;
		tagSerialIntInfo.nSerialIndex = nSerialIndex;
		tagSerialIntInfo.nSerialFd = nSerialFd;

		m_vtSerialIfInfo.push_back(tagSerialIntInfo);

		ret = m_piSerialDataRecv->f_AddSerialIndex(nSerialIndex, nSerialFd);
		if (0 > ret)
		{
			return -1;
		}
		ret = m_piSerialDataSend->f_AddSerialIndex(nSerialIndex, nSerialFd);
		if (0 > ret)
		{
			return -2;
		}
	}

	return (int)nSerialIndex;
}

int CSerialInterface::f_Close(std::string& strSerialName)
{
	int nSerialFd = -1;
	
	size_t max = m_vtSerialIfInfo.size();
	for (size_t i=0; i<max; i++)
	{
		if (m_vtSerialIfInfo[i].strSerialName == strSerialName)
		{
			m_piSerialDataRecv->f_DeleteSerialIndex(m_vtSerialIfInfo[i].nSerialIndex);
			m_piSerialDataSend->f_DeleteSerialIndex(m_vtSerialIfInfo[i].nSerialIndex);

			close(m_vtSerialIfInfo[i].nSerialFd);

			break;
		}
	}
	
	return 0;
}

int CSerialInterface::f_Set(std::string& strSerialName, ACE_UINT32 nBitrate, ACE_UINT32 nCs, ACE_UINT32 nParity, ACE_UINT32 nStopbit)
{
	int nSerialFd = -1;
	
	size_t max = m_vtSerialIfInfo.size();
	for (size_t i=0; i<max; i++)
	{
		if (m_vtSerialIfInfo[i].strSerialName == strSerialName)
		{
			nSerialFd = m_vtSerialIfInfo[i].nSerialFd;
			break;
		}
	}

	if (-1 != nSerialFd)
	{
		struct termios serial_param;
		
		tcgetattr(nSerialFd, &serial_param);
		
		//������		
		if (4800 == nBitrate)
		{
			cfsetispeed(&serial_param, B4800);
			cfsetospeed(&serial_param, B4800);
		}
		else if (9600 == nBitrate)
		{
			cfsetispeed(&serial_param, B9600);
			cfsetospeed(&serial_param, B9600);
		}
		else if (19200 == nBitrate)
		{
			cfsetispeed(&serial_param, B19200);
			cfsetospeed(&serial_param, B19200);
		}
		else if (38400 == nBitrate)
		{
			cfsetispeed(&serial_param, B38400);
			cfsetospeed(&serial_param, B38400);
		}
		else if (57600 == nBitrate)
		{
			cfsetispeed(&serial_param, B57600);
			cfsetospeed(&serial_param, B57600);
		}
		else if (115200 == nBitrate)
		{
			cfsetispeed(&serial_param, B115200);
			cfsetospeed(&serial_param, B115200);
		}
		else
		{
			cfsetispeed(&serial_param, B115200);
			cfsetospeed(&serial_param, B115200);
		}

		//����λ
		if (7 == nCs)
		{
			serial_param.c_cflag &= ~CSIZE;
			serial_param.c_cflag |= CS7;
		}
		else if (8 == nCs)
		{
			serial_param.c_cflag &= ~CSIZE;
			serial_param.c_cflag |= CS8;
		}
		else
		{
			serial_param.c_cflag &= ~CSIZE;
			serial_param.c_cflag |= CS8;
		}

		//��żУ��
		if (0 == nParity)
		{
			serial_param.c_cflag |= PARENB;
			serial_param.c_cflag |= PARODD;
		}
		else if (1 == nParity)
		{
			serial_param.c_cflag |= PARENB;
			serial_param.c_cflag &= ~PARODD;
		}
		else if (2 == nParity)
		{
			serial_param.c_cflag &= ~PARENB;
		}
		else
		{
			serial_param.c_cflag &= ~PARENB;
		}

		//ֹͣλ
		if (1 == nStopbit)
		{
			serial_param.c_cflag &= ~CSTOPB;
		}
		else
		{
			serial_param.c_cflag |= CSTOPB;
		}
		
		// ����������Ч
		tcsetattr(nSerialFd, TCSANOW, &serial_param);
	}

	return nSerialFd;
}


