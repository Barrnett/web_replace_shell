//////////////////////////////////////////////////////////////////////////
//MirrorOutConfig.h

#pragma once

#include "cpf/BaseXMLConfig.h"
#include "flux_control_common.h"

class FLUX_CONTROL_COMMON_CLASS CMirrorOutConfig : public CBaseXMLConfig
{

public:
	CMirrorOutConfig(void);
	~CMirrorOutConfig(void);

public:
	virtual void close();

public:
	CMirrorOutConfig& operator = (const CMirrorOutConfig& other);

	int operator == (const CMirrorOutConfig& other) const;

	int operator != (const CMirrorOutConfig& other) const
	{
		return !(*this == other);
	}

private:
	virtual BOOL LoadConfig(CTinyXmlEx& xml_reader);
	virtual BOOL SaveConfig(CTinyXmlEx& xml_writter);

public:
	int					m_mirror_status;
	std::vector< std::string > m_vt_mirror_from_card;

	int					m_use_flt;

	int					m_copy_data_to_send;
	int					m_nMirrorSendBuffer;
	int					m_nMirrorSendType;
	std::string			m_strMirrorToCard;

};

#include "ace/Singleton.h"
#include "ace/Null_Mutex.h"

class FLUX_CONTROL_COMMON_CLASS CCMirrorOutConfig_Singleton : public ACE_Singleton<CMirrorOutConfig,ACE_Null_Mutex>
{
public:
	static CMirrorOutConfig * instance()
	{
		return ACE_Singleton<CMirrorOutConfig,ACE_Null_Mutex>::instance();
	}

};
