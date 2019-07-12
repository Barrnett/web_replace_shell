//////////////////////////////////////////////////////////////////////////
//SerialInterface.h

#pragma once

#include "flux_control_base_export.h"
#include "PacketIO/SerialDataRecv.h"
#include "PacketIO/SerialDataSend.h"

typedef struct SERIAL_INTERFACE_INFO
{
	std::string	strSerialName;
	ACE_UINT8	nSerialIndex;
	int			nSerialFd;
}SERIAL_INTERFACE_INFO_S;

class FLUX_CONTROL_BASE_CLASS CSerialInterface
{
public:
	CSerialInterface();
	virtual ~CSerialInterface();

public:
	
	CSerialDataRecv*	m_piSerialDataRecv;
	CSerialDataSend*	m_piSerialDataSend;

	std::vector<SERIAL_INTERFACE_INFO_S> m_vtSerialIfInfo;

public:
	int f_Init();
	int f_Open(std::string & strSerialName, ACE_UINT32 nBitrate, ACE_UINT32 nCs, ACE_UINT32 nParity, ACE_UINT32 nStopbit);
	int f_Close(std::string & strSerialName);
	int f_Set(std::string & strSerialName, ACE_UINT32 nBitrate, ACE_UINT32 nCs, ACE_UINT32 nParity, ACE_UINT32 nStopbit);
};
