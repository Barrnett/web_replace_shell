//////////////////////////////////////////////////////////////////////////
//MyCapDataMgr.cpp

#include "stdafx.h"
#include "MyCapDataMgr.h"
#include "cpf/path_tools.h"
#include "InterfaceFactory.h"
#include "cpf/simple_filter/CPF_SimpleFilterTool.h"
#include "cpf/simple_filter/CPF_SimpleFltReader.h"

CMyCapDataMgr::CMyCapDataMgr(void)
{
	m_pcap_data_stream = NULL;

	m_bCapping = false;
	m_bFull = false;

	m_ullWriteBytes = 0;

	m_bUsedFlt = false;

	m_drv_type = DRIVER_TYPE_NONE;
}

CMyCapDataMgr::~CMyCapDataMgr(void)
{
}


BOOL CMyCapDataMgr::start_cap(DRIVER_TYPE drv_type,
							  BOOL bUsedFlt,
							  const char * flt_fiile_name,
							  unsigned int one_filesize_mb,
							  unsigned int file_nums,
							  unsigned int captime,
							  const char * path,
							  const char * filename)
{
	m_drv_type = drv_type;
	m_bUsedFlt = bUsedFlt;

	if( m_bUsedFlt )
	{
		if( CPF_SimpleFltIO::read_filter(flt_fiile_name,m_flt_condition) )
		{
			m_bUsedFlt = true;
		}
	}

	m_cap_data_mgr.m_datasend_param_info.SetDrvType(drv_type);

	if( drv_type == DRIVER_TYPE_FILE_ETHEREAL )
	{
		Ethereal_Write_Param& ethereal_write_param =
			m_cap_data_mgr.m_datasend_param_info.GetEthereal_Write_Param();

		ethereal_write_param.captime = captime;
		ethereal_write_param.num_files = file_nums;
		ethereal_write_param.one_filesize_mb = one_filesize_mb;

		ethereal_write_param.strPath = path;
		ethereal_write_param.strFileName = filename;
	}
	else if( drv_type == DRIVER_TYPE_FILE_ZCCPT )
	{
		Cpt_Write_Param& cpt_write_param = 
			m_cap_data_mgr.m_datasend_param_info.GetCpt_Write_Param();

		cpt_write_param.captime = captime;
		cpt_write_param.num_files = file_nums;
		cpt_write_param.one_filesize_mb = one_filesize_mb;

		cpt_write_param.strPath = path;
		cpt_write_param.strFileName = filename;
	}
	else
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	m_pcap_data_stream = m_cap_data_mgr.OpenWriteDataStream();

	if( !m_pcap_data_stream )
		return false;

	m_ullWriteBytes = 0;
	m_bFull = false;
	m_bCapping = true;

	m_start_time = ACE_OS::gettimeofday();

	return true;
}

void CMyCapDataMgr::stop_cap(BOOL bDeleteFile)
{
	m_cap_data_mgr.close();

	m_bCapping = false;

	if( bDeleteFile )
	{
		std::string file_path;
		std::string file_name;

		if( m_drv_type == DRIVER_TYPE_FILE_ETHEREAL )
		{
			Ethereal_Write_Param& ethereal_write_param =
				m_cap_data_mgr.m_datasend_param_info.GetEthereal_Write_Param();

			file_path = ethereal_write_param.strPath;
			file_name = ethereal_write_param.strFileName;
		}
		else
		{
			Cpt_Write_Param& cpt_write_param =
				m_cap_data_mgr.m_datasend_param_info.GetCpt_Write_Param();

			file_path = cpt_write_param.strPath;
			file_name = cpt_write_param.strFileName;
		}

		char full_name[MAX_PATH];

		CPF::JoinPathToPath(
			full_name,sizeof(full_name),
			file_path.c_str(),
			file_name.c_str());

		ACE_OS::unlink(full_name);
	}	

	return;
}

int CMyCapDataMgr::get_cap_status(int& pos)
{
	if( !m_bCapping )
		return -1;

	unsigned int cap_time;

	if( m_drv_type == DRIVER_TYPE_FILE_ETHEREAL )
	{
		Ethereal_Write_Param& ethereal_write_param =
			m_cap_data_mgr.m_datasend_param_info.GetEthereal_Write_Param();

		cap_time = ethereal_write_param.captime;
	}
	else
	{
		Cpt_Write_Param& cpt_write_param =
			m_cap_data_mgr.m_datasend_param_info.GetCpt_Write_Param();

		cap_time = cpt_write_param.captime;
	}

	if( 0 != cap_time )
	{
		pos = (int)((ACE_OS::gettimeofday() - m_start_time).sec());
	}
	else
	{
		pos = (int)(m_ullWriteBytes/1024);
	}

	return m_bFull?0:1;
}

int CMyCapDataMgr::write_packet(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array)
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

	if( type == PACKET_OK )
	{
		m_ullWriteBytes += packet.nCaplen;
	}

	int nrtn = m_pcap_data_stream->WritePacket(type,packet,index_array,1);

	if(nrtn == -3)
	{
		m_bFull = true;
	}

	return nrtn;
}

int CMyCapDataMgr::write_packet(PACKET_LEN_TYPE type,const char * pdata,unsigned int datalen,int index_array)
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

	if( type == PACKET_OK )
	{
		m_ullWriteBytes += datalen;
	}

	int nrtn = m_pcap_data_stream->WritePacket(type,pdata,datalen,index_array,1);

	if(nrtn == -3)
	{
		m_bFull = true;
	}

	return nrtn;
}


BOOL CMyCapDataMgr::IsCapping() const
{
	return (m_bCapping && !m_bFull);
}




//////////////////////////////////////////////////////////////////////////
//

#include "InterfaceCfg.h"

CMyCapDataMgrEx::CMyCapDataMgrEx()
{

}

CMyCapDataMgrEx::~CMyCapDataMgrEx()
{

}

BOOL CMyCapDataMgrEx::init(CInterfaceFactory * pInterfaceCfgFactory,
						   std::vector< std::string >& vt_from_card)
{
	m_vt_cap_from_index.clear();

	if( vt_from_card.size() == 0 )
	{
		return false;
	}

	for(size_t i = 0; i < vt_from_card.size(); ++i)
	{
		int card_index = pInterfaceCfgFactory->GetCardIndexByShowName(vt_from_card[i].c_str());

		if( card_index != -1 )
		{
			m_vt_cap_from_index.push_back(card_index);
		}
	}

	return true;
}
