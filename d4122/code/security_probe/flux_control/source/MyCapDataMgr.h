//////////////////////////////////////////////////////////////////////////
//MyCapDataMgr.h

#pragma once

#include "PacketIO/DataSendMgr.h"
#include "cpf/simple_filter/CPF_SimpleFltCondition.h"

class CMyCapDataMgr
{
public:
	CMyCapDataMgr(void);
	~CMyCapDataMgr(void);

	//drv_type:	DRIVER_TYPE_FILE_ETHEREAL = 2,
	//DRIVER_TYPE_FILE_ZCCPT = 3,

	BOOL start_cap(DRIVER_TYPE drv_type,
		BOOL bUsedFlt,
		const char * flt_fiile_name,
		unsigned int one_filesize_mb,
		unsigned int file_nums,
		unsigned int captime,
		const char * path,
		const char * filename);

	void stop_cap(BOOL bDeleteFile);

	//-1表示没有开始，0表示结束，1表示开始
	int get_cap_status(int& pos);

	int write_packet(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array);
	int write_packet(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array);

	BOOL IsCapping() const;

public:
	CDataSendMgr			m_cap_data_mgr;
	IWriteDataStream *		m_pcap_data_stream;
	ACE_Time_Value			m_start_time;	

	BOOL					m_bCapping;

	ACE_UINT64				m_ullWriteBytes;
	BOOL					m_bFull;

private:
	CPF_SimpleFltCondition	m_flt_condition;
	BOOL					m_bUsedFlt;

	DRIVER_TYPE				m_drv_type;
		
};

class CInterfaceFactory;

class CMyCapDataMgrEx : public CMyCapDataMgr
{
public:
	CMyCapDataMgrEx();
	~CMyCapDataMgrEx();

public:
	BOOL init(CInterfaceFactory * pInterfaceCfgFactory,
		std::vector< std::string >& vt_from_card);

public:
	inline BOOL IsCapFromCardIndex(int card_idnex) const
	{
		for(size_t i = 0; i < m_vt_cap_from_index.size(); ++i)
		{
			if( m_vt_cap_from_index[i] == card_idnex )
			{
				return true;
			}
		}

		return false;
	}

private:
	std::vector<int>		m_vt_cap_from_index;
};