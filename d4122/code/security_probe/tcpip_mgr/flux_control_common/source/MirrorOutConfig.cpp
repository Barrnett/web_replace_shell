//////////////////////////////////////////////////////////////////////////
//MirrorOutConfig.cpp

#include "StdAfx.h"
#include "MirrorOutConfig.h"
#include "cpf/strtools.h"

CMirrorOutConfig::CMirrorOutConfig(void)
{
	close();
}

CMirrorOutConfig::~CMirrorOutConfig(void)
{
	close();
}

CMirrorOutConfig& CMirrorOutConfig::operator = (const CMirrorOutConfig& other)
{
	if( this != &other )
	{
		m_mirror_status = other.m_mirror_status;
		m_vt_mirror_from_card = other.m_vt_mirror_from_card;

		m_nMirrorSendBuffer = other.m_nMirrorSendBuffer;
		m_nMirrorSendType = other.m_nMirrorSendType;
		m_strMirrorToCard = other.m_strMirrorToCard;

		m_use_flt = other.m_use_flt;

		m_copy_data_to_send = other.m_copy_data_to_send;
	}

	return (*this);
}

int CMirrorOutConfig::operator == (const CMirrorOutConfig& other) const
{
	if( m_mirror_status == other.m_mirror_status
		&& m_vt_mirror_from_card == other.m_vt_mirror_from_card

		&& m_nMirrorSendBuffer == other.m_nMirrorSendBuffer
		&& m_nMirrorSendType == other.m_nMirrorSendType
		&& m_strMirrorToCard == other.m_strMirrorToCard

		&& m_copy_data_to_send == other.m_copy_data_to_send

		&& m_use_flt == other.m_use_flt)
	{
		return 1;
	}
	
	return 0;
}

void CMirrorOutConfig::close()
{
	m_mirror_status = 0;

	m_vt_mirror_from_card.clear();

	m_nMirrorSendBuffer = 5;
	m_nMirrorSendType = 1;
	m_strMirrorToCard ="MIR"; 

	m_copy_data_to_send = true;

	m_use_flt = false;

	return;
}

BOOL CMirrorOutConfig::LoadConfig(CTinyXmlEx& xml_reader)
{
	TiXmlNode * mirror_data = xml_reader.GetRootNode()->FirstChildElement("mirror_data");

	if( !mirror_data )
		return false;

	xml_reader.GetValueLikeIni(mirror_data,"mirror_status",m_mirror_status);
	xml_reader.GetValueLikeIni(mirror_data,"mirror_from_card",m_vt_mirror_from_card);

	xml_reader.GetValueLikeIni(mirror_data,"send_buffer",m_nMirrorSendBuffer);

	//目前固定为3，和转发程序完全一致
	xml_reader.GetValueLikeIni(mirror_data,"send_type",m_nMirrorSendType);

	xml_reader.GetValueLikeIni(mirror_data,"mirror_to_card",m_strMirrorToCard);

	xml_reader.GetValueLikeIni(mirror_data,"use_flt",m_use_flt);

	xml_reader.GetValueLikeIni(mirror_data,"copy_data_to_send",m_copy_data_to_send);

	return true;
}

BOOL CMirrorOutConfig::SaveConfig(CTinyXmlEx& xml_writter)
{
	TiXmlNode* pRootNode = 
		xml_writter.InsertRootElement("mirror_outter_config");

	if( !pRootNode )
		return false;

	TiXmlNode * mirror_data = 
		xml_writter.InsertEndChildElement(pRootNode,"mirror_data");

	if( !mirror_data )
		return false;

	xml_writter.SetValueLikeIni(mirror_data,"mirror_status",m_mirror_status);
	xml_writter.SetValueLikeIni(mirror_data,"mirror_from_card",m_vt_mirror_from_card);

	xml_writter.SetValueLikeIni(mirror_data,"send_buffer",m_nMirrorSendBuffer);
	xml_writter.SetValueLikeIni(mirror_data,"send_type",m_nMirrorSendType);
	xml_writter.SetValueLikeIni(mirror_data,"mirror_to_card",m_strMirrorToCard);

	xml_writter.SetValueLikeIni(mirror_data,"use_flt",m_use_flt);

	xml_writter.SetValueLikeIni(mirror_data,"copy_data_to_send",m_copy_data_to_send);

	return true;

}
