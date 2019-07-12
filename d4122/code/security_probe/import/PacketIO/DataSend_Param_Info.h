//////////////////////////////////////////////////////////////////////////
//DataSend_Param_Info.h

#pragma once

#include "PacketIO/PacketIO.h"


typedef struct tagEtherio_SendParam_Info
{
	tagEtherio_SendParam_Info()
	{
		m_nMemSize = 20;
		m_sendtype = 1;
		m_drv_load_type = 0;
		m_copy_data_to_send = 1;
	}

	tagEtherio_SendParam_Info& operator = (const tagEtherio_SendParam_Info& other )
	{
		if( &other != this )
		{
			m_strSysFileName = other.m_strSysFileName;
			m_strDriverName = other.m_strDriverName;
			m_strMacAddr = other.m_strMacAddr;
			m_nMemSize = other.m_nMemSize;
			m_sendtype = other.m_sendtype;
			m_drv_load_type = other.m_drv_load_type;
			m_copy_data_to_send = other.m_copy_data_to_send;
		}

		return *this;
	}

	std::string			m_strSysFileName;	
	std::string			m_strDriverName;
	std::string			m_strMacAddr;
	unsigned int		m_nMemSize;
	unsigned int		m_sendtype;
	int					m_drv_load_type;
	unsigned int		m_copy_data_to_send;

}Etherio_SendParam_Info;

typedef struct tagWINPCAP_SENDDRV_PARAM
{
	tagWINPCAP_SENDDRV_PARAM()
	{
		cardIndex = 0;
	}

	tagWINPCAP_SENDDRV_PARAM& operator = (const tagWINPCAP_SENDDRV_PARAM& other )
	{
		if( &other != this )
		{
			cardIndex = other.cardIndex;
		}

		return *this;
	}

	int		cardIndex;

}WINPCAP_SENDDRV_PARAM;

typedef struct tagCpt_Write_Param
{

	tagCpt_Write_Param()
	{
		filetype = 0;
		one_filesize_mb = 0;
		num_files = 1;
		captime = 60;
		buffer_size_mb = 4;
	}

	tagCpt_Write_Param& operator = (const tagCpt_Write_Param& other )
	{
		if( &other != this )
		{
			strPath = other.strPath;
			strFileName = other.strFileName;

			filetype = other.filetype;

			num_files = other.num_files;
			one_filesize_mb = other.one_filesize_mb;
			captime = other.captime;
			buffer_size_mb = other.buffer_size_mb;
		}

		return *this;
	}

	std::string	strPath;
	std::string strFileName;

	unsigned int	filetype;
	unsigned int	num_files;//�Ƿ��������ļ�	
	unsigned int	one_filesize_mb;//ÿ���ļ����
	unsigned int	captime;//����೤ʱ�䣬��λΪ��
	unsigned int	buffer_size_mb;//���ݻ���Ĵ�С,��λΪMB

	/*
	;���ݱ����·�������ܰ����ļ�������Ϣ��ȱʡ�ڵ�ǰĿ¼�µ�capdata·����
	savefilepath=

	;���Ϊ�գ����õ�ǰʱ����Ϊ�ļ�����,���ܰ���·����Ϣ��
	savefilename=

	;�Ƿ�Ϊ����ļ���filename_0001,filename_0001����ʾ
	;���Ϊ0��ʾ��������n>1��ʾ���ٸ���1��ʾ���������ļ�

	multifile=0

	;�ļ����ͣ���ʱû�ж���
	filetype=

	;�ļ������С�����û�л���Ϊ0����һֱ����,���Ϊ4G��
	;����������ļ�������ÿ���ļ����������С����λΪMB.
	one_filesize_mb=20	

	;����೤ʱ��,��λΪ�롣ȱʡΪ0��ʾ��ʱ���޹ء�
	;����ļ���С�����ƣ���ʱ����ļ���С˭�����������������κ�һ������ͣ����
	captime = 60
	*/

}Cpt_Write_Param;

typedef Cpt_Write_Param	Ethereal_Write_Param;


struct ShareMem_Write_Param
{
	ShareMem_Write_Param()
	{
		m_mem_size_mb = 0;
	}

	ShareMem_Write_Param& operator = (const ShareMem_Write_Param& other )
	{
		if( &other != this )
		{
			m_strShareName = other.m_strShareName;
			m_mem_size_mb = other.m_mem_size_mb;
		}

		return *this;
	}

public:
	std::string		m_strShareName;
	unsigned int	m_mem_size_mb;
};

class PACKETIO_CLASS CDataSend_Param_Info
{
public:
	CDataSend_Param_Info(void)
	{
		m_drvType = DRIVER_TYPE_NONE;

	}
	~CDataSend_Param_Info(void)
	{

	}

public:
	DRIVER_TYPE GetDrvType() const
	{
		return m_drvType;
	}

	void SetDrvType(DRIVER_TYPE drvtype)
	{
		m_drvType = drvtype;
	}

	WINPCAP_SENDDRV_PARAM& GetWinpcapDrvParam()
	{
		return m_winpcap_drv_param;
	}

	Etherio_SendParam_Info& GetEtherioParamInfo()
	{		
		return m_etherioParamInfo;
	}
	Ethereal_Write_Param& GetEthereal_Write_Param()
	{
		return m_ethereal_write_param;
	}

	Cpt_Write_Param& GetCpt_Write_Param()
	{
		return m_cpt_write_param;
	}

	ShareMem_Write_Param& GetShareMem_Write_Param()
	{
		return m_sharemem_write_param;
	}

private:
	DRIVER_TYPE			m_drvType;

	//for etherio
	Etherio_SendParam_Info	m_etherioParamInfo;

	//for winpcap
	WINPCAP_SENDDRV_PARAM	m_winpcap_drv_param;

	Ethereal_Write_Param	m_ethereal_write_param;
	Cpt_Write_Param			m_cpt_write_param;

	ShareMem_Write_Param	m_sharemem_write_param;

	friend class CDataSendParamReader;
	friend class C_Ini_DataSendParamReader;
	friend class C_Xml_DataSendParamReader;
	friend class CDataSendMgr;

};
