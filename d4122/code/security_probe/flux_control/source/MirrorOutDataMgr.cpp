//////////////////////////////////////////////////////////////////////////
//MirrorOutDataMgr.h

#include "stdafx.h"
#include "MirrorOutDataMgr.h"
#include "InterfaceCfg.h"
#include "InterfaceFactory.h"
#include "MirrorOutConfig.h"
#include "cpf/simple_filter/CPF_SimpleFltReader.h"
#include "cpf/simple_filter/CPF_SimpleFilterTool.h"

CMirrorOutDataMgr::CMirrorOutDataMgr(void)
{
	m_pMirrorEtherioDataStream = NULL;

	m_bUsedFlt = false;
	m_bMirroring = false;
}

CMirrorOutDataMgr::~CMirrorOutDataMgr(void)
{
}

BOOL CMirrorOutDataMgr::start_mirror(CInterfaceFactory * pInterfaceCfgFactory,
									 const CMirrorOutConfig& new_config,
									 const char * flt_fiile_name)
{
	m_vt_mirror_from_index.clear();

	StopMirrorDevice();

	m_bUsedFlt = new_config.m_use_flt;

	if( m_bUsedFlt )
	{
		if( CPF_SimpleFltIO::read_filter(flt_fiile_name,m_flt_condition) )
		{
			m_bUsedFlt = true;
		}
		else
		{
			m_bUsedFlt = false;
		}
	}

	if( new_config.m_vt_mirror_from_card.size() == 0 )
	{
		return false;
	}

	for(size_t i = 0; i < new_config.m_vt_mirror_from_card.size(); ++i)
	{
		int card_index = pInterfaceCfgFactory->GetCardIndexByShowName(new_config.m_vt_mirror_from_card[i].c_str());

		if( card_index != -1 )
		{
			m_vt_mirror_from_index.push_back(card_index);
		}
	}
			

	if( pInterfaceCfgFactory->FindCard(new_config.m_strMirrorToCard.c_str()) )
	{
		return false;
	}

	if( !StartMirrorDevice(new_config) )
	{
		return false;
	}

	m_mirror_out_config = new_config;

	m_bMirroring = true;

	return true;
}

void CMirrorOutDataMgr::stop_mirror()
{
	m_bMirroring = false;

	StopMirrorDevice();

	return;
}

void CMirrorOutDataMgr::Start_To_Write()
{
	if( m_pMirrorEtherioDataStream )
	{
		m_bMirroring = true;

		m_pMirrorEtherioDataStream->StartToWrite();
	}

	return;
}

void CMirrorOutDataMgr::Stop_Writting()
{
	m_bMirroring = false;

	if( m_pMirrorEtherioDataStream )
	{
		m_pMirrorEtherioDataStream->StopWriting(false);
	}

	return;
}

int CMirrorOutDataMgr::write_packet(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array)
{
	BOOL bSave = true;

	if( m_bUsedFlt )
	{
		bSave = CPF_SimpleFilterTool::filter_packet(
			(const char *)packet.pPacket,packet.nPktlen,m_flt_condition);
	}

	if( !bSave )
	{
		return 0;
	}

	if( m_pMirrorEtherioDataStream )
	{
		return m_pMirrorEtherioDataStream->WritePacket(type,packet,-1,1);
	}

	return 0;
}

int CMirrorOutDataMgr::write_packet(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array)
{
	BOOL bSave = true;

	if( m_bUsedFlt )
	{
		bSave = CPF_SimpleFilterTool::filter_packet(
			(const char *)pdata,datalen,m_flt_condition);
	}

	if( !bSave )
	{
		return 0;
	}

	if( m_pMirrorEtherioDataStream )
	{
		return m_pMirrorEtherioDataStream->WritePacket(type,pdata,datalen,index_array,1);
	}

	return 0;
}

BOOL CMirrorOutDataMgr::IsMirroring() const
{
	return m_bMirroring;
}

BOOL CMirrorOutDataMgr::StartMirrorDevice(const CMirrorOutConfig& new_config)
{
	StopMirrorDevice();

	m_pMirrorEtherioDataStream = new CEtherioWriteDataStream;

	if( !m_pMirrorEtherioDataStream )
	{
		return false;
	}

	if( !m_pMirrorEtherioDataStream->Open(
		1,
		new_config.m_copy_data_to_send,
		NULL,NULL,
		new_config.m_strMirrorToCard.c_str(),
		new_config.m_nMirrorSendType,
		new_config.m_nMirrorSendBuffer) )
	{
		delete m_pMirrorEtherioDataStream;
		m_pMirrorEtherioDataStream = NULL;
		return false;
	}

	m_pMirrorEtherioDataStream->StartToWrite();

	return true;
}

BOOL CMirrorOutDataMgr::StopMirrorDevice()
{
	if( m_pMirrorEtherioDataStream )
	{
		m_pMirrorEtherioDataStream->StopWriting(true);
		m_pMirrorEtherioDataStream->Close();
		delete m_pMirrorEtherioDataStream;
		m_pMirrorEtherioDataStream = NULL;
	}

	return true;
}
