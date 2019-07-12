//////////////////////////////////////////////////////////////////////////
//MirrorOutDataMgr.h

#pragma once

#include "cpf/simple_filter/CPF_SimpleFltCondition.h"
#include "MirrorOutConfig.h"
#include "PacketIO/PacketIO.h"
#include "PacketIO/EtherioSendPacket.h"

class CInterfaceFactory;

class CMirrorOutDataMgr
{
public:
	CMirrorOutDataMgr(void);
	~CMirrorOutDataMgr(void);

public:
	BOOL start_mirror(CInterfaceFactory * pInterfaceCfgFactory,
		const CMirrorOutConfig& new_config,
		const char * flt_fiile_name);

	void stop_mirror();

	//-1表示没有开始，0表示结束，1表示开始
	int get_mirror_status();

	int write_packet(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array);
	int write_packet(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array);

	BOOL IsMirroring() const;

public:
	void Start_To_Write();
	void Stop_Writting();

public:
	inline BOOL IsMirrorFromCardIndex(int card_idnex) const
	{
		for(size_t i = 0; i < m_vt_mirror_from_index.size(); ++i)
		{
			if( m_vt_mirror_from_index[i] == card_idnex )
			{
				return true;
			}
		}

		return false;
	}

private:
	std::vector<int>		m_vt_mirror_from_index;

private:
	BOOL StartMirrorDevice(const CMirrorOutConfig& new_config);
	BOOL StopMirrorDevice();

private:
	CEtherioWriteDataStream *	m_pMirrorEtherioDataStream;

private:
	CMirrorOutConfig			m_mirror_out_config;

private:
	CPF_SimpleFltCondition		m_flt_condition;
	BOOL						m_bUsedFlt;

	BOOL						m_bMirroring;

};
